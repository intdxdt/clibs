#include <memory>
#include <utility>
#include "../mutil/mutil.h"
#include "../mbr/mbr.h"
#include "util.h"

namespace rtree {
    //universe type with bounds [+inf +inf -inf -inf]
    struct Universe {
    };

    ///Node type for internal node
    struct Node {
        Object item {};
        int height = 0;
        bool leaf = false;
        MBR bbox = emptyMbr();
        std::vector<std::shared_ptr<Node>> children{};


        //BBox returns bbox property
        const MBR& BBox() {
            return bbox;
        }

        void addChild(std::shared_ptr<Node> child) {
            children.emplace_back(child);
        }
    };

    struct XBoxes {
        inline bool operator()(const MBR& a,  const MBR& b) {
            return a.minx < b.minx;
        }
    };
    struct YBoxes {
        inline bool operator()(const MBR& a,  const MBR& b) {
            return a.miny < b.miny;
        }
    };
    struct XYBoxes {
        inline bool operator()(const MBR& a,  const MBR& b) {
            auto d = a.minx  - b.minx ;
            if (feq(d, 0)) {
                d = a.miny - b.miny;
            }
            return d < 0;
        }
    };

    struct XNodePath {
        inline bool operator()(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) {
            return a->bbox.minx < b->bbox.minx;
        }
    };

    struct YNodePath {
        inline bool operator()(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) {
            return a->bbox.miny < b->bbox.miny;
        }
    };

    struct XYNodePath {
        inline bool operator()(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) {
            auto d = a->bbox.minx  - b->bbox.minx ;
            if (feq(d, 0)) {
                d = a->bbox.miny - b->bbox.miny;
            }
            return d < 0;
        }
    };

    //compareNodeMinX computes change in minimum x
    double compareMinX(const MBR& a, const MBR& b) {
        return a.minx - b.minx;
    }

    //compareNodeMinY computes change in minimum y
    double compareMinY(const MBR& a, const MBR& b) {
        return a.miny - b.miny;
    }


    std::shared_ptr<Node> NewNode(Object item, size_t height, bool leaf,
                                  std::vector<std::shared_ptr<Node>>&& children) {
        Node node{};
        node.item = item;
        node.height = height;
        node.leaf = leaf;
        node.bbox = item.bbox;
        node.children = std::move(children);
        return std::make_shared<Node>(std::move(node));
    }

    //NewNode creates a new node
    std::shared_ptr<Node> newLeafNode(Object item) {
        return NewNode(item, 1, true, std::move(std::vector<std::shared_ptr<Node>>{}));
    }


//Constructs children of node
    std::vector<std::shared_ptr<Node>> makeChildren(std::vector<Object>& items) {
        std::vector<std::shared_ptr<Node>> chs;
        auto n = items.size();
        chs.reserve(n);
        for (auto i = 0; i < n; ++i) {
            chs.emplace_back(newLeafNode(items[i]));
        }
        return chs;
    }

    std::vector<std::shared_ptr<Node>> emptyChildren(std::size_t length) {
        std::vector<std::shared_ptr<Node>> chs;
        chs.resize(length);
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
    MBR distBBox(std::shared_ptr<Node>& node, size_t k, size_t p) {
        auto bbox = emptyMbr();
        for (auto i = k; i < p; i++) {
            auto child = node->children[i];
            extend(bbox, child->bbox);
        }
        return bbox;
    }

    //calcBBox calculates its bbox from bboxes of its children.
    void calcBBox(std::shared_ptr<Node>& node) {
        node->bbox = distBBox(node, 0, node->children.size());
    }


    // adjust bboxes along the given tree path
    void adjustParentBBoxes(const MBR& bbox, std::vector<std::shared_ptr<Node>>& path, int level) {
        for (int i = level; i >= 0; i--) {
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
