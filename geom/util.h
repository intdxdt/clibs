//
//06/06/18.
//
#include <cmath>
#include <vector>
#include <cassert>
#include <array>
#include <vector>
#include <sstream>
#include <functional>
#include <optional>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <locale>
#include "../mbr/mbr.h"
#include "../mutil/mutil.h"
#include "../rtree/rtree.h"

#ifndef GEOM_UTIL_H
#define GEOM_UTIL_H

namespace geom {
    double inf = std::numeric_limits<double>::infinity();
    double neginf = -std::numeric_limits<double>::infinity();
    const int x = 0;
    const int y = 1;
    const int z = 2;
    const int null = -9;

    enum GeoType {
        UnkownType = -1,
        PointType = 0,
        SegmentType = 1,
        LineStringType = 2,
        PolygonType = 3,
    };

    ///find the sign of value -1, 0 , 1
    inline int xy_sign(double v) {
        int i{0};
        if (v > 0) {
            i = 1;
        }
        else if (v < 0) {
            i = -1;
        }
        return i;
    }

std::string str_tolower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); }
                  );
    return s;
}

    /// trim from start (in place)
    static inline void ltrim(std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
            return !std::isspace(ch);
        }));
    }

    /// trim from end (in place)
    static inline void rtrim(std::string& s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    /// trim from both ends (in place)
    static inline void trim(std::string& s) {
        ltrim(s);
        rtrim(s);
    }

    /// trim from start (copying)
    static inline std::string ltrim_copy(std::string s) {
        ltrim(s);
        return s;
    }

    /// trim from end (copying)
    static inline std::string rtrim_copy(std::string s) {
        rtrim(s);
        return s;
    }

    /// trim from both ends (copying)
    static inline std::string trim_copy(std::string s) {
        trim(s);
        return s;
    }
}

#endif //GEOM_UTIL_H
