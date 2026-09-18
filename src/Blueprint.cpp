#include "Blueprint.h"
#include "Scene.h"

#include <algorithm>
#include <cmath>

void Blueprint::setup() {
    // Nothing to initialise yet; kept for symmetry with the Entity contract.
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

    const int x0 = static_cast<int>(std::floor(minX));
    const int x1 = static_cast<int>(std::ceil(maxX));
    const int y0 = static_cast<int>(std::floor(minY));
    const int y1 = static_cast<int>(std::ceil(maxY));

    ofSetColor(ofColor::white);

    // --- Vertical grid lines (one per integer world-x) ---
    for (int x = x0; x <= x1; ++x) {
        ofSetLineWidth(lineWidthFor(x));
        const glm::vec2 a = scene_->worldToScreen(glm::vec2(x, minY));
        const glm::vec2 b = scene_->worldToScreen(glm::vec2(x, maxY));
        ofDrawLine(a.x, a.y, b.x, b.y);
    }

    // --- Horizontal grid lines (one per integer world-y) ---
    for (int y = y0; y <= y1; ++y) {
        ofSetLineWidth(lineWidthFor(y));
        const glm::vec2 a = scene_->worldToScreen(glm::vec2(minX, y));
        const glm::vec2 b = scene_->worldToScreen(glm::vec2(maxX, y));
        ofDrawLine(a.x, a.y, b.x, b.y);
    }

    // Restore the default line width for any subsequent drawing.
    ofSetLineWidth(1.0f);
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
