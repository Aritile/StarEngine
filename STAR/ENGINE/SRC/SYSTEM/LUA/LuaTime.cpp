#include "LuaTime.h"
#include "../../GAME/Game.h"

static Game* game = Game::GetSingleton();

void LuaTime::LuaAdd(sol::state& state)
{
	state["Time"] = sol::new_table();
	state["Time"]["FrameTime"] = []() { return game->GetFrameTime();     };
	state["Time"]["DeltaTime"] = []() { return game->GetDeltaTime();     };
	state["Time"]["ElapsedTime"] = []() { return game->GetElapsedTime(); };
	state["Time"]["FrameCount"] = []() { return game->GetFrameCount();   };
}