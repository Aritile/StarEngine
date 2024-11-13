#pragma once

#include "../../STAR/Star.h"

struct RectangleComponent
{
private:
	bool activeComponent = true;
	Vector3 color;

public:
	void Render();

public:
	void SetActive(bool _Active);
	bool IsActive();

public:
	void SetColor(Vector3 _Color);
	Vector3 GetColor();
};