#include "ofEntity.h"

#include "Cursor.h"

namespace {

// Minimum bounding-box edge size while resizing (px).
constexpr float kMinSize = 8.0f;

} // namespace

ofEntity::ofEntity() {
    shoulders_[0].setPosition(Shoulder::Position::TopLeft);
    shoulders_[1].setPosition(Shoulder::Position::Top);
    shoulders_[2].setPosition(Shoulder::Position::TopRight);
    shoulders_[3].setPosition(Shoulder::Position::Left);
    shoulders_[4].setPosition(Shoulder::Position::Right);
    shoulders_[5].setPosition(Shoulder::Position::BottomLeft);
    shoulders_[6].setPosition(Shoulder::Position::Bottom);
    shoulders_[7].setPosition(Shoulder::Position::BottomRight);
}

void ofEntity::setup() {
    onWorldToBounds();
    updateShoulders();
}

void ofEntity::update() {
    // Keep the bounds in sync with the entity's own data (world coords etc.)
    onWorldToBounds();
    updateShoulders();
}

void ofEntity::updateShoulders() {
    const float x = boundingBox_.x;
    const float y = boundingBox_.y;
    const float w = boundingBox_.width;
    const float h = boundingBox_.height;
    const float cx = x + w * 0.5f;
    const float cy = y + h * 0.5f;

    shoulders_[0].setCenter(glm::vec2(x, y));         // TopLeft
    shoulders_[1].setCenter(glm::vec2(cx, y));        // Top
    shoulders_[2].setCenter(glm::vec2(x + w, y));     // TopRight
    shoulders_[3].setCenter(glm::vec2(x, cy));        // Left
    shoulders_[4].setCenter(glm::vec2(x + w, cy));    // Right
    shoulders_[5].setCenter(glm::vec2(x, y + h));     // BottomLeft
    shoulders_[6].setCenter(glm::vec2(cx, y + h));    // Bottom
    shoulders_[7].setCenter(glm::vec2(x + w, y + h)); // BottomRight
}

void ofEntity::postDraw() {
    if (!selected_ || !hovered_) {
        return;
    }
    for (auto& shoulder : shoulders_) {
        shoulder.draw();
    }
}

void ofEntity::mouseMoved(int x, int y) {
    hovered_ = boundingBox_.inside(x, y);

    const bool showShoulders = selected_ && hovered_;
    for (auto& shoulder : shoulders_) {
        if (showShoulders) {
            shoulder.mouseMoved(x, y);
        } else {
            shoulder.resetInteraction();
        }
    }

    updateCursor();
}

void ofEntity::updateCursor() {
    // Only shoulders change the cursor; the Scene resets it to the arrow before
    // forwarding mouse events, so we only ever override with a resize cursor.
    if (!selected_ || !hovered_) {
        return;
    }
    for (auto& shoulder : shoulders_) {
        if (shoulder.isHovered()) {
            cursor::apply(shoulder.getCursor());
            return;
        }
    }
}

void ofEntity::mousePressed(int x, int y, int button) {
    if (button != OF_MOUSE_BUTTON_LEFT) {
        return;
    }

    // Shoulders take priority: they sit on top of the box.
    if (selected_ && hovered_) {
        for (auto& shoulder : shoulders_) {
            if (shoulder.getRect().inside(x, y)) {
                activeShoulder_ = &shoulder;
                interaction_ = Interaction::Resizing;
                dragStartMouse_ = glm::vec2(x, y);
                dragStartBounds_ = boundingBox_;
                shoulder.mousePressed(x, y, button);
                return;
            }
        }
    }

    if (boundingBox_.inside(x, y)) {
        selected_ = true;
        interaction_ = Interaction::Moving;
        dragStartMouse_ = glm::vec2(x, y);
        dragStartBounds_ = boundingBox_;
    } else {
        // Clicked away -> deselect.
        selected_ = false;
    }
}

void ofEntity::mouseDragged(int x, int y, int button) {
    if (button != OF_MOUSE_BUTTON_LEFT) {
        return;
    }

    if (interaction_ == Interaction::Resizing) {
        resizeActiveShoulder(x, y);
    } else if (interaction_ == Interaction::Moving) {
        const float dx = static_cast<float>(x) - dragStartMouse_.x;
        const float dy = static_cast<float>(y) - dragStartMouse_.y;
        boundingBox_.set(dragStartBounds_.x + dx, dragStartBounds_.y + dy,
                         dragStartBounds_.width, dragStartBounds_.height);
        onBoundsToWorld();
    }
}

void ofEntity::resizeActiveShoulder(int x, int y) {
    if (!activeShoulder_) {
        return;
    }

    const float dx = static_cast<float>(x) - dragStartMouse_.x;
    const float dy = static_cast<float>(y) - dragStartMouse_.y;

    float newX = dragStartBounds_.x;
    float newY = dragStartBounds_.y;
    float newW = dragStartBounds_.width;
    float newH = dragStartBounds_.height;

    if (activeShoulder_->affectsLeft()) {
        newX += dx;
        newW -= dx;
    } else if (activeShoulder_->affectsRight()) {
        newW += dx;
    }

    if (activeShoulder_->affectsTop()) {
        newY += dy;
        newH -= dy;
    } else if (activeShoulder_->affectsBottom()) {
        newH += dy;
    }

    // Enforce a minimum size, keeping the dragged edge on the correct side.
    if (newW < kMinSize) {
        if (activeShoulder_->affectsLeft()) {
            newX = dragStartBounds_.x + dragStartBounds_.width - kMinSize;
        }
        newW = kMinSize;
    }
    if (newH < kMinSize) {
        if (activeShoulder_->affectsTop()) {
            newY = dragStartBounds_.y + dragStartBounds_.height - kMinSize;
        }
        newH = kMinSize;
    }

    boundingBox_.set(newX, newY, newW, newH);
    onBoundsToWorld();
}

void ofEntity::mouseReleased(int x, int y, int button) {
    if (button != OF_MOUSE_BUTTON_LEFT) {
        return;
    }

    interaction_ = Interaction::None;
    if (activeShoulder_) {
        activeShoulder_->mouseReleased(x, y, button);
        activeShoulder_ = nullptr;
    }
}
