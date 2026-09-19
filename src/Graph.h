#pragma once

#include "Edge.h"
#include "Node.h"

#include <memory>
#include <vector>

namespace graph {

// A graph: owns the set of all nodes and all edges it contains. Nodes and
// edges are stored as shared_ptr so their addresses stay stable while the
// topology (Node <-> Edge pointers) is maintained with raw, non-owning
// pointers.
class Graph {
public:
    // Takes ownership of the node; returns a raw pointer for convenience.
    Node* addNode(std::shared_ptr<Node> node);

    // Takes ownership of the edge; returns a raw pointer for convenience.
    Edge* addEdge(std::shared_ptr<Edge> edge);

    const std::vector<std::shared_ptr<Node>>& getNodes() const { return nodes_; }
    const std::vector<std::shared_ptr<Edge>>& getEdges() const { return edges_; }

    // Removes every edge and node (edges first, since they reference nodes).
    void clear();

private:
    std::vector<std::shared_ptr<Node>> nodes_;
    std::vector<std::shared_ptr<Edge>> edges_;
};

} // namespace graph
