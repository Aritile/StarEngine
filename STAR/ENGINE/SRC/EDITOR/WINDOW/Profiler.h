#pragma once

#include <chrono>
#include <vector>

class ProfilerWindow
{
public:
	static ProfilerWindow* GetSingleton();
	bool renderProfiler = false;
	void Render();
};