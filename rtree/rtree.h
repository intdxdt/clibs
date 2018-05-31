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
        size_t maxEntries = 9;
        size_t minEntries = 4;
        std::unique_ptr<Node> data = nullptr;

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
            data = std::move(NewNode(Object{}, 1, true));
            return *this;
        }

        // is_empty checks for empty tree
        inline bool is_empty() {
            return data->children.empty();
        }


        // insert item
        RTree& insert(Object item) {
            insert(item, this->data->height - 1);
            return *this;
        }

        // load implements bulk loading
        RTree& load(std::vector<Object>& objects) {
            if (objects.empty()) {
                return *this;
            }

            if (objects.size() < minEntries) {
                for (auto& o : objects) {
                    insert(o);
                }
                return *this;
            }

            std::vector<Object> objs(objects.begin(), objects.end());

            // recursively build the tree from stratch using OMT algorithm
            auto node = _build(objs, 0, objs.size() - 1, 0);

            if (data->children.empty()) {
                // save as is if tree is empty
                data = std::move(node);
            }
            else if (data->height == node->height) {
                // split root if trees have the same height
                split_root(std::move(data), std::move(node));
            }
            else {
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
        RTree& load_boxes(std::vector<MBR>& boxes) {
            std::vector<Object> items;
            items.reserve(boxes.size());
            for (size_t i = 0; i < boxes.size(); ++i) {
                items.emplace_back(Object{i, boxes[i]});
            }
            return load(items);
        }

        // load_boxes loads bounding boxes
        RTree& load_boxes(std::vector<MBR>&& boxes) {
            return load_boxes(boxes);
        }

        // search item
        std::vector<Node*> search(const MBR& bbox) {
            Node* node = data.get();
            std::vector<Node*> result;

            if (!intersects(bbox, node->bbox)) {
                return result;
            }

            std::vector<Node*> nodesToSearch;

            while (true) {
                for (size_t i = 0, length = node->children.size(); i < length; i++) {
                    Node* child = node->children[i].get();
                    const MBR& childBBox = child->bbox;

                    if (intersects(bbox, childBBox)) {
                        if (node->leaf) {
                            result.emplace_back(child);
                        }
                        else if (contains(bbox, childBBox)) {
                            all(child, result);
                        }
                        else {
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
        std::vector<Node*> all() {
            std::vector<Node*> result{};
            all(data.get(), result);
            return std::move(result);
        }

        // Remove Item from RTree
        // NOTE:if item is a bbox , then first found bbox is removed
        RTree& remove(const Object& item) {
            if (item.bbox.equals(empty_mbr())) { //uninitialized object
                return *this;
            }
            remove_item(
                    item.bbox,
                    [&](const Node* node, size_t i) {
                        return node->children[i]->bbox.equals(item.bbox);
                    }
            );
            return *this;
        }

        //Remove Item from RTree
        //NOTE:if item is a bbox , then first found bbox is removed
        RTree& remove(const MBR& item) {
            remove_item(
                    item,
                    [&](const Node* node, size_t i) {
                        return node->children[i]->bbox.equals(item);
                    }
            );
            return *this;
        }

        // Remove Item from RTree
        // NOTE:if item is a bbox , then first found bbox is removed
        RTree& remove(const Node* item) {
            remove_item(
                    item->bbox,
                    [&](const Node* node, size_t i) {
                        return node->children[i].get() == item;
                    }
            );
            return *this;
        }

        bool collides(const MBR& bbox) {
            Node* node = data.get();
            if (!intersects(bbox, node->bbox)) {
                return false;
            }

            bool bln = false;
            Node* child;
            std::vector<Node*> searchList;

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

    private:
        // all - fetch all items from node
        void all(Node* node, std::vector<Node*>& result) {
            std::vector<Node*> nodesToSearch;
            while (true) {
                if (node->leaf) {
                    for (const auto& o : node->children) {
                        result.emplace_back(o.get());
                    }
                }
                else {
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
        void insert(Object item, int level) {
            auto bbox = item.bbox;
            std::vector<Node*> insertPath{};

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
        void insert(std::unique_ptr<Node>&& item, int level) {
            auto bbox = item->bbox;
            std::vector<Node*> insertPath{};

            // find the best node for accommodating the item, saving all nodes along the path too
            auto node = choose_subtree(bbox, data.get(), level, insertPath);

            node->children.emplace_back(std::move(item));
            extend(node->bbox, bbox);

            // split on node overflow propagate upwards if necessary
            split_on_overflow(level, insertPath);

            // adjust bboxes along the insertion path
            adjust_parent_bboxes(bbox, insertPath, level);
        }

        // Remove Item from RTree
        // NOTE:if item is a bbox , then first found bbox is removed
        RTree& remove_item(const MBR& bbox,
                           const std::function<bool(const Node*, size_t)>& predicate) {
            Node* node = data.get();
            Node* parent = nullptr;
            std::vector<Node*> path;
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
                    i = pop_index(indexes);
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
                        path.push_back(node);
                        condense(path);
                        return *this;
                    }
                }

                if (!goingUp && !node->leaf && contains(node->bbox, bbox)) {
                    //go down
                    path.push_back(node);
                    indexes.push_back(i);
                    i = 0;
                    parent = node;
                    node = node->children[0].get();
                }
                else if (parent != nullptr) {
                    //go right
                    i++;
                    node = node_at_index(parent->children, i);;
                    goingUp = false;
                }
                else {
                    node = nullptr;
                } //nothing found;
            }

            return *this;
        }

        // split on node overflow propagate upwards if necessary
        void split_on_overflow(int level, std::vector<Node*>& insertPath) {
            while (level >= 0) {
                if (insertPath[level]->children.size() > maxEntries) {
                    split(insertPath, level);
                    level--;
                }
                else {
                    break;
                }
            }
        }

        // sort an array so that items come in groups of n unsorted items,
        // with groups sorted between each other and
        // combines selection algorithm with binary divide & conquer approach.
        void multi_select(std::vector<Object>& arr, size_t left, size_t right, size_t n,
                          const std::function<double(const MBR&, const MBR&)>& compare) {
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
        void select_box(std::vector<Object>& arr, size_t left, size_t right, size_t k,
                        const std::function<double(const MBR&, const MBR&)>& compare) {
            size_t i = 0, j = 0;
            double fn, fi, fNewLeft, fNewRight, fsn, fz, fs, fsd;
            double fLeft(left), fRight(right), fk(k);
            Object t;

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
                               compare
                    );
                }

                t = arr[k];
                i = left;
                j = right;

                swap_item(arr, left, k);
                if (compare(arr[right].bbox, t.bbox) > 0) {
                    swap_item(arr, left, right);
                }

                while (i < j) {
                    swap_item(arr, i, j);
                    i++;
                    j--;
                    while (compare(arr[i].bbox, t.bbox) < 0) {
                        i++;
                    }
                    while (compare(arr[j].bbox, t.bbox) > 0) {
                        j--;
                    }
                }

                if (compare(arr[left].bbox, t.bbox) == 0) {
                    swap_item(arr, left, j);
                }
                else {
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
        std::unique_ptr<Node> _build(std::vector<Object>& items, size_t left, size_t right, int height) {
            auto N = double(right - left + 1);
            auto M = double(maxEntries);
            std::unique_ptr<Node> node;
            if (N <= M) {
                std::vector<Object> chs(items.begin() + left, items.begin() + right + 1);
                // reached leaf level return leaf
                node = NewNode(Object(), 1, true, make_children(chs));
                calculate_bbox(node);
                return node;
            }

            if (height == 0) {
                // target height of the bulk-loaded tree
                height = static_cast<int>(std::ceil(std::log(N) / std::log(M)));

                // target number of root entries to maximize storage utilization
                M = std::ceil(N / std::pow(M, double(height - 1)));
            }

            // TODO eliminate recursion?
            node = NewNode(Object{}, height, false, std::vector<std::unique_ptr<Node>>{});

            // split the items into M mostly square tiles

            auto N2 = size_t(std::ceil(N / M));
            auto N1 = N2 * size_t(std::ceil(std::sqrt(M)));
            size_t i, j, right2, right3;

            std::function<double(const MBR&, const MBR&)> cmpX = compare_minx;
            std::function<double(const MBR&, const MBR&)> cmpY = compare_miny;
            multi_select(items, left, right, N1, cmpX);

            for (i = left; i <= right; i += N1) {
                right2 = min(i + N1 - 1, right);
                multi_select(items, i, right2, N2, cmpY);

                for (j = i; j <= right2; j += N2) {
                    right3 = min(j + N2 - 1, right2);
                    // pack each entry recursively
                    node->add_child(_build(items, j, right3, height - 1));
                }
            }

            calculate_bbox(node);
            return node;
        }

        //_split overflowed node into two
        void split(std::vector<Node*>& insertPath, int level) {
            Node* node = insertPath[level];

            const size_t M = node->children.size();
            size_t m = minEntries;

            choose_split_axis(node, m, M);
            const size_t at = choose_split_index(node, m, M);

            auto newNode = NewNode(Object{}, node->height, node->leaf);
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
            }
            else {
                auto nn = NewNode(node->item, node->height, node->leaf, std::move(node->children));
                split_root(std::move(nn), std::move(newNode));
            }
        }

        //_splitRoot splits the root of tree.
        void split_root(std::unique_ptr<Node>&& node, std::unique_ptr<Node>&& newNode) {
            // split root node
            auto root_height = node->height + 1;
            std::vector<std::unique_ptr<Node>> path;
            path.emplace_back(std::move(node));
            path.emplace_back(std::move(newNode));
            auto root = NewNode(Object{}, root_height, false, std::move(path));
            data = std::move(root);
            calculate_bbox(data);
        }

        //condense node and its path from the root
        void condense(std::vector<Node*>& path) {
            Node* parent;
            size_t i = !path.empty() ? path.size() - 1 : 0;
            //go through the path, removing empty nodes and updating bboxes
            while (!path.empty()) {
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
                    }
                    else {
                        //root is empty, rest root
                        this->clear();
                    }
                }
                else {
                    calculate_bbox(path[i]);
                }

                if (i == 0) {
                    break;
                }
                i--;
            }
        }

        //_chooseSplitAxis selects split axis : sorts node children
        //by the best axis for split.
        void choose_split_axis(Node* node, size_t m, size_t M) {
            auto xMargin = all_dist_margin(node, m, M, ByX);
            auto yMargin = all_dist_margin(node, m, M, ByY);

            // if total distributions margin value is minimal for x, sort by minX,
            // otherwise it's already sorted by minY
            if (xMargin < yMargin) {
                std::sort(node->children.begin(), node->children.end(), x_node_path());
            }
        }

        //_chooseSplitIndex selects split index.
        template<typename T>
        std::size_t choose_split_index(T& node, std::size_t m, std::size_t M) const {
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

        //all_dist_margin computes total margin of all possible split distributions.
        //Each node is at least m full.
        double all_dist_margin(Node* node, size_t m, size_t M, SortBy sortBy) const {
            if (sortBy == ByX) {
                std::sort(node->children.begin(), node->children.end(), x_node_path());
                //bubbleAxis(*node.getChildren(), ByX, ByY)
            }
            else if (sortBy == ByY) {
                std::sort(node->children.begin(), node->children.end(), y_node_path());
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

    RTree NewRTree(size_t cap) {
        RTree tree;
        tree.clear();

        if (cap == 0) {
            cap = 9;
        }
        // max entries in a node is 9 by default min node fill is 40% for best performance
        tree.maxEntries = max(4ul, cap);
        tree.minEntries = max(2ul, static_cast<size_t>(std::ceil(cap * 0.4)));
        return std::move(tree);
    }

}