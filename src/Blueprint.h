#pragma once

#include "Entity.h"
#include "ofMain.h"

// Settings for a single level of grid lines. `stepUnitSize` is a multiplier
// of the adaptive base step; `lineColor`/`lineWidth` describe how the lines
// are drawn.
struct GridStepSettings {
    GridStepSettings() = default;
    GridStepSettings(const ofColor& color, float width, int unitSize)
        : lineColor(color), lineWidth(width), stepUnitSize(unitSize) {}

    ofColor lineColor{ofColor::white};
    float lineWidth = 1.0f;
    int stepUnitSize = 1;
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

    // Returns the settings for the grid line at the given base-step index.
    const GridStepSettings& settingsFor(int k) const;

    void drawGrid(float minX, float maxX, float minY, float maxY);
    void drawAxes(float minX, float maxX, float minY, float maxY);
    void drawWorldLine(const glm::vec2& a, const glm::vec2& b) const;

    // Called whenever the camera position/zoom changes.
    void onCameraChanged();

    // --- Settings loaded from XML (with sane defaults) ---
    ofColor backgroundColor_{30, 70, 140};
    ofColor lineColor_{ofColor::white}; // axes colour
    float axisLineWidth_ = 5.0f;
    bool bVisibleGrid_ = true;
    float kMinSpacingPx_ = 50.0f;
    float kMaxSpacingPx_ = 250.0f;
    float stepMultiplier_ = 5.0f;

    GridStepSettings smallStepSettings_{ofColor::white, 1.0f, 1};
    GridStepSettings middleStepSettings_{ofColor::white, 2.0f, 5};
    GridStepSettings bigStepSettings_{ofColor::white, 3.0f, 10};

    // --- Runtime state (synced from the camera) ---
    float zoom_ = 1.0f;
    glm::vec2 cameraPosition_{0.0f, 0.0f};
};
