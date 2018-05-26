#include <memory>
#include <utility>
#include "../object/object.h"
#include "../mutil/mutil.h"
#include "../mbr/mbr.h"
#include "util.h"

namespace rtree {
    //universe type with bounds [+inf +inf -inf -inf]
    struct Universe : Object {
        Bounds bounds() override {
            return emptyBounds();
        }
    };

    ///Node type for internal node
    struct Node : Object {
        Object *item    = nullptr;
        int height      = -1;
        bool leaf       = false;
        MBR bbox        = emptyMbr();
        std::vector<std::shared_ptr<Node>> children{};

        Bounds bounds() override {
            return bbox.as_array();
        }

        //BBox returns bbox property
        const MBR& BBox() {
            return bbox;
        }

        void addChild(std::shared_ptr<Node> child) {
            children.emplace_back(child);
        }
    };

    struct XNodePath {
        inline bool operator()(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) {
            return a->bbox[0] < b->bbox[0];
        }
    };

    struct YNodePath {
        inline bool operator()(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) {
            return a->bbox[1] < b->bbox[1];
        }
    };

    //compareNodeMinX computes change in minimum x
    double compareNodeMinX(const Node& a, const Node& b) {
        return a.bbox.minx - b.bbox.minx;
    }

    //compareNodeMinY computes change in minimum y
    double compareNodeMinY(const Node& a, const Node& b) {
        return a.bbox.miny - b.bbox.miny;
    }


    std::shared_ptr<Node> NewNode(Object *item, int height, bool leaf,
                                  std::vector<std::shared_ptr<Node>>&& children) {
        Node node{};
        Bounds bounds = (item == nullptr) ? emptyBounds() : item->bounds();
        node.item = item;
        node.height = height;
        node.leaf = leaf;
        node.bbox = MBR{bounds, true};
        node.children = std::move(children);
        return std::make_shared<Node>(std::move(node));
    }

    //NewNode creates a new node
    std::shared_ptr<Node> newLeafNode(Object *item) {
        return NewNode(item, 1, true, std::move(std::vector<std::shared_ptr<Node>>{}));
    }


//Constructs children of node
    std::vector<std::shared_ptr<Node>> makeChildren(std::vector<Object *>& items) {
        std::vector<std::shared_ptr<Node>> chs;
        auto n = items.size();
        chs.reserve(n);
        for (auto i = 0; i < n; ++i) {
            Node node{};
            node.item = items[i];
            chs[i] = std::make_shared<Node>(std::move(node));
        }
        return chs;
    }

    std::vector<std::shared_ptr<Node>> emptyChildren(std::size_t length) {
        std::vector<std::shared_ptr<Node>> chs;
        chs.reserve(length);
        return std::move(chs);
    }


    //split at index
    template<typename T>
    std::vector<T> splitAtIndex(std::vector<T>& nodes, std::size_t index) {
        std::vector<T> newNodes(nodes.begin() + index, nodes.end());
        nodes.resize(index);
        return std::move(newNodes);
    }

//distBBox computes min bounding rectangle of node children from k to p-1.
    MBR distBBox(std::shared_ptr<Node>& node, std::size_t k, std::size_t p) {
        auto bbox = emptyMbr();
        for (auto i = k; i < p; i++) {
            auto child = node->children[i];
            extend(bbox, child->bbox);
        }
        return std::move(bbox);
    }

    //calcBBox calculates its bbox from bboxes of its children.
    void calcBBox(std::shared_ptr<Node>& node) {
        node->bbox = distBBox(node, 0, node->children.size());
    }


    // adjust bboxes along the given tree path
    void adjustParentBBoxes(const MBR& bbox, std::vector<std::shared_ptr<Node>>& path, int level) {
        for (auto i = level; i >= 0; i--) {
            extend((*path[i]).bbox, bbox);
        }
    }

    //_chooseSubtree select child of node and updates path to selected node.
    std::shared_ptr<Node>
    chooseSubtree(const MBR& bbox, std::shared_ptr<Node>& node, int level, std::vector<std::shared_ptr<Node>>& path) {

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
                area = bboxArea(child->bbox);
                enlargement = enlargedArea(bbox, child->bbox) - area;

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
