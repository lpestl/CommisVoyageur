#pragma once

#include "ofMain.h"
#include "Entity.h"

#include <memory>
#include <vector>

// A Scene is a 2D coordinate system that owns and renders all entities
// added to it. World coordinate (0,0) is placed at the centre of the screen,
// and the horizontal extent of the view is, by default, [-50, 50] units.
class Scene {
public:
    Scene();

    // Initialises the coordinate transform and sets up every entity.
    void setup();

    // Called every frame; updates then draws all entities.
    void update();
    void draw();

    // Registers an entity with this scene. The entity's scene pointer is set
    // automatically, and its setup() is invoked immediately if the scene is
    // already set up.
    void addEntity(std::shared_ptr<Entity> entity);

    // Coordinate transforms.
    // worldToScreen: world units -> screen pixels (y flipped: +y points up).
    // screenToWorld: screen pixels -> world units.
    glm::vec2 worldToScreen(const glm::vec2& world) const;
    glm::vec2 screenToWorld(const glm::vec2& screen) const;

    // Sets the horizontal world range of the view and recalculates the scale.
    void setViewRange(float minX, float maxX);

    float getViewMin() const { return viewMin_; }
    float getViewMax() const { return viewMax_; }
    float getPixelsPerUnit() const { return pixelsPerUnit_; }

private:
    void recomputeTransform();

    std::vector<std::shared_ptr<Entity>> entities_;

    // Coordinate-system state.
    float viewMin_;       // left  world boundary (default -50)
    float viewMax_;       // right world boundary (default  50)
    float pixelsPerUnit_; // screen pixels per one world unit
    glm::vec2 origin_;    // screen position of world (0,0)

    bool setup_ = false;
};
