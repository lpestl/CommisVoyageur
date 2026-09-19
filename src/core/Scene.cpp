#include "Scene.h"
#include "../graph/views/ofNode.h"

Scene::Scene() {
}

void Scene::setup() {
    camera_.updateViewport();

    setup_ = true;
    for (auto& entity : entities_) {
        entity->setup();
    }
}

void Scene::update() {
    for (auto& entity : entities_) {
        entity->update();
    }
}

void Scene::draw() {
    for (auto& entity : entities_) {
        entity->draw();
    }
}

void Scene::addEntity(std::shared_ptr<Entity> entity) {
    if (!entity) {
        return;
    }

    entity->setScene(this);
    entities_.push_back(entity);

    // If the scene is already running, bring the new entity up to date.
    if (setup_) {
        entity->setup();
    }
}

float Scene::getPixelsPerUnit() const {
    return (ofGetWidth() / 100.0f) * camera_.getZoom();
}

glm::vec2 Scene::worldToScreen(const glm::vec2& world) const {
    const float cx = ofGetWidth() / 2.0f;
    const float cy = ofGetHeight() / 2.0f;
    const float ppu = getPixelsPerUnit();
    const glm::vec2 pos = camera_.getPosition();

    return glm::vec2(
        cx + (world.x - pos.x) * ppu,
        cy - (world.y - pos.y) * ppu);
}

glm::vec2 Scene::screenToWorld(const glm::vec2& screen) const {
    const float cx = ofGetWidth() / 2.0f;
    const float cy = ofGetHeight() / 2.0f;
    const float ppu = getPixelsPerUnit();
    const glm::vec2 pos = camera_.getPosition();

    return glm::vec2(
        pos.x + (screen.x - cx) / ppu,
        pos.y - (screen.y - cy) / ppu);
}

// --- Mouse interaction -----------------------------------------------------

void Scene::mouseScrolled(int x, int y, float scrollX, float scrollY) {
    constexpr float zoomStep = 0.1f;

    // World point currently under the mouse cursor (before zooming).
    const glm::vec2 mouseScreen(x, y);
    const glm::vec2 worldUnderMouse = screenToWorld(mouseScreen);

    // Apply the new zoom.
    const float factor = 1.0f + scrollY * zoomStep;
    camera_.setZoom(camera_.getZoom() * factor);

    // Keep the point under the cursor fixed: recompute the camera position so
    // the same world point stays under the mouse after zooming.
    const float cx = ofGetWidth() / 2.0f;
    const float cy = ofGetHeight() / 2.0f;
    const float ppu = getPixelsPerUnit(); // reflects the new zoom

    const glm::vec2 newPosition(
        worldUnderMouse.x - (mouseScreen.x - cx) / ppu,
        worldUnderMouse.y + (mouseScreen.y - cy) / ppu);

    camera_.setPosition(newPosition);
}

void Scene::mouseMoved(int x, int y) {
    for (auto& entity : entities_) {
        entity->mouseMoved(x, y);
    }
}

void Scene::mousePressed(int x, int y, int button) {
    if (button == OF_MOUSE_BUTTON_MIDDLE) {
        panning_ = true;
        lastMouseScreen_ = glm::vec2(x, y);
    }

    for (auto& entity : entities_) {
        entity->mousePressed(x, y, button);
    }
}

void Scene::mouseDragged(int x, int y, int button) {
    if (panning_ && button == OF_MOUSE_BUTTON_MIDDLE) {
        const glm::vec2 current(x, y);
        const glm::vec2 deltaScreen = current - lastMouseScreen_;
        const float ppu = getPixelsPerUnit();

        // Move the camera opposite to the mouse so the content follows the
        // cursor (grab-and-drag). The y sign is inverted because screen +y
        // points down.
        camera_.moveBy(glm::vec2(-deltaScreen.x / ppu, deltaScreen.y / ppu));

        lastMouseScreen_ = current;
    }

    for (auto& entity : entities_) {
        entity->mouseDragged(x, y, button);
    }
}

void Scene::mouseReleased(int x, int y, int button) {
    if (button == OF_MOUSE_BUTTON_MIDDLE) {
        panning_ = false;
    }

    for (auto& entity : entities_) {
        entity->mouseReleased(x, y, button);
    }
}

void Scene::keyPressed(int key) {
    // Pressing 'N' creates a new node at the centre of the world (= centre of
    // the screen when the camera is at the origin).
    if (key == 'n' || key == 'N') {
        auto node = std::make_shared<graph::ofNode>(glm::vec2(0.0f, 0.0f));
        graph_.addNode(node);
        addEntity(node);
    }
}

void Scene::windowResized(int w, int h) {
    camera_.updateViewport();
}
