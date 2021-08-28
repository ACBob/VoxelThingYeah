#include "inputmanager.hpp"

// Usually set/handled by the window on the client
// Or by the network in the case of the server

InputManager::InputManager()
{
#ifdef CLIENTEXE
	// Fill the array
	for (int i = 0; i < sizeof(keyboardState) / sizeof(bool); i++)
		keyboardState[i] = false;
	for (int i = 0; i < sizeof(inputState) / sizeof(bool); i++)
		inputState[i] = false;
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

	// TODO: Custom Controls (ConVars?)
	
	inputState[INKEY_FRONT] = keyboardState['W'];
	inputState[INKEY_LEFT] = keyboardState['A'];
	inputState[INKEY_BACK] = keyboardState['S'];
	inputState[INKEY_RIGHT] = keyboardState['D'];
}
#endif