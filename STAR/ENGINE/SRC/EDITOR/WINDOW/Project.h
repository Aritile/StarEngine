#pragma once

class ProjectWindow
{
public:
	void Render();

public:
	static ProjectWindow* GetSingleton();
};

ProjectWindow& ProjectClass();