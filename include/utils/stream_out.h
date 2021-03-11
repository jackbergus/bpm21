//
// Created by giacomo on 03/01/21.
//

#ifndef CUCCIOLO_STREAM_OUT_H
#define CUCCIOLO_STREAM_OUT_H

#include <iostream>
#include <vector>

template<typename T, typename V> std::ostream &operator<<(std::ostream &os, const std::pair<T,V> &insertion) {
    return os << "⟪" << insertion.first << ", " << insertion.second << "⟫";
}

template<typename T> std::ostream &operator<<(std::ostream &os, const std::vector<T> &insertion) {
    os << '{';
    for (size_t i = 0, N = insertion.size(); i<N; i++) {
        os << insertion.at(i);
        if (i < (N-1)) os << ", ";
    }
    return os << '}';
}

#endif //CUCCIOLO_STREAM_OUT_H
