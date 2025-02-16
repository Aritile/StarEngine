#pragma once

#include <dinput.h>
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

class UserInput
{
public:
	static UserInput* GetSingleton();

public:
	bool Init();
	bool GetKey(unsigned char key);
	Vector2 GetMouse();
	bool Acquire();
	void Release();

private:
	LPDIRECTINPUT8 gDirectInput = NULL;
	IDirectInputDevice8* DIKeyboard = NULL;
	IDirectInputDevice8* DIMouse = NULL;
};