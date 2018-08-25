#include <vector>

#ifndef RBSEGS_BRUTE_H
#define RBSEGS_BRUTE_H
namespace rbsegs {
    struct BruteForceList {
        std::vector<double> intervals;
        std::vector<size_t> index;
        size_t count;

        explicit BruteForceList(size_t capacity) {
            intervals = std::vector<double>(2 * capacity);
            index = std::vector<size_t>(capacity);
            count = 0;
        }

        void insert(double lo, double hi, size_t idx) {
            index[count] = idx;
            intervals[2 * count] = lo;
            intervals[2 * count + 1] = hi;
            count += 1;
        }

        void remove(size_t idx) {
            for (auto i = count; (i-- > 0);) {
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
}
#endif //RBSEGS_BRUTE_H
