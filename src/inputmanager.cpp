#include "inputmanager.h"

// Usually set/handled by the window

InputManager::InputManager()
{
	// Fill the array
	for (int i = 0; i < sizeof(keyboardState); i++)
		keyboardState[i] = false;
}
InputManager::~InputManager()
{

}

