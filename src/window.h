#include "vector.h"

class BobWindow
{
	public:
		// BobWindow(const char *title = "BobWindow", Vector size = Vector(800,600)); // SDL_CreateWindow
		// ~BobWindow(); // SDL_DestroyWindow

		// SDL_HideWindow / SDL_ShowWindow
		virtual bool IsVisible() = 0; // getter(?)
		virtual void SetVisible(bool v) = 0; // setter(?)

		// SDL_SetWindowTitle / SDL_GetWindowTitle
		virtual void SetTitle(const char *title) = 0;
		virtual const char *GetTitle() = 0;

		// Z Ignored
		// SDL_SetWindowSize / SDL_GetWindowSize
		virtual Vector GetSize() = 0;
		virtual void SetSize(Vector s) = 0;
		// Z Ignored
		// SDL_SetWindowPos / SDL_GetWindowPos
		virtual Vector GetPos() = 0;
		virtual void SetPos(Vector p) = 0;

		// Poll events
		virtual void PollEvents() = 0;

		bool shouldClose = false;

		// Places the mouse in the middle of the screen
		virtual void CaptureMouse() = 0;
};