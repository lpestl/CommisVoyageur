#include "Scene.h"

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
    const float factor = 1.0f + scrollY * zoomStep;
    camera_.setZoom(camera_.getZoom() * factor);
}

void Scene::mousePressed(int x, int y, int button) {
    if (button == OF_MOUSE_BUTTON_MIDDLE) {
        panning_ = true;
        lastMouseScreen_ = glm::vec2(x, y);
    }
}

void Scene::mouseDragged(int x, int y, int button) {
    if (!panning_ || button != OF_MOUSE_BUTTON_MIDDLE) {
        return;
    }

    const glm::vec2 current(x, y);
    const glm::vec2 deltaScreen = current - lastMouseScreen_;
    const float ppu = getPixelsPerUnit();

    // Move the camera opposite to the mouse so the content follows the cursor
    // (grab-and-drag). The y sign is inverted because screen +y points down.
    camera_.moveBy(glm::vec2(-deltaScreen.x / ppu, deltaScreen.y / ppu));

    lastMouseScreen_ = current;
}

void Scene::mouseReleased(int x, int y, int button) {
    if (button == OF_MOUSE_BUTTON_MIDDLE) {
        panning_ = false;
    }
}

void Scene::windowResized(int w, int h) {
    camera_.updateViewport();
}
