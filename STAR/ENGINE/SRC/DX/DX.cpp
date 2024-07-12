#include "DX.h"

DX* DX::GetSingleton()
{
	static DX dx;
	return &dx;
}