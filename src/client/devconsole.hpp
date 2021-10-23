// Devconsole
// Quake-style console to enter commands and change variables

#include "gui/gui.hpp"
#include "cvar_clientside.hpp"

// The text input buffer we use
#define DEVCONSOLE_TEXTBUFFER 909

class CDevConsole
{
	public:
		CDevConsole();
		~CDevConsole();

		void Update();

		bool m_bVisible = false;
		CGui *m_pGui = nullptr;
		CInputManager *m_pInputMan = nullptr;
};