#include "Camera.h"

#include <algorithm>

Camera::Camera() {
    updateViewport();
}

void Camera::setPosition(const glm::vec2& position) {
    position_ = position;
    notifyChange();
}

void Camera::moveBy(const glm::vec2& worldDelta) {
    position_ += worldDelta;
    notifyChange();
}

void Camera::setZoom(float zoom) {
    zoom_ = std::max(0.01f, zoom);
    updateViewport();
    notifyChange();
}

void Camera::updateViewport() {
    const int w = ofGetWidth();
    const int h = ofGetHeight();

    if (w <= 0 || h <= 0) {
        width_ = 100.0f;
        height_ = 100.0f;
        return;
    }

    // At zoom 1.0 the window spans 100 world units horizontally.
    const float pixelsPerUnit = (w / 100.0f) * zoom_;
    width_ = w / pixelsPerUnit;
    height_ = h / pixelsPerUnit;
}

void Camera::addChangeListener(ChangeListener listener) {
    listeners_.push_back(listener);
}

void Camera::notifyChange() {
    for (auto& listener : listeners_) {
        if (listener) {
            listener();
        }
    }
}
