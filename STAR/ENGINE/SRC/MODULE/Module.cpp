#include "Module.h"
#include "../STAR/Star.h"

/**/

bool Module::Init()
{
	/*
	HMODULE hDLL = LoadLibrary(L"D:\\repos\\module\\x64\\Release\\module.dll");
	if (hDLL == NULL)
	{
		Star::AddLog("[Module] -> Failed to load Dynamic Link Library!");
		return false;
	}
	
	myFunc = (MYFUNC)GetProcAddress(hDLL, "UserInterface");
	if (myFunc == NULL)
	{
		Star::AddLog("[Module] -> Failed to get function address!");
		return false;
	}
	*/

	return true;
}

Module* Module::GetSingleton()
{
	static Module module;
	return &module;
}