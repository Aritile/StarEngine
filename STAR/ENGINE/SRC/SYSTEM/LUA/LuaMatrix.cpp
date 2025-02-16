#include "LuaMatrix.h"
#include <SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

void LuaMatrix::LuaAdd(sol::state& state)
{
	sol::usertype<Matrix> matrix = state.new_usertype<Matrix>(
		"Matrix",
		sol::constructors<Matrix(), Matrix(
			float, float, float, float,
			float, float, float, float,
			float, float, float, float,
			float, float, float, float)>(),
		sol::meta_function::addition, [](const Matrix& x, const Matrix& y) { return x + y;                         },
		sol::meta_function::subtraction, [](const Matrix& x, const Matrix& y) { return x - y;                         },
		sol::meta_function::multiplication, [](const Matrix& x, const Matrix& y) { return x * y;                         },
		sol::meta_function::division, [](const Matrix& x, const Matrix& y) { return x / y;                         },
		sol::meta_function::unary_minus, [](const Matrix& x) { return -x;                            });
	matrix["Up"] = [](const Matrix& x) { return x.Up();                        };
	matrix["Down"] = [](const Matrix& x) { return x.Down();                      };
	matrix["Right"] = [](const Matrix& x) { return x.Right();                     };
	matrix["Left"] = [](const Matrix& x) { return x.Left();                      };
	matrix["Forward"] = [](const Matrix& x) { return x.Forward();                   };
	matrix["Backward"] = [](const Matrix& x) { return x.Backward();                  };
	matrix["Lerp"] = [](const Matrix& x, const Matrix& y, const float z) { return x.Lerp(x, y, z);               };
	matrix["CreateLookAt"] = [](const Vector3& x, const Vector3& y, const Vector3& z) { return Matrix::CreateLookAt(x, y, z); };
	matrix["CreateRotationX"] = [](const Matrix& x, const float y) { return x.CreateRotationX(y); };
	matrix["CreateRotationY"] = [](const Matrix& x, const float y) { return x.CreateRotationY(y); };
	matrix["CreateRotationZ"] = [](const Matrix& x, const float y) { return x.CreateRotationZ(y); };
	matrix["CreateTranslation"] = [](const Matrix& x, const Vector3& y) { return x.CreateTranslation(y); };
	matrix["CreateScale"] = [](const Matrix& x, const Vector3& y) { return x.CreateScale(y); };
	matrix["Invert"] = [](const Matrix& x) { return x.Invert(); };
	matrix["CreateFromAxisAngle"] = [](const Vector3& x, const float& y) { return Matrix::CreateFromAxisAngle(x, y); };
	matrix["CreateFromQuaternion"] = [](const Quaternion& x) { return Matrix::CreateFromQuaternion(x); };
	matrix["CreateFromYawPitchRoll"] = [](const Vector3& x) { return Matrix::CreateFromYawPitchRoll(x); };
	matrix["Identity"] = sol::var(Matrix::Identity);
	matrix["_11"] = &Matrix::_11;
	matrix["_12"] = &Matrix::_12;
	matrix["_13"] = &Matrix::_13;
	matrix["_14"] = &Matrix::_14;
	matrix["_21"] = &Matrix::_21;
	matrix["_22"] = &Matrix::_22;
	matrix["_23"] = &Matrix::_23;
	matrix["_24"] = &Matrix::_24;
	matrix["_31"] = &Matrix::_31;
	matrix["_32"] = &Matrix::_32;
	matrix["_33"] = &Matrix::_33;
	matrix["_34"] = &Matrix::_34;
	matrix["_41"] = &Matrix::_41;
	matrix["_42"] = &Matrix::_42;
	matrix["_43"] = &Matrix::_43;
	matrix["_44"] = &Matrix::_44;
}