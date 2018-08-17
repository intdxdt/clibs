#define CATCH_CONFIG_MAIN

#include <iostream>
#include <cmath>
#include "../catch/catch.h"
#include "mbr.h"

using namespace mbr;


TEST_CASE("mbr 1", "[mbr 1]") {
    SECTION("construction") {
        MBR m0 = {0, 0, 0.5, 0.2};
        MBR m1 = {2, 2, -0.5, -0.2};
        MBR m = m0 + m1;
        REQUIRE(m.minx == -0.5);
        REQUIRE(m.miny == -0.2);
        REQUIRE(m.maxx == 2);
        REQUIRE(m.maxy == 2);

        m1 = MBR{2, 2, -0.5, -0.2, true};
        REQUIRE(m1.minx == 2);
        REQUIRE(m1.miny == 2);
        REQUIRE(m1.maxx == -0.5);
        REQUIRE(m1.maxy == -0.2);

        m = MBR{-0.5, -0.2, 2, 2, true};
        m1 = MBR{2, 2, -0.5, -0.2, false};
        REQUIRE(m == m1);


        m = {2, 2, 0.5, 0.2};
        REQUIRE(m.minx == 0.5);
        REQUIRE(m.miny == 0.2);
        REQUIRE(m.maxx == 2.0);
        REQUIRE(m.maxy == 2.0);

        std::array<double, 4> bounds = {2, 2, 0.5, 0.2};
        auto marr = MBR(bounds);
        REQUIRE(m.equals(marr));
        marr = MBR(bounds, false);
        REQUIRE(m.equals(marr));
        marr = MBR(bounds, true);

        REQUIRE(marr.minx == 2);
        REQUIRE(marr.miny == 2);
        REQUIRE(marr.maxx == 0.5);
        REQUIRE(marr.maxy == 0.2);
    }

    SECTION("methods") {
        auto m = MBR{2, 2, 0.5, 0.2};
        REQUIRE(m.height() == 1.8);
        REQUIRE(m.width() == 1.5);
        REQUIRE(m.area() == 1.5 * 1.8);
        REQUIRE(!m.is_point());
        std::tuple<double, double, double, double> tup = {0.5, 0.2, 2.0, 2.0};
        REQUIRE(m.as_tuple() == tup);

        std::vector<std::vector<double>> b = m.as_poly_array();
        std::vector<double> b0 = {0.5, 0.2};
        std::vector<double> b1 = {0.5, 2};
        std::vector<double> b2 = {2, 2};
        std::vector<double> b3 = {2, 0.2};
        std::vector<double> b4 = {0.5, 0.2};

        REQUIRE(b.size() == 5);
        REQUIRE(b[0] == b0);
        REQUIRE(b[1] == b1);
        REQUIRE(b[2] == b2);
        REQUIRE(b[3] == b3);
        REQUIRE(b[4] == b4);
        REQUIRE(b[0] == b[4]);

        auto m1 = m;
        auto m2 = m.clone();
        REQUIRE(m1.area() == m.area());
        REQUIRE(m1.equals(m));
        REQUIRE(m1 == m);

        REQUIRE(m2.area() == m.area());
        REQUIRE(m2.equals(m));
        REQUIRE(m2 == m);
        m1.minx = -1;
        REQUIRE_FALSE(m2 == m1);
    }
}

TEST_CASE("mbr 2", "[mbr 2]") {
    MBR m00{0, 0, 0, 0};
    m00.expand_to_include(2, 2);

    MBR n00{0, 0, 0, 0};
    n00.expand_to_include(-2, -2);

    auto m0 = MBR(1, 1, 1, 1);
    m0.expand_by_delta(1, 1);

    MBR m1{0, 0, 2, 2};
    MBR m2{4, 5, 8, 9};
    MBR m3{1.7, 1.5, 5, 9};
    MBR m4{5, 0, 8, 2};
    MBR m5{5, 11, 8, 9};
    MBR m6{0, 0, 2, -2};
    MBR m7{-2, 1, 4, -2};
    MBR m8{-1, 0, 1, -1.5};

    std::vector<double> p = {1.7, 1.5, 3.4};// POINT(1.7 1.5, 3.4)
    std::vector<double> p0 = {1.7};         // POINT(1.7 1.5)

    MBR m0123{0, 2, 1, 3};
    auto clone_m0123 = m0123;

    const int x1 = 0;
    const int y1 = 1;


    SECTION("equals") {
        std::array<double, 4> r1{{0, 0, 2, 2}};
        REQUIRE(m1.as_array() == r1);
        REQUIRE(clone_m0123 == m0123);
        REQUIRE(m0.equals(m1));
        REQUIRE(m0.bbox() == m0);
        REQUIRE(m00.equals(m1));
        REQUIRE(m1 != m2);
    }

    SECTION("intersects , distance") {
        REQUIRE(m1.intersects(p[0], p[1]));
        REQUIRE(m1.intersects(p0[0], 0));

        REQUIRE(m00.intersects(n00));
        auto nm00 = m00.intersection(n00);
        REQUIRE(nm00 != std::nullopt);

        auto bln1 = (nm00.value().minx == 0.0 && nm00.value().miny == 0.0);
        auto bln2 = (nm00.value().maxx == 0.0 && nm00.value().maxy == 0.0);
        REQUIRE(bln1);
        REQUIRE(bln2);
        REQUIRE(nm00.value().is_point());

        REQUIRE(!m1.intersects(m2));
        auto null_mbr = m1.intersection(m2);
        REQUIRE(!null_mbr.has_value());
        REQUIRE(m1.intersects(m3));
        REQUIRE(m2.intersects(m3));

        auto m13 = m1.intersection(m3);
        auto m23 = m2.intersection(m3);
        std::array<double, 4> _m13{1.7, 1.5, 2, 2};
        std::array<double, 4> _m23{4, 5, 5, 9};

        REQUIRE(_m13 == m13.value().as_array());
        REQUIRE(_m23 == m23.value().as_array());

        REQUIRE(m3.intersects(m4));
        REQUIRE(m2.intersects(m5));
        REQUIRE(m7.intersects(m6));
        REQUIRE(m6.intersects(m7));

        auto m67 = m6.intersection(m7);
        auto m76 = m7.intersection(m6);
        auto m78 = m7.intersection(m8);

        REQUIRE(m67.value().equals(m6));
        REQUIRE(m67.value().equals(m76.value()));
        REQUIRE(m78.value().equals(m8));

        auto m25 = m2.intersection(m5);
        auto m34 = m3.intersection(m4);

        REQUIRE(m25.value().width() == m5.width());
        REQUIRE(m25.value().height() == 0.0);
        REQUIRE(m34.value().width() == 0.0);
        REQUIRE(m34.value().height() == 0.5);
        REQUIRE(m3.distance(m4) == 0.0);

        auto d = std::hypot(2, 3);
        REQUIRE(m1.distance(m2) == d);
        REQUIRE(m1.distance_square(m2) == round(d * d, 12));
        REQUIRE(m1.distance(m3) == 0.0);
        REQUIRE(m1.distance_square(m3) == 0.0);

        MBR a{
                -7.703505430214746, 3.0022503796012305,
                -5.369812194018422, 5.231449888803689
        };
        REQUIRE(m1.distance(a) == std::hypot(-5.369812194018422, 3.0022503796012305 - 2));

        MBR b{
                -4.742849832055231, -4.1033230559816065,
                -1.9563504455521576, -2.292098454754609
        };
        REQUIRE(m1.distance(b) == std::hypot(-1.9563504455521576, -2.292098454754609));

        MBR aa{
                56.375782597898, 56.375782597898,
                123.375782597898, 823.375782597898
        };
        std::cout << "-----------------------------------------------------------------------------------" << std::endl;
        std::cout << aa.wkt() << std::endl;
        std::cout << b.wkt() << std::endl;
        std::cout << "-----------------------------------------------------------------------------------" << std::endl;

    }

    SECTION("contains, disjoint , contains completely") {
        Pt p1{-5.95, 9.28};
        Pt p2{-0.11, 12.56};
        Pt p3{3.58, 11.79};
        Pt p4{-1.16, 14.71};

        MBR mp12 = {p1.x, p1.y, p2.x, p2.y};
        MBR mp34 = {p3.x, p3.y, p4.x, p4.y};

        // intersects but segment are disjoint
        REQUIRE(mp12.intersects(mp34));
        REQUIRE(mp12.intersects(p3, p4));
        REQUIRE(!mp12.intersects(Pt{m1.minx, m1.miny}, Pt{m1.maxx, m1.maxy}));
        REQUIRE(!mp12.intersects(p3.x, p3.y));
        REQUIRE(m1.contains(1, 1));

        MBR mbr11{1, 1, 1.5, 1.5};
        MBR mbr12{1, 1, 2, 2};
        MBR mbr13{1, 1, 2.000045, 2.00001};
        MBR mbr14{2.000045, 2.00001, 4.000045, 4.00001};

        REQUIRE(m1.contains(mbr11));
        REQUIRE(m1.contains(mbr12));
        REQUIRE(!m1.contains(mbr13));
        REQUIRE(!m1.disjoint(mbr13));// False
        REQUIRE(m1.disjoint(mbr14)); // True disjoint

        REQUIRE(m1.contains(1.5, 1.5));
        REQUIRE(m1.contains(2, 2));

        REQUIRE(m1.completely_contains(mbr11));
        REQUIRE(m1.completely_contains(1.5, 1.5));
        REQUIRE(m1.completely_contains(1.5, 1.5));
        REQUIRE(!m1.completely_contains(2, 2));
        REQUIRE(!m1.completely_contains(mbr12));
        REQUIRE(!m1.completely_contains(mbr13));
    }

    SECTION("translate, expand by, area") {
        MBR ma = {0, 0, 2, 2};
        MBR mb = {-1, -1, 1.5, 1.9};
        MBR mc = {1.7, 1.5, 5, 9};
        MBR md = ma;
        ma.expand_to_include(mc);
        md.expand_to_include(mb);

        std::array<double, 4> arr{0, 0, 5, 9};
        std::vector<std::vector<double>> polyarr{{0, 0},
                                                 {0, 9},
                                                 {5, 9},
                                                 {5, 0},
                                                 {0, 0}};
        REQUIRE(ma.as_array() == arr);  //ma modified by expand
        REQUIRE(ma.as_poly_array() == polyarr); //ma modified by expand
        arr = {1.7, 1.5, 5, 9};
        REQUIRE(mc.as_array() == arr); //should not be touched
        arr = {-1, -1, 2, 2};
        REQUIRE(md.as_array() == arr);//ma modified by expand

        //mc area
        REQUIRE(mc.area() == 24.75);

        auto mt = m1.translate(1, 1);
        auto mby = m1;
        mby.expand_by_delta(-3, -3);

        auto m1c = m1.center();
        auto mtc = mt.center();

        auto pt = Pt{1, 1};
        REQUIRE(m1c == pt);
        pt = Pt{2, 2};
        REQUIRE(mtc == pt);
        arr = {{1, 1, 3, 3}};
        REQUIRE(mt.as_array() == arr);
        arr = {{-1, -1, 3, 3}};
        REQUIRE(mby.as_array() == arr);
    }

    SECTION("wkt string") {
        REQUIRE(m1.wkt() == "POLYGON ((0 0, 0 2, 2 2, 2 0, 0 0))");
    }
}