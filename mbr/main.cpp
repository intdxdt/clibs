#define CATCH_CONFIG_MAIN

#include <iostream>
#include "../catch/catch.h"
#include "mbr.h"

TEST_CASE("mbr", "[mbr]") {
    SECTION("construction") {
        mbr::MBR m0 = {0, 0, 0.5, 0.2};
        mbr::MBR m1 = {2, 2, -0.5, -0.2};
        mbr::MBR m = m0 + m1;
        REQUIRE(m.minx == -0.5);
        REQUIRE(m.miny == -0.2);
        REQUIRE(m.maxx == 2);
        REQUIRE(m.maxy == 2);

        m1 = mbr::MBR{2, 2, -0.5, -0.2, true};
        REQUIRE(m1.minx == 2);
        REQUIRE(m1.miny == 2);
        REQUIRE(m1.maxx == -0.5);
        REQUIRE(m1.maxy == -0.2);

        m = mbr::MBR{-0.5, -0.2, 2, 2, true};
        m1 = mbr::MBR{2, 2, -0.5, -0.2, false};
        REQUIRE(m == m1);


        m = {2, 2, 0.5, 0.2};
        REQUIRE(m.minx == 0.5);
        REQUIRE(m.miny == 0.2);
        REQUIRE(m.maxx == 2.0);
        REQUIRE(m.maxy == 2.0);
    }

    SECTION("methods") {
        mbr::MBR m = {2, 2, 0.5, 0.2};
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
        REQUIRE(m1.area() == m.area());
        REQUIRE(m1.equals(m));
        REQUIRE(m1 == m);


    }
}
