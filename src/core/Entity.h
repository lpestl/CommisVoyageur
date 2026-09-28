#pragma once

#include "ofMain.h"

#include <string>

// Forward declaration: entities need access to the Scene (coordinate system)
// they belong to, but must not create a circular include.
class Scene;

// Abstract base class for every object that can live inside a Scene.
// All future entities (actors, widgets, decorations, etc.) derive from this.
class Entity {
public:
    enum class MouseEventResult {
        NotHandled,
        Handled
    };

    virtual ~Entity() = default;

    // Called once when the entity is added to an already-set-up scene,
    // or when the scene itself is set up.
    virtual void setup() {}

    // Called every frame before draw().
    virtual void update() {}

    // Called every frame; must be implemented by concrete entities.
    virtual void draw() = 0;

    // Mouse interaction, forwarded from the Scene. All coordinates are in
    // screen pixels. Default implementations do nothing; interactive entities
    // override the events they care about.
    virtual void mouseMoved(int x, int y) {}
    virtual void mouseDragged(int x, int y, int button) {}
    virtual void mousePressed(int x, int y, int button) {}
    virtual void mouseReleased(int x, int y, int button) {}
    virtual void mouseScrolled(int x, int y, float scrollX, float scrollY) {}

    // Called by Scene for a click on this visual entity. Returning NotHandled
    // lets Scene bubble the click to its next parent (ultimately Scene).
    // Subclasses can override either method to consume the event.
    virtual MouseEventResult onLeftMouseClicked(int x, int y) {
        std::cout << "Left mouse clicked on " << getName()
                      << " (x: " << x << "; y: " << y << ")" << std::endl;
        return MouseEventResult::NotHandled;
    }

    virtual MouseEventResult onRightMouseClicked(int x, int y) {
        std::cout << "Right mouse clicked on " << getName()
                      << " (x: " << x << "; y: " << y << ")" << std::endl;
        return MouseEventResult::NotHandled;
    }

    // A readable name used in the default click log. Override for a more
    // specific type name when needed.
    virtual std::string getName() const { return "Entity"; }

    // Injected by Scene::addEntity; provides world<->screen transforms.
    void setScene(Scene* scene) { scene_ = scene; }

protected:
    Scene* scene_ = nullptr;
};
