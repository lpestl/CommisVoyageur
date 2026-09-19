#pragma once

#include "Entity.h"
#include "Node.h"
#include "ofMain.h"

namespace graph {

// Visual, interactive representation of a graph Node. It inherits the graph
// topology from Node and the rendering/mouse plumbing from Entity, so it can
// live in a Graph and be drawn + interacted with through a Scene.
//
// Renders a yellow rounded rectangle with a black 2px outline. It reacts to the
// mouse: hovering the cursor over it (mouseEnter / mouseExit effect) highlights
// it, left-clicking selects it (clicking elsewhere deselects it) and holding
// the left button drags it around the world.
class ofNode : public Node, public Entity {
public:
    ofNode() = default;
    explicit ofNode(const glm::vec2& position) : position_(position) {}

    void setup() override;
    void update() override;
    void draw() override;

    void mouseMoved(int x, int y) override;
    void mousePressed(int x, int y, int button) override;
    void mouseDragged(int x, int y, int button) override;
    void mouseReleased(int x, int y, int button) override;

    // Screen-space bounding box, recomputed every frame from the world position
    // and the current scene transform.
    const ofRectangle& getBoundingBox() const { return boundingBox_; }

    bool isHovered() const { return hovered_; }
    bool isSelected() const { return selected_; }

    // World-space centre of the node.
    const glm::vec2& getPosition() const { return position_; }
    void setPosition(const glm::vec2& position) { position_ = position; }

    // World-space width/height of the node.
    const glm::vec2& getSize() const { return size_; }
    void setSize(const glm::vec2& size) { size_ = size; }

private:
    void updateBoundingBox();

    // Visual settings.
    glm::vec2 position_{0.0f, 0.0f}; // world-space centre
    glm::vec2 size_{6.0f, 3.0f};     // world-space width/height
    float cornerRadius_ = 8.0f;      // px
    float outlineWidth_ = 2.0f;      // px

    // Screen-space bounds (cache of the world -> screen transform).
    ofRectangle boundingBox_;

    // Interaction state.
    bool hovered_ = false;
    bool selected_ = false;
    bool dragging_ = false;
    glm::vec2 dragOffset_{0.0f, 0.0f}; // world offset: node centre - grab point
};

} // namespace graph
