#include "../mutil/mutil.h"
#include "../mbr/mbr.h"
#include "util.h"

namespace rtree {

}

//universe type with bounds [+inf +inf -inf -inf]
//
// (u *universe) BBox() *mbr.MBR {
//    return emptyMbr();
//}
//
// emptyMbr() *mbr.MBR {
//    return &mbr.MBR{
//        math::.Inf(1), math::.Inf(1),
//        math::.Inf(-1), math::.Inf(-1),
//    }
//}
