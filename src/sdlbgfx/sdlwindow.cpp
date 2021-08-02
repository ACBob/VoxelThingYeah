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
	SDL_SetRelativeMouseMode(SDL_TRUE);
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

void GameWindow::PollEvents()
{
	SDL_Event currentEvent;
	while(SDL_PollEvent(&currentEvent) != 0) {
		switch (currentEvent.type)
		{
			case SDL_QUIT: // Handle quitting directly
				shouldClose = true;
			break;
		}
	}
}

void GameWindow::CaptureMouse()
{
	Vector size = GetSize();
	SDL_WarpMouseInWindow(internalWindow.get(), size.x/2, size.y/2);
}