#include "LuaVector2.h"
#include <SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

void LuaVector2::LuaAdd(sol::state& state)
{
	typedef Vector2 VECTORX;

	sol::usertype<VECTORX> vector = state.new_usertype<VECTORX>(
		"Vector2",
		sol::constructors<VECTORX(), VECTORX(float, float)>(),
		sol::meta_function::addition, [](const VECTORX& x, const VECTORX& y) { return x + y;                     },
		sol::meta_function::subtraction, [](const VECTORX& x, const VECTORX& y) { return x - y;                     },
		sol::meta_function::multiplication, [](const VECTORX& x, const VECTORX& y) { return x * y;                     },
		sol::meta_function::division, [](const VECTORX& x, const VECTORX& y) { return x / y;                     },
		sol::meta_function::addition, [](const VECTORX& x, const float y) { return VECTORX(x.x + y, x.y + y); },
		sol::meta_function::subtraction, [](const VECTORX& x, const float y) { return VECTORX(x.x - y, x.y - y); },
		sol::meta_function::multiplication, [](const VECTORX& x, const float y) { return VECTORX(x.x * y, x.y * y); },
		sol::meta_function::division, [](const VECTORX& x, const float y) { return VECTORX(x.x / y, x.y / y); },
		sol::meta_function::unary_minus, [](const VECTORX& x) { return -x;                        });
	vector["Length"] = [](const VECTORX& x) { return x.Length();                };
	vector["Dot"] = [](const VECTORX& x, const VECTORX& y) { return x.Dot(y);                  };
	vector["Cross"] = [](const VECTORX& x, const VECTORX& y) { return x.Cross(y);                };
	vector["Normalize"] = [](VECTORX& x) { x.Normalize();    return x;       };
	vector["Clamp"] = [](VECTORX& x, const VECTORX& y) { x.Clamp(x, y);    return x;       };
	vector["Distance"] = [](const VECTORX& x, const VECTORX& y) { x.Distance(x, y); return x;       };
	vector["Min"] = [](const VECTORX& x, const VECTORX& y) { x.Min(x, y);      return x;       };
	vector["Max"] = [](const VECTORX& x, const VECTORX& y) { x.Max(x, y);      return x;       };
	vector["Lerp"] = [](const VECTORX& x, const VECTORX& y, float z) { x.Lerp(x, y, z);  return x;       };
	vector["Zero"] = sol::var(VECTORX::Zero);
	vector["One"] = sol::var(VECTORX::One);
	vector["x"] = &VECTORX::x;
	vector["y"] = &VECTORX::y;
}