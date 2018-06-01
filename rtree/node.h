#include <memory>
#include <sstream>
#include <utility>
#include "../mutil/mutil.h"
#include "../mbr/mbr.h"
#include "util.h"

namespace rtree {
    ///Node type for internal node
    struct Node {
        Object item;
        size_t height;
        bool leaf;
        MBR bbox;
        std::vector<std::unique_ptr<Node>> children;

        Node() : item(Object{}), height(0), leaf(false), bbox(empty_mbr()) {
            children = std::vector<std::unique_ptr<Node>>{};
        };

        Node(Object item, size_t height, bool leaf, MBR bbox) :
                item(item), height(height), leaf(leaf), bbox(bbox) {
            children = std::vector<std::unique_ptr<Node>>{};
        };

        Node(Node&& other) noexcept:
                item(other.item),
                leaf(other.leaf),
                height(other.height),
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

        Object get_item() {
            return item;
        }
    };


    ///KObj instance struct
    struct KObj {
        Node* node;
        MBR bbox;
        bool is_item;
        double dist;

        double score(){
            return this->dist;
        }

        Object get_item(){
            return this->node->get_item();
        }

        std::string string(){
            std::ostringstream ss;
            ss << this->node->bbox.wkt() << " -> "  << this->dist;
           return ss.str();
        }
    };

    struct kobj_cmp {
        inline bool operator()(const KObj& a, const KObj& b) const {
            return a.dist > b.dist;
        }
    };

    namespace {
        void destruct_node(std::unique_ptr<Node>&& a) {
            if (a == nullptr) {
                return;
            }
            std::vector<std::unique_ptr<Node>> stack;
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
            inline bool operator()(const MBR& a, const MBR& b) {
                return a.minx < b.minx;
            }
        };

        struct y_boxes {
            inline bool operator()(const MBR& a, const MBR& b) {
                return a.miny < b.miny;
            }
        };

        struct xy_boxes {
            inline bool operator()(const MBR& a, const MBR& b) {
                auto d = a.minx - b.minx;
                if (feq(d, 0)) {
                    d = a.miny - b.miny;
                }
                return d < 0;
            }
        };

        struct x_node_path {
            inline bool operator()(const std::unique_ptr<Node>& a, const std::unique_ptr<Node>& b) {
                return a->bbox.minx < b->bbox.minx;
            }
        };

        struct y_node_path {
            inline bool operator()(const std::unique_ptr<Node>& a, const std::unique_ptr<Node>& b) {
                return a->bbox.miny < b->bbox.miny;
            }
        };

        struct xy_node_path {
            template<typename T>
            inline bool operator()(const T& a, const T& b) {
                auto d = a->bbox.minx - b->bbox.minx;
                if (feq(d, 0)) {
                    d = a->bbox.miny - b->bbox.miny;
                }
                return d < 0;
            }
        };

        //compareNodeMinX computes change in minimum x
        double compare_minx(const MBR& a, const MBR& b) {
            return a.minx - b.minx;
        }

        //compareNodeMinY computes change in minimum y
        double compare_miny(const MBR& a, const MBR& b) {
            return a.miny - b.miny;
        }


        std::unique_ptr<Node> NewNode(Object item, size_t height, bool leaf,
                                      std::vector<std::unique_ptr<Node>>&& children) {
            Node node{};
            node.item = item;
            node.height = height;
            node.leaf = leaf;
            node.bbox = item.bbox;
            node.children = std::move(children);
            return std::make_unique<Node>(std::move(node));
        }

        std::unique_ptr<Node> NewNode(Object item, size_t height, bool leaf) {
            return std::make_unique<Node>(Node{item, height, leaf, item.bbox});
        }

        //NewNode creates a new node
        std::unique_ptr<Node> new_leaf_Node(Object item) {
            return std::move(NewNode(
                    item, 1, true, std::vector<std::unique_ptr<Node>>{}
            ));
        }


        //Constructs children of node
        std::vector<std::unique_ptr<Node>> make_children(std::vector<Object>& items) {
            std::vector<std::unique_ptr<Node>> chs;
            auto n = items.size();
            chs.reserve(n);
            for (auto i = 0; i < n; ++i) {
                chs.emplace_back(new_leaf_Node(items[i]));
            }
            return chs;
        }

        //dist_bbox computes min bounding rectangle of node children from k to p-1.
        template<typename T>
        MBR dist_bbox(const T& node, size_t k, size_t p) {
            auto bbox = empty_mbr();
            for (auto i = k; i < p; i++) {
                extend(bbox, node->children[i]->bbox);
            }
            return bbox;
        }


        Node* node_at_index(const std::vector<std::unique_ptr<Node>>& a, size_t i) {
            if (a.empty() || (i > a.size() - 1)) {
                return nullptr;
            }
            return a[i].get();
        }

        Node* node_at_index(const std::vector<Node*>& a, size_t i) {
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
        void adjust_parent_bboxes(const MBR& bbox, std::vector<Node*>& path, size_t level) {
            auto n = static_cast<size_t>(-1);
            for (auto i = level; i != n; i--) {
                extend(path[i]->bbox, bbox);
            }
        }

        //_chooseSubtree select child of node and updates path to selected node.
        Node* choose_subtree(const MBR& bbox, Node* node, size_t level, std::vector<Node*>& path) {

            Node* child{nullptr};
            Node* targetNode{nullptr};
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
                    }
                    else if (feq(enlargement, minEnlargement)) {
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
}