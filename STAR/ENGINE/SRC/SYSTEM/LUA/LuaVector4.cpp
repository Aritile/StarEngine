#include "LuaVector4.h"
#include <SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

void LuaVector4::LuaAdd(sol::state& state)
{
	typedef Vector4 VECTORX;

	sol::usertype<VECTORX> vector = state.new_usertype<VECTORX>(
		"Vector4",
		sol::constructors<VECTORX(), VECTORX(float, float, float, float)>(),
		sol::meta_function::addition, [](const VECTORX& x, const VECTORX& y) { return x + y;                                       },
		sol::meta_function::subtraction, [](const VECTORX& x, const VECTORX& y) { return x - y;                                       },
		sol::meta_function::multiplication, [](const VECTORX& x, const VECTORX& y) { return x * y;                                       },
		sol::meta_function::division, [](const VECTORX& x, const VECTORX& y) { return x / y;                                       },
		sol::meta_function::addition, [](const VECTORX& x, const float y) { return VECTORX(x.x + y, x.y + y, x.z + y, x.w + y); },
		sol::meta_function::subtraction, [](const VECTORX& x, const float y) { return VECTORX(x.x - y, x.y - y, x.z - y, x.w - y); },
		sol::meta_function::multiplication, [](const VECTORX& x, const float y) { return VECTORX(x.x * y, x.y * y, x.z * y, x.w * y); },
		sol::meta_function::division, [](const VECTORX& x, const float y) { return VECTORX(x.x / y, x.y / y, x.z / y, x.w / y); },
		sol::meta_function::unary_minus, [](const VECTORX& x) { return -x;                                          });
	vector["Length"] = [](const VECTORX& x) { return x.Length();                                  };
	vector["Dot"] = [](const VECTORX& x, const VECTORX& y) { return x.Dot(y);                                    };
	vector["Cross"] = [](const VECTORX& x, const VECTORX& y) { return x.Cross(x, y);                               };
	vector["Normalize"] = [](VECTORX& x) { x.Normalize();    return x;                         };
	vector["Clamp"] = [](VECTORX& x, const VECTORX& y) { x.Clamp(x, y);    return x;                         };
	vector["Distance"] = [](const VECTORX& x, const VECTORX& y) { x.Distance(x, y); return x;                         };
	vector["Min"] = [](const VECTORX& x, const VECTORX& y) { x.Min(x, y);      return x;                         };
	vector["Max"] = [](const VECTORX& x, const VECTORX& y) { x.Max(x, y);      return x;                         };
	vector["Lerp"] = [](const VECTORX& x, const VECTORX& y, const float z) { x.Lerp(x, y, z);  return x;                         };
	vector["Zero"] = sol::var(VECTORX::Zero);
	vector["One"] = sol::var(VECTORX::One);
	vector["x"] = &VECTORX::x;
	vector["y"] = &VECTORX::y;
	vector["z"] = &VECTORX::z;
	vector["w"] = &VECTORX::w;
}