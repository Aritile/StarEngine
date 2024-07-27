#include "LuaInput.h"
#include "../../GAME/Game.h"

static Game* game = Game::GetSingleton();

void LuaInput::LuaAdd(sol::state& state)
{
	state["Input"] = sol::new_table();

	/* Mouse */
	state["Input"]["HideCursor"] = [](bool value) { game->HideCursor(value); };
	state["Input"]["LockCursor"] = [](bool value) { game->LockCursor(value); };
	state["Input"]["IsCursorHidden"] = []() { return game->IsCursorHidden(); };
	state["Input"]["IsCursorLocked"] = []() { return game->IsCursorLocked(); };
	state["Input"]["GetCursorAxis"] = []() { return game->GetCursorAxis(); };
	state["Input"]["GetCursorAxisX"] = []() { return game->GetCursorAxis().x; };
	state["Input"]["GetCursorAxisY"] = []() { return game->GetCursorAxis().y; };

	/* Keyboard */
	state["Input"]["GetKeyDown"] = [](unsigned char key) { return game->InputGetKey(key); };
	state["Input"]["KeyCode"] = sol::new_table();

	state["Input"]["KeyCode"]["Q"] = DIK_Q;
	state["Input"]["KeyCode"]["W"] = DIK_W;
	state["Input"]["KeyCode"]["E"] = DIK_E;
	state["Input"]["KeyCode"]["R"] = DIK_R;
	state["Input"]["KeyCode"]["T"] = DIK_T;
	state["Input"]["KeyCode"]["Y"] = DIK_Y;
	state["Input"]["KeyCode"]["U"] = DIK_U;
	state["Input"]["KeyCode"]["I"] = DIK_I;
	state["Input"]["KeyCode"]["O"] = DIK_O;
	state["Input"]["KeyCode"]["P"] = DIK_P;

	state["Input"]["KeyCode"]["A"] = DIK_A;
	state["Input"]["KeyCode"]["S"] = DIK_S;
	state["Input"]["KeyCode"]["D"] = DIK_D;
	state["Input"]["KeyCode"]["F"] = DIK_F;
	state["Input"]["KeyCode"]["G"] = DIK_G;
	state["Input"]["KeyCode"]["H"] = DIK_H;
	state["Input"]["KeyCode"]["J"] = DIK_J;
	state["Input"]["KeyCode"]["K"] = DIK_K;
	state["Input"]["KeyCode"]["L"] = DIK_L;

	state["Input"]["KeyCode"]["Z"] = DIK_Z;
	state["Input"]["KeyCode"]["X"] = DIK_X;
	state["Input"]["KeyCode"]["C"] = DIK_C;
	state["Input"]["KeyCode"]["V"] = DIK_V;
	state["Input"]["KeyCode"]["B"] = DIK_B;
	state["Input"]["KeyCode"]["N"] = DIK_N;
	state["Input"]["KeyCode"]["M"] = DIK_M;

	state["Input"]["KeyCode"]["1"] = DIK_1;
	state["Input"]["KeyCode"]["2"] = DIK_2;
	state["Input"]["KeyCode"]["3"] = DIK_3;
	state["Input"]["KeyCode"]["4"] = DIK_4;
	state["Input"]["KeyCode"]["5"] = DIK_5;
	state["Input"]["KeyCode"]["6"] = DIK_6;
	state["Input"]["KeyCode"]["7"] = DIK_7;
	state["Input"]["KeyCode"]["8"] = DIK_8;
	state["Input"]["KeyCode"]["9"] = DIK_9;
	state["Input"]["KeyCode"]["0"] = DIK_0;

	state["Input"]["KeyCode"]["F1"] = DIK_F1;
	state["Input"]["KeyCode"]["F2"] = DIK_F2;
	state["Input"]["KeyCode"]["F3"] = DIK_F3;
	state["Input"]["KeyCode"]["F4"] = DIK_F4;
	state["Input"]["KeyCode"]["F5"] = DIK_F5;
	state["Input"]["KeyCode"]["F6"] = DIK_F6;
	state["Input"]["KeyCode"]["F7"] = DIK_F7;
	state["Input"]["KeyCode"]["F8"] = DIK_F8;
	state["Input"]["KeyCode"]["F9"] = DIK_F9;
	state["Input"]["KeyCode"]["F10"] = DIK_F10;
	state["Input"]["KeyCode"]["F11"] = DIK_F11;
	state["Input"]["KeyCode"]["F12"] = DIK_F12;

	state["Input"]["KeyCode"]["Up"] = DIK_UP;
	state["Input"]["KeyCode"]["Down"] = DIK_DOWN;
	state["Input"]["KeyCode"]["Left"] = DIK_LEFT;
	state["Input"]["KeyCode"]["Right"] = DIK_RIGHT;

	state["Input"]["KeyCode"]["LeftControl"] = DIK_LCONTROL;
	state["Input"]["KeyCode"]["RightControl"] = DIK_RCONTROL;

	state["Input"]["KeyCode"]["LeftShift"] = DIK_LSHIFT;
	state["Input"]["KeyCode"]["RightShift"] = DIK_RSHIFT;

	state["Input"]["KeyCode"]["LeftAlt"] = DIK_LALT;
	state["Input"]["KeyCode"]["RightAlt"] = DIK_RALT;
}