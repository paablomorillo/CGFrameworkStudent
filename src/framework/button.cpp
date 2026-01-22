#include "button.h"
#include <iostream>

Button::Button(const char* filename, const Vector2& position, ButtonType type)
{
    // Load icon (check return value)
    if (!image.LoadPNG(filename))
        std::cout << "Error loading button image: " << filename << std::endl;

    // Top-left position in framebuffer coords
    this->position = position;

    // Action id (tool/color/clear/load/save)
    this->type = type;
}

void Button::Render(Image& framebuffer) const
{
    // Draw icon
    if (image.pixels)
        framebuffer.DrawImage(image, (int)position.x, (int)position.y);
}

bool Button::IsMouseInside(const Vector2& mousePosition) const
{
    // Point vs rect hit test
    return mousePosition.x >= position.x &&
        mousePosition.x < (position.x + (float)image.width) &&
        mousePosition.y >= position.y &&
        mousePosition.y < (position.y + (float)image.height);
}
