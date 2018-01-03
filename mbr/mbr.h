//
// Created by titus on 27/12/17.
//
#ifndef MBR_MBR_H
#define MBR_MBR_H

#include "util.h"

typedef struct {
    double minx, miny;
    double maxx, maxy;
} mbr;

mbr new_mbr(double minx, double miny, double maxx, double maxy) {
    return (mbr) {
            fmin(minx, maxx), fmin(miny, maxy),
            fmax(minx, maxx), fmax(miny, maxy),
    };
}

mbr new_mbr_raw(double minx, double miny, double maxx, double maxy) {
    return (mbr) {minx, miny, maxx, maxy};
}

//Width of bounding box.
double width(mbr *self) {
    return self->maxx - self->minx;
}

//Height of bounding box.
double height(mbr *self) {
    return self->maxy - self->miny;
}

//Area of bounding box.
double area(mbr *self) {
    return height(self) * height(self);
}

//Bounding box as a closed polygon array.
void as_poly_array(mbr *self, double poly[5][2]) {
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
void as_array(mbr *self, double arr[4]) {
    arr[0] = self->minx;
    arr[1] = self->miny;
    arr[2] = self->maxx;
    arr[3] = self->maxy;
}

//lower left and upper right as tuple ((minx,miny),(maxx,maxy))
void llur(mbr *self, double ll[2], double ur[2]) {
    ll[0] = self->minx;
    ll[1] = self->miny;
    ur[0] = self->maxx;
    ur[1] = self->maxy;
}

//Compare equality of two bounding boxes
bool mbr_equals(mbr *a, mbr *b) {
    return (
            feq(a->maxx, b->maxx) &&
            feq(a->maxy, b->maxy) &&
            feq(a->minx, b->minx) &&
            feq(a->miny, b->miny));
}


//Checks if bounding box can be represented as a point.
bool is_point(mbr *self) {
    return (
            feq(height(self), 0.0) &&
            feq(width(self), 0.0));
}

///contains x, y
bool contains_xy(mbr *self, double x, double y) {
    return (
            (x >= self->minx) &&
            (x <= self->maxx) &&
            (y >= self->miny) &&
            (y <= self->maxy));
}


//completely_contains_xy is true if mbr
//completely contains location with {x, y}
//without touching boundaries
bool completely_contains_xy(mbr *self, double x, double y) {
    return (
            (x > self->minx) &&
            (x < self->maxx) &&
            (y > self->miny) &&
            (y < self->maxy));
}

///Contains bonding box
///is true if mbr completely contains other, boundaries may touch
bool contains(mbr *self, mbr *other) {
    return (
            (other->minx >= self->minx) &&
            (other->miny >= self->miny) &&
            (other->maxx <= self->maxx) &&
            (other->maxy <= self->maxy));
}
///Completely contains bonding box
///is true if mbr completely contains other without touching boundaries
bool completely_contains(mbr *self, mbr *other) {
    return (
            (other->minx > self->minx) &&
            (other->miny > self->miny) &&
            (other->maxx < self->maxx) &&
            (other->maxy < self->maxy));
}

//Translate bounding box by change in dx and dy.
mbr translate(mbr *self, double dx, double dy) {
    return new_mbr(
            self->minx + dx, self->miny + dy,
            self->maxx + dx, self->maxy + dy);
}

//Computes the center of minimum bounding box - (x, y)
void center(mbr *self, double ctr[2]) {
    ctr[0] = (self->minx + self->maxx) / 2.0;
    ctr[1] = (self->miny + self->maxy) / 2.0;
}

///Checks if bounding box intersects other
bool intersects(mbr *self, mbr *other) {
    //not disjoint
    return !(
            other->minx > self->maxx ||
            other->maxx < self->minx ||
            other->miny > self->maxy ||
            other->maxy < self->miny);
}
///intersects point
bool intersects_xy(mbr *self, double x, double y) {
    return contains_xy(self, x, y);
}

///Computes the intersection of two bounding box
mbr intersection(mbr *self, mbr *other) {
    assert (intersects(self, other));

    double minx = self->minx > other->minx ? self->minx : other->minx;
    double miny = self->miny > other->miny ? self->miny : other->miny;
    double maxx = self->maxx < other->maxx ? self->maxx : other->maxx;
    double maxy = self->maxy < other->maxy ? self->maxy : other->maxy;

    return new_mbr(minx, miny, maxx, maxy);
}

//Test for disjoint between two mbrs
bool disjoint(mbr *self, mbr *other) {
    return !intersects(self, other);
}

int wkt(mbr *self, char *buffer) {
    double lx = self->minx;
    double ly = self->miny;
    double ux = self->maxx;
    double uy = self->maxy;
    return sprintf(buffer, "POLYGON ((%f %f ,%f  %f ,%f  %f ,%f  %f ,%f  %f))",
                   lx, ly, lx, uy, ux, uy, ux, ly, lx, ly);
}

#endif //MBR_MBR_H
