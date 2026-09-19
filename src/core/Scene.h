#pragma once

#include "ofMain.h"
#include "Entity.h"
#include "Camera.h"
#include "../graph/Graph.h"

#include <memory>
#include <vector>

// A Scene is a 2D coordinate system that owns and renders all entities added
// to it. A Camera controls which part of the world is visible, providing the
// world<->screen transforms, zooming and panning. World (0,0) starts at the
// centre of the screen with a horizontal extent of [-50, 50] units.
class Scene {
public:
    Scene();

    // Initialises the camera and sets up every entity.
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

    // Screen pixels per one world unit at the current zoom.
    float getPixelsPerUnit() const;

    // The camera controlling this scene's view.
    Camera& getCamera() { return camera_; }
    const Camera& getCamera() const { return camera_; }

    // Mouse interaction (forwarded from ofApp).
    void mouseMoved(int x, int y);
    void mouseScrolled(int x, int y, float scrollX, float scrollY);
    void mousePressed(int x, int y, int button);
    void mouseDragged(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);

    // Keyboard interaction (forwarded from ofApp).
    void keyPressed(int key);

    // The graph owned by this scene (nodes + edges).
    graph::Graph& getGraph() { return graph_; }
    const graph::Graph& getGraph() const { return graph_; }

private:
    std::vector<std::shared_ptr<Entity>> entities_;
    Camera camera_;
    graph::Graph graph_;

    bool setup_ = false;

    // Panning state (middle mouse button).
    bool panning_ = false;
    glm::vec2 lastMouseScreen_{0.0f, 0.0f};
};
