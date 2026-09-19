#include "Edge.h"
#include "Node.h"

namespace graph {

Edge::Edge(Node* start, Node* end, bool directed)
    : startNode_(start), endNode_(end), directed_(directed) {
    attach();
}

Edge::~Edge() {
    detach();
}

void Edge::setStartNode(Node* node) {
    if (startNode_ == node) {
        return;
    }
    detach();
    startNode_ = node;
    attach();
}

void Edge::setEndNode(Node* node) {
    if (endNode_ == node) {
        return;
    }
    detach();
    endNode_ = node;
    attach();
}

void Edge::setDirected(bool directed) {
    if (directed_ == directed) {
        return;
    }
    detach();
    directed_ = directed;
    attach();
}

void Edge::attach() {
    if (directed_) {
        // start -> end: outgoing from start, incoming into end.
        if (startNode_) {
            startNode_->addOutEdge(this);
        }
        if (endNode_) {
            endNode_->addInEdge(this);
        }
    } else {
        // Undirected: both endpoints can traverse the edge in either direction.
        if (startNode_) {
            startNode_->addOutEdge(this);
            startNode_->addInEdge(this);
        }
        if (endNode_) {
            endNode_->addOutEdge(this);
            endNode_->addInEdge(this);
        }
    }
}

void Edge::detach() {
    if (directed_) {
        if (startNode_) {
            startNode_->removeOutEdge(this);
        }
        if (endNode_) {
            endNode_->removeInEdge(this);
        }
    } else {
        if (startNode_) {
            startNode_->removeOutEdge(this);
            startNode_->removeInEdge(this);
        }
        if (endNode_) {
            endNode_->removeOutEdge(this);
            endNode_->removeInEdge(this);
        }
    }
}

} // namespace graph
