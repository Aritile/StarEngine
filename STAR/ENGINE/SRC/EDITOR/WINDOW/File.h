#pragma once

#include "../../IMGUI/imgui.h"

class FileWindow
{
public:
	void Render();
	bool renderFile = true;

public:
	static FileWindow* GetSingleton();
};