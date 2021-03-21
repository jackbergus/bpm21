//
// Created by giacomo on 21/03/21.
//

#ifndef BPM21_FAST_MINIMIZATION_H
#define BPM21_FAST_MINIMIZATION_H


#include <unordered_set>




#include <vector>

class DFA2{
public:
    DFA2(int num_states, int alph_size, int start_index, std::vector<bool> finals, std::vector<int> H, std::vector<int> L, std::vector<int> T);
    DFA2() = default;
    DFA2(const DFA2&) = default;
    DFA2(DFA2&& ) = default;
    DFA2& operator=(DFA2&& ) = default;
    DFA2& operator=(const DFA2& ) = default;

    void print_DFA();
    //number of states
    int Q;
    //whether a state is final
    std::vector<bool> final;
    //starting state
    int q0;
    //size of alphabet; a letter is an integer from 0 to alph_size-1
    int alph_size;
    // transitions[i] maps j to k iff there exists a transition from i to k under letter j
    std::vector<int> Heads;
    std::vector<int> Labels;
    std::vector<int> Tails;
};

struct fast_minimization {
    //This is Valmari's code for his algorithm that I modified a bit.

/* Refinable partition */
    std::vector<int> M;
    std::vector<int> W;
    int w = 0;  // temporary worksets

    struct partition{
        int z;
        std::vector<int> E, L, S, F, P;

        void init( int n ){
            z = bool( n );  E = std::vector<int>(n);
            L = std::vector<int> (n); S = std::vector<int> (n);
            F = std::vector<int> (n); P = std::vector<int> (n);
            for( int i = 0; i < n; ++i ){
                E[i] = L[i] = i; S[i] = 0; }
            if( z ){ F[0] = 0; P[0] = n; }
        }

        void mark( int e, std::vector<int>& M, std::vector<int> W, int& w){
            int s = S[e], i = L[e], j = F[s]+M[s];
            E[i] = E[j]; L[E[i]] = i;
            E[j] = e; L[e] = j;
            if( !M[s]++ ){ W[w++] = s; }
        }

        void split(std::vector<int>& M, std::vector<int> W, int& w){
            while( w ){
                int s = W[--w], j = F[s]+M[s];
                if( j == P[s] ){M[s] = 0; continue;}
                if( M[s] <= P[s]-j ){
                    F[z] = F[s]; P[z] = F[s] = j; }
                else{
                    P[z] = P[s]; F[z] = P[s] = j; }
                for( int i = F[z]; i < P[z]; ++i ){
                    S[E[i]] = z; }
                M[s] = M[z++] = 0;
            }
        }

    };

    partition
            B,     // blocks (consist of states)
    C;     // cords (consist of transitions)

    int
            nn,    // number of states
    mm,    // number of transitions
    ff,    // number of final states
    q0;    // initial state
    std::vector<int>
            T,    // tails of transitions
    L,    // labels of transitions
    H;    // heads of transitions

    bool cmp( int i, int j ){
        return L[i] < L[j]; }

/* Adjacent transitions */
    std::vector<int> A, F;
    void make_adjacent( std::vector<int> &K ){
        int q, t;
        for( q = 0; q <= nn; ++q ){ F[q] = 0; }
        for( t = 0; t < mm; ++t ){ ++F[K[t]]; }
        for( q = 0; q < nn; ++q )F[q+1] += F[q];
        for( t = mm; t--; ){ A[--F[K[t]]] = t; }
    }

/* Removal of irrelevant parts */
    int rr = 0;   // number of reached states

    inline void reach( int q ){
        int i = B.L[q];
        if( i >= rr ){
            B.E[i] = B.E[rr]; B.L[B.E[i]] = i;
            B.E[rr] = q; B.L[q] = rr++; }
    }

    void rem_unreachable( std::vector<int> &T, std::vector<int> &H ){
        make_adjacent( T ); int i, j;
        for( i = 0; i < rr; ++i ){
            for( j = F[B.E[i]];
                 j < F[B.E[i] + 1]; ++j ){
                reach( H[A[j]] ); } }
        j = 0;
        for( int t = 0; t < mm; ++t ){
            if( B.L[T[t]] < rr ){
                H[j] = H[t]; L[j] = L[t];
                T[j] = T[t]; ++j; } }
        mm = j; B.P[0] = rr; rr = 0;
    }

/* Main program */
    DFA2 Valmari(const DFA2 *in){

        /* Read sizes and reserve most memory\*/
        nn = in->Q;
        std::vector<std::vector<std::pair<int, int> > > labels(in->alph_size, std::vector<std::pair<int, int> > (0));
        for(int i = 0; i<in->Heads.size(); i++){
            labels[in->Labels[i]].push_back({in->Tails[i], in->Heads[i]});
        }
        for(int i = 0; i<in->alph_size; i++){
            for(std::pair<int, int> x : labels[i]){
                T.push_back(x.first);
                L.push_back(i);
                H.push_back(x.second);
            }
        }
        mm = H.size();
        q0 = in->q0;
        std::vector<int> fi;
        for(int i = 0; i<in->Q; i++)
            if(in->final[i])
                fi.push_back(i);
        ff = fi.size();
        B.init( nn );
        A = std::vector<int> (mm); F = std::vector<int> (nn+1);
        /* Remove states that cannot be reached
            from the initial state, and from which
            final states cannot be reached */
        reach( q0 ); rem_unreachable( T, H );
        for(int q : fi){
            if( B.L[q] < B.P[0] ){ reach( q ); } }
        ff = rr; rem_unreachable( H, T );

        /* Make initial partition */
        W = std::vector<int> ( mm+1 ); M = std::vector<int> ( mm+1);
        M[0] = ff;
        if( ff ){ W[w++] = 0; B.split(M, W, w); }

        /* Make transition partition */
        C.init( mm );
        if( mm ){
            C.z = M[0] = 0; int a = L[C.E[0]];
            for( int i = 0; i < mm; ++i ){
                int t = C.E[i];
                if( L[t] != a ){
                    a = L[t]; C.P[C.z++] = i;
                    C.F[C.z] = i; M[C.z] = 0; }
                C.S[t] = C.z; C.L[t] = i; }
            C.P[C.z++] = mm;
        }

        /* Split blocks and cords */
        make_adjacent( H );
        int b = 1, c = 0, i, j;
        while( c < C.z ){
            for( i = C.F[c]; i < C.P[c]; ++i ){
                B.mark( T[C.E[i]], M, W, w ); }
            B.split(M, W, w); ++c;
            while( b < B.z ){
                for( i = B.F[b]; i < B.P[b]; ++i ){
                    for(
                            j = F[B.E[i]];
                            j < F[B.E[i]+1]; ++j
                            ){
                        C.mark( A[j], M, W, w); } }
                C.split(M, W, w); ++b; }
        }

        /* Count the numbers of transitions
            and final states in the result */
        int mo = 0, fo = 0;
        for( int t = 0; t < mm; ++t ){
            if( B.L[T[t]] == B.F[B.S[T[t]]] ){
                ++mo; } }
        for( int b = 0; b < B.z; ++b ){
            if( B.F[b] < ff ){ ++fo; } }

        int num_states = B.z;
        int start_index = B.S[q0];
        std::vector<bool> finals (num_states, 0);
        std::vector<int> Tails;
        std::vector<int> Labels;
        std::vector<int> Heads;
        for( int t = 0; t < mm; ++t ){
            if( B.L[T[t]] == B.F[B.S[T[t]]] ){
                Tails.push_back(B.S[T[t]]);
                Labels.push_back(L[t]);
                Heads.push_back(B.S[H[t]]);
            }
        }

        for( int b = 0; b < B.z; ++b ){
            if( B.F[b] < ff ){
                finals[b] = 1;
            }
        }

        return {num_states, in->alph_size, start_index, finals, Tails, Labels, Heads};
    }

};

#include <graphs/FlexibleFA.h>

template<typename NodeLabel, typename EdgeLabel>
FlexibleFA<size_t, EdgeLabel> fast_minimization(FlexibleFA<NodeLabel, EdgeLabel>& graph) {
//    struct fast_minimization algo;
    //int w = 0;


    FlexibleFA<size_t, EdgeLabel> result;

    struct fast_minimization algorithm;

    auto V = graph.getNodeIds();
    size_t maxV = 0;
    for (const auto& x : V) maxV = std::max(maxV, x);
    //std::vector<bool> final;
    int num_states;
    int alph_size;
    int start_index = -1;
    std::vector<bool> finals(maxV+1, false);
    std::vector<int> H;
    std::vector<int> L;
    std::vector<int> T;


    std::vector<int>    node_id_map(maxV+1, 0);
    std::vector<size_t> node_id_map_inv(V.size(), 0);
    std::unordered_map<EdgeLabel, size_t> edgeLabel_id_map;
    std::unordered_map<size_t, EdgeLabel> edgeLabel_id_map_inv;

    int node_id = 0;
    int edge_id = 0;
    assert(graph.init().size() == 1);
    for (const size_t& id : V) {
        std::cout << id << std::endl;
        node_id_map[id] = node_id;
        node_id_map_inv[node_id] = id;
        //final.emplace_back(graph.isFinalNodeByID(id));
        if (graph.isInitialNodeByID(id)) {
            start_index = (node_id);
        }
        finals[node_id] = graph.isFinalNodeByID(id);
        node_id++;

        for (const auto& edge: graph.outgoingEdges(id)) {
            auto it = edgeLabel_id_map.emplace(edge.first, edge_id);
            if (it.second) {
                edgeLabel_id_map_inv[edge_id] = edge.first;
                edge_id++;
            }
        }
    }
    assert(start_index >= 0);
    num_states = node_id;

    int edge_curr = 0;
    for (const auto& id : V) {
        int t_id = node_id_map.at(id);
        for (const auto& edge: graph.outgoingEdges(id)) {
            int l_id = edgeLabel_id_map.at(edge.first);
            int h_id = node_id_map.at(edge.second);
            H.emplace_back(h_id);
            L.emplace_back(l_id);
            T.emplace_back(t_id);
            edge_curr++;
        }
    }

    alph_size = edgeLabel_id_map_inv.size();
    DFA2 graph_converted(num_states, alph_size, start_index, finals, H, L, T);
    DFA2 preliminar_result = algorithm.Valmari(&graph_converted);

    std::unordered_map<int, size_t> result_nodes;
    for (size_t i = 0, N = preliminar_result.Heads.size(); i<N; i++) {
        {
            int src = preliminar_result.Tails[i];
            int lab = preliminar_result.Labels[i];
            int dst = preliminar_result.Heads[i];
            if (!result_nodes.contains(src)) {
                result_nodes[src] = result.addNewNodeWithLabel(src);
            }
            if (!result_nodes.contains(dst)) {
                result_nodes[dst] = result.addNewNodeWithLabel(dst);
            }
            result.addNewEdgeFromId(result_nodes[src], result_nodes[dst], edgeLabel_id_map_inv.at(lab));
            /*std::cout << algo.B.S[algo.T[t]] << ' ' << algo.L[t]
                      << ' ' << algo.B.S[algo.H[t]] << '\n';*/
        }
    }
    result.addToInitialNodesFromId(result_nodes.at(preliminar_result.q0));


    for (size_t i = 0, N = preliminar_result.final.size(); i<N; i++)
        if (preliminar_result.final[i])
            result.addToFinalNodesFromId(result_nodes.at(i));

    return result;

}

#if 0
/* Code below reproduced from "Fast brief practical DFA minimization" by Antti Valmari (2011) */
#include <iostream>
#include <algorithm>
struct partition {
    int z, *E = nullptr, *L = nullptr, *S = nullptr, *F = nullptr, *P = nullptr;
    ~partition();
    void clear();
    void init(int n);
    void mark(int e) const;
    void split();
};


void fast_minimization_init();
void make_adjacent(int K[]);
inline void reach(int q);
void rem_unreachable(int T[], int H[]);

/* Main program */
int main_dfs_program(std::vector<minimization_edge> &edges, int& q0, std::unordered_set<int>& F);

#include <vector>
#include <graphs/FlexibleFA.h>
#include <unordered_map>

void set_q0(int val);
int get_q0();
void add_edge(size_t idx, int t, int l, int h);
void main_dfs_program_init(int nn, int mm);
void finalize_edges();
void quit_dfs_program();
void add_final(int q);

template<typename NodeLabel, typename EdgeLabel>
FlexibleFA<size_t, EdgeLabel> fast_minimization(FlexibleFA<NodeLabel, EdgeLabel>& graph) {
//    struct fast_minimization algo;
    //int w = 0;

    FlexibleFA<size_t, EdgeLabel> result;
    auto V = graph.getNodeIds();
    //std::vector<bool> final;

    size_t maxV = 0;
    for (const auto& x : V) maxV = std::max(maxV, x);

    std::vector<int>    node_id_map(maxV+1, 0);
    std::vector<size_t> node_id_map_inv(V.size(), 0);
    std::unordered_map<EdgeLabel, size_t> edgeLabel_id_map;
    std::unordered_map<size_t, EdgeLabel> edgeLabel_id_map_inv;

    int node_id = 0;
    int edge_id = 0;
    set_q0(-1);
    assert(graph.init().size() == 1);
    for (const size_t& id : V) {
        std::cout << id << std::endl;
        node_id_map[id] = node_id;
        node_id_map_inv[node_id] = id;
        //final.emplace_back(graph.isFinalNodeByID(id));
        if (graph.isInitialNodeByID(id)) {
            set_q0(node_id);
        }
        node_id++;

        for (const auto& edge: graph.outgoingEdges(id)) {
            auto it = edgeLabel_id_map.emplace(edge.first, edge_id);
            if (it.second) {
                edgeLabel_id_map_inv[edge_id] = edge.first;
                edge_id++;
            }
        }
    }
    assert(get_q0() >= 0);

    main_dfs_program_init(node_id, edge_id);

    int edge_curr = 0;
    for (const auto& id : V) {
        int t_id = node_id_map.at(id);
        for (const auto& edge: graph.outgoingEdges(id)) {
            int l_id = edgeLabel_id_map.at(edge.first);
            int h_id = node_id_map.at(edge.second);
            add_edge(edge_curr, t_id, l_id, h_id);
            edge_curr++;
        }
    }

    finalize_edges();
    for (const auto& final: graph.fini()) {
        if (!graph.removed_nodes.contains(final)) {
            int q = node_id_map[final];
            add_final(q);
        }
    }

    std::vector<minimization_edge> edges;
    int q0;
    std::unordered_set<int> F;
    main_dfs_program(edges, q0, F);

    std::unordered_map<int, size_t> result_nodes;
    for (const minimization_edge& edge : edges) {
        {
            int src = edge.src;
            int lab = edge.label;
            int dst = edge.dst;
            if (!result_nodes.contains(src)) {
                result_nodes[src] = result.addNewNodeWithLabel(src);
            }
            if (!result_nodes.contains(dst)) {
                result_nodes[dst] = result.addNewNodeWithLabel(dst);
            }
            result.addNewEdgeFromId(result_nodes[src], result_nodes[dst], edgeLabel_id_map_inv.at(lab));
            /*std::cout << algo.B.S[algo.T[t]] << ' ' << algo.L[t]
                      << ' ' << algo.B.S[algo.H[t]] << '\n';*/
        }
    }

    result.addToInitialNodesFromId(result_nodes.at(q0));
    for (const int& f : F)
        result.addToFinalNodesFromId(result_nodes.at(f));

    quit_dfs_program();
    return result;

}



#endif //BPM21_FAST_MINIMIZATION_H
#endif