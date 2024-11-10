#pragma once

struct OpacityComponent
{
private:
	float opacity = 1.0f;

public:
	void Render();

public:
	void SetOpacity(float _Opacity);
	float GetOpacity();
};