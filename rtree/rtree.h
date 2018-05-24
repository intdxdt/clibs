/**
 (c) 2015, Titus Tienaah;
 A library for 2D spatial indexing of points and rectangles.;
 https://github.com/mourner/rbush;
 @after  (c) 2015, Vladimir Agafonkin;
*/
namespace rtree {
//RTree type
    struct RTreestruct   {
        Node Data;
        int maxEntries;
        int minEntries;

        NewRTree(int nodeCapacity){
            this.Clear();

            if (nodeCapacity <= 0) {
                nodeCapacity = 9;
            }
            // max entries in a node is 9 by default min node fill is 40% for best performance
            maxEntries = maxEntries(nodeCapacity);
            minEntries = minEntries(maxEntries);
        }
    }
    

     const []Node& emptyChildren(int length) const {
        return make([]*Node, length);
    }

     (tree *RTree) Clear() *RTree {
        auto node = NewNode(&universe{}, 1, true, make([]*Node, 0));
        tree.Data = node;
        return tree;
    }

    //IsEmpty checks for empty tree
     (tree *RTree) IsEmpty() bool {
        return len(tree.Data.children) == 0;
    }

}