/**
 (c) 2015, Titus Tienaah;
 A library for 2D spatial indexing of points and rectangles.;
 https://github.com/mourner/rbush;
 @after  (c) 2015, Vladimir Agafonkin;
*/
#include <limits>
#include <memory>
#include <utility>
#include <tuple>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <functional>

#include "../ref/ref.h"
#include "../mbr/mbr.h"
#include "../mutil/mutil.h"

#include "util.h"
#include "node.h"

#ifndef RTREE_CPP_RTREE_H
#define RTREE_CPP_RTREE_H
//RTree
namespace rtree {

    template<typename T>
    struct RTree {
        size_t maxEntries{9};
        size_t minEntries{4};
        std::unique_ptr<Node<T>> data = nullptr;

        RTree() = default;

        RTree(RTree& other) noexcept :
                maxEntries(other.maxEntries),
                minEntries(other.minEntries) {
            data = std::move(other.data);
        }

        RTree(RTree&& other) noexcept:
                maxEntries(other.maxEntries),
                minEntries(other.minEntries) {
            data = std::move(other.data);
        }


        RTree& operator=(RTree&& other) noexcept {
            maxEntries = other.maxEntries;
            minEntries = other.minEntries;
            data = std::move(other.data);
            return *this;
        }

        ~RTree() = default;

        bool is_empty() {
            return data->children.empty();
        }

        RTree& clear() {
            destruct_node(std::move(data));
            data = NewNode<T>(nullptr, 1, true);
            return *this;
        }

        RTree& insert(T* item) {
            insert(item, this->data->height - 1);
            return *this;
        }


    private:

        // insert - private
        void insert(T* item, size_t level) {
            auto bbox = item->bbox();
            std::vector<Node<T>*> insertPath{};

            // find the best node for accommodating the item, saving all nodes along the path too
            auto node = choose_subtree(bbox, data.get(), level, insertPath);

            // put the item into the node item_bbox
            node->add_child(new_leaf_Node(item));
            extend(node->bbox, bbox);

            // split on node overflow propagate upwards if necessary
            split_on_overflow(level, insertPath);

            // adjust bboxes along the insertion path
            adjust_parent_bboxes(bbox, insertPath, level);
        }

        // insert - private
        void insert(std::unique_ptr<Node<T>>&& item, size_t level) {
            auto bbox = item->bbox;
            std::vector<Node<T>*> insertPath{};

            // find the best node for accommodating the item, saving all nodes along the path too
            auto node = choose_subtree(bbox, data.get(), level, insertPath);

            node->children.emplace_back(std::move(item));
            extend(node->bbox, bbox);

            // split on node overflow propagate upwards if necessary
            split_on_overflow(level, insertPath);

            // adjust bboxes along the insertion path
            adjust_parent_bboxes(bbox, insertPath, level);
        }

        //_splitRoot splits the root of tree.
        void split_root(std::unique_ptr<Node<T>>&& node, std::unique_ptr<Node<T>>&& newNode) {
            // split root node
            auto root_height = node->height + 1;

            std::vector<std::unique_ptr<Node<T>>> path;
            path.emplace_back(std::move(node));
            path.emplace_back(std::move(newNode));

            data = NewNode<T>(nullptr, root_height, false, std::move(path));
            calculate_bbox(data);
        }

        // split on node overflow propagate upwards if necessary
        void split_on_overflow(size_t& level, std::vector<Node<T>*>& insertPath) {
            auto n = static_cast<size_t>(-1);
            while ((level != n) && (insertPath[level]->children.size() > maxEntries)) {
                split(insertPath, level);
                level--;
            }
        }

        //split overflowed node into two
        void split(std::vector<Node<T>*>& insertPath, size_t& level) {
            Node<T>* node = insertPath[level];

            const size_t M = node->children.size();
            size_t m = minEntries;

            choose_split_axis(node, m, M);
            const size_t at = choose_split_index(node, m, M);

            auto newNode = NewNode<T>(nullptr, node->height, node->leaf);
            //pre allocate children space = m - index
            newNode->children.reserve(M - at);

            //move kids from at to the end
            for (auto i = at; i < M; i++) {
                newNode->children.emplace_back(std::move(node->children[i]));
            }
            node->children.resize(at);//shrink size

            calculate_bbox(node);
            calculate_bbox(newNode);

            if (level > 0) {
                insertPath[level - 1]->add_child(std::move(newNode));
            } else {
                auto nn = std::make_unique<Node<T>>(Node<T>{
                        node->item,
                        node->height,
                        node->leaf,
                        node->bbox
                });
                nn->children = std::move(node->children);
                split_root(std::move(nn), std::move(newNode));
            }
        }

        //_chooseSplitAxis selects split axis : sorts node children
        //by the best axis for split.
        void choose_split_axis(Node<T>* node, size_t m, size_t M) {
            auto xMargin = all_dist_margin(node, m, M, ByX);
            auto yMargin = all_dist_margin(node, m, M, ByY);

            // if total distributions margin value is minimal for x, sort by minX,
            // otherwise it's already sorted by minY
            if (xMargin < yMargin) {
                std::sort(node->children.begin(), node->children.end(), x_node_path<T>());
            }
        }

        //_chooseSplitIndex selects split index.
        std::size_t choose_split_index(Node<T>* node, std::size_t m, std::size_t M) const {
            std::size_t i = 0, index = 0;
            double overlap, area, minOverlap, minArea;

            minOverlap = std::numeric_limits<double>::infinity();
            minArea = std::numeric_limits<double>::infinity();

            for (i = m; i <= M - m; i++) {
                auto bbox1 = dist_bbox(node, 0, i);
                auto bbox2 = dist_bbox(node, i, M);

                overlap = intersection_area(bbox1, bbox2);
                area = bbox_area(bbox1) + bbox_area(bbox2);

                // choose distribution with minimum overlap
                if (overlap < minOverlap) {
                    minOverlap = overlap;
                    index = i;

                    if (area < minArea) {
                        minArea = area;
                    }

                } else if (overlap == minOverlap) {
                    // otherwise choose distribution with minimum area
                    if (area < minArea) {
                        minArea = area;
                        index = i;
                    }
                }
            }
            return index;
        }

        //all_dist_margin computes total margin of all possible split distributions.
        //Each node is at least m full.
        double all_dist_margin(Node<T>* node, size_t m, size_t M, SortBy sortBy) const {
            if (sortBy == ByX) {
                std::sort(node->children.begin(), node->children.end(), x_node_path<T>());
                //bubbleAxis(*node.getChildren(), ByX, ByY)
            } else if (sortBy == ByY) {
                std::sort(node->children.begin(), node->children.end(), y_node_path<T>());
                //bubbleAxis(*node.getChildren(), ByY, ByX)
            }

            size_t i = 0;

            auto leftBBox = dist_bbox(node, 0, m);
            auto rightBBox = dist_bbox(node, M - m, M);
            auto margin = bbox_margin(leftBBox) + bbox_margin(rightBBox);

            for (i = m; i < M - m; i++) {
                auto child = node->children[i].get();
                extend(leftBBox, child->bbox);
                margin += bbox_margin(leftBBox);
            }

            for (i = M - m - 1; i >= m; i--) {
                auto child = node->children[i].get();
                extend(rightBBox, child->bbox);
                margin += bbox_margin(rightBBox);
            }
            return margin;
        }

    };

    template<typename T>
    RTree<T> NewRTree(size_t cap) {
        RTree<T> tree;
        tree.clear();

        if (cap == 0) {
            cap = 9;
        }
        // max entries in a node is 9 by default min node fill is 40% for best performance
        tree.maxEntries = max(4ul, cap);
        tree.minEntries = max(2ul, static_cast<size_t>(std::ceil(cap * 0.4)));
        return tree;
    }

}
#endif //RTREE_CPP_RTREE_H
