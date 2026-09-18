#pragma once

#include "Entity.h"
#include "ofMain.h"

// A blueprint-style background entity: fills the scene with a solid colour
// and draws a white reference grid. Grid lines are drawn at every integer
// world unit, with thicker lines every 5 and 10 units. The X=0 and Y=0 axes
// are highlighted (white 5px with a black 1px core).
class Blueprint : public Entity {
public:
    void setup() override;
    void draw() override;

    // Zoom / scale property, kept in sync with the camera via a subscription.
    void setZoom(float zoom) { zoom_ = zoom; }
    float getZoom() const { return zoom_; }

private:
    // Thickness (in pixels) of a grid line at a given integer coordinate.
    int lineWidthFor(int coord) const;

    void drawGrid(float minX, float maxX, float minY, float maxY);
    void drawAxes(float minX, float maxX, float minY, float maxY);
    void drawWorldLine(const glm::vec2& a, const glm::vec2& b) const;

    // Called whenever the camera position/zoom changes.
    void onCameraChanged();

    ofColor backgroundColor_{30, 70, 140};

    float zoom_ = 1.0f;                    // synced from the camera
    glm::vec2 cameraPosition_{0.0f, 0.0f}; // synced from the camera
};
