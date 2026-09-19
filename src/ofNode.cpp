#include "ofNode.h"

#include "Scene.h"

namespace graph {

void ofNode::setup() {
    updateBoundingBox();
}

void ofNode::update() {
    // Keep the cached bounds in sync with the (possibly moving) camera.
    updateBoundingBox();
}

void ofNode::draw() {
    if (!scene_) {
        return;
    }

    updateBoundingBox();

    // Fill colour depends on the interaction state.
    ofColor fill = ofColor::yellow;
    if (selected_) {
        fill = ofColor::orange;
    } else if (hovered_) {
        fill = ofColor(255, 215, 0); // golden highlight
    }

    // Filled rounded rectangle.
    ofSetColor(fill);
    ofFill();
    ofDrawRectRounded(boundingBox_, cornerRadius_);

    // Black 2px outline.
    ofNoFill();
    ofSetColor(ofColor::black);
    ofSetLineWidth(outlineWidth_);
    ofDrawRectRounded(boundingBox_, cornerRadius_);

    // Restore sensible defaults for any subsequent drawing.
    ofFill();
    ofSetLineWidth(1.0f);
}

void ofNode::updateBoundingBox() {
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

void ofNode::mouseMoved(int x, int y) {
    updateBoundingBox();
    hovered_ = boundingBox_.inside(x, y);
}

void ofNode::mousePressed(int x, int y, int button) {
    if (button != OF_MOUSE_BUTTON_LEFT) {
        return;
    }

    updateBoundingBox();
    if (boundingBox_.inside(x, y)) {
        selected_ = true;
        dragging_ = true;
        const glm::vec2 mouseWorld = scene_->screenToWorld(glm::vec2(x, y));
        dragOffset_ = position_ - mouseWorld;
    } else {
        // Clicked away from this node -> clear selection.
        selected_ = false;
    }
}

void ofNode::mouseDragged(int x, int y, int button) {
    if (!dragging_ || button != OF_MOUSE_BUTTON_LEFT || !scene_) {
        return;
    }
    const glm::vec2 mouseWorld = scene_->screenToWorld(glm::vec2(x, y));
    position_ = mouseWorld + dragOffset_;
}

void ofNode::mouseReleased(int x, int y, int button) {
    if (button == OF_MOUSE_BUTTON_LEFT) {
        dragging_ = false;
    }
}

} // namespace graph
