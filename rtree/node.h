#include <memory>
#include <utility>
#include "../mutil/mutil.h"
#include "../mbr/mbr.h"
#include "util.h"

namespace rtree {
    namespace {
        ///Node type for internal node
        struct Node {
            Object item{};
            int height = 0;
            bool leaf = false;
            MBR bbox = emptyMbr();
            std::vector<std::shared_ptr<Node>> children{};


            void add_child(std::shared_ptr<Node> child) {
                children.emplace_back(child);
            }
        };

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
            inline bool operator()(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) {
                return a->bbox.minx < b->bbox.minx;
            }
        };

        struct y_node_path {
            inline bool operator()(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) {
                return a->bbox.miny < b->bbox.miny;
            }
        };

        struct xy_node_path {
            inline bool operator()(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) {
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


        std::shared_ptr<Node> new_Node(Object item, int height, bool leaf,
                                       std::vector<std::shared_ptr<Node>>&& children) {
            Node node{};
            node.item = item;
            node.height = height;
            node.leaf = leaf;
            node.bbox = item.bbox;
            node.children = std::move(children);
            return std::make_shared<Node>(std::move(node));
        }

        //new_Node creates a new node
        std::shared_ptr<Node> new_leaf_Node(Object item) {
            return std::move(new_Node(
                    item, 1, true, std::vector<std::shared_ptr<Node>>{}
            ));
        }


        //Constructs children of node
        std::vector<std::shared_ptr<Node>> make_children(std::vector<Object>& items) {
            std::vector<std::shared_ptr<Node>> chs;
            auto n = items.size();
            chs.reserve(n);
            for (auto i = 0; i < n; ++i) {
                chs.emplace_back(new_leaf_Node(items[i]));
            }
            return chs;
        }

        std::vector<std::shared_ptr<Node>> empty_children(std::size_t length) {
            std::vector<std::shared_ptr<Node>> chs;
            chs.resize(length);
            return std::move(chs);
        }

        //dist_bbox computes min bounding rectangle of node children from k to p-1.
        MBR dist_bbox(std::shared_ptr<Node>& node, size_t k, size_t p) {
            auto bbox = emptyMbr();
            for (auto i = k; i < p; i++) {
                extend(bbox, node->children[i]->bbox);
            }
            return bbox;
        }

        //calculate_bbox calculates its bbox from bboxes of its children.
        void calculate_bbox(std::shared_ptr<Node>& node) {
            node->bbox = dist_bbox(node, 0, node->children.size());
        }


        // adjust bboxes along the given tree path
        void adjust_parent_bboxes(const MBR& bbox, std::vector<std::shared_ptr<Node>>& path, int level) {
            for (int i = level; i >= 0; i--) {
                extend(path[i]->bbox, bbox);
            }
        }

        //_chooseSubtree select child of node and updates path to selected node.
        std::shared_ptr<Node>
        choose_subtree(const MBR& bbox, std::shared_ptr<Node> node, int level,
                       std::vector<std::shared_ptr<Node>>& path) {

            std::shared_ptr<Node> child;
            std::shared_ptr<Node> targetNode;
            double minArea, minEnlargement, area, enlargement;

            while (true) {
                path.emplace_back(node);
                if (node->leaf || path.size() - 1 == level) {
                    break;
                }
                minEnlargement = std::numeric_limits<double>::infinity();
                minArea = std::numeric_limits<double>::infinity();

                for (std::size_t i = 0, length = node->children.size(); i < length; i++) {
                    child = node->children[i];
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