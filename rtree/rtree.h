/**
 (c) 2015, Titus Tienaah;
 A library for 2D spatial indexing of points and rectangles.;
 https://github.com/mourner/rbush;
 @after  (c) 2015, Vladimir Agafonkin;
*/
#include <cmath>
#include <memory>
#include "node.h"
#include "clear.h"

namespace rtree {



    //RTree type
    struct RTree {
        int maxEntries;
        int minEntries;
        std::shared_ptr<Node> Data;

        RTree(int cap) {
            this.Clear();

            if (cap <= 0) {
                cap = 9;
            }
            // max entries in a node is 9 by default min node fill is 40% for best performance
            maxEntries = std::max(4, cap);
            minEntries = std::min(2, int(std::ceil(cap * 0.4)));
        }

        RTree& Clear() {
            auto node = Node(&universe, 1, true, std::vector<std::shared_ptr<Node>>{});
            Data = std::make_shared<Node>(node);
        }
    };

//
//     const []Node& emptyChildren(int length) const {
//        return make([]*Node, length);
//    }
//

//
//    //IsEmpty checks for empty tree
//     (tree *RTree) IsEmpty() bool {
//        return len(tree.Data.children) == 0;
//    }

}