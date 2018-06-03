#include <cmath>
#include <vector>
#ifndef SUM_SUM_H
#define SUM_SUM_H
std::vector<double> scalarScalar(double a, double b );
std::vector<double> linearExpansionSum(const std::vector<double>&, const  std::vector<double>&) ;
//robust sum
std::vector<double> Sum(const std::vector<double>& e,const std::vector<double>& f ) {
	return linearExpansionSum(e, f);
}

//linear expansion sum
std::vector<double> linearExpansionSum(const std::vector<double>& e,const std::vector<double>& f )  {
	auto ne = e.size();
	auto nf = f.size();
	if( ne == 1 && nf == 1) {
		return scalarScalar(e[0], f[0]);
	}
	auto n = ne + nf;
	std::vector<double>  g;
	g.reserve(n);

	size_t count = 0;
	auto eptr  = 0;
	auto fptr  = 0;

	auto ei = e[eptr];
	auto ea = std::abs(ei);
	auto fi = f[fptr];
	auto fa = std::abs(fi);
	double  a, b ;
	if (ea < fa ){
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
			fi = f[fptr];
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
		if (fptr < nf ){
			fi = f[fptr];
			fa = std::abs(fi);
		}
	}
	auto x = a + b;
	auto bv = x - a;
	auto y = b - bv;
	auto q0 = y;
	auto q1 = x;
	double  _x, _bv, _av, _br, _ar ;
	while (eptr < ne && fptr < nf) {
		if( ea < fa) {
			a = ei;
			eptr += 1;
			if (eptr < ne ){
				ei = e[eptr];
				ea = std::abs(ei);
			}
		} else {
			a = fi;
			fptr += 1;
			if (fptr < nf) {
				fi = f[fptr];
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
	while( eptr < ne ){
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
		if( y != 0) {
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
		if (fptr < nf ){
			fi = f[fptr];
		}
	}
	if (q0 != 0) {
		g[count] = q0;
		count += 1;
	}
	if (q1 != 0 ){
		g[count] = q1;
		count += 1;
	}
	if (count == 0 ){
		g[count] = 0.0;
		count += 1;
	}

	return g(g.begin(),g.begin()+count);
}

//scalar sum{ easy case{ add two scalars
std::vector<double> scalarScalar(double a, double b )  {
	auto x  = a + b;
	auto bv = x - a;
	auto av = x - bv;
	auto br = b - bv;
	auto ar = a - av;
	auto y  = ar + br;
	if (y != 0) {
		return {y, x};
	}
	return {x};
}

#endif //SUM_SUM_H
