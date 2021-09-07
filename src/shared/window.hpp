#include "shared/inputmanager.hpp"
#include "utility/vector.hpp"

class CBobWindow
{
  public:
	// BobWindow(const char *title = "BobWindow", Vector size = Vector(800,600)); // SDL_CreateWindow
	// ~BobWindow(); // SDL_DestroyWindow

	// SDL_HideWindow / SDL_ShowWindow
	virtual bool IsVisible()		  = 0; // getter(?)
	virtual void SetVisible( bool v ) = 0; // setter(?)

	virtual bool IsFocused() = 0;

	// SDL_SetWindowTitle / SDL_GetWindowTitle
	virtual void SetTitle( const char *title ) = 0;
	virtual const char *GetTitle()			   = 0;

	// Z Ignored
	// SDL_SetWindowSize / SDL_GetWindowSize
	virtual CVector GetSize()		  = 0;
	virtual void SetSize( CVector s ) = 0;
	// Z Ignored
	// SDL_SetWindowPos / SDL_GetWindowPos
	virtual CVector GetPos()		 = 0;
	virtual void SetPos( CVector p ) = 0;

	// Returns the (milli)seconds per frame
	// To get the FPS, 1000/this
	virtual float GetSPF() = 0;

	// Poll events
	virtual void PollEvents() = 0;

	// Returns the milliseconds since the beginning of the program
	virtual unsigned int GetMS() = 0;

	bool m_bShouldClose = false;

	float m_fDelta = 0;

	// Places the mouse in the middle of the screen
	virtual void CaptureMouse() = 0;

	virtual void SwapBuffers() = 0;

	bool m_bSizeChanged = false;

	// Pointer to Input manager
	CInputManager *m_pInputMan = nullptr;
};