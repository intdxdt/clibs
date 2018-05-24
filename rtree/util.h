package rtree;

//split at index
 const ([]Node, []Node)& splitAtIndex(const []Node& nodes, int index) const {
    auto ln = len(nodes);
    auto newNodes = make([]*Node, ln - index);
    copy(newNodes, nodes[index:]);
    auto for (i = index; i < ln; i++ ) {
        nodes[i] = nullptr;
    }
    nodes = nodes[:index];
    return nodes, newNodes;
}

//slice index
 int sliceIndex(int limit, bool int) (i predicate) const {
    auto for (i = 0; i < limit; i++ ) {
        if (predicate(i)) {
            return i;
        }
    }
    return -1;
}


