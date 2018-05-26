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

        RTree& Clear() {
            std::vector<std::shared_ptr<Node>> ch{};
            auto node = NewNode(nullptr, 1, true, std::move(ch));
            Data = std::move(node);
            return *this;
        }

        //IsEmpty checks for empty tree
        bool IsEmpty() {
            return Data.get()->children.empty();
        }


        //allDistMargin computes total margin of all possible split distributions.
        //Each node is at least m full.
        double allDistMargin(std::shared_ptr<Node>& node, std::size_t m, std::size_t M, SortBy sortBy) const {
            if (sortBy == ByX) {
                std::sort(node->children.begin(), node->children.end(), XNodePath());
                //bubbleAxis(*node.getChildren(), ByX, ByY)
            }
            else if (sortBy == ByY) {
                std::sort(node->children.begin(), node->children.end(), YNodePath());
                //bubbleAxis(*node.getChildren(), ByY, ByX)
            }

            std::size_t i;

            auto leftBBox = distBBox(node, 0, m);
            auto rightBBox = distBBox(node, M - m, M);
            auto margin = bboxMargin(leftBBox) + bboxMargin(rightBBox);

            for (i = m; i < M - m; i++) {
                auto child = node->children[i];
                extend(leftBBox, child->bbox);
                margin += bboxMargin(leftBBox);
            }

            for (i = M - m - 1; i >= m; i--) {
                auto child = node->children[i];
                extend(rightBBox, child->bbox);
                margin += bboxMargin(rightBBox);
            }
            return margin;
        }

        //_chooseSplitAxis selects split axis : sorts node children
        //by the best axis for split.
        void chooseSplitAxis(std::shared_ptr<Node>& node, std::size_t m, std::size_t M) {
            auto xMargin = allDistMargin(node, m, M, ByX);
            auto yMargin = allDistMargin(node, m, M, ByY);

            // if total distributions margin value is minimal for x, sort by minX,
            // otherwise it's already sorted by minY
            if (xMargin < yMargin) {
                std::sort(node->children.begin(), node->children.end(), XNodePath());
            }
        }

        //_chooseSplitIndex selects split index.
        std::size_t chooseSplitIndex(std::shared_ptr<Node>& node, std::size_t m, std::size_t M) const {
            std::size_t i = 0, index = 0;
            double overlap, area, minOverlap, minArea;

            minOverlap = std::numeric_limits<double>::infinity();
            minArea = std::numeric_limits<double>::infinity();

            for (i = m; i <= M - m; i++) {
                auto bbox1 = distBBox(node, 0, i);
                auto bbox2 = distBBox(node, i, M);

                overlap = intersectionArea(bbox1, bbox2);
                area = bboxArea(bbox1) + bboxArea(bbox2);

                // choose distribution with minimum overlap
                if (overlap < minOverlap) {
                    minOverlap = overlap;
                    index = i;

                    if (area < minArea) {
                        minArea = area;
                    }

                }
                else if (overlap == minOverlap) {
                    // otherwise choose distribution with minimum area
                    if (area < minArea) {
                        minArea = area;
                        index = i;
                    }
                }
            }

            return index;
        }

        //_splitRoot splits the root of tree.
        void splitRoot(std::shared_ptr<Node>& node, std::shared_ptr<Node>& newNode) {
            // split root node
            auto path = std::vector<std::shared_ptr<Node>>{node, newNode};
            auto root = NewNode(nullptr, node->height + 1, false, std::move(path));
            Data = root;
            calcBBox(Data);
        }


        //_split overflowed node into two
        void split(std::vector<std::shared_ptr<Node>>& insertPath, int level) {
            auto node = insertPath[level];
            auto newNode = NewNode(
                    nullptr,
                    node->height,
                    node->leaf,
                    emptyChildren(0)
            );

            auto M = node->children.size();
            auto m = minEntries;

            chooseSplitAxis(node, m, M);
            auto at = chooseSplitIndex(node, m, M);
            //perform split at index
            newNode->children = splitAtIndex(node->children, at);

            calcBBox(node);
            calcBBox(newNode);

            if (level > 0) {
                insertPath[level - 1]->addChild(newNode);
            }
            else {
                splitRoot(node, newNode);
            }
        }

        // split on node overflow propagate upwards if necessary
        void split_on_overflow(int level, std::vector<std::shared_ptr<Node>>& insertPath) {
            while (level >= 0) {
                //fmt.Printf("size of insert path: %v\n", insertPath[level].Size())
                if (insertPath[level]->children.size() > maxEntries) {
                    split(insertPath, level);
                    level--;
                }
                else {
                    break;
                }
            }
        }

        //insert - private
        void insert(Object *item, int level) {
            auto bbox = MBR{item->bounds()};
            std::vector<std::shared_ptr<Node>> insertPath{};

            // find the best node for accommodating the item, saving all nodes along the path too
            auto node = chooseSubtree(bbox, Data, level, insertPath);


            //put the item into the node item_bbox
            node->addChild(newLeafNode(item));
            extend(node->bbox, bbox);

            // split on node overflow propagate upwards if necessary
            split_on_overflow(level, insertPath);

            // adjust bboxes along the insertion path
            adjustParentBBoxes(bbox, insertPath, level);
        }

        //insert - private
        void insert(std::shared_ptr<Node>& item, int level) {
            auto bbox = item->bbox;
            std::vector<std::shared_ptr<Node>> insertPath{};

            // find the best node for accommodating the item, saving all nodes along the path too
            auto node = chooseSubtree(bbox, Data, level, insertPath);

            node->children.emplace_back(item);
            extend(node->bbox, bbox);

            // split on node overflow propagate upwards if necessary
            split_on_overflow(level, insertPath);

            //adjust bboxes along the insertion path
            adjustParentBBoxes(bbox, insertPath, level);
        }

        //Insert item
        RTree& Insert(Object *item) {
            if (item == nullptr) {
                return *this;
            }
            insert(item, (*this).Data->height - 1);
            return *this;
        }

        //Load implements bulk loading
        RTree& Load(std::vector<Object *>& objects) {
            if (objects.empty()) {
                return *this;
            }

            if (objects.size() < minEntries) {
                for (auto o : objects) {
                    Insert(o);
                }
                return *this;
            }

            std::vector<Object *> data(objects.begin(), objects.end());

            // recursively build the tree with the given data from stratch using OMT algorithm
            auto node = _build(data, 0, data.size() - 1, 0);

            if (Data->children.empty()) {
                // save as is if tree is empty
                Data = std::move(node);
            }
            else if (Data->height == node.height) {
                // split root if trees have the same height
                splitRoot(Data, node);
            }
            else {
                if (Data->height < node.height) {
                    // swap trees if inserted one is bigger
                    std::swap(Data, node);
                }

                // insert the small tree into the large tree at appropriate level
                insert(node, Data->height - node.height - 1);
            }

            return *this;
        }

        //LoadBoxes loads bounding boxes
        RTree& LoadBoxes(std::vector<MBR>& data) {
            std::vector<Object *> items(data.begin(), data.end());
            return Load(items);
        }
    };

    RTree NewRTree(int cap) {
        RTree tree;
        tree.Clear();

        if (cap <= 0) {
            cap = 9;
        }
        // max entries in a node is 9 by default min node fill is 40% for best performance
        tree.maxEntries = (size_t) std::max(4, cap);
        tree.minEntries = (size_t) std::min(2, int(std::ceil(cap * 0.4)));
        return std::move(tree);
    }


}