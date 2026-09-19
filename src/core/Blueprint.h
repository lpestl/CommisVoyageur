#pragma once

#include "Entity.h"
#include "ofMain.h"

#include <map>
#include <vector>

// A single line pattern for the grid: how often the lines repeat and how they
// (and their optional value label) are drawn.
struct LinesPattern {
    LinesPattern() = default;
    LinesPattern(const ofColor& lineColor, float lineWidth, int stepUnitSize,
                 const ofColor& labelColor = ofColor(0, 0, 0, 255),
                 bool bDrawLabel = false,
                 int fontSize = 12, bool bBold = false)
        : lineColor(lineColor), lineWidth(lineWidth), stepUnitSize(stepUnitSize),
          labelColor(labelColor), bDrawLabel(bDrawLabel),
          fontSize(fontSize), bBold(bBold) {}

    ofColor lineColor{255, 255, 255, 255};
    float lineWidth = 1.0f;
    int stepUnitSize = 1;
    ofColor labelColor{0, 0, 0, 255};
    bool bDrawLabel = false;
    int fontSize = 12;
    bool bBold = false;
};

// A blueprint-style background entity: fills the scene with a solid colour
// and draws a reference grid. All visual settings are loaded from an XML file
// (BlueprintSettings.xml) via ofxXmlSettings. The grid step adapts to the
// zoom, staying between kMinSpacingPx and kMaxSpacingPx and scaling by
// stepMultiplier. The X=0 and Y=0 axes are always highlighted.
class Blueprint : public Entity {
public:
    void setup() override;
    void draw() override;

    // Zoom / scale property, kept in sync with the camera via a subscription.
    void setZoom(float zoom) { zoom_ = zoom; }
    float getZoom() const { return zoom_; }

private:
    // Loads settings from the XML file (BlueprintSettings.xml in the data dir).
    bool loadSettings();

    // Chooses the adaptive base grid step for the current zoom.
    float computeBaseStep() const;

    // Returns the pattern for the grid line at the given base-step index.
    const LinesPattern& settingsFor(int k) const;

    void drawGrid(float minX, float maxX, float minY, float maxY);
    void drawGridLabels(float minX, float maxX, float minY, float maxY,
                        int smallUnit, float smallSpacing);
    void drawAxes(float minX, float maxX, float minY, float maxY);
    void drawWorldLine(const glm::vec2& a, const glm::vec2& b) const;

    // Unit-value label helpers.
    const ofTrueTypeFont& getLabelFont(int fontSize);
    void drawLabel(const std::string& text, const glm::vec2& worldAnchor,
                   const ofColor& color, bool alignRight, bool alignBottom,
                   int fontSize, bool bold);
    std::string formatUnitLabel(float value) const;

    // Called whenever the camera position/zoom changes.
    void onCameraChanged();

    // --- Settings loaded from XML (with sane defaults) ---
    ofColor backgroundColor_{30, 70, 140, 255};
    ofColor lineColor_{255, 255, 255, 140}; // axes colour
    float axisLineWidth_ = 5.0f;
    bool bVisibleGrid_ = true;
    float kMinSpacingPx_ = 50.0f;
    float kMaxSpacingPx_ = 250.0f;
    float stepMultiplier_ = 5.0f;

    // Grid line patterns, kept sorted by stepUnitSize ascending (minor -> major).
    std::vector<LinesPattern> linesPatterns_ = {
        LinesPattern{ofColor(255, 255, 255, 140), 1.0f, 1},                              // minor
        LinesPattern{ofColor(255, 255, 255, 140), 2.0f, 5, ofColor(0, 0, 0, 255), true}, // medium
        LinesPattern{ofColor(255, 255, 255, 140), 3.0f, 10, ofColor(0, 0, 0, 255), true} // major
    };

    // Cached label fonts, keyed by font size.
    std::map<int, ofTrueTypeFont> labelFonts_;

    // --- Runtime state (synced from the camera) ---
    float zoom_ = 1.0f;
    glm::vec2 cameraPosition_{0.0f, 0.0f};
};
