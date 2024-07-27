#include "LuaQuaternion.h"
#include "../../XTK/MATH/SimpleMath.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void LuaQuaternion::LuaAdd(sol::state& state)
{
	sol::usertype<Quaternion> quaternion = state.new_usertype<Quaternion>(
		"Quaternion",
		sol::constructors<Quaternion(), Quaternion(float, float, float, float)>(),
		sol::meta_function::addition, [](const Quaternion& x, const Quaternion& y) { return x + y; },
		sol::meta_function::subtraction, [](const Quaternion& x, const Quaternion& y) { return x - y; },
		sol::meta_function::multiplication, [](const Quaternion& x, const Quaternion& y) { return x * y; },
		sol::meta_function::division, [](const Quaternion& x, const Quaternion& y) { return x / y; },
		sol::meta_function::unary_minus, [](const Quaternion& x) { return -x;                         }); // why not
	quaternion["CreateFromAxisAngle"] = [](const Vector3& x, const float y) { return Quaternion::CreateFromAxisAngle(x, y); };
	quaternion["CreateFromRotationMatrix"] = [](const Matrix& x) { return Quaternion::CreateFromRotationMatrix(x); };
	quaternion["CreateFromYawPitchRoll"] = [](const Vector3& x) { return Quaternion::CreateFromYawPitchRoll(x); };
	quaternion["RotateTowards"] = [](const Quaternion& x, const Quaternion& y, const float z) { Quaternion w; w.RotateTowards(y, z); return w; };
	quaternion["Identity"] = sol::var(Quaternion::Identity);
	quaternion["x"] = &Quaternion::x;
	quaternion["y"] = &Quaternion::y;
	quaternion["z"] = &Quaternion::z;
	quaternion["w"] = &Quaternion::w;
}