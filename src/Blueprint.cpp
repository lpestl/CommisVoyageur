#include "Blueprint.h"
#include "Camera.h"
#include "Scene.h"
#include "ofxXmlSettings.h"

#include <algorithm>
#include <cmath>

void Blueprint::setup() {
    if (!scene_) {
        return;
    }

    loadSettings();

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
    if (!bVisibleGrid_) {
        return;
    }

    const float baseStep = computeBaseStep();
    const int smallUnit = std::max(1, smallStepSettings_.stepUnitSize);
    const float smallSpacing = baseStep * smallUnit;

    // Vertical grid lines.
    const int n0 = static_cast<int>(std::ceil(minX / smallSpacing));
    const int n1 = static_cast<int>(std::floor(maxX / smallSpacing));
    for (int n = n0; n <= n1; ++n) {
        const float wx = n * smallSpacing;
        const GridStepSettings& s = settingsFor(n * smallUnit);
        ofSetColor(s.lineColor);
        ofSetLineWidth(s.lineWidth);
        drawWorldLine(glm::vec2(wx, minY), glm::vec2(wx, maxY));
    }

    // Horizontal grid lines.
    const int m0 = static_cast<int>(std::ceil(minY / smallSpacing));
    const int m1 = static_cast<int>(std::floor(maxY / smallSpacing));
    for (int m = m0; m <= m1; ++m) {
        const float wy = m * smallSpacing;
        const GridStepSettings& s = settingsFor(m * smallUnit);
        ofSetColor(s.lineColor);
        ofSetLineWidth(s.lineWidth);
        drawWorldLine(glm::vec2(minX, wy), glm::vec2(maxX, wy));
    }
}

void Blueprint::drawAxes(float minX, float maxX, float minY, float maxY) {
    const bool showYAxis = (minX <= 0.0f && 0.0f <= maxX);
    const bool showXAxis = (minY <= 0.0f && 0.0f <= maxY);

    // Thick highlight drawn first...
    ofSetColor(lineColor_);
    ofSetLineWidth(axisLineWidth_);
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

bool Blueprint::loadSettings() {
    ofxXmlSettings xml;
    if (!xml.load("BlueprintSettings.xml")) {
        ofLogWarning("Blueprint") << "Could not load BlueprintSettings.xml; using defaults.";
        return false;
    }

    // Read an RGB colour stored as r/g/b attributes on the given tag.
    auto readColor = [&xml](const std::string& tag, const ofColor& fallback) {
        const int r = xml.getAttribute(tag, "r", static_cast<int>(fallback.r));
        const int g = xml.getAttribute(tag, "g", static_cast<int>(fallback.g));
        const int b = xml.getAttribute(tag, "b", static_cast<int>(fallback.b));
        return ofColor(r, g, b);
    };

    // Read a nested <...StepSettings> block.
    auto readStep = [&xml, &readColor](const std::string& tag,
                                       const GridStepSettings& fallback) {
        GridStepSettings s = fallback;
        if (xml.pushTag(tag)) {
            s.lineColor = readColor("lineColor", s.lineColor);
            s.lineWidth = static_cast<float>(
                xml.getValue("lineWidth", static_cast<double>(s.lineWidth)));
            s.stepUnitSize = xml.getValue("stepUnitSize", s.stepUnitSize);
            xml.popTag();
        }
        return s;
    };

    xml.pushTag("Blueprint");
    
    backgroundColor_ = readColor("backgroundColor", backgroundColor_);
    lineColor_ = readColor("lineColor", lineColor_);
    axisLineWidth_ = static_cast<float>(
        xml.getValue("axisLineWidth", static_cast<double>(axisLineWidth_)));
    bVisibleGrid_ = xml.getValue("bVisibleGrid", bVisibleGrid_ ? 1 : 0) != 0;
    kMinSpacingPx_ = static_cast<float>(
        xml.getValue("kMinSpacingPx", static_cast<double>(kMinSpacingPx_)));
    kMaxSpacingPx_ = static_cast<float>(
        xml.getValue("kMaxSpacingPx", static_cast<double>(kMaxSpacingPx_)));
    stepMultiplier_ = static_cast<float>(
        xml.getValue("stepMultiplier", static_cast<double>(stepMultiplier_)));

    smallStepSettings_ = readStep("smallStepSettings", smallStepSettings_);
    middleStepSettings_ = readStep("middleStepSettings", middleStepSettings_);
    bigStepSettings_ = readStep("bigStepSettings", bigStepSettings_);

    xml.popTag();
    
    // Sanitise so the adaptive-step loops terminate cleanly.
    stepMultiplier_ = std::max(stepMultiplier_, 1.0001f);
    kMinSpacingPx_ = std::max(kMinSpacingPx_, 1.0f);
    kMaxSpacingPx_ = std::max(kMaxSpacingPx_, kMinSpacingPx_ * stepMultiplier_);

    return true;
}

float Blueprint::computeBaseStep() const {
    const float ppu = scene_->getPixelsPerUnit();
    if (ppu <= 0.0f) {
        return 1.0f;
    }

    const float smallUnit =
        static_cast<float>(std::max(1, smallStepSettings_.stepUnitSize));

    float step = 1.0f;
    // Coarsen while the smallest grid lines are denser than kMinSpacingPx.
    while (step * smallUnit * ppu < kMinSpacingPx_) {
        step *= stepMultiplier_;
    }
    // Refine while the smallest grid lines are sparser than kMaxSpacingPx.
    while (step * smallUnit * ppu > kMaxSpacingPx_) {
        step /= stepMultiplier_;
    }
    return step;
}

const GridStepSettings& Blueprint::settingsFor(int k) const {
    if (bigStepSettings_.stepUnitSize > 0 &&
        (k % bigStepSettings_.stepUnitSize) == 0) {
        return bigStepSettings_;
    }
    if (middleStepSettings_.stepUnitSize > 0 &&
        (k % middleStepSettings_.stepUnitSize) == 0) {
        return middleStepSettings_;
    }
    return smallStepSettings_;
}
