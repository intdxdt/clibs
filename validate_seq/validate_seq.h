//
//04/06/18.
//

#ifndef VALIDATE_SEQ_VALIDATE_SEQ_H
#define VALIDATE_SEQ_VALIDATE_SEQ_H

#include <cmath>
#include <vector>
#include "../test_overlap/test_overlap.h"
namespace robust {
    bool validate_sequence(std::vector<double> x) {
        auto n = x.size();
        if (n < 1) {
            return false;
        }
        for (size_t i = 1; i < n; i++) {
            if (std::abs(x[i - 1]) >= std::abs(x[i])) {
                return false;
            }
            if (test_overlap(x[i], x[i - 1])) {
                return false;
            }
        }
        return true;
    }
}
#endif //VALIDATE_SEQ_VALIDATE_SEQ_H
