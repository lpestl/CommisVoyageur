#pragma once

#include "Entity.h"
#include "ofMain.h"

// A blueprint-style background entity: fills the scene with a solid colour
// and draws a white reference grid. Grid lines are drawn at every integer
// world unit, with thicker lines every 5 and 10 units.
class Blueprint : public Entity {
public:
    void setup() override;
    void draw() override;

private:
    // Thickness (in pixels) of a grid line at a given integer coordinate.
    int lineWidthFor(int coord) const;

    ofColor backgroundColor_{30, 70, 140};
};
