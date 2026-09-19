#include "Graph.h"

namespace graph {

Node* Graph::addNode(std::shared_ptr<Node> node) {
    if (!node) {
        return nullptr;
    }
    nodes_.push_back(node);
    return node.get();
}

Edge* Graph::addEdge(std::shared_ptr<Edge> edge) {
    if (!edge) {
        return nullptr;
    }
    edges_.push_back(edge);
    return edge.get();
}

void Graph::clear() {
    // Edges hold non-owning pointers to nodes, so they must be released first.
    edges_.clear();
    nodes_.clear();
}

} // namespace graph
