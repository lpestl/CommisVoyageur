#pragma once

#include "Entity.h"
#include "Shoulder.h"
#include "ofMain.h"

#include <array>

// Interactive visual entity. On top of Entity it owns a screen-space bounding
// box and the standard mouse interaction: hovering, selection, dragging to move
// and resizing through the eight "shoulder" handles drawn on the box's corners
// and edge midpoints.
class ofEntity : public Entity {
public:
    ofEntity();

    void setup() override;
    void update() override;

    void mouseMoved(int x, int y) override;
    void mousePressed(int x, int y, int button) override;
    void mouseDragged(int x, int y, int button) override;
    void mouseReleased(int x, int y, int button) override;

    // Draws the resize handles (only when selected AND hovered). Concrete
    // entities call this from their draw() after drawing themselves.
    virtual void postDraw();

    // Screen-space bounding box.
    const ofRectangle& getBoundingBox() const { return boundingBox_; }
    void setBoundingBox(const ofRectangle& box) { boundingBox_ = box; }

    bool isHovered() const { return hovered_; }
    bool isSelected() const { return selected_; }
    void setSelected(bool selected) { selected_ = selected; }

protected:
    // Called each frame so a subclass can derive the bounding box from its own
    // data (e.g. world-space position/size). Default: no-op.
    virtual void onWorldToBounds() {}

    // Called after the bounding box has been changed by user interaction so a
    // subclass can push the new box back to its own data. Default: no-op.
    virtual void onBoundsToWorld() {}

    ofRectangle boundingBox_;

private:
    enum class Interaction { None, Moving, Resizing };

    void updateShoulders();
    void updateCursor();
    void resizeActiveShoulder(int x, int y);

    bool hovered_ = false;
    bool selected_ = false;

    Interaction interaction_ = Interaction::None;
    glm::vec2 dragStartMouse_{0.0f, 0.0f};
    ofRectangle dragStartBounds_;

    std::array<Shoulder, 8> shoulders_;
    Shoulder* activeShoulder_ = nullptr;
};
