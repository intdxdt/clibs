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


        RTree& clear() {
            destruct_node(std::move(data));
            data = NewNode<T>(nullptr, 1, true);
            return *this;
        }

        RTree& insert(T* item) {
            insert(item, this->data->height - 1);
            return *this;
        }

        bool is_empty() {
            return data->children.empty();
        }

        // load implements bulk loading
        RTree& load(std::vector<T*>& objects) {
            if (objects.empty()) {
                return *this;
            }

            if (objects.size() < minEntries) {
                for (auto& o : objects) {
                    insert(o);
                }
                return *this;
            }

            std::vector<T*> objs(objects.begin(), objects.end());

            // recursively build the tree from scratch using OMT algorithm
            auto node = bulk_build(objs, 0, objs.size() - 1, 0);

            if (data->children.empty()) {
                // save as is if tree is empty
                data = std::move(node);
            } else if (data->height == node->height) {
                // split root if trees have the same height
                split_root(std::move(data), std::move(node));
            } else {
                if (data->height < node->height) {
                    // swap trees if inserted one is bigger
                    std::swap(data, node);
                }

                auto nlevel = data->height - node->height - 1;
                // insert the small tree into the large tree at appropriate level
                insert(std::move(node), nlevel);
            }

            return *this;
        }

        // load_boxes loads bounding boxes
        RTree& load_boxes(std::vector<mbr::MBR>& boxes) {
            std::vector<T*> items;
            items.reserve(boxes.size());
            for (size_t i = 0; i < boxes.size(); ++i) {
                items.emplace_back(&boxes[i]);
            }
            return load(items);
        }

        // load_boxes loads bounding boxes
        RTree& load_boxes(std::vector<mbr::MBR>&& boxes) {
            return load_boxes(boxes);
        }

        // search item
        std::vector<Node<T>*> search(const mbr::MBR& bbox) {
            Node<T>* node = data.get();
            std::vector<Node<T>*> result;

            if (!intersects(bbox, node->bbox)) {
                return result;
            }

            std::vector<Node<T>*> nodesToSearch;

            while (true) {
                for (size_t i = 0, length = node->children.size(); i < length; i++) {
                    Node<T>* child = node->children[i].get();
                    const mbr::MBR& childBBox = child->bbox;

                    if (intersects(bbox, childBBox)) {
                        if (node->leaf) {
                            result.emplace_back(child);
                        } else if (contains(bbox, childBBox)) {
                            all(child, result);
                        } else {
                            nodesToSearch.emplace_back(child);
                        }
                    }
                }
                node = pop(nodesToSearch);
                if (node == nullptr) {
                    break;
                }
            }

            return result;
        }

        // all items from  root node
        std::vector<Node<T>*> all() {
            std::vector<Node<T>*> result{};
            all(data.get(), result);
            return result;
        }

        // Remove Item from RTree
        // NOTE:if item is a bbox , then first found bbox is removed
        RTree& remove(T* item) {
            if (item == nullptr) { //uninitialized object
                return *this;
            }
            remove_item(item->bbox(), [&](const Node<T>* node, size_t i) {
                return node->children[i]->item == item;
            });
            return *this;
        }

        //Remove Item from RTree
        //NOTE:if item is a bbox , then first found bbox is removed
        RTree& remove(const mbr::MBR& item) {
            remove_item(
                    item,
                    [&](const Node<T>* node, size_t i) {
                        return node->children[i]->bbox.equals(item);
                    });
            return *this;
        }

        // Remove Item from RTree
        // NOTE:if item is a bbox , then first found bbox is removed
        RTree& remove(const Node<T>* item) {
            remove_item(
                    item->bbox,
                    [&](const Node<T>* node, size_t i) {
                        return node->children[i].get() == item;
                    });
            return *this;
        }

        bool collides(const mbr::MBR& bbox) {
            Node<T>* node = data.get();
            if (!intersects(bbox, node->bbox)) {
                return false;
            }

            bool bln = false;
            Node<T>* child;
            std::vector<Node<T>*> searchList;

            while (!bln && node != nullptr) {
                size_t i = 0, length = node->children.size();
                for (; !bln && i < length; ++i) {
                    child = node->children[i].get();
                    if (intersects(bbox, child->bbox)) {
                        bln = node->leaf || contains(bbox, child->bbox);
                        searchList.emplace_back(child);
                    }
                }
                node = pop(searchList);
            }
            return bln;
        }

        std::vector<T*> KNN(
                const mbr::MBR& query, size_t limit,
                const std::function<double(const mbr::MBR&, KObj<T>)>& score) {
            return KNN(query, limit, score, [](KObj<T>) { return std::tuple<bool, bool>(true, false); });
        }

        std::vector<T*> KNN(
                const mbr::MBR query, size_t limit,
                const std::function<double(const mbr::MBR&, KObj<T>)>& score,
                const std::function<std::tuple<bool, bool>(KObj<T>)>& predicate) {

            auto node = data.get();
            std::vector<T*> result{};

            Node<T>* child{nullptr};
            std::vector<KObj<T>> queue{};
            bool stop{false}, pred{false};
            double dist{0};
            auto cmp = kobj_cmp<T>();

            while (!stop && (node != nullptr)) {
                for (auto& o : node->children) {
                    child = o.get();

                    if (child->children.empty()) {
                        dist = score(query, KObj<T>{child, child->bbox, true, -1});
                    } else {
                        dist = score(query, KObj<T>{nullptr, child->bbox, false, -1});
                    }
                    queue.push_back(KObj<T>{child, child->bbox, child->children.empty(), dist});
                }

                //make heap
                std::make_heap(queue.begin(), queue.end(), cmp);
                std::pop_heap(queue.begin(), queue.end(), cmp); //pop heap

                while (!queue.empty() && queue.back().is_item) {

                    auto candidate = queue.back(); //back
                    queue.pop_back();              //pop

                    auto pred_stop = predicate(candidate);
                    pred = std::get<0>(pred_stop);
                    stop = std::get<1>(pred_stop);

                    if (pred) {
                        result.emplace_back(candidate.node->item);
                    }

                    if (stop) {
                        break;
                    }

                    if ((limit != 0) && (result.size() == limit)) {
                        return result;
                    }

                    std::pop_heap(queue.begin(), queue.end(), cmp); //pop heap
                }

                if (!stop) {
                    if (queue.empty()) {
                        node = nullptr;
                    } else {
                        auto q = queue.back(); //back
                        queue.pop_back();      //pop
                        node = q.node;
                    }
                }
            }
            return result;
        }

    private:

        // all - fetch all items from node
        void all(Node<T>* node, std::vector<Node<T>*>& result) {
            std::vector<Node<T>*> nodesToSearch;
            while (true) {
                if (node->leaf) {
                    for (const auto& o : node->children) {
                        result.emplace_back(o.get());
                    }
                } else {
                    for (const auto& o : node->children) {
                        nodesToSearch.emplace_back(o.get());
                    }
                }

                node = pop(nodesToSearch);
                if (node == nullptr) {
                    break;
                }
            }
        }

        // insert - private
        void insert(T* item, size_t level) {
            if (item == nullptr) {
                return;
            }
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


        // sort an array so that items come in groups of n unsorted items,
        // with groups sorted between each other and
        // combines selection algorithm with binary divide & conquer approach.
        void multi_select(std::vector<T*>& arr, size_t left, size_t right, size_t n,
                          const std::function<double(const mbr::MBR&, const mbr::MBR&)>& compare) {
            size_t mid = 0;
            std::vector<size_t> stack{left, right};

            while (!stack.empty()) {
                right = stack.back();
                stack.pop_back();

                left = stack.back();
                stack.pop_back();

                if (right - left <= n) {
                    continue;
                }

                mid = left + size_t(std::ceil(double(right - left) / double(n) / 2.0)) * n;
                select_box(arr, left, right, mid, compare);
                stack.insert(stack.end(), {left, mid, mid, right});
            }
        }

        // sort array between left and right (inclusive) so that the smallest k elements come first (unordered)
        void select_box(std::vector<T*>& arr, size_t left, size_t right, size_t k,
                        const std::function<double(const mbr::MBR&, const mbr::MBR&)>& compare) {
            size_t i{0}, j{0};
            double fn, fi, fNewLeft, fNewRight, fsn, fz, fs, fsd;
            double fLeft(left), fRight(right), fk(k);
            T* t;

            while (right > left) {
                // the arbitrary constants 600 and 0.5 are used in the original
                // version to minimize execution time
                if (right - left > 600) {
                    fn = fRight - fLeft + 1.0;
                    fi = fk - fLeft + 1.0;
                    fz = std::log(fn);
                    fs = 0.5 * std::exp(2 * fz / 3.0);
                    fsn = 1.0;
                    if ((fi - fn / 2) < 0) {
                        fsn = -1.0;
                    }
                    fsd = 0.5 * std::sqrt(fz * fs * (fn - fs) / fn) * (fsn);
                    fNewLeft = max(fLeft, std::floor(fk - fi * fs / fn + fsd));
                    fNewRight = min(fRight, std::floor(fk + (fn - fi) * fs / fn + fsd));
                    select_box(arr,
                               static_cast<size_t>(fNewLeft),
                               static_cast<size_t>(fNewRight),
                               static_cast<size_t>(fk),
                               compare);
                }

                t = arr[k];
                i = left;
                j = right;

                swap_item(arr, left, k);
                if (compare(arr[right]->bbox(), t->bbox()) > 0) {
                    swap_item(arr, left, right);
                }

                while (i < j) {
                    swap_item(arr, i, j);
                    i++;
                    j--;
                    while (compare(arr[i]->bbox(), t->bbox()) < 0) {
                        i++;
                    }
                    while (compare(arr[j]->bbox(), t->bbox()) > 0) {
                        j--;
                    }
                }

                if (compare(arr[left]->bbox(), t->bbox()) == 0) {
                    swap_item(arr, left, j);
                } else {
                    j++;
                    swap_item(arr, j, right);
                }

                if (j <= k) {
                    left = j + 1;
                }
                if (k <= j) {
                    right = j - 1;
                }
            }
        }

        // build
        std::unique_ptr<Node<T>> bulk_build(std::vector<T*>& items, size_t left, size_t right, size_t height) {
            double N = double(right - left + 1);
            double M = double(maxEntries);
            std::unique_ptr<Node<T>> node;

            if (N <= M) {
                std::vector<T*> chs(items.begin() + left, items.begin() + right + 1);
                // reached leaf level return leaf
                node = NewNode<T>(nullptr, 1, true, make_children(chs));
                calculate_bbox(node);
                return node;
            }

            if (height == 0) {
                // target height of the bulk-loaded tree
                height = static_cast<size_t>(std::ceil(std::log(N) / std::log(M)));

                // target number of root entries to maximize storage utilization
                M = std::ceil(N / std::pow(M, double(height - 1)));
            }

            // TODO eliminate recursion?
            node = NewNode<T>(nullptr, height, false, std::vector<std::unique_ptr<Node<T>>>{});

            // split the items into M mostly square tiles

            auto N2 = size_t(std::ceil(N / M));
            auto N1 = N2 * size_t(std::ceil(std::sqrt(M)));
            size_t i, j, right2, right3;

            std::function<double(const mbr::MBR&, const mbr::MBR&)> cmpX = compare_minx;
            std::function<double(const mbr::MBR&, const mbr::MBR&)> cmpY = compare_miny;

            multi_select(items, left, right, N1, compare_minx);

            for (i = left; i <= right; i += N1) {
                right2 = min(i + N1 - 1, right);
                multi_select(items, i, right2, N2, compare_miny);

                for (j = i; j <= right2; j += N2) {
                    right3 = min(j + N2 - 1, right2);
                    // pack each entry recursively
                    node->add_child(bulk_build(items, j, right3, height - 1));
                }
            }

            calculate_bbox(node);
            return node;
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


        // Remove Item from RTree
        // NOTE:if item is a bbox , then first found bbox is removed
        RTree& remove_item(const mbr::MBR& bbox, const std::function<bool(const Node<T>*, size_t)>& predicate) {
            Node<T>* node = data.get();
            Node<T>* parent = nullptr;
            std::vector<Node<T>*> path;
            std::vector<size_t> indexes;
            std::optional<size_t> index;

            size_t i = 0;
            bool goingUp = false;

            //depth-first iterative this traversal
            while ((node != nullptr) || !path.empty()) {
                if (node == nullptr) {
                    //go up
                    node = pop(path);
                    parent = node_at_index(path, path.size() - 1);
                    i = pop(indexes);
                    goingUp = true;
                }

                if (node->leaf) {
                    //check current node
                    //index = node.children.indexOf(item)
                    index = slice_index(node->children.size(), [&](size_t i) {
                        return predicate(node, i);
                    });

                    //if found
                    if (index.has_value()) {
                        //item found, remove the item and condense this upwards
                        //node.children.splice(index, 1)
                        node->children.erase(node->children.begin() + index.value());
                        path.emplace_back(node);
                        condense(path);
                        return *this;
                    }
                }

                if (!goingUp && !node->leaf && contains(node->bbox, bbox)) {
                    //go down
                    path.emplace_back(node);
                    indexes.emplace_back(i);
                    i = 0;
                    parent = node;
                    node = node->children[0].get();
                } else if (parent != nullptr) {
                    //go right
                    i++;
                    node = node_at_index(parent->children, i);;
                    goingUp = false;
                } else {
                    node = nullptr;
                } //nothing found;
            }

            return *this;
        }


        //condense node and its path from the root
        void condense(std::vector<Node<T>*>& path) {
            Node<T>* parent{nullptr};
            auto sentinel{static_cast<size_t>(-1)};
            auto i{path.size() - 1};
            //go through the path, removing empty nodes and updating bboxes
            while (i != sentinel) {
                if (path[i]->children.empty()) {
                    //go through the path, removing empty nodes and updating bboxes
                    if (i > 0) {
                        parent = path[i - 1];
                        auto index = slice_index(parent->children.size(), [&](size_t j) {
                            return path[i] == parent->children[j].get();
                        });
                        if (index.has_value()) {
                            parent->children.erase(parent->children.begin() + index.value());
                        }
                    } else {
                        //root is empty, rest root
                        this->clear();
                    }
                } else {
                    calculate_bbox(path[i]);
                }
                i--;
            }
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
        tree.maxEntries = max(4UL, cap);
        tree.minEntries = max(2UL, static_cast<size_t>(std::ceil(cap * 0.4)));
        return tree;
    }

}
#endif //RTREE_CPP_RTREE_H
