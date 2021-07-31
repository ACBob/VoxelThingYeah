#include "vector.h"

class BobWindow
{
	public:
		BobWindow(const char *title = "BobWindow", Vector size = Vector(800,600)); // SDL_CreateWindow
		~BobWindow(); // SDL_DestroyWindow

		// SDL_HideWindow / SDL_ShowWindow
		bool IsVisible(); // getter(?)
		void SetVisible(bool v); // setter(?)

		// SDL_SetWindowTitle / SDL_GetWindowTitle
		void SetTitle(const char *title);
		const char *GetTitle();

		// Z Ignored
		// SDL_SetWindowSize / SDL_GetWindowSize
		Vector GetSize();
		void SetSize(Vector s);
		// Z Ignored
		// SDL_SetWindowPos / SDL_GetWindowPos
		Vector GetPos();
		void SetPos(Vector p);

		// Pointer to native window
		void *nativeWindow;
		#ifdef __linux__
		// Pointer to native display type
		void *nativeDisplayType;
		#endif
};