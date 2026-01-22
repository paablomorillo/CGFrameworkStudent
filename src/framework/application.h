/*
	+ This class encapsulates the application, is in charge of creating the data, getting the user input, process the update and render.
*/

#pragma once

#include "main/includes.h"
#include "framework.h"
#include "image.h"
#include <vector>
#include <algorithm> // std::min/std::max
#include "button.h"
#include "particle_system.h"

class Application
{
public:

	// Window
	SDL_Window* window = nullptr;
	int window_width;
	int window_height;

	float time;

	// Input (updated by the framework main loop)
	const Uint8* keystate;
	int mouse_state;          // Which mouse buttons are pressed
	Vector2 mouse_position;   // Current mouse position
	Vector2 mouse_delta;      // Mouse movement in last frame

	void OnKeyPressed(SDL_KeyboardEvent event);
	void OnMouseButtonDown(SDL_MouseButtonEvent event);
	void OnMouseButtonUp(SDL_MouseButtonEvent event);
	void OnMouseMove(SDL_MouseButtonEvent event);
	void OnWheel(SDL_MouseWheelEvent event);
	void OnFileChanged(const char* filename);

	// CPU framebuffer (canvas)
	Image framebuffer;

	// ---- Lab1 state ----
	enum AppMode { MODE_PAINT, MODE_ANIMATION };
	enum Tool { TOOL_PENCIL, TOOL_ERASER, TOOL_LINE, TOOL_RECT, TOOL_TRIANGLE };

	AppMode mode = MODE_PAINT;
	Tool tool = TOOL_PENCIL;

	bool isFilled = false;
	int borderWidth = 2;

	Color drawingColor = Color::WHITE;

	// Toolbar UI
	std::vector<Button> toolbarButtons;
	bool clickedOnToolbarButton = false;

	// Base canvas for previews (line/rect)
	Image tempbuffer;

	// Drawing state
	bool mouseDown = false;
	Vector2 startPos;                          // Drag start for line/rect
	Vector2 lastMousePosition = Vector2(-1, -1); // For pencil/eraser continuous stroke

	static constexpr int MIN_BORDER_WIDTH = 1;
	static constexpr int MAX_BORDER_WIDTH = 64;

	// Triangle tool (3 clicks)
	std::vector<Vector2> triPoints;

	// Particle system (animation mode)
	ParticleSystem particleSystem;

	// Toolbar action handler
	void HandleButton(ButtonType type);

	// Constructor and main methods
	Application(const char* caption, int width, int height);
	~Application();

	void Init(void);
	void Render(void);
	void Update(float dt);

	// Other methods to control the app
	void SetWindowSize(int width, int height) {
		glViewport(0, 0, width, height);
		this->window_width = width;
		this->window_height = height;
		this->framebuffer.Resize(width, height);
	}

	Vector2 GetWindowSize()
	{
		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		return Vector2(float(w), float(h));
	}
};
