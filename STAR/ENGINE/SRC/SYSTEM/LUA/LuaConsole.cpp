#include "LuaConsole.h"
#include "../../EDITOR/WINDOW/Console.h"

static ConsoleWindow* consoleWindow = ConsoleWindow::GetSingleton();

void LuaConsole::LuaAdd(sol::state& state)
{
	state["Console"] = sol::new_table();
	state["Console"]["Info"] = [](const char* message) { consoleWindow->AddInfoMessage(message);       };
	state["Console"]["Warning"] = [](const char* message) { consoleWindow->AddWarningMessage(message); };
	state["Console"]["Error"] = [](const char* message) { consoleWindow->AddErrorMessage(message);     };
}