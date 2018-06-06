//
//05/06/18.
//
#include <cmath>
#include <vector>

#ifndef SUBTRACT_SUBTRACT_H
#define SUBTRACT_SUBTRACT_H
namespace robust {
    //scalar sum: easy case: add two scalars
    std::vector<double> scalar_scalar_subtract(double a, double b) {
        auto x = a + b;
        auto bv = x - a;
        auto av = x - bv;
        auto br = b - bv;
        auto ar = a - av;
        auto y = ar + br;
        if (y != 0) {
            return {y, x};
        }
        return {x};
    }


//linear expansion
    std::vector<double> linearExpansionSub(const std::vector<double> &e, const std::vector<double> &f) {
        auto ne = e.size();
        auto nf = f.size();
        if (ne == 1 && nf == 1) {
            return scalar_scalar_subtract(e[0], -f[0]);
        }
        auto n = ne + nf;
        std::vector<double> g;
        g.resize(n, 0);

        auto count = 0;
        auto eptr = 0;
        auto fptr = 0;

        auto ei = e[eptr];
        auto ea = std::abs(ei);
        auto fi = -f[fptr];
        auto fa = std::abs(fi);
        double a{0}, b{0};
        if (ea < fa) {
            b = ei;
            eptr += 1;
            if (eptr < ne) {
                ei = e[eptr];
                ea = std::abs(ei);
            }
        } else {
            b = fi;
            fptr += 1;
            if (fptr < nf) {
                fi = -f[fptr];
                fa = std::abs(fi);
            }
        }
        if ((eptr < ne && ea < fa) || (fptr >= nf)) {
            a = ei;
            eptr += 1;
            if (eptr < ne) {
                ei = e[eptr];
                ea = std::abs(ei);
            }
        } else {
            a = fi;
            fptr += 1;
            if (fptr < nf) {
                fi = -f[fptr];
                fa = std::abs(fi);
            }
        }
        auto x = a + b;
        auto bv = x - a;
        auto y = b - bv;
        auto q0 = y;
        auto q1 = x;
        double _x{0}, _bv{0}, _av{0}, _br{0}, _ar{0};
        while (eptr < ne && fptr < nf) {
            if (ea < fa) {
                a = ei;
                eptr += 1;
                if (eptr < ne) {
                    ei = e[eptr];
                    ea = std::abs(ei);
                }
            } else {
                a = fi;
                fptr += 1;
                if (fptr < nf) {
                    fi = -f[fptr];
                    fa = std::abs(fi);
                }
            }
            b = q0;
            x = a + b;
            bv = x - a;
            y = b - bv;
            if (y != 0) {
                g[count] = y;
                count += 1;
            }
            _x = q1 + x;
            _bv = _x - q1;
            _av = _x - _bv;
            _br = x - _bv;
            _ar = q1 - _av;
            q0 = _ar + _br;
            q1 = _x;
        }
        while (eptr < ne) {
            a = ei;
            b = q0;
            x = a + b;
            bv = x - a;
            y = b - bv;
            if (y != 0) {
                g[count] = y;
                count += 1;
            }
            _x = q1 + x;
            _bv = _x - q1;
            _av = _x - _bv;
            _br = x - _bv;
            _ar = q1 - _av;
            q0 = _ar + _br;
            q1 = _x;
            eptr += 1;
            if (eptr < ne) {
                ei = e[eptr];
            }
        }
        while (fptr < nf) {
            a = fi;
            b = q0;
            x = a + b;
            bv = x - a;
            y = b - bv;
            if (y != 0) {
                g[count] = y;
                count += 1;
            }
            _x = q1 + x;
            _bv = _x - q1;
            _av = _x - _bv;
            _br = x - _bv;
            _ar = q1 - _av;
            q0 = _ar + _br;
            q1 = _x;
            fptr += 1;
            if (fptr < nf) {
                fi = -f[fptr];
            }
        }
        if (q0 != 0) {
            g[count] = q0;
            count += 1;
        }
        if (q1 != 0) {
            g[count] = q1;
            count += 1;
        }
        if (count == 0) {
            g[count] = 0.0;
            count += 1;
        }

        return std::vector<double>(g.begin(), g.begin() + count);
    }

//Robust subtraction of two floats
    std::vector<double> Subtract(const std::vector<double> &e, const std::vector<double> &f) {
        return linearExpansionSub(e, f);
    }

}
#endif //SUBTRACT_SUBTRACT_H
