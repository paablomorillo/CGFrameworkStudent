#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h" 

Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	this->mouse_state = 0;
	this->time = 0.f;
	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(nullptr);

	this->framebuffer.Resize(w, h);
}

Application::~Application()
{
}

void Application::Init(void)
{
	std::cout << "Initiating app..." << std::endl;

	// Clear canvas and store a base copy for previews
	framebuffer.Fill(Color::BLACK);
	tempbuffer = framebuffer;

	// Toolbar icons (path relative to /images)
	std::vector<const char*> imagePaths = {
		"images/clear.png",
		"images/load.png",
		"images/save.png",
		"images/eraser.png",
		"images/pencil.png",
		"images/line.png",
		"images/rectangle.png",
		"images/triangle.png",
		"images/black.png",
		"images/white.png",
		"images/pink.png",
		"images/yellow.png",
		"images/red.png",
		"images/blue.png",
		"images/cyan.png"
	};

	// What each button does
	std::vector<ButtonType> buttonTypes = {
		BTN_CLEAR, BTN_LOAD, BTN_SAVE,
		BTN_ERASER, BTN_PENCIL,
		BTN_LINE, BTN_RECTANGLE, BTN_TRIANGLE,
		BTN_BLACK, BTN_WHITE, BTN_PINK, BTN_YELLOW, BTN_RED, BTN_BLUE, BTN_CYAN
	};

	int toolbarIndexX = 10;

	for (size_t i = 0; i < imagePaths.size(); ++i)
	{
		// Create button and place it in a horizontal row
		toolbarButtons.emplace_back(imagePaths[i], Vector2(toolbarIndexX, 10), buttonTypes[i]);
		toolbarIndexX += toolbarButtons.back().GetImage().width + 10;
	}
}


// Render one frame
void Application::Render(void)
{
	// In animation mode we clear every frame; in paint mode we keep the canvas
	if (mode == MODE_ANIMATION)
		framebuffer.Fill(Color::BLACK);

	// Toolbar background
	framebuffer.DrawRect(0, 0, window_width, 50, Color::GRAY, 2, true, Color::GRAY);

	// Draw toolbar icons
	for (Button& b : toolbarButtons)
		b.Render(framebuffer);

	// Draw particles only in animation mode
	if (mode == MODE_ANIMATION)
		particleSystem.Render(&framebuffer);

	// Send framebuffer to screen
	framebuffer.Render();
}


// Called after render
void Application::Update(float seconds_elapsed)
{
	// Update particles using dt (seconds)
	if (mode == MODE_ANIMATION)
		particleSystem.Update(seconds_elapsed, window_width, window_height);
}

//keyboard press event 
void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
	// KEY CODES: [https://wiki.libsdl.org/SDL2/SDL_Keycode](https://wiki.libsdl.org/SDL2/SDL_Keycode)

	/* Keystoke Console Debug */
	std::cout << "Key pressed: " << event.keysym.sym << std::endl;
	std::cout << borderWidth << std::endl;

	switch (event.keysym.sym)
	{
	case SDLK_ESCAPE: exit(0); break;

	case SDLK_1:
		// Paint mode
		mode = MODE_PAINT;
		break;

	case SDLK_2:
		// Animation mode
		mode = MODE_ANIMATION;
		particleSystem.Init(window_width, window_height);
		break;

	case SDLK_f:
		// Toggle fill for rect/triangle
		isFilled = !isFilled;
		break;

	case SDLK_PLUS:
	case SDLK_KP_PLUS:
		// Increase border width
		borderWidth = std::min(borderWidth + 1, 64);
		break;

	case SDLK_MINUS:
	case SDLK_KP_MINUS:
		// Decrease border width
		borderWidth = std::max(borderWidth - 1, 1);
		break;
	}
}


void Application::OnMouseButtonDown(SDL_MouseButtonEvent event)
{
	// Mouse down: toolbar click OR start drawing 
	if (event.button != SDL_BUTTON_LEFT) return;

	clickedOnToolbarButton = false;

	// Check toolbar first
	for (Button& b : toolbarButtons)
	{
		if (b.IsMouseInside(mouse_position))
		{
			clickedOnToolbarButton = true;
			HandleButton(b.GetType());
			return;
		}
	}

	// Only draw in paint mode
	if (mode != MODE_PAINT) return;

	mouseDown = true;
	startPos = mouse_position;

	// Save current canvas for preview tools (line/rect)
	tempbuffer = framebuffer;
}


void Application::OnMouseButtonUp(SDL_MouseButtonEvent event)
{
	// Mouse up: finish current action
	if (event.button != SDL_BUTTON_LEFT) return;

	mouseDown = false;
	lastMousePosition = Vector2(-1, -1);

	if (mode != MODE_PAINT) return;
	if (clickedOnToolbarButton) return;

	// Commit final shape
	if (tool == TOOL_LINE)
	{
		framebuffer = tempbuffer;
		framebuffer.DrawLineDDA((int)startPos.x, (int)startPos.y,
			(int)mouse_position.x, (int)mouse_position.y,
			drawingColor);
		tempbuffer = framebuffer;
	}
	else if (tool == TOOL_RECT)
	{
		framebuffer = tempbuffer;

		int x0 = (int)startPos.x;
		int y0 = (int)startPos.y;
		int x1 = (int)mouse_position.x;
		int y1 = (int)mouse_position.y;

		// Normalize drag to always get positive size
		int rx = std::min(x0, x1);
		int ry = std::min(y0, y1);
		int rw = std::abs(x1 - x0);
		int rh = std::abs(y1 - y0);

		framebuffer.DrawRect(rx, ry, rw, rh, drawingColor, borderWidth, isFilled, drawingColor);
		tempbuffer = framebuffer;
	}
	else if (tool == TOOL_TRIANGLE)
	{
		// Triangle by 3 clicks
		triPoints.push_back(mouse_position);

		if (triPoints.size() == 3)
		{
			// Filled: white border, otherwise same as draw color
			Color border = isFilled ? Color(255, 255, 255) : drawingColor;

			framebuffer.DrawTriangle(triPoints[0], triPoints[1], triPoints[2],
				border, isFilled, drawingColor);

			triPoints.clear();
			tempbuffer = framebuffer;
		}
	}
	else if (tool == TOOL_PENCIL || tool == TOOL_ERASER)
	{
		// Pencil/eraser already commit while dragging
		tempbuffer = framebuffer;
	}
}


void Application::HandleButton(ButtonType type)
{
	// Apply toolbar action (tool/color/clear/load/save)
	switch (type)
	{
	case BTN_PENCIL: tool = TOOL_PENCIL; break;
	case BTN_ERASER: tool = TOOL_ERASER; break;
	case BTN_LINE: tool = TOOL_LINE; break;
	case BTN_RECTANGLE: tool = TOOL_RECT; break;
	case BTN_TRIANGLE: tool = TOOL_TRIANGLE; break;

	case BTN_BLACK: drawingColor = Color::BLACK; break;
	case BTN_WHITE: drawingColor = Color::WHITE; break;
	case BTN_RED: drawingColor = Color::RED; break;
	case BTN_BLUE: drawingColor = Color::BLUE; break;
	case BTN_YELLOW: drawingColor = Color::YELLOW; break;
	case BTN_CYAN: drawingColor = Color::CYAN; break;
	case BTN_PINK: drawingColor = Color::PURPLE; break;

	case BTN_CLEAR:
		framebuffer.Fill(Color::BLACK);
		tempbuffer = framebuffer;
		break;

	case BTN_LOAD:
		framebuffer.LoadPNG("images/fruits.png"); // example
		tempbuffer = framebuffer;
		break;

	case BTN_SAVE:
		framebuffer.SaveTGA("images/out.tga");
		break;

	default: break;
	}
}


void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
	// Dragging: pencil/eraser draws segments, line/rect uses preview
	if (!mouseDown) return;
	if (mode != MODE_PAINT) return;
	if (clickedOnToolbarButton) return;

	if (tool == TOOL_PENCIL || tool == TOOL_ERASER)
	{
		Color c = (tool == TOOL_ERASER) ? Color::BLACK : drawingColor;

		// First move just sets the start point
		if (lastMousePosition.x < 0 || lastMousePosition.y < 0)
		{
			lastMousePosition = mouse_position;
			return;
		}

		framebuffer.DrawLineDDA((int)lastMousePosition.x, (int)lastMousePosition.y,
			(int)mouse_position.x, (int)mouse_position.y, c);

		lastMousePosition = mouse_position;
		tempbuffer = framebuffer;
		return;
	}

	// Preview tools: restore base canvas and draw temporary shape
	framebuffer = tempbuffer;

	if (tool == TOOL_LINE)
	{
		framebuffer.DrawLineDDA((int)startPos.x, (int)startPos.y,
			(int)mouse_position.x, (int)mouse_position.y, drawingColor);
	}
	else if (tool == TOOL_RECT)
	{
		int x0 = (int)startPos.x;
		int y0 = (int)startPos.y;
		int x1 = (int)mouse_position.x;
		int y1 = (int)mouse_position.y;

		int rx = std::min(x0, x1);
		int ry = std::min(y0, y1);
		int rw = std::abs(x1 - x0);
		int rh = std::abs(y1 - y0);

		framebuffer.DrawRect(rx, ry, rw, rh, drawingColor, borderWidth, isFilled, drawingColor);
	}
}


void Application::OnWheel(SDL_MouseWheelEvent event)
{
	float dy = event.preciseY;

	// Wheel changes border width
	if (dy > 0)
	{
		borderWidth = std::min(borderWidth + dy, (float)(MAX_BORDER_WIDTH));
	}
	else if (dy < 0)
	{
		borderWidth = std::max(borderWidth + dy, (float)(MIN_BORDER_WIDTH));
	}
}

void Application::OnFileChanged(const char* filename)
{
	Shader::ReloadSingleShader(filename);
}
