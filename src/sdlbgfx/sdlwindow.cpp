// Implements window with SDL

#include "sdlwindow.h"
#include <cstdio>

#include <memory>

GameWindow::GameWindow(const char *title, Vector size) :
	internalWindow(nullptr, &SDL_DestroyWindow)
{
	internalWindow.reset(SDL_CreateWindow("VoxelThingYeah",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		size.x, size.y,
		SDL_WINDOW_SHOWN));

	if (internalWindow == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n",
					SDL_GetError());
	}

	// SDL_VERSION(&wmi.version);

	// SDL_GetWindowWMInfo(internalWindow, &wmi);
	// SDL_SetRelativeMouseMode(SDL_TRUE);
}
GameWindow::~GameWindow()
{
}

bool GameWindow::IsVisible()
{
	return (SDL_GetWindowFlags(internalWindow.get()) & SDL_WINDOW_HIDDEN) == SDL_WINDOW_HIDDEN;
}
void GameWindow::SetVisible(bool visible)
{
	if (!visible)
		SDL_HideWindow(internalWindow.get());
	else
		SDL_ShowWindow(internalWindow.get());
}

const char* GameWindow::GetTitle()
{
	return SDL_GetWindowTitle(internalWindow.get());
}
void GameWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(internalWindow.get(), title);
}

Vector GameWindow::GetSize()
{
	int x, y;
	SDL_GetWindowSize(internalWindow.get(), &x, &y);
	return Vector(x,y);
}
void GameWindow::SetSize(Vector size)
{
	SDL_SetWindowSize(internalWindow.get(), size.x, size.y);
}

Vector GameWindow::GetPos()
{
	int x, y;
	SDL_GetWindowPosition(internalWindow.get(), &x, &y);
	return Vector(x,y);
}
void GameWindow::SetPos(Vector pos)
{
	SDL_SetWindowPosition(internalWindow.get(), pos.x, pos.y);
}

const int scancodeToStateIndex[] =
{
	SDL_SCANCODE_A, 'A',
    SDL_SCANCODE_B, 'B',
    SDL_SCANCODE_C, 'C',
    SDL_SCANCODE_D, 'D',
    SDL_SCANCODE_E, 'E',
    SDL_SCANCODE_F, 'F',
    SDL_SCANCODE_G, 'G',
    SDL_SCANCODE_H, 'H',
    SDL_SCANCODE_I, 'I',
    SDL_SCANCODE_J, 'J',
    SDL_SCANCODE_K, 'K',
    SDL_SCANCODE_L, 'L',
    SDL_SCANCODE_M, 'M',
    SDL_SCANCODE_N, 'N',
    SDL_SCANCODE_O, 'O',
    SDL_SCANCODE_P, 'P',
    SDL_SCANCODE_Q, 'Q',
    SDL_SCANCODE_R, 'R',
    SDL_SCANCODE_S, 'S',
    SDL_SCANCODE_T, 'T',
    SDL_SCANCODE_U, 'U',
    SDL_SCANCODE_V, 'V',
    SDL_SCANCODE_W, 'W',
    SDL_SCANCODE_X, 'X',
    SDL_SCANCODE_Y, 'Y',
    SDL_SCANCODE_Z, 'Z'
};

void GameWindow::PollEvents()
{
	inputMan->mouseMovement = Vector(0,0);

	SDL_Event currentEvent;
	while(SDL_PollEvent(&currentEvent) != 0) {
		switch (currentEvent.type)
		{
			case SDL_QUIT: // Handle quitting directly
				shouldClose = true;
			break;
			case SDL_MOUSEMOTION:
				inputMan->mouseMovement = Vector(currentEvent.motion.xrel, currentEvent.motion.yrel);
				inputMan->mousePos = Vector(0,0); // TODO:
			break;
		}
	}

	// Fill the input manager
	const uint8_t *state = SDL_GetKeyboardState(NULL);
	for (int i = -1; i < sizeof(scancodeToStateIndex); i+= 2)
	{
		int scanCode = scancodeToStateIndex[i];
		int convCode = scancodeToStateIndex[i+1];
		inputMan->keyboardState[convCode] = (state[scanCode] == 1);
	}
}

void GameWindow::CaptureMouse()
{
	Vector size = GetSize();
	SDL_WarpMouseInWindow(internalWindow.get(), size.x/2, size.y/2);
}