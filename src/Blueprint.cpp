#include "Blueprint.h"
#include "Camera.h"
#include "Scene.h"

#include <algorithm>
#include <cmath>

void Blueprint::setup() {
    if (!scene_) {
        return;
    }

    // Subscribe to camera position/zoom changes and sync our local state.
    scene_->getCamera().addChangeListener([this]() { onCameraChanged(); });
    onCameraChanged();
}

void Blueprint::draw() {
    if (!scene_) {
        return;
    }

    // --- Background ---
    ofClear(backgroundColor_);

    // Determine the visible world-space rectangle by converting the screen
    // corners into world coordinates.
    const glm::vec2 topLeft = scene_->screenToWorld(glm::vec2(0.0f, 0.0f));
    const glm::vec2 bottomRight =
        scene_->screenToWorld(glm::vec2(ofGetWidth(), ofGetHeight()));

    const float minX = std::min(topLeft.x, bottomRight.x);
    const float maxX = std::max(topLeft.x, bottomRight.x);
    const float minY = std::min(topLeft.y, bottomRight.y);
    const float maxY = std::max(topLeft.y, bottomRight.y);

    drawGrid(minX, maxX, minY, maxY);
    drawAxes(minX, maxX, minY, maxY);

    // Restore the default line width for any subsequent drawing.
    ofSetLineWidth(1.0f);
}

void Blueprint::onCameraChanged() {
    if (!scene_) {
        return;
    }
    zoom_ = scene_->getCamera().getZoom();
    cameraPosition_ = scene_->getCamera().getPosition();
}

void Blueprint::drawGrid(float minX, float maxX, float minY, float maxY) {
    const int x0 = static_cast<int>(std::floor(minX));
    const int x1 = static_cast<int>(std::ceil(maxX));
    const int y0 = static_cast<int>(std::floor(minY));
    const int y1 = static_cast<int>(std::ceil(maxY));

    ofSetColor(ofColor::white);

    // Vertical grid lines (one per integer world-x).
    for (int x = x0; x <= x1; ++x) {
        ofSetLineWidth(lineWidthFor(x));
        drawWorldLine(glm::vec2(x, minY), glm::vec2(x, maxY));
    }

    // Horizontal grid lines (one per integer world-y).
    for (int y = y0; y <= y1; ++y) {
        ofSetLineWidth(lineWidthFor(y));
        drawWorldLine(glm::vec2(minX, y), glm::vec2(maxX, y));
    }
}

void Blueprint::drawAxes(float minX, float maxX, float minY, float maxY) {
    const bool showYAxis = (minX <= 0.0f && 0.0f <= maxX);
    const bool showXAxis = (minY <= 0.0f && 0.0f <= maxY);

    // Thick white highlight drawn first...
    ofSetColor(ofColor::white);
    ofSetLineWidth(5.0f);
    if (showYAxis) {
        drawWorldLine(glm::vec2(0.0f, minY), glm::vec2(0.0f, maxY));
    }
    if (showXAxis) {
        drawWorldLine(glm::vec2(minX, 0.0f), glm::vec2(maxX, 0.0f));
    }

    // ...then a thin black core on top.
    ofSetColor(ofColor::black);
    ofSetLineWidth(1.0f);
    if (showYAxis) {
        drawWorldLine(glm::vec2(0.0f, minY), glm::vec2(0.0f, maxY));
    }
    if (showXAxis) {
        drawWorldLine(glm::vec2(minX, 0.0f), glm::vec2(maxX, 0.0f));
    }
}

void Blueprint::drawWorldLine(const glm::vec2& a, const glm::vec2& b) const {
    const glm::vec2 sa = scene_->worldToScreen(a);
    const glm::vec2 sb = scene_->worldToScreen(b);
    ofDrawLine(sa.x, sa.y, sb.x, sb.y);
}

int Blueprint::lineWidthFor(int coord) const {
    if (coord % 10 == 0) {
        return 3; // every 10 units
    }
    if (coord % 5 == 0) {
        return 2; // every 5 units
    }
    return 1; // every 1 unit
}
