//
// Created by giacomo on 21/03/21.
//

#include "fast_minimization.h"

#include <stdio.h>
DFA2::DFA2(int num_states, int a, int start_index, std::vector<bool> finals, std::vector<int> T, std::vector<int> L, std::vector<int> H){
    Heads = H;
    Labels = L;
    Tails = T;
    Q = num_states;
    alph_size = a;
    q0 = start_index;
    final = finals;
}
void DFA2::print_DFA(){
    printf("There are %d states\n", Q);
    printf("The starting state is %d\n", q0);
    printf("The final states are: ");
    for(int i = 0; i<Q; i++){
        if(final[i])
            printf("%d ", i);
    }
    printf("\nThe alphabet size is %d\n", alph_size);
    printf("The transitions are:\n");
    for(int i = 0; i<Heads.size(); i++){
        printf("%d %d %d\n", Tails[i], Labels[i], Heads[i]);
    }
    printf("\n");
}

#if 0


/* Refinable partition */
int *M = nullptr, *W = nullptr, w = 0; // temporary worksets

partition::~partition() {
    delete[] E;
    delete[] L;
    delete[] S;
    delete[] F;
    delete[] P;
}

void partition::init(int n) {
    z = bool(n); E = new int[n];
    L = new int[n]; S = new int[n];
    F = new int[n]; P = new int[n];
    for (int i = 0; i < n; ++i) {
        E[i] = L[i] = i; S[i] = 0;
    }
    if (z) { F[0] = 0; P[0] = n; }
}

void partition::mark(int e) const {
    int s = S[e], i = L[e], j = F[s] + M[s];
    E[i] = E[j]; L[E[i]] = i;
    E[j] = e; L[e] = j;
    if (!M[s]++) { W[w++] = s; }
}

void partition::split() {
    while (w) {
        int s = W[--w], j = F[s] + M[s];
        if (j == P[s]) { M[s] = 0; continue; }
        if (M[s] <= P[s] - j) {
            F[z] = F[s]; P[z] = F[s] = j;
        }
        else {
            P[z] = P[s]; F[z] = P[s] = j;
        }
        for (int i = F[z]; i < P[z]; ++i) {
            S[E[i]] = z;
        }
        M[s] = M[z++] = 0;
    }
}

void partition::clear() {
    delete[] E; E= nullptr;
    delete[] L; L = nullptr;
    delete[] S; S = nullptr;
    delete[] F; F = nullptr;
    delete[] P; P = nullptr;
}

partition
        B, // blocks (consist of states)
C; // cords (consist of transitions)
int
        nn, // number of states
mm, // number of transitions
ff, // number of final states
q0, // initial state
*T = nullptr, // tails of transitions
*L = nullptr, // labels of transitions
*H = nullptr; // heads of transitions
bool cmp(int i, int j) {
    return L[i] < L[j];
}
/* Adjacent transitions */
int *A = nullptr, *F = nullptr;
/* Removal of irrelevant parts */
int rr = 0; // number of reached states

void fast_minimization_init() {
    M = nullptr;
    W = nullptr;
    w = 0;
    T = nullptr;
    L = nullptr;
    H = nullptr;
    A = nullptr;
    rr = 0;
    w = 0;
}

void make_adjacent(int *K) {
    int q, t;
    for (q = 0; q <= nn; ++q) { F[q] = 0; }
    for (t = 0; t < mm; ++t) { ++F[K[t]]; }
    for (q = 0; q < nn; ++q)F[q + 1] += F[q];
    for (t = mm; t--; ) { A[--F[K[t]]] = t; }
}

void reach(int q) {
    int i = B.L[q];
    if (i >= rr) {
        B.E[i] = B.E[rr]; B.L[B.E[i]] = i;
        B.E[rr] = q; B.L[q] = rr++;
    }
}

void rem_unreachable(int *T, int *H) {
    make_adjacent(T); int i, j;
    for (i = 0; i < rr; ++i) {
        for (j = F[B.E[i]];
             j < F[B.E[i] + 1]; ++j) {
            reach(H[A[j]]);
        }
    }
    j = 0;
    for (int t = 0; t < mm; ++t) {
        if (B.L[T[t]] < rr) {
            H[j] = H[t]; L[j] = L[t];
            T[j] = T[t]; ++j;
        }
    }
    mm = j; B.P[0] = rr; rr = 0;
}

void set_q0(int val) {
    q0 = val;
}

int get_q0() {
    return q0;
}


void add_edge(size_t idx, int t, int l, int h) {
    T[idx] = t;
    L[idx] = l;
    H[idx] = h;
}

void finalize_edges() {
    reach(q0); rem_unreachable(T, H);
}

void main_dfs_program_init(int N, int M) {
    T = new int[M]; L = new int[M];
    H = new int[M]; B.init(N);
    A = new int[M]; F = new int[N + 1];
    for (int i = 0; i<M; i++) {
        A[i] = 0;
    }
    for (int i = 0; i<=N; i++) {
        F[i] = 0;
    }
    mm = M;
    nn = N;
}

void quit_dfs_program() {
    delete[] T;
    T = nullptr;
    delete[] L;
    L = nullptr;
    delete[] H;
    H = nullptr;
    delete[] A;
    A = nullptr;
    delete[] F;
    F = nullptr;
    B.clear();
    C.clear();
    delete[] W;
    W = nullptr;
    delete[] M;
    M = nullptr;
    w = 0;
}

void add_final(int q) {
    if (B.L[q] < B.P[0]) { reach(q); }
}


int main_dfs_program(std::vector<minimization_edge> &edges, int& q0Res, std::unordered_set<int>& Finals) {
    ff = rr; rem_unreachable(H, T);
    /* Make initial partition */
    W = new int[mm + 1]; M = new int[mm + 1];
    M[0] = ff;
    if (ff) { W[w++] = 0; B.split(); }
    /* Make transition partition */
    C.init(mm);
    if (mm) {
        std::sort(C.E, C.E + mm, cmp);
        C.z = M[0] = 0; int a = L[C.E[0]];
        for (int i = 0; i < mm; ++i) {
            int t = C.E[i];
            if (L[t] != a) {
                a = L[t]; C.P[C.z++] = i;
                C.F[C.z] = i; M[C.z] = 0;
            }
            C.S[t] = C.z; C.L[t] = i;
        }
        C.P[C.z++] = mm;
    }

    /* Split blocks and cords */
    make_adjacent(H);
    int b = 1, c = 0, i, j;
    while (c < C.z) {
        for (i = C.F[c]; i < C.P[c]; ++i) {
            B.mark(T[C.E[i]]);
        }
        B.split(); ++c;
        while (b < B.z) {
            for (i = B.F[b]; i < B.P[b]; ++i) {
                for (
                        j = F[B.E[i]];
                        j < F[B.E[i] + 1]; ++j
                        ) {
                    C.mark(A[j]);
                }
            }
            C.split(); ++b;
        }
    }
    /* Count the numbers of transitions
    and final states in the result */
    int mo = 0, fo = 0;
    for (int t = 0; t < mm; ++t) {
        if (B.L[T[t]] == B.F[B.S[T[t]]]) {
            ++mo;
        }
    }
    for (int b = 0; b < B.z; ++b) {
        if (B.F[b] < ff) { ++fo; }
    }


    q0Res = B.S[q0];
    for (int t = 0; t < mm; ++t) {
        if (B.L[T[t]] == B.F[B.S[T[t]]]) {
            edges.emplace_back(B.S[T[t]] , L[t], B.S[H[t]]);
        }
    }

    for (int b = 0; b < B.z; ++b) {
        if (B.F[b] < ff) {
            Finals.insert(b);
        }
    }
#if 0
    /* Print the result */
    std::cout << B.z << ' ' << mo
              << ' ' << B.S[q0] << ' ' << fo << '\n';
    for (int t = 0; t < mm; ++t) {
        if (B.L[T[t]] == B.F[B.S[T[t]]]) {
            std::cout << B.S[T[t]] << ' ' << L[t]
                      << ' ' << B.S[H[t]] << '\n';
        }
    }

#endif
}
#endif