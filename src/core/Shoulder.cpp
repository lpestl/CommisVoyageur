#include "Shoulder.h"

void Shoulder::setCenter(const glm::vec2& center) {
    center_ = center;
    updateRect();
}

void Shoulder::updateRect() {
    const float half = size_ * 0.5f;
    rect_.set(center_.x - half, center_.y - half, size_, size_);
}

void Shoulder::mouseMoved(int x, int y) {
    hovered_ = rect_.inside(x, y);
}

void Shoulder::mousePressed(int x, int y, int button) {
    if (button == OF_MOUSE_BUTTON_LEFT) {
        pressed_ = rect_.inside(x, y);
    }
}

void Shoulder::mouseReleased(int x, int y, int button) {
    if (button == OF_MOUSE_BUTTON_LEFT) {
        pressed_ = false;
    }
}

void Shoulder::draw() {
    // White square: opaque when hovered, semi-transparent otherwise.
    const int alpha = hovered_ ? 255 : 128;
    ofSetColor(255, 255, 255, alpha);
    ofFill();
    ofDrawRectangle(rect_);

    // Blue outline.
    ofNoFill();
    ofSetColor(ofColor::blue);
    ofSetLineWidth(1.0f);
    ofDrawRectangle(rect_);

    ofFill();
    ofSetLineWidth(1.0f);
}

cursor::Type Shoulder::getCursor() const {
    switch (position_) {
        case Position::TopLeft:
        case Position::BottomRight:
            return cursor::Type::ResizeNWSE;
        case Position::TopRight:
        case Position::BottomLeft:
            return cursor::Type::ResizeNESW;
        case Position::Top:
        case Position::Bottom:
            return cursor::Type::ResizeVertical;
        case Position::Left:
        case Position::Right:
            return cursor::Type::ResizeHorizontal;
    }
    return cursor::Type::Arrow;
}

bool Shoulder::affectsLeft() const {
    return position_ == Position::TopLeft || position_ == Position::Left ||
           position_ == Position::BottomLeft;
}

bool Shoulder::affectsRight() const {
    return position_ == Position::TopRight || position_ == Position::Right ||
           position_ == Position::BottomRight;
}

bool Shoulder::affectsTop() const {
    return position_ == Position::TopLeft || position_ == Position::Top ||
           position_ == Position::TopRight;
}

bool Shoulder::affectsBottom() const {
    return position_ == Position::BottomLeft || position_ == Position::Bottom ||
           position_ == Position::BottomRight;
}
