#include "LuaVector3.h"
#include "../../XTK/MATH/SimpleMath.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void LuaVector3::LuaAdd(sol::state& state)
{
	typedef Vector3 VECTORX;

	sol::usertype<VECTORX> vector = state.new_usertype<VECTORX>(
		"Vector3",
		sol::constructors<VECTORX(), VECTORX(float, float, float)>(),
		sol::meta_function::addition, [](const VECTORX& x, const VECTORX& y) { return x + y;                              },
		sol::meta_function::subtraction, [](const VECTORX& x, const VECTORX& y) { return x - y;                              },
		sol::meta_function::multiplication, [](const VECTORX& x, const VECTORX& y) { return x * y;                              },
		sol::meta_function::division, [](const VECTORX& x, const VECTORX& y) { return x / y;                              },
		sol::meta_function::addition, [](const VECTORX& x, const float y) { return VECTORX(x.x + y, x.y + y, x.z + y); },
		sol::meta_function::subtraction, [](const VECTORX& x, const float y) { return VECTORX(x.x - y, x.y - y, x.z - y); },
		sol::meta_function::multiplication, [](const VECTORX& x, const float y) { return VECTORX(x.x * y, x.y * y, x.z * y); },
		sol::meta_function::division, [](const VECTORX& x, const float y) { return VECTORX(x.x / y, x.y / y, x.z / y); },
		sol::meta_function::unary_minus, [](const VECTORX& x) { return -x;                                 });
	vector["Length"] = [](const VECTORX& x) { return x.Length();                         };
	vector["Dot"] = [](const VECTORX& x, const VECTORX& y) { return x.Dot(y);                           };
	vector["Cross"] = [](const VECTORX& x, const VECTORX& y) { return x.Cross(y);                         };
	vector["Normalize"] = [](VECTORX& x) { x.Normalize();    return x;                };
	vector["Clamp"] = [](VECTORX& x, const VECTORX& y) { x.Clamp(x, y);    return x;                };
	vector["Distance"] = [](const VECTORX& x, const VECTORX& y) { x.Distance(x, y); return x;                };
	vector["Min"] = [](const VECTORX& x, const VECTORX& y) { x.Min(x, y);      return x;                };
	vector["Max"] = [](const VECTORX& x, const VECTORX& y) { x.Max(x, y);      return x;                };
	vector["Lerp"] = [](const VECTORX& x, const VECTORX& y, float z) { return Vector3::Lerp(x, y, z);             };
	vector["Zero"] = sol::var(VECTORX::Zero);
	vector["One"] = sol::var(VECTORX::One);
	vector["Up"] = sol::var(VECTORX::Up);
	vector["Down"] = sol::var(VECTORX::Down);
	vector["Right"] = sol::var(VECTORX::Right);
	vector["Left"] = sol::var(VECTORX::Left);
	vector["Forward"] = sol::var(VECTORX::Forward);
	vector["Backward"] = sol::var(VECTORX::Backward);
	vector["x"] = &VECTORX::x;
	vector["y"] = &VECTORX::y;
	vector["z"] = &VECTORX::z;
}