//
//06/06/18.
//
#include "../mbr/mbr.h"
#include "util.h"

#ifndef GEOM_LINE_MONO_H
#define GEOM_LINE_MONO_H
namespace geom {
    struct MonoMBR {
        MBR mbr{inf, inf, neginf, neginf, true};
        int i{null};
        int j{null};

        ///mono mbr
        MonoMBR() = default;

        ///mono mbr
        explicit MonoMBR(MBR box) : mbr(box){};

        ///mono mbr
        MonoMBR(MBR box, int i, int j) : mbr(box), i(i), j(j) {};

        ///clone  mono mbr
        MonoMBR clone(const MonoMBR& box) {
            return MonoMBR{box.mbr, box.i, box.j};
        }

        ///update mono chain index
        void update_index(int i, int j) {
            this->i = i;
            this->j = j;
        }
    };

}
#endif //GEOM_LINE_MONO_H
