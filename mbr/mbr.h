#ifndef MBR_MBR_H
#define MBR_MBR_H

#include "util.h"
/*

    mbr(double x1, double y1, double x2, double y2) :
            minx(std::min(x1, x2)), miny(std::min(y1, y2)),
            maxx(std::max(x1, x2)), maxy(std::max(y1, y2)) {}

    inline double width() {
        return maxx - minx;
    }

    inline double height() {
        return maxy - miny;
    }

    inline double area() {
        return height() * width();
    }

    std::string wkt() {
        std::ostringstream ss;
        ss << "POLYGON (("
           << minx << " " << miny << ","
           << minx << " " << maxy << ","
           << maxx << " " << maxy << ","
           << maxx << " " << miny << ","
           << minx << " " << miny
           << "))";
        return std::move(ss.str());
    }

    std::vector<double> center() {
        return {(minx + maxx) / 2.0, (miny + maxy) / 2.0};
    }

    double operator[](const int index) {
        assert(index >= 0 && index < 4);
        return index == 0 ? minx :
               index == 1 ? miny :
               index == 2 ? maxx :
               index == 3 ? maxy : std::nan("-9");
    }
};

*/
typedef struct {
    double minx, miny;
    double maxx, maxy;
} MBR;

MBR new_mbr(double minx, double miny, double maxx, double maxy) {
    return (MBR) {
            fmin(minx, maxx), fmin(miny, maxy),
            fmax(minx, maxx), fmax(miny, maxy),
    };
}

MBR new_mbr_raw(double minx, double miny, double maxx, double maxy) {
    return (MBR) {minx, miny, maxx, maxy};
}

//Width of bounding box.
double mbr_width(MBR *self) {
    return self->maxx - self->minx;
}

//Height of bounding box.
double mbr_height(MBR *self) {
    return self->maxy - self->miny;
}

//Area of bounding box.
double mbr_area(MBR *self) {
    return mbr_height(self) * mbr_width(self);
}

//Bounding box as a closed polygon array.
void mbr_as_poly_array(MBR *self, double poly[5][2]) {
    poly[0][0] = self->minx;
    poly[0][1] = self->miny;
    poly[1][0] = self->minx;
    poly[1][1] = self->maxy;
    poly[2][0] = self->maxx;
    poly[2][1] = self->maxy;
    poly[3][0] = self->maxx;
    poly[3][1] = self->miny;
    poly[4][0] = self->minx;
    poly[4][1] = self->miny;
}

//Lower left and upper right corners as a array double[minx, miny, maxx, maxy]
void mbr_as_array(MBR *self, double arr[4]) {
    arr[0] = self->minx;
    arr[1] = self->miny;
    arr[2] = self->maxx;
    arr[3] = self->maxy;
}

//lower left and upper right as tuple ((minx,miny),(maxx,maxy))
void mbr_llur(MBR *self, double ll[2], double ur[2]) {
    ll[0] = self->minx;
    ll[1] = self->miny;
    ur[0] = self->maxx;
    ur[1] = self->maxy;
}

//Compare equality of two bounding boxes
bool mbr_equals(MBR *a, MBR *b) {
    return (
            feq(a->maxx, b->maxx) &&
            feq(a->maxy, b->maxy) &&
            feq(a->minx, b->minx) &&
            feq(a->miny, b->miny));
}


//Checks if bounding box can be represented as a point.
bool mbr_is_point(MBR *self) {
    return (
            feq(mbr_height(self), 0.0) &&
            feq(mbr_width(self), 0.0));
}

//contains x, y
bool mbr_contains_xy(MBR *self, double x, double y) {
    return (
            (x >= self->minx) &&
            (x <= self->maxx) &&
            (y >= self->miny) &&
            (y <= self->maxy));
}


//completely_contains_xy is true if MBR
//completely contains location with {x, y}
//without touching boundaries
bool mbr_completely_contains_xy(MBR *self, double x, double y) {
    return (
            (x > self->minx) &&
            (x < self->maxx) &&
            (y > self->miny) &&
            (y < self->maxy));
}

//Contains bonding box
//is true if MBR completely contains other, boundaries may touch
bool mbr_contains(MBR *self, MBR *other) {
    return (
            (other->minx >= self->minx) &&
            (other->miny >= self->miny) &&
            (other->maxx <= self->maxx) &&
            (other->maxy <= self->maxy));
}
//Completely contains bonding box
//is true if MBR completely contains other without touching boundaries
bool mbr_completely_contains(MBR *self, MBR *other) {
    return (
            (other->minx > self->minx) &&
            (other->miny > self->miny) &&
            (other->maxx < self->maxx) &&
            (other->maxy < self->maxy));
}

//Translate bounding box by change in dx and dy.
MBR mbr_translate(MBR *self, double dx, double dy) {
    return new_mbr(
            self->minx + dx, self->miny + dy,
            self->maxx + dx, self->maxy + dy);
}

//Computes the center of minimum bounding box - (x, y)
void mbr_center(MBR *self, double ctr[2]) {
    ctr[0] = (self->minx + self->maxx) / 2.0;
    ctr[1] = (self->miny + self->maxy) / 2.0;
}

//Checks if bounding box intersects other
bool mbr_intersects(MBR *self, MBR *other) {
    //not disjoint
    return !(
            other->minx > self->maxx ||
            other->maxx < self->minx ||
            other->miny > self->maxy ||
            other->maxy < self->miny);
}

//intersects point
bool mbr_intersects_xy(MBR *self, double x, double y) {
    return mbr_contains_xy(self, x, y);
}

//Computes the intersection of two bounding box
MBR mbr_intersection(MBR *self, MBR *other) {
    assert (mbr_intersects(self, other));

    double minx = self->minx > other->minx ? self->minx : other->minx;
    double miny = self->miny > other->miny ? self->miny : other->miny;
    double maxx = self->maxx < other->maxx ? self->maxx : other->maxx;
    double maxy = self->maxy < other->maxy ? self->maxy : other->maxy;

    return new_mbr(minx, miny, maxx, maxy);
}

//Test for disjoint between two mbrs
bool mbr_disjoint(MBR *self, MBR *other) {
    return !mbr_intersects(self, other);
}

int mbr_wkt(MBR *self, char *buffer) {
    double lx = self->minx;
    double ly = self->miny;
    double ux = self->maxx;
    double uy = self->maxy;
    return sprintf(buffer, "POLYGON ((%f %f ,%f  %f ,%f  %f ,%f  %f ,%f  %f))",
                   lx, ly, lx, uy, ux, uy, ux, ly, lx, ly);
}


//computes dx and dy for computing hypot
MBR *mbr_distance_dxdy(MBR *self, MBR *other, double *dx, double *dy) {
    // find closest edge by x
    *dx = (self->maxx < other->minx) ?
          other->minx - self->maxx :
          (self->minx > other->maxx) ?
          self->minx - other->maxx : 0;
    // find closest edge by y
    *dy = (self->maxy < other->miny) ?
          other->miny - self->maxy :
          (self->miny > other->maxy) ?
          self->miny - other->maxy : 0;

}

//distance computes the distance between two mbrs
double mbr_distance(MBR *self, MBR *other) {
    if (mbr_intersects(self, other)) {
        return 0;
    }
    double dx = 0, dy = 0;
    mbr_distance_dxdy(self, other, &dx, &dy);
    hypot(dx, dy);
}

//distance square computes the squared distance
//between bounding boxes
double mbr_distance_square(MBR *self, MBR *other) {
    if (mbr_intersects(self, other)) {
        return 0;
    }
    double dx = 0, dy = 0;
    mbr_distance_dxdy(self, other, &dx, &dy);
    return (dx * dx) + (dy * dy); //Note this can overflow
}


//Expand by delta in x and y
MBR mbr_union(MBR *self, MBR *other) {
    return (MBR) {
        fmin(self->minx, other->minx),
        fmin(self->miny, other->miny),
        fmax(self->maxx, other->maxx),
        fmax(self->maxy, other->maxy),
    };
}


//Expand include other bounding box
MBR *mbr_expand_to_include_mbr(MBR *self, MBR *other) {
    (other->minx < self->minx) ? self->minx = other->minx : 0;
    (other->maxx > self->maxx) ? self->maxx = other->maxx : 0;

    (other->miny < self->miny) ? self->miny = other->miny : 0;
    (other->maxy > self->maxy) ? self->maxy = other->maxy : 0;
    return self;
}


//Expand by delta in x and y
MBR *mbr_expand_by_delta(MBR *self, double dx, double dy) {
    double minx = self->minx - dx;
    double miny = self->miny - dy;
    double maxx = self->maxx + dx;
    double maxy = self->maxy + dy;

    self->minx = fmin(minx, maxx);
    self->miny = fmin(miny, maxy);
    self->maxx = fmax(minx, maxx);
    self->maxy = fmax(miny, maxy);

    return self;
}


//Expand to include x,y
MBR *mbr_expand_include_xy(MBR *self, double x, double y) {
    (x < self->minx) ? self->minx = x :
    (x > self->maxx) ? self->maxx = x : 0;

    (y < self->miny) ? self->miny = y :
    (y > self->maxy) ? self->maxy = y : 0;
    return self;
}

#endif //MBR_MBR_H
