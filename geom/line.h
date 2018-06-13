//
//06/06/18.
//
#include "line_mono.h"
#include "point.h"
#include "util.h"
#include "../rtree/rtree.h"

#ifndef GEOM_LINE_H
#define GEOM_LINE_H
namespace geom {
    const size_t bucketSize = 8;

    struct LineString {
        //@formatter:off
        std::vector<MonoMBR> chains{};
        std::vector<Point>   coordinates{};
        int                  monosize{0};
        rtree::RTree         index;
        MonoMBR              bbox;
        double               length{0};
        //@formatter:on

        explicit LineString(std::vector<Point> coordinates) :
                coordinates(std::move(coordinates)),
                monosize(int(std::log2(coordinates.size() + 1.0))),
                bbox(rtree::empty_mbr()),
                index(rtree::NewRTree(bucketSize)) {
            int n = static_cast<int>(coordinates.size());
            assert(n > 2);
            process_chains(0, n - 1);
            build_index();
        }

        ///build xymonotone chain, perimeter length,
        ///monotone build starts from i and ends at j, designed for
        ///appending new points to the end of line
        LineString& process_chains(int i, int j) {
            double dx, dy;
            Point v0{0, 0};
            Point v1{0, 0};
            int cur_x, cur_y, prev_x, prev_y;
            MonoMBR mono;
            auto mono_limit = monosize;

            prev_x = null;
            prev_y = null;

            if (j == 0 || j >= (coordinates.size())) {
                j = int(coordinates.size() - 1);
            }

            v0 = coordinates[i];
            auto box = MBR{v0.x, v0.y, v0.x, v0.y};

            this->bbox = MonoMBR(box);
            mono = MonoMBR(box);
            xy_monobox(mono, i, i);
            chains.emplace_back(mono);
            auto m_index = chains.size() - 1;


            auto mono_size = 0;
            for (i = i + 1; i <= j; i += 1) {
                v0 = coordinates[i - 1];
                v1 = coordinates[i];
                dx = v1.x - v0.x;
                dy = v1.y - v0.y;

                this->length += std::hypot(dx, dy);

                cur_x = xy_sign(dx);
                cur_y = xy_sign(dy);

                if (prev_x == null) {
                    prev_x = cur_x;
                }

                if (prev_y == null) {
                    prev_y = cur_y;
                }

                //((cur_x + prev_x > 0) && (prev_y + cur_y > 0))
                mono_size += 1;
                if (prev_x == cur_x && prev_y == cur_y && mono_size <= mono_limit) {
                    xy_monobox(chains[m_index], i, null);
                }
                else {
                    mono_size = 1;
                    prev_x = cur_x;
                    prev_y = cur_y;

                    auto p0 = coordinates[i - 1], p1 = coordinates[i];
                    mono = MonoMBR(MBR(p0.x, p0.y, p1.x, p1.y));
                    xy_monobox(mono, i - 1, i);
                    chains.emplace_back(mono);
                    m_index = chains.size() - 1;
                }
            }
            return *this;
        }


        ///builds rtree index of chains
        LineString& build_index() {
            if (!index.is_empty()) {
                index.clear();
            }
            std::vector<rtree::Object> data;
            data.reserve(chains.size());

            for (size_t i = 0; i < chains.size(); i++) {
                data.emplace_back() = rtree::Object{i, chains[i].mbr, &chains[i]};
            }
            index.load(data); //bulkload;
            return *this;
        }

        ///compute bbox of x or y mono chain
        void xy_monobox(MonoMBR& mono, int i, int j) {
            if (i != null) {
                mono.mbr.expand_to_include(coordinates[i].x, coordinates[i].y);
                if (j == null) {
                    mono.j = i;
                }
                else {
                    mono.i, mono.j = i, j;
                }

                bbox.mbr.expand_to_include(mono.mbr);
                if (bbox.i == null) {
                    bbox.i, bbox.j = mono.i, mono.j;
                }
                else {
                    if (mono.j > bbox.j) {
                        bbox.j = mono.j;
                    }
                }
            }
        }


    };

}
#endif //GEOM_LINE_H
