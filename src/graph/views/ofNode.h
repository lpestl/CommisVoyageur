#pragma once

#include "../../core/ofEntity.h"
#include "../Node.h"
#include "ofMain.h"

namespace graph {

// Visual, interactive representation of a graph Node. It inherits the graph
// topology from Node and the bounding-box / mouse / resize-handle behaviour
// from ofEntity. It draws a yellow rounded rectangle with a black 2px outline
// and keeps its world-space position/size in sync with the screen bounding box.
class ofNode : public Node, public ofEntity {
public:
    ofNode() = default;
    explicit ofNode(const glm::vec2& position) : position_(position) {}

    void draw() override;

    // World-space centre of the node.
    const glm::vec2& getPosition() const { return position_; }
    void setPosition(const glm::vec2& position) { position_ = position; }

    // World-space width/height of the node.
    const glm::vec2& getSize() const { return size_; }
    void setSize(const glm::vec2& size) { size_ = size; }

protected:
    // world position/size -> screen bounding box (called each frame).
    void onWorldToBounds() override;
    // screen bounding box -> world position/size (called after interaction).
    void onBoundsToWorld() override;

private:
    glm::vec2 position_{0.0f, 0.0f}; // world-space centre
    glm::vec2 size_{6.0f, 3.0f};     // world-space width/height
    float cornerRadius_ = 8.0f;      // px
    float outlineWidth_ = 2.0f;      // px
};

} // namespace graph
