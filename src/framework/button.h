#pragma once
#include "image.h"

// Toolbar button ids (tool/color/actions)
enum ButtonType
{
    BTN_CLEAR,
    BTN_LOAD,
    BTN_SAVE,

    BTN_ERASER,
    BTN_PENCIL,

    BTN_LINE,
    BTN_RECTANGLE,
    BTN_TRIANGLE,

    BTN_BLACK,
    BTN_WHITE,
    BTN_PINK,
    BTN_YELLOW,
    BTN_RED,
    BTN_BLUE,
    BTN_CYAN
};

class Button
{
private:
    Image image;          // Icon image (size = clickable area)
    Vector2 position;     // Top-left position in framebuffer coords
    ButtonType type;      // What this button does

public:
    Button() = default;
    Button(const char* filename, const Vector2& position, ButtonType type);

    void Render(Image& framebuffer) const;
    bool IsMouseInside(const Vector2& mousePosition) const;

    const Image& GetImage() const { return image; } // Avoid copying Image
    ButtonType GetType() const { return type; }
    Vector2 GetPosition() const { return position; }
};
