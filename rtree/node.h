#include <limits>
#include <memory>
#include <utility>
#include <tuple>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <functional>
#include "../mbr/mbr.h"
#include "util.h"

#ifndef RTREE_CPP_NODE_H
#define RTREE_CPP_NODE_H

//Node
namespace rtree {
    ///Node type for internal node
    template<typename T>
    struct Node {
        T* item;
        size_t height;
        bool leaf;
        mbr::MBR bbox;
        std::vector<std::unique_ptr<Node>> children;


        Node(T* item, size_t height, bool leaf, mbr::MBR bbox) : item(item), height(height), leaf(leaf), bbox(bbox) {
            children = std::vector<std::unique_ptr<Node>>{};
        };

        Node(Node&& other) noexcept:
                item(other.item),
                height(other.height),
                leaf(other.leaf),
                bbox(other.bbox) {
            children = std::move(other.children);
        }

        ~Node() = default;

        Node& operator=(Node&& other) noexcept {
            item = other.item;
            leaf = other.leaf;
            height = other.height;
            bbox = other.bbox;
            children = std::move(other.children);
            return *this;
        }

        void add_child(std::unique_ptr<Node>&& child) {
            children.emplace_back(std::move(child));
        }

        T get_item() {
            return item;
        }
    };

    ///KObj instance struct
    template<typename T>
    struct KObj {
        Node<T>* node;
        mbr::MBR bbox;
        bool is_item;
        double dist;

        double score() {
            return this->dist;
        }

        T get_item() {
            return this->node->get_item();
        }

        std::string string() {
            std::ostringstream ss;
            ss << this->node->bbox.wkt() << " -> " << this->dist;
            return ss.str();
        }
    };

    template<typename T>
    struct kobj_cmp {
        inline bool operator()(const KObj<T>& a, const KObj<T>& b) const {
            return a.dist > b.dist;
        }
    };


    template<typename T>
    void destruct_node(std::unique_ptr<Node<T>>&& a) {
        if (a == nullptr) {
            return;
        }
        std::vector<std::unique_ptr<Node<T>>> stack;
        stack.reserve(a->children.size());
        stack.emplace_back(std::move(a));

        while (!stack.empty()) {
            auto node = std::move(stack[stack.size() - 1]);
            stack.pop_back();
            //adopt children on stack and let node go out of scope
            for (auto& o : node->children) {
                stack.emplace_back(std::move(o));
            }
        }
    }

    struct x_boxes {
        inline bool operator()(const mbr::MBR& a, const mbr::MBR& b) {
            return a.minx < b.minx;
        }
    };

    struct y_boxes {
        inline bool operator()(const mbr::MBR& a, const mbr::MBR& b) {
            return a.miny < b.miny;
        }
    };

    struct xy_boxes {
        inline bool operator()(const mbr::MBR& a, const mbr::MBR& b) {
            auto d = a.minx - b.minx;
            if (feq(d, 0)) {
                d = a.miny - b.miny;
            }
            return d < 0;
        }
    };

    template<typename T>
    struct x_node_path {
        inline bool operator()(const std::unique_ptr<Node<T>>& a, const std::unique_ptr<Node<T>>& b) {
            return a->bbox.minx < b->bbox.minx;
        }
    };

    template<typename T>
    struct y_node_path {
        inline bool operator()(const std::unique_ptr<Node<T>>& a, const std::unique_ptr<Node<T>>& b) {
            return a->bbox.miny < b->bbox.miny;
        }
    };

    template<typename T>
    struct xy_node_path {
        inline bool operator()(T* a, T* b) {
            auto d = a->bbox.minx - b->bbox.minx;
            if (feq(d, 0)) {
                d = a->bbox.miny - b->bbox.miny;
            }
            return d < 0;
        }
    };

    //compareNodeMinX computes change in minimum x
    inline double compare_minx(const mbr::MBR& a, const mbr::MBR& b) {
        return a.minx - b.minx;
    }

    //compareNodeMinY computes change in minimum y
    inline double compare_miny(const mbr::MBR& a, const mbr::MBR& b) {
        return a.miny - b.miny;
    }


    template<typename T>
    std::unique_ptr<Node<T>> NewNode(T* item, size_t height, bool leaf,
                                     std::vector<std::unique_ptr<Node<T>>>&& children) {
        mbr::MBR box = empty_mbr();
        if (item != nullptr) {
            box = item->bbox();
        }
        auto node = Node<T>(item, height, leaf, box);
        node.children = std::move(children);
        return std::make_unique<Node<T>>(std::move(node));
    }

    template<typename T>
    std::unique_ptr<Node<T>> NewNode(T* item, size_t height, bool leaf) {
        mbr::MBR box = empty_mbr();
        if (item != nullptr) {
            box = item->bbox();
        }
        return std::make_unique<Node<T>>(Node{item, height, leaf, box});
    }

    //NewNode creates a new node
    template<typename T>
    std::unique_ptr<Node<T>> new_leaf_Node(T* item) {
        return NewNode(item, 1, true, std::vector<std::unique_ptr<Node<T>>>{});
    }


    //Constructs children of node
    template<typename T>
    std::vector<std::unique_ptr<Node<T>>> make_children(std::vector<T*>& items) {
        std::vector<std::unique_ptr<Node<T>>> chs;
        auto n = items.size();
        chs.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            chs.emplace_back(new_leaf_Node(items[i]));
        }
        return chs;
    }

    //dist_bbox computes min bounding rectangle of node children from k to p-1.
    template<typename T>
    mbr::MBR dist_bbox(const T& node, size_t k, size_t p) {
        auto bbox = empty_mbr();
        for (auto i = k; i < p; i++) {
            extend(bbox, node->children[i]->bbox);
        }
        return bbox;
    }


    template<typename T>
    Node<T>* node_at_index(const std::vector<std::unique_ptr<Node<T>>>& a, size_t i) {
        if (a.empty() || (i > a.size() - 1)) {
            return nullptr;
        }
        return a[i].get();
    }

    template<typename T>
    Node<T>* node_at_index(const std::vector<Node<T>*>& a, size_t i) {
        if (a.empty() || (i > a.size() - 1)) {
            return nullptr;
        }
        return a[i];
    }

    //calculate_bbox calculates its bbox from bboxes of its children.
    template<typename T>
    void calculate_bbox(T& node) {
        node->bbox = dist_bbox(node, 0, node->children.size());
    }

    // adjust bboxes along the given tree path
    template<typename T>
    void adjust_parent_bboxes(const mbr::MBR& bbox, std::vector<Node<T>*>& path, size_t& level) {
        auto n = static_cast<size_t>(-1);
        for (auto i = level; i != n; i--) {
            extend(path[i]->bbox, bbox);
        }
    }

    //_chooseSubtree select child of node and updates path to selected node.
    template<typename T>
    Node<T>* choose_subtree(const mbr::MBR& bbox, Node<T>* node, size_t& level, std::vector<Node<T>*>& path) {
        Node<T>* child{nullptr};
        Node<T>* targetNode{nullptr};
        double minArea, minEnlargement, area, enlargement;

        while (true) {
            path.emplace_back(node);
            if (node->leaf || (path.size() - 1 == level)) {
                break;
            }
            minEnlargement = std::numeric_limits<double>::infinity();
            minArea = std::numeric_limits<double>::infinity();

            for (auto& o : node->children) {
                child = o.get();
                area = bbox_area(child->bbox);
                enlargement = enlarged_area(bbox, child->bbox) - area;

                // choose entry with the least area enlargement
                if (enlargement < minEnlargement) {
                    minEnlargement = enlargement;
                    if (area < minArea) {
                        minArea = area;
                    }
                    targetNode = child;
                } else if (feq(enlargement, minEnlargement)) {
                    // otherwise choose one with the smallest area
                    if (area < minArea) {
                        minArea = area;
                        targetNode = child;
                    }
                }
            }
            node = targetNode;
        }
        return node;
    }


}


#endif //RTREE_CPP_NODE_H
