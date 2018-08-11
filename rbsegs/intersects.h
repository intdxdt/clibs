//
// Created on 11/08/18.
//
#include <vector>
#include "util.h"
#include "../mbr/mbr.h"
#include "../mutil/mutil.h"

#ifndef RBSEGS_INTERSECTS_H
#define RBSEGS_INTERSECTS_H

//envelope of segment
inline MBR bbox(const std::vector<double>& a, const std::vector<double>& b ) {
	return  MBR(a[x], a[y], b[x], b[y]);
}
//clamp to zero if float is near zero
inline double snap_to_zero( double v) {
	return feq(v, 0.0) ? 0.0 : v;
}

//clamp to zero or one
inline double snap_to_zero_or_one(double v){
	return feq(v, 0.0) ? 0.0 : feq(v, 1.0)? 1.0 : v ;
}
std::tuple<double, double,double> segseg_abd(
		const std::vector<double>& sa, const std::vector<double>& sb,
		const std::vector<double>&oa, const std::vector<double>&ob
		)  {
	double x1, y1, x2, y2, x3, y3, x4, y4, d, a, b ;

	x1 = sa[x], y1 = sa[y];
	x2 = sb[x], y2 = sb[y];

	x3= oa[x],  y3 =oa[y];
	x4= ob[x],  y4 =ob[y];

	d = ((y4 - y3) * (x2 - x1)) - ((x4 - x3) * (y2 - y1));
	a = ((x4 - x3) * (y1 - y3)) - ((y4 - y3) * (x1 - x3));
	b = ((x2 - x1) * (y1 - y3)) - ((y2 - y1) * (x1 - x3));

	return std::tuple<double, double,double>(a, b, d);
}

//do two lines intersect line segments a && b with
//vertices sa, sb, oa, ob
bool intersects(
		const std::vector<double>& sa, const std::vector<double>& sb,
		const std::vector<double>&oa, const std::vector<double>& ob ) {
	auto  bln = false;
	auto  abd = segseg_abd(sa, sb, oa, ob);
	auto a = std::get<0>(abd);
	auto b = std::get<1>(abd);
	auto d = std::get<2>(abd);

	//snap to zero if near -0 or 0
	a = snap_to_zero(a);
	b = snap_to_zero(b);
	d = snap_to_zero(d);

	if (d == 0.0) {
		if (a == 0.0 && b == 0.0) {
			bln = bbox(sa, sb).intersects(bbox(oa, ob));
		}
		return bln;
	}
	//intersection along the the seg or extended seg
	auto ua = snap_to_zero_or_one(a / d);
	auto ub = snap_to_zero_or_one(b / d);
	return (0 <= ua && ua <= 1) && (0 <= ub && ub <= 1);
}
#endif //RBSEGS_INTERSECTS_H
