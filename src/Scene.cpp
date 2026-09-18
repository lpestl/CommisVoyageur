#include "Scene.h"

Scene::Scene()
    : viewMin_(-50.0f)
    , viewMax_(50.0f)
    , pixelsPerUnit_(1.0f)
    , origin_(0.0f, 0.0f)
{
}

void Scene::setup() {
    recomputeTransform();

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

void Scene::setViewRange(float minX, float maxX) {
    viewMin_ = minX;
    viewMax_ = maxX;
    recomputeTransform();
}

void Scene::recomputeTransform() {
    const float worldWidth = viewMax_ - viewMin_;
    const int screenWidth = ofGetWidth();
    const int screenHeight = ofGetHeight();

    pixelsPerUnit_ = screenWidth / worldWidth;
    // World (0,0) sits exactly at the centre of the window.
    origin_ = glm::vec2(screenWidth / 2.0f, screenHeight / 2.0f);
}

glm::vec2 Scene::worldToScreen(const glm::vec2& world) const {
    const float x = origin_.x + world.x * pixelsPerUnit_;
    // Negate y so that positive world-y points upward on screen.
    const float y = origin_.y - world.y * pixelsPerUnit_;
    return glm::vec2(x, y);
}

glm::vec2 Scene::screenToWorld(const glm::vec2& screen) const {
    const float x = (screen.x - origin_.x) / pixelsPerUnit_;
    const float y = (origin_.y - screen.y) / pixelsPerUnit_;
    return glm::vec2(x, y);
}
