//
// Created on 11/08/18.
//
#include <vector>

#ifndef RBSEGS_BRUTE_H
#define RBSEGS_BRUTE_H

struct BruteForceList {
    std::vector<double> intervals;
    std::vector<int> index;
    int count;

    explicit BruteForceList(size_t capacity) {
        intervals = std::vector<double>(2 * capacity);
        index = std::vector<int>(capacity);
        count = 0;
    }

    void insert(double lo, double hi, int idx) {
        index[count] = idx;
        intervals[2 * count] = lo;
        intervals[2 * count + 1] = hi;
        count += 1;
    }

    void remove(int idx) {
        for (auto i = count - 1; i >= 0; i--) {
            if (index[i] == idx) {
                index[i] = index[count - 1];
                intervals[2 * i] = intervals[2 * (count - 1)];
                intervals[2 * i + 1] = intervals[2 * count - 1];
                count += -1;
                break;
            }
        }
    }

};
#endif //RBSEGS_BRUTE_H
