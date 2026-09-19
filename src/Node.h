#pragma once

#include <algorithm>
#include <vector>

// Graph data structures live in their own namespace. This is required because
// openFrameworks already defines a global `ofNode` class (its 3D transform
// node); scoping our `ofNode` (and friends) here avoids the collision.
namespace graph {

// Forward declaration: nodes reference the edges that touch them, but must not
// include the full Edge definition (avoids a circular include).
class Edge;

// A node in a graph. It keeps non-owning pointers to every edge that touches
// it, split into incoming and outgoing sets. Edges register themselves with a
// node automatically, so the lists are always kept in sync by the Edge class.
class Node {
public:
    virtual ~Node() = default;

    // Edges whose direction points into this node.
    const std::vector<Edge*>& getInEdges() const { return inEdges_; }

    // Edges whose direction points away from this node.
    const std::vector<Edge*>& getOutEdges() const { return outEdges_; }

    // Every edge that touches this node (incoming + outgoing, deduplicated:
    // an undirected edge is reported in both lists but only once here).
    std::vector<Edge*> getAllEdges() const {
        std::vector<Edge*> result;
        result.reserve(inEdges_.size() + outEdges_.size());
        result.insert(result.end(), inEdges_.begin(), inEdges_.end());
        for (Edge* edge : outEdges_) {
            if (std::find(result.begin(), result.end(), edge) == result.end()) {
                result.push_back(edge);
            }
        }
        return result;
    }

private:
    friend class Edge;

    void addInEdge(Edge* edge) { inEdges_.push_back(edge); }
    void addOutEdge(Edge* edge) { outEdges_.push_back(edge); }

    void removeInEdge(Edge* edge) {
        const auto it = std::find(inEdges_.begin(), inEdges_.end(), edge);
        if (it != inEdges_.end()) {
            inEdges_.erase(it);
        }
    }

    void removeOutEdge(Edge* edge) {
        const auto it = std::find(outEdges_.begin(), outEdges_.end(), edge);
        if (it != outEdges_.end()) {
            outEdges_.erase(it);
        }
    }

    std::vector<Edge*> inEdges_;
    std::vector<Edge*> outEdges_;
};

} // namespace graph
