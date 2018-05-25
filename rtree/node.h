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
        Object *item = nullptr;
        int height = -1;
        bool leaf = false;
        std::shared_ptr<MBR> bbox = nullptr;
        std::vector<std::shared_ptr<Node>> children;

        Bounds bounds() override {
            return (*bbox).as_array();
        }

        //BBox returns bbox property
        const MBR& BBox() {
            return *bbox;
        }

        void addChild(std::shared_ptr<Node> child) {
            children.emplace_back(child);
        }
    };


    std::shared_ptr<Node> NewNode(Object *item, int height, bool leaf,
                                  std::vector<std::shared_ptr<Node>> children) {
        Node node;
        Bounds bounds = (item == nullptr) ? emptyBounds() : item->bounds();
        node.item = item;
        node.height = height;
        node.leaf = leaf;
        node.bbox = std::make_shared<MBR>(std::move(MBR{bounds, true}));
        node.children = std::move(children);
        return std::make_shared<Node>(std::move(node));
    }

    //NewNode creates a new node
    std::shared_ptr<Node> newLeafNode(Object *item) {
        return NewNode(item, 1, true, std::vector<std::shared_ptr<Node>>{});
    }


//Constructs children of node
    std::vector<std::shared_ptr<Node>> makeChildren(const std::vector<Object *>& items) {
        std::vector<std::shared_ptr<Node>> chs;
        auto n = items.size();
        chs.reserve(n);
        for (auto i = 0; i < n; ++i) {
            Node node;
            node.item = items[i];
            chs[i] = std::make_shared<Node>(std::move(node));
        }
        return chs;
    }

}
