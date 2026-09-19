#include "ofNode.h"

#include "../../core/Scene.h"

namespace graph {

void ofNode::draw() {
    if (!scene_) {
        return;
    }

    // Fill colour depends on the interaction state (inherited from ofEntity).
    ofColor fill = ofColor::yellow;
    if (isSelected()) {
        fill = ofColor::orange;
    } else if (isHovered()) {
        fill = ofColor(255, 215, 0); // golden highlight
    }

    // Filled rounded rectangle.
    ofSetColor(fill);
    ofFill();
    ofDrawRectRounded(getBoundingBox(), cornerRadius_);

    // Black 2px outline.
    ofNoFill();
    ofSetColor(ofColor::black);
    ofSetLineWidth(outlineWidth_);
    ofDrawRectRounded(getBoundingBox(), cornerRadius_);

    // Restore sensible defaults for any subsequent drawing.
    ofFill();
    ofSetLineWidth(1.0f);

    // Resize handles on top.
    postDraw();
}

void ofNode::onWorldToBounds() {
    if (!scene_) {
        return;
    }

    const glm::vec2 half = size_ * 0.5f;

    // World +y points up, so the "top-left" in screen terms is (x - half, y + half).
    const glm::vec2 topLeft =
        scene_->worldToScreen(glm::vec2(position_.x - half.x, position_.y + half.y));
    const glm::vec2 bottomRight =
        scene_->worldToScreen(glm::vec2(position_.x + half.x, position_.y - half.y));

    boundingBox_.set(topLeft.x, topLeft.y,
                     bottomRight.x - topLeft.x, bottomRight.y - topLeft.y);
}

void ofNode::onBoundsToWorld() {
    if (!scene_) {
        return;
    }

    const glm::vec2 topLeft =
        scene_->screenToWorld(glm::vec2(boundingBox_.x, boundingBox_.y));
    const glm::vec2 bottomRight = scene_->screenToWorld(glm::vec2(
        boundingBox_.x + boundingBox_.width, boundingBox_.y + boundingBox_.height));

    position_.x = (topLeft.x + bottomRight.x) * 0.5f;
    position_.y = (topLeft.y + bottomRight.y) * 0.5f;
    size_.x = bottomRight.x - topLeft.x;
    size_.y = topLeft.y - bottomRight.y;
}

} // namespace graph
