#pragma once

#include "ofMain.h"
#include "Cursor.h"

// A resize handle attached to the bounding box of an ofEntity. Handles are
// placed at the four corners and the four edge midpoints. Each is drawn as a
// 5x5 px square (a fixed screen size, unaffected by scene/entity scale) with a
// blue outline. Hovering a shoulder makes it opaque and requests a matching
// resize cursor.
class Shoulder {
public:
    // Which part of the bounding box this handle controls.
    enum class Position {
        TopLeft, Top, TopRight,
        Left, Right,
        BottomLeft, Bottom, BottomRight
    };

    Shoulder() { updateRect(); }
    explicit Shoulder(Position position) : position_(position) { updateRect(); }

    Position getPosition() const { return position_; }
    void setPosition(Position position) { position_ = position; }

    // Screen-space centre (px). The hit-test rect is kept in sync.
    void setCenter(const glm::vec2& center);

    const ofRectangle& getRect() const { return rect_; }

    bool isHovered() const { return hovered_; }
    bool isPressed() const { return pressed_; }

    // Mouse handlers (forwarded by the owning ofEntity).
    void mouseMoved(int x, int y);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);

    // Clears transient hover/press state (used when the handles are hidden).
    void resetInteraction() {
        hovered_ = false;
        pressed_ = false;
    }

    // Draws the square (opaque when hovered, semi-transparent otherwise) with
    // a blue outline.
    void draw();

    // The resize cursor to show while this handle is hovered.
    cursor::Type getCursor() const;

    // Which edges of the bounding box move when this handle is dragged.
    bool affectsLeft() const;
    bool affectsRight() const;
    bool affectsTop() const;
    bool affectsBottom() const;

private:
    void updateRect();

    Position position_ = Position::TopLeft;
    glm::vec2 center_{0.0f, 0.0f};
    float size_ = 5.0f;

    ofRectangle rect_;
    bool hovered_ = false;
    bool pressed_ = false;
};
