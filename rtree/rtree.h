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
        std::shared_ptr<Node> Data = nullptr;


        RTree& Clear() {
            std::vector<std::shared_ptr<Node>> ch{};
            auto node = NewNode(Object{}, 1, true, std::move(ch));
            Data = std::move(node);
            return *this;
        }

        //IsEmpty checks for empty tree
        bool IsEmpty() {
            return Data.get()->children.empty();
        }


        //Insert item
        RTree& Insert(Object item) {
            insert(item, this->Data->height - 1);
            return *this;
        }

        //Load implements bulk loading
        RTree& Load(std::vector<Object>& objects) {
            if (objects.empty()) {
                return *this;
            }

            if (objects.size() < minEntries) {
                for (auto& o : objects) {
                    Insert(o);
                }
                return *this;
            }

            std::vector<Object> data(objects.begin(), objects.end());

            // recursively build the tree with the given data from stratch using OMT algorithm
            auto node = _build(data, 0, data.size() - 1, 0);

            if (Data->children.empty()) {
                // save as is if tree is empty
                Data = std::move(node);
            }
            else if (Data->height == node->height) {
                // split root if trees have the same height
                splitRoot(Data, node);
            }
            else {
                if (Data->height < node->height) {
                    // swap trees if inserted one is bigger
                    std::swap(Data, node);
                }

                // insert the small tree into the large tree at appropriate level
                insert(node, Data->height - node->height - 1);
            }

            return *this;
        }

        //LoadBoxes loads bounding boxes
        RTree& LoadBoxes(std::vector<MBR>& data) {
            std::vector<Object> items;
            items.reserve(data.size());
            for (size_t i = 0; i < data.size(); ++i) {
                items.emplace_back(Object{i, data[i]});
            }
            return Load(items);
        }

        //LoadBoxes loads bounding boxes
        RTree& LoadBoxes(std::vector<MBR>&& data) {
            return LoadBoxes(data);
        }

        //Search item
        std::vector<std::shared_ptr<Node>> Search(const MBR& bbox) {
            auto node = Data;
            std::vector<std::shared_ptr<Node>> result;

            if (!intersects(bbox, node->bbox)) {
                return result;
            }

            std::vector<std::shared_ptr<Node>> nodesToSearch;

            while (true) {
                for (size_t i = 0, length = node->children.size(); i < length; i++) {
                    std::shared_ptr<Node>& child = node->children[i];
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

        //All items from  root node
        std::vector<std::shared_ptr<Node>> All() {
            std::vector<std::shared_ptr<Node>> result{};
            all(Data, result);
            return std::move(result);
        }

    private:
        //all - fetch all items from node
       void all(std::shared_ptr<Node> node, std::vector<std::shared_ptr<Node>>& result) {
            std::vector<std::shared_ptr<Node>> nodesToSearch;
            while (true) {
                if (node->leaf) {
                    for (auto& o : node->children) {
                        result.emplace_back(o);
                    }
                }
                else {
                    for (auto& o : node->children) {
                        nodesToSearch.emplace_back(o);
                    }
                }
                node = pop(nodesToSearch);
                if (node == nullptr) {
                    break;
                }
            }
        }

        //insert - private
        void insert(Object item, int level) {
            auto bbox = item.bbox;
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

        // sort an array so that items come in groups of n unsorted items,
        // with groups sorted between each other and
        // combines selection algorithm with binary divide & conquer approach.
        void multiSelect(std::vector<Object>& arr, size_t left, size_t right, size_t n,
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
                selectBox(arr, left, right, mid, compare);
                stack.insert(stack.end(), {left, mid, mid, right});
            }
        }


        // sort array between left and right (inclusive) so that the smallest k elements come first (unordered)
        void selectBox(std::vector<Object>& arr, size_t left, size_t right, size_t k,
                       const std::function<double(const MBR&, const MBR&)>& compare) {
            size_t i = 0, j = 0;
            double fn, fi, fNewLeft, fNewRight, fsn, fz, fs, fsd;
            double fLeft(left), fRight(right), fk(k);
            Object t;

            while (right > left) {
                //the arbitrary constants 600 and 0.5 are used in the original
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
                    selectBox(arr,
                              static_cast<size_t>(fNewLeft),
                              static_cast<size_t>(fNewRight),
                              static_cast<size_t>(fk),
                              compare
                    );
                }

                t = arr[k];
                i = left;
                j = right;

                swapItem(arr, left, k);
                if (compare(arr[right].bbox, t.bbox) > 0) {
                    swapItem(arr, left, right);
                }

                while (i < j) {
                    swapItem(arr, i, j);
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
                    swapItem(arr, left, j);
                }
                else {
                    j++;
                    swapItem(arr, j, right);
                }

                if (j <= k) {
                    left = j + 1;
                }
                if (k <= j) {
                    right = j - 1;
                }
            }
        }


        //build
        std::shared_ptr<Node> _build(std::vector<Object>& items, size_t left, size_t right, size_t height) {

            auto N = double(right - left + 1);
            auto M = double(maxEntries);
            std::shared_ptr<Node> node;
            if (N <= M) {
                std::vector<Object> chs(items.begin() + left, items.begin() + right + 1);
                // reached leaf level return leaf
                node = NewNode(Object(), 1, true, makeChildren(chs));
                calcBBox(node);
                return node;
            }

            if (height == 0) {
                // target height of the bulk-loaded tree
                height = size_t(std::ceil(std::log(N) / std::log(M)));

                // target number of root entries to maximize storage utilization
                M = std::ceil(N / std::pow(M, double(height - 1)));
            }

            // TODO eliminate recursion?

            node = NewNode(Object{}, height, false, std::vector<std::shared_ptr<Node>>{});

            // split the items into M mostly square tiles

            auto N2 = size_t(std::ceil(N / M));
            auto N1 = N2 * size_t(std::ceil(std::sqrt(M)));
            size_t i, j, right2, right3;

            std::function<double(const MBR&, const MBR&)> cmpX = compareMinX;
            std::function<double(const MBR&, const MBR&)> cmpY = compareMinY;
            multiSelect(items, left, right, N1, cmpX);

            for (i = left; i <= right; i += N1) {
                right2 = min(i + N1 - 1, right);
                multiSelect(items, i, right2, N2, cmpY);

                for (j = i; j <= right2; j += N2) {
                    right3 = min(j + N2 - 1, right2);
                    // pack each entry recursively
                    node->addChild(_build(items, j, right3, height - 1));
                }
            }

            calcBBox(node);
            return node;
        }


        //_split overflowed node into two
        void split(std::vector<std::shared_ptr<Node>>& insertPath, int level) {
            auto node = insertPath[level];
            auto newNode = NewNode(
                    Object{},
                    node->height,
                    node->leaf,
                    emptyChildren(0)
            );

            auto M = node->children.size();
            size_t m = minEntries;

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

        //_splitRoot splits the root of tree.
        void splitRoot(std::shared_ptr<Node>& node, std::shared_ptr<Node>& newNode) {
            // split root node
            auto path = std::vector<std::shared_ptr<Node>>{node, newNode};
            auto root = NewNode(Object{}, node->height + 1, false, std::move(path));
            Data = root;
            calcBBox(Data);
        }

        //_chooseSplitAxis selects split axis : sorts node children
        //by the best axis for split.
        void chooseSplitAxis(std::shared_ptr<Node>& node, size_t m, size_t M) {
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


        //allDistMargin computes total margin of all possible split distributions.
        //Each node is at least m full.
        double allDistMargin(std::shared_ptr<Node>& node, size_t m, size_t M, SortBy sortBy) const {
            if (sortBy == ByX) {
                std::sort(node->children.begin(), node->children.end(), XNodePath());
                //bubbleAxis(*node.getChildren(), ByX, ByY)
            }
            else if (sortBy == ByY) {
                std::sort(node->children.begin(), node->children.end(), YNodePath());
                //bubbleAxis(*node.getChildren(), ByY, ByX)
            }

            size_t i = 0;

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