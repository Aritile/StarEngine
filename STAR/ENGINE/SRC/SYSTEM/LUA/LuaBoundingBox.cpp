#include "LuaBoundingBox.h"
#include "../../XTK/MATH/SimpleMath.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void LuaBoundingBox::LuaAdd(sol::state& state)
{
	sol::usertype<BoundingBox> boundingBox = state.new_usertype<BoundingBox>(
		"BoundingBox",
		sol::constructors<BoundingBox(), BoundingBox(Vector3, Vector3)>());
}