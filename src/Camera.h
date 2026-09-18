#pragma once

#include "ofMain.h"

#include <functional>
#include <vector>

// A Camera describes which part of the Scene's 2D world is currently visible
// in the active window. It stores its position (world coordinates of the view
// centre), the visible width/height (derived from the window size), and a zoom
// (scale) factor. Other entities can subscribe to position/zoom changes.
class Camera {
public:
    using ChangeListener = std::function<void()>;

    Camera();

    // Position: world coordinates of the centre of the view.
    const glm::vec2& getPosition() const { return position_; }
    void setPosition(const glm::vec2& position);
    void moveBy(const glm::vec2& worldDelta);

    // Zoom / scale. 1.0 is the default view (100 world units across the window).
    float getZoom() const { return zoom_; }
    void setZoom(float zoom);

    // Visible area in world units, derived from the window size and zoom.
    float getWidth() const { return width_; }
    float getHeight() const { return height_; }

    // Recompute width/height from the current window size and zoom.
    void updateViewport();

    // Subscribe to position/zoom changes.
    void addChangeListener(ChangeListener listener);

private:
    void notifyChange();

    glm::vec2 position_{0.0f, 0.0f};
    float zoom_{1.0f};
    float width_{100.0f};
    float height_{100.0f};

    std::vector<ChangeListener> listeners_;
};
