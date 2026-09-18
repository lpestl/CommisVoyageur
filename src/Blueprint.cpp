#include "Blueprint.h"
#include "Camera.h"
#include "Scene.h"
#include "ofxXmlSettings.h"

#include <algorithm>
#include <cmath>

// Gap between a value label and its line/edge (px).
constexpr float kLabelPadding = 4.0f;

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
    const int smallUnit = std::max(1, linesPatterns_.front().stepUnitSize);
    const float smallSpacing = baseStep * smallUnit;

    // Vertical grid lines.
    const int n0 = static_cast<int>(std::ceil(minX / smallSpacing));
    const int n1 = static_cast<int>(std::floor(maxX / smallSpacing));
    for (int n = n0; n <= n1; ++n) {
        const float wx = n * smallSpacing;
        const LinesPattern& s = settingsFor(n * smallUnit);
        ofSetColor(s.lineColor);
        ofSetLineWidth(s.lineWidth);
        drawWorldLine(glm::vec2(wx, minY), glm::vec2(wx, maxY));
    }

    // Horizontal grid lines.
    const int m0 = static_cast<int>(std::ceil(minY / smallSpacing));
    const int m1 = static_cast<int>(std::floor(maxY / smallSpacing));
    for (int m = m0; m <= m1; ++m) {
        const float wy = m * smallSpacing;
        const LinesPattern& s = settingsFor(m * smallUnit);
        ofSetColor(s.lineColor);
        ofSetLineWidth(s.lineWidth);
        drawWorldLine(glm::vec2(minX, wy), glm::vec2(maxX, wy));
    }

    // Value labels for middle/big lines.
    drawGridLabels(minX, maxX, minY, maxY, smallUnit, smallSpacing);
}

void Blueprint::drawGridLabels(float minX, float maxX, float minY, float maxY,
                               int smallUnit, float smallSpacing) {
    const bool xAxisVisible = (minY <= 0.0f && 0.0f <= maxY);
    const bool yAxisVisible = (minX <= 0.0f && 0.0f <= maxX);

    // Vertical line labels (x values).
    const int n0 = static_cast<int>(std::ceil(minX / smallSpacing));
    const int n1 = static_cast<int>(std::floor(maxX / smallSpacing));
    for (int n = n0; n <= n1; ++n) {
        const int k = n * smallUnit;
        const LinesPattern& s = settingsFor(k);
        if (!s.bDrawLabel) {
            continue;
        }

        const float wx = n * smallSpacing;
        float anchorY;
        bool alignBottom;
        if (xAxisVisible) {
            anchorY = 0.0f;       // on the X axis
            alignBottom = false;  // text below the axis
        } else if (maxY < 0.0f) {
            anchorY = maxY;       // top edge
            alignBottom = false;  // text below the top edge
        } else {
            anchorY = minY;       // bottom edge
            alignBottom = true;   // text above the bottom edge
        }

        drawLabel(formatUnitLabel(wx), glm::vec2(wx, anchorY), s.labelColor,
                  /*alignRight=*/false, alignBottom, s.fontSize, s.bBold);
    }

    // Horizontal line labels (y values).
    const int m0 = static_cast<int>(std::ceil(minY / smallSpacing));
    const int m1 = static_cast<int>(std::floor(maxY / smallSpacing));
    for (int m = m0; m <= m1; ++m) {
        const int k = m * smallUnit;
        const LinesPattern& s = settingsFor(k);
        if (!s.bDrawLabel) {
            continue;
        }

        const float wy = m * smallSpacing;
        float anchorX;
        bool alignRight;
        if (yAxisVisible) {
            anchorX = 0.0f;      // on the Y axis
            alignRight = false;  // text to the right of the axis
        } else if (maxX < 0.0f) {
            anchorX = maxX;      // right edge
            alignRight = true;   // text to the left of the right edge
        } else {
            anchorX = minX;      // left edge
            alignRight = false;  // text to the right of the left edge
        }

        drawLabel(formatUnitLabel(wy), glm::vec2(anchorX, wy), s.labelColor,
                  alignRight, /*alignBottom=*/false, s.fontSize, s.bBold);
    }
}

void Blueprint::drawLabel(const std::string& text, const glm::vec2& worldAnchor,
                          const ofColor& color, bool alignRight, bool alignBottom,
                          int fontSize, bool bold) {
    const glm::vec2 screen = scene_->worldToScreen(worldAnchor);
    const ofTrueTypeFont& font = getLabelFont(fontSize);
    const float textWidth = font.getStringBoundingBox(text, 0, 0).width;
    const float lineHeight = font.getLineHeight();

    float x = screen.x;
    if (alignRight) {
        x -= textWidth + kLabelPadding; // text ends just left of the anchor
    } else {
        x += kLabelPadding;             // text starts just right of the anchor
    }

    float y = screen.y;
    if (alignBottom) {
        // Baseline sits just above the anchor line.
        y -= kLabelPadding;
    } else {
        // Text top sits just below the anchor line.
        y += lineHeight + kLabelPadding;
    }

    ofSetColor(color);
    font.drawString(text, x, y);
    if (bold) {
        // Fake bold: draw once more with a 1px horizontal offset.
        font.drawString(text, x + 1.0f, y);
    }
}

const ofTrueTypeFont& Blueprint::getLabelFont(int fontSize) {
    auto it = labelFonts_.find(fontSize);
    if (it == labelFonts_.end()) {
        ofTrueTypeFont font;
        if (!font.load("verdana.ttf", fontSize)) {
            ofLogWarning("Blueprint") << "Failed to load verdana.ttf at size " << fontSize;
        }
        it = labelFonts_.emplace(fontSize, font).first;
    }
    return it->second;
}

std::string Blueprint::formatUnitLabel(float value) const {
    // Round away floating-point noise (e.g. 50.000001) and drop a trailing ".0".
    const double rounded = std::round(value * 100.0) / 100.0;
    if (std::fabs(rounded - std::round(rounded)) < 0.001) {
        return ofToString(static_cast<int>(std::round(rounded)));
    }
    return ofToString(rounded);
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

    // Reads a single <linePattern> block (assumes the tag is already pushed).
    auto readPattern = [&xml, &readColor]() {
        LinesPattern p;
        p.lineColor = readColor("lineColor", p.lineColor);
        p.lineWidth = static_cast<float>(
            xml.getValue("lineWidth", static_cast<double>(p.lineWidth)));
        p.stepUnitSize = xml.getValue("stepUnitSize", p.stepUnitSize);
        p.labelColor = readColor("labelColor", p.labelColor);
        p.bDrawLabel = xml.getValue("bDrawLabel", p.bDrawLabel ? 1 : 0) != 0;
        p.fontSize = xml.getValue("fontSize", p.fontSize);
        p.bBold = xml.getValue("bBold", p.bBold ? 1 : 0) != 0;
        return p;
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

    // Read the <linesPatterns> array of <linePattern> blocks.
    if (xml.pushTag("linesPatterns")) {
        const int numPatterns = xml.getNumTags("linePattern");
        std::vector<LinesPattern> patterns;
        patterns.reserve(numPatterns);
        for (int i = 0; i < numPatterns; ++i) {
            xml.pushTag("linePattern", i);
            patterns.push_back(readPattern());
            xml.popTag();
        }
        xml.popTag(); // linesPatterns

        if (!patterns.empty()) {
            linesPatterns_ = std::move(patterns);
        }
    }

    // Keep patterns sorted by stepUnitSize ascending (minor -> major).
    std::sort(linesPatterns_.begin(), linesPatterns_.end(),
              [](const LinesPattern& a, const LinesPattern& b) {
                  return a.stepUnitSize < b.stepUnitSize;
              });

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
        static_cast<float>(std::max(1, linesPatterns_.front().stepUnitSize));

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

const LinesPattern& Blueprint::settingsFor(int k) const {
    // Patterns are sorted ascending by stepUnitSize; pick the largest one
    // whose step divides k (major lines win over minor ones).
    for (auto it = linesPatterns_.rbegin(); it != linesPatterns_.rend(); ++it) {
        if (it->stepUnitSize > 0 && (k % it->stepUnitSize) == 0) {
            return *it;
        }
    }
    return linesPatterns_.front();
}
