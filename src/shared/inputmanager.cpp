#include "inputmanager.h"

// Usually set/handled by the window on the client
// Or by the network in the case of the server

InputManager::InputManager()
{
#ifdef CLIENTEXE
	// Fill the array
	for (int i = 0; i < sizeof(keyboardState); i++)
		keyboardState[i] = false;
#endif
}
InputManager::~InputManager()
{

}

#ifdef CLIENTEXE
void InputManager::Update()
{
	for (int i = 0; i < INKEY_LAST; i++)
		inputState[i] = false;
	
	inputState[INKEY_FRONT] = keyboardState['W'];
	inputState[INKEY_LEFT] = keyboardState['A'];
	inputState[INKEY_BACK] = keyboardState['S'];
	inputState[INKEY_RIGHT] = keyboardState['D'];
}
#endif
