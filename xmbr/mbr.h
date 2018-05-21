#ifndef MBR_MBR_H
#define MBR_MBR_H

#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

#define EPSILON  1.0e-12

double feq(double a, double b){
    return a == b || fabs(a - b) < EPSILON;
}
class MBR {

};


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



//completely_contains_xy is true if MBR
//completely contains location with {x, y}

//Contains bonding box


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
