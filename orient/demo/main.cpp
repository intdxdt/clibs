#include <iostream>
#include <vector>
#include <cmath>
#include <functional>
#include <sstream>
#include <iomanip>
#include <fstream>
#include "../../orient/orient.h"
//#include "json.hpp"

const int NX = 512;
const int NY = 512;
//using json = nlohmann::json;
using pt = std::vector<double>;

std::string f2str(double f, int precision = 16)  {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << f;
    auto s = ss.str();
    s.erase(s.find_last_not_of('0') + 1, std::string::npos);
    if (s.back() == '.') {
        s.pop_back();
    }
    return s;
}

std::string as_json_array(std::vector<double> data) {
    std::ostringstream ss;
    auto n = data.size() - 1;
    ss << "[";
    for (auto i = 0; i <= n; i++) {
        ss << f2str(data[i]) << (i < n ? "," : "");
    }
    ss << "]";
    return ss.str();
}

double naiveLeftRight(pt a, pt b, pt c) {
    auto abx = c[0] - a[0];
    auto aby = c[1] - a[1];
    auto acx = b[0] - a[0];
    auto acy = b[1] - a[1];
    return abx * acy - aby * acx;
}

pt ar(double a, double b)  {
    return pt{a, b};
}

void plotPredicate(const std::function<double(pt, pt, pt)>& pred, std::string out) {
    std::vector<double> res;
    for (auto i = 0; i < NX; i++) {
        for (auto j = 0; j < NY; j++) {
            auto px = 0.5 + double(i) * std::pow(2, -53);
            auto py = 0.5 + double(j) * std::pow(2, -53);

            auto o = pred(ar(px, py), ar(12, 12), ar(24, 24));
            res.emplace_back(o);
        }
    }
    std::cout << "len or arr = " << res.size() << std::endl;
    std::fstream fid;
    fid.open(out, std::fstream::in | std::fstream::out | std::fstream::app);
    auto vals = as_json_array(res);
    fid << "module.exports=" + vals + "\n";
    fid.close();
}


int main() {
    plotPredicate(naiveLeftRight, "cpp-naive.js");
    plotPredicate(robust::Orientation2D, "cpp-robust.js");
    return 0;
}