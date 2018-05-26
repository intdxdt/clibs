/**
 (c) 2015, Titus Tienaah;
 A library for 2D spatial indexing of points and rectangles.;
 https://github.com/mourner/rbush;
 @after  (c) 2015, Vladimir Agafonkin;
*/
#include <cmath>
#include <memory>
#include "node.h"

namespace rtree {

    //RTree type
    struct RTree {
        int maxEntries = 9;
        int minEntries = 4;
        std::shared_ptr<Node> Data = nullptr;

        RTree &Clear() {
            Universe universe;
            std::vector<std::shared_ptr<Node>> ch{};
            auto node = NewNode(&universe, 1, true, ch);
            Data = std::move(node);
            return *this;
        }

        //IsEmpty checks for empty tree
        bool IsEmpty() {
            return this->Data.get()->children.empty();
        }

    };

    RTree NewRTree(int cap) {
        RTree tree;
        tree.Clear();

        if (cap <= 0) {
            cap = 9;
        }
        // max entries in a node is 9 by default min node fill is 40% for best performance
        tree.maxEntries = std::max(4, cap);
        tree.minEntries = std::min(2, int(std::ceil(cap * 0.4)));
        return std::move(tree);
    }


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