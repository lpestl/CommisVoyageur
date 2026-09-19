#pragma once

namespace graph {

// Forward declaration: an edge references its two endpoint nodes but must not
// include the full Node definition.
class Node;

// An edge connecting two nodes. The endpoints may be null (a "dangling" edge
// that is not yet fully connected). An edge may be directed (StartNode ->
// EndNode) or undirected; for an undirected edge the two endpoints are simply
// the two ends and the edge is traversable in both directions.
class Edge {
public:
    Edge() = default;
    Edge(Node* start, Node* end, bool directed = true);
    virtual ~Edge();

    Node* getStartNode() const { return startNode_; }
    Node* getEndNode() const { return endNode_; }

    bool isDirected() const { return directed_; }

    // Re-wire the endpoints / direction. The node edge lists are updated
    // automatically so they always reflect the current topology.
    void setStartNode(Node* node);
    void setEndNode(Node* node);
    void setDirected(bool directed);

private:
    // (Un)register this edge in the in/out lists of its endpoint nodes.
    void attach();
    void detach();

    Node* startNode_ = nullptr;
    Node* endNode_ = nullptr;
    bool directed_ = true;
};

} // namespace graph
