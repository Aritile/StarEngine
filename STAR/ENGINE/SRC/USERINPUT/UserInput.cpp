#include "UserInput.h"
#include "../ENGINE/Engine.h"
#include "../WINDOW/MainWindow.h"

#define DINPUT_VERSION 0x0800

static Engine* engine = Engine::GetSingleton();
static MainWindow* mainWindow = MainWindow::GetSingleton();

UserInput* UserInput::GetSingleton()
{
	static UserInput userInput;
	return &userInput;
}
bool UserInput::Init()
{
	if (FAILED(DirectInput8Create(engine->hInstance,
		DINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&gDirectInput,
		NULL)))
		return false;

	if (FAILED(gDirectInput->CreateDevice(GUID_SysKeyboard,
		&DIKeyboard,
		NULL)))
		return false;

	if (FAILED(gDirectInput->CreateDevice(GUID_SysMouse,
		&DIMouse,
		NULL)))
		return false;

	if (FAILED(DIKeyboard->SetDataFormat(&c_dfDIKeyboard)))
		return false;

	if (FAILED(DIKeyboard->SetCooperativeLevel(mainWindow->GetHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
		return false;

	if (FAILED(DIMouse->SetDataFormat(&c_dfDIMouse)))
		return false;

	if (FAILED(DIMouse->SetCooperativeLevel(mainWindow->GetHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
		return false;

	return true;
}
bool UserInput::GetKey(unsigned char key)
{
	if (DIKeyboard)
	{
		DIKeyboard->Acquire();
		uint8_t keyboardState[256];
		ZeroMemory(&keyboardState, sizeof(uint8_t));
		DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);
		if (keyboardState[key] & 0x80)
			return true;
	}

	return false;
}
Vector2 UserInput::GetMouse()
{
	if (DIMouse)
	{
		DIMouse->Acquire();
		DIMOUSESTATE mouseState;
		ZeroMemory(&mouseState, sizeof(DIMOUSESTATE));
		DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState);
		return Vector2((float)mouseState.lX, (float)mouseState.lY);
	}

	return Vector2::Zero;
}
bool UserInput::Acquire()
{
	if (DIKeyboard)
		if (FAILED(DIKeyboard->Acquire()))
			return false;

	if (DIMouse)
		if (FAILED(DIMouse->Acquire()))
			return false;

	return true;
}
void UserInput::Release()
{
	if (gDirectInput)
		gDirectInput->Release();
	if (DIKeyboard)
		DIKeyboard->Release();
	if (DIMouse)
		DIMouse->Release();
}