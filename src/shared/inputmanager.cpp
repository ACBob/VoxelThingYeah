#include "inputmanager.hpp"

// Usually set/handled by the window on the client
// Or by the network in the case of the server

CInputManager::CInputManager()
{
#ifdef CLIENTEXE
	// Fill the array
	for ( int i = 0; i < sizeof( keyboardState ) / sizeof( bool ); i++ )
		keyboardState[i] = false;
	for ( int i = 0; i < sizeof( inputState ) / sizeof( bool ); i++ )
		inputState[i] = false;
#endif
}
CInputManager::~CInputManager() {}

#ifdef CLIENTEXE
void CInputManager::Update()
{
	for ( int i = 0; i < INKEY_LAST; i++ )
	{
		oldInputState[i] = inputState[i];
		inputState[i]	 = false;
	}

	// TODO: Custom Controls (ConVars?)

	inputState[INKEY_FRONT] = keyboardState['W'];
	inputState[INKEY_LEFT]	= keyboardState['A'];
	inputState[INKEY_BACK]	= keyboardState['S'];
	inputState[INKEY_RIGHT] = keyboardState['D'];

	inputState[INKEY_CHAT] = keyboardState['T'];
	inputState[INKEY_OUT]  = keyboardState[KBD_ESCAPE];
}
#endif
