#include "ScriptingSystem.h"
#include "../HELPERS/Helpers.h"
#include "../EDITOR/WINDOW/Console.h"
#include <fstream>
#include <filesystem>
#include "../EDITOR/WINDOW/Assets.h"
#include "../ENTITY/COMPONENT/GeneralComponent.h"
#include "../GAME/Game.h"
#include "../ENTITY/COMPONENT/TransformComponent.h"
#include "../ENTITY/COMPONENT/CameraComponent.h"
#include "../ENTITY/COMPONENT/RigidbodyComponent.h"
#include "../ENTITY/COMPONENT/TextMeshComponent.h"
#include "ProjectSceneSystem.h"
#include "PlayerPrefs.h"

#define COMPONENT_ERROR "Failed to get %s because it was not found!"


ScriptingSystem* ScriptingSystem::GetSingleton()
{
	static ScriptingSystem scriptingSystem;
	return &scriptingSystem;
}

/* ----------------------------------- */

static ConsoleWindow* consoleWindow = ConsoleWindow::GetSingleton();
static Game* game = Game::GetSingleton();
static Entity* ecs = Entity::GetSingleton();
static ScriptingSystem* scriptingSystem = ScriptingSystem::GetSingleton();
static ProjectSceneSystem* projectSceneSystem = ProjectSceneSystem::GetSingleton();
static PlayerPrefs* playerPrefs = PlayerPrefs::GetSingleton();

bool ScriptingSystem::Init()
{
	/* libs */
	lua.open_libraries(sol::lib::base);
	lua.open_libraries(sol::lib::package);
	lua.open_libraries(sol::lib::coroutine);
	lua.open_libraries(sol::lib::string);
	lua.open_libraries(sol::lib::os);
	lua.open_libraries(sol::lib::math);
	lua.open_libraries(sol::lib::table);
	lua.open_libraries(sol::lib::debug);
	lua.open_libraries(sol::lib::bit32);
	lua.open_libraries(sol::lib::io);

	/* system */
	lua_add_vector2();
	lua_add_vector3();
	lua_add_vector4();
	lua_add_quaternion();
	lua_add_matrix();
	lua_add_console();
	lua_add_time();
	lua_add_input();
	lua_add_bounding_box();

	/* entity */
	lua_add_entity();
	lua_add_engine();

	GeneralComponent::LuaAdd(lua);
	TransformComponent::LuaAdd(lua);
	CameraComponent::LuaAdd(lua);
	RigidBodyComponent::LuaAdd(lua);
	MeshComponent::LuaAdd(lua);
	TextMeshComponent::LuaAdd(lua);
	PlayerPrefs::LuaAdd(lua);

	return true;
}

/* system */
void ScriptingSystem::lua_add_vector2()
{
	typedef Vector2 VECTORX;

	sol::usertype<VECTORX> vector = lua.new_usertype<VECTORX>(
		"Vector2",
		sol::constructors<VECTORX(), VECTORX(float, float)>(),
		sol::meta_function::addition,       [](const VECTORX& x, const VECTORX& y)          { return x + y;                     },
		sol::meta_function::subtraction,    [](const VECTORX& x, const VECTORX& y)          { return x - y;                     },
		sol::meta_function::multiplication, [](const VECTORX& x, const VECTORX& y)          { return x * y;                     },
		sol::meta_function::division,       [](const VECTORX& x, const VECTORX& y)          { return x / y;                     },
		sol::meta_function::addition,       [](const VECTORX& x, const float y)             { return VECTORX(x.x + y, x.y + y); },
		sol::meta_function::subtraction,    [](const VECTORX& x, const float y)             { return VECTORX(x.x - y, x.y - y); },
		sol::meta_function::multiplication, [](const VECTORX& x, const float y)             { return VECTORX(x.x * y, x.y * y); },
		sol::meta_function::division,       [](const VECTORX& x, const float y)             { return VECTORX(x.x / y, x.y / y); },
		sol::meta_function::unary_minus,    [](const VECTORX& x)                            { return -x;                        });
	vector["Length"]     =                  [](const VECTORX& x)                            { return x.Length();                };
	vector["Dot"]        =                  [](const VECTORX& x, const VECTORX& y)          { return x.Dot(y);                  };
	vector["Cross"]      =                  [](const VECTORX& x, const VECTORX& y)          { return x.Cross(y);                };
	vector["Normalize"]  =                  [](VECTORX& x)                                  { x.Normalize();    return x;       };
	vector["Clamp"]      =                  [](VECTORX& x, const VECTORX& y)                { x.Clamp(x, y);    return x;       };
	vector["Distance"]   =                  [](const VECTORX& x, const VECTORX& y)          { x.Distance(x, y); return x;       };
	vector["Min"]        =                  [](const VECTORX& x, const VECTORX& y)          { x.Min(x, y);      return x;       };
	vector["Max"]        =                  [](const VECTORX& x, const VECTORX& y)          { x.Max(x, y);      return x;       };
	vector["Lerp"]       =                  [](const VECTORX& x, const VECTORX& y, float z) { x.Lerp(x, y, z);  return x;       };
	vector["Zero"]       = sol::var(VECTORX::Zero);
	vector["One"]        = sol::var(VECTORX::One);
	vector["x"]          = &VECTORX::x;
	vector["y"]          = &VECTORX::y;
}
void ScriptingSystem::lua_add_vector3()
{
	typedef Vector3 VECTORX;

	sol::usertype<VECTORX> vector = lua.new_usertype<VECTORX>(
		"Vector3",
		sol::constructors<VECTORX(), VECTORX(float, float, float)>(),
		sol::meta_function::addition,       [](const VECTORX& x, const VECTORX& y)          { return x + y;                              },
		sol::meta_function::subtraction,    [](const VECTORX& x, const VECTORX& y)          { return x - y;                              },
		sol::meta_function::multiplication, [](const VECTORX& x, const VECTORX& y)          { return x * y;                              },
		sol::meta_function::division,       [](const VECTORX& x, const VECTORX& y)          { return x / y;                              },
		sol::meta_function::addition,       [](const VECTORX& x, const float y)             { return VECTORX(x.x + y, x.y + y, x.z + y); },
		sol::meta_function::subtraction,    [](const VECTORX& x, const float y)             { return VECTORX(x.x - y, x.y - y, x.z - y); },
		sol::meta_function::multiplication, [](const VECTORX& x, const float y)             { return VECTORX(x.x * y, x.y * y, x.z * y); },
		sol::meta_function::division,       [](const VECTORX& x, const float y)             { return VECTORX(x.x / y, x.y / y, x.z / y); },
		sol::meta_function::unary_minus,    [](const VECTORX& x)                            { return -x;                                 });
	vector["Length"]    =                   [](const VECTORX& x)                            { return x.Length();                         };
	vector["Dot"]       =                   [](const VECTORX& x, const VECTORX& y)          { return x.Dot(y);                           };
	vector["Cross"]     =                   [](const VECTORX& x, const VECTORX& y)          { return x.Cross(y);                         };
	vector["Normalize"] =                   [](VECTORX& x)                                  { x.Normalize();    return x;                };
	vector["Clamp"]     =                   [](VECTORX& x, const VECTORX& y)                { x.Clamp(x, y);    return x;                };
	vector["Distance"]  =                   [](const VECTORX& x, const VECTORX& y)          { x.Distance(x, y); return x;                };
	vector["Min"]       =                   [](const VECTORX& x, const VECTORX& y)          { x.Min(x, y);      return x;                };
	vector["Max"]       =                   [](const VECTORX& x, const VECTORX& y)          { x.Max(x, y);      return x;                };
	vector["Lerp"]      =                   [](const VECTORX& x, const VECTORX& y, float z) { x.Lerp(x, y, z);  return x;                };
	vector["Zero"]      = sol::var(VECTORX::Zero);
	vector["One"]       = sol::var(VECTORX::One);
	vector["Up"]        = sol::var(VECTORX::Up);
	vector["Down"]      = sol::var(VECTORX::Down);
	vector["Right"]     = sol::var(VECTORX::Right);
	vector["Left"]      = sol::var(VECTORX::Left);
	vector["Forward"]   = sol::var(VECTORX::Forward);
	vector["Backward"]  = sol::var(VECTORX::Backward);
	vector["x"]         = &VECTORX::x;
	vector["y"]         = &VECTORX::y;
	vector["z"]         = &VECTORX::z;
}
void ScriptingSystem::lua_add_vector4()
{
	typedef Vector4 VECTORX;

	sol::usertype<VECTORX> vector = lua.new_usertype<VECTORX>(
		"Vector4",
		sol::constructors<VECTORX(), VECTORX(float, float, float, float)>(),
		sol::meta_function::addition,       [](const VECTORX& x, const VECTORX& y)                { return x + y;                                       },
		sol::meta_function::subtraction,    [](const VECTORX& x, const VECTORX& y)                { return x - y;                                       },
		sol::meta_function::multiplication, [](const VECTORX& x, const VECTORX& y)                { return x * y;                                       },
		sol::meta_function::division,       [](const VECTORX& x, const VECTORX& y)                { return x / y;                                       },
		sol::meta_function::addition,       [](const VECTORX& x, const float y)                   { return VECTORX(x.x + y, x.y + y, x.z + y, x.w + y); },
		sol::meta_function::subtraction,    [](const VECTORX& x, const float y)                   { return VECTORX(x.x - y, x.y - y, x.z - y, x.w - y); },
		sol::meta_function::multiplication, [](const VECTORX& x, const float y)                   { return VECTORX(x.x * y, x.y * y, x.z * y, x.w * y); },
		sol::meta_function::division,       [](const VECTORX& x, const float y)                   { return VECTORX(x.x / y, x.y / y, x.z / y, x.w / y); },
		sol::meta_function::unary_minus,    [](const VECTORX& x)                                  { return -x;                                          });
	vector["Length"]    =                   [](const VECTORX& x)                                  { return x.Length();                                  };
	vector["Dot"]       =                   [](const VECTORX& x, const VECTORX& y)                { return x.Dot(y);                                    };
	vector["Cross"]     =                   [](const VECTORX& x, const VECTORX& y)                { return x.Cross(x, y);                               };
	vector["Normalize"] =                   [](VECTORX& x)                                        { x.Normalize();    return x;                         };
	vector["Clamp"]     =                   [](VECTORX& x, const VECTORX& y)                      { x.Clamp(x, y);    return x;                         };
	vector["Distance"]  =                   [](const VECTORX& x, const VECTORX& y)                { x.Distance(x, y); return x;                         };
	vector["Min"]       =                   [](const VECTORX& x, const VECTORX& y)                { x.Min(x, y);      return x;                         };
	vector["Max"]       =                   [](const VECTORX& x, const VECTORX& y)                { x.Max(x, y);      return x;                         };
	vector["Lerp"]      =                   [](const VECTORX& x, const VECTORX& y, const float z) { x.Lerp(x, y, z);  return x;                         };
	vector["Zero"]      = sol::var(VECTORX::Zero);
	vector["One"]       = sol::var(VECTORX::One);
	vector["x"]         = &VECTORX::x;
	vector["y"]         = &VECTORX::y;
	vector["z"]         = &VECTORX::z;
	vector["w"]         = &VECTORX::w;
}
void ScriptingSystem::lua_add_quaternion()
{
	sol::usertype<Quaternion> quaternion = lua.new_usertype<Quaternion>(
		"Quaternion",
		sol::constructors<Quaternion(), Quaternion(float, float, float, float)>(),
		sol::meta_function::addition,       [](const Quaternion& x, const Quaternion& y) { return x + y; },
		sol::meta_function::subtraction,    [](const Quaternion& x, const Quaternion& y) { return x - y; },
		sol::meta_function::multiplication, [](const Quaternion& x, const Quaternion& y) { return x * y; },
		sol::meta_function::division,       [](const Quaternion& x, const Quaternion& y) { return x / y; },
		sol::meta_function::unary_minus,    [](const Quaternion& x) { return -x;                         }); // why not
	quaternion["Identity"] = sol::var(Quaternion::Identity);
	quaternion["x"]        = &Quaternion::x;
	quaternion["y"]        = &Quaternion::y;
	quaternion["z"]        = &Quaternion::z;
	quaternion["w"]        = &Quaternion::w;
}
void ScriptingSystem::lua_add_matrix()
{
	sol::usertype<Matrix> matrix = lua.new_usertype<Matrix>(
		"Matrix",
		sol::constructors<Matrix(), Matrix(
			float, float, float, float,
			float, float, float, float,
			float, float, float, float,
			float, float, float, float)>(),
		sol::meta_function::addition,       [](const Matrix& x, const Matrix& y)                     { return x + y;                         },
		sol::meta_function::subtraction,    [](const Matrix& x, const Matrix& y)                     { return x - y;                         },
		sol::meta_function::multiplication, [](const Matrix& x, const Matrix& y)                     { return x * y;                         },
		sol::meta_function::division,       [](const Matrix& x, const Matrix& y)                     { return x / y;                         },
		sol::meta_function::unary_minus,    [](const Matrix& x)                                      { return -x;                            });
	matrix["Up"]       =                    [](const Matrix& x)                                      { return x.Up();                        };
	matrix["Down"]     =                    [](const Matrix& x)                                      { return x.Down();                      };
	matrix["Right"]    =                    [](const Matrix& x)                                      { return x.Right();                     };
	matrix["Left"]     =                    [](const Matrix& x)                                      { return x.Left();                      };
	matrix["Forward"]  =                    [](const Matrix& x)                                      { return x.Forward();                   };
	matrix["Backward"] =                    [](const Matrix& x)                                      { return x.Backward();                  };
	matrix["Lerp"]     =                    [](const Matrix& x, const Matrix& y, const float z)      { return x.Lerp(x, y, z);               };
	matrix["CreateLookAt"] =                [](const Vector3& x, const Vector3& y, const Vector3& z) { return Matrix::CreateLookAt(x, y, z); };
	matrix["Identity"] = sol::var(Matrix::Identity);
	matrix["_11"]      = &Matrix::_11;
	matrix["_12"]      = &Matrix::_12;
	matrix["_13"]      = &Matrix::_13;
	matrix["_14"]      = &Matrix::_14;
	matrix["_21"]      = &Matrix::_21;
	matrix["_22"]      = &Matrix::_22;
	matrix["_23"]      = &Matrix::_23;
	matrix["_24"]      = &Matrix::_24;
	matrix["_31"]      = &Matrix::_31;
	matrix["_32"]      = &Matrix::_32;
	matrix["_33"]      = &Matrix::_33;
	matrix["_34"]      = &Matrix::_34;
	matrix["_41"]      = &Matrix::_41;
	matrix["_42"]      = &Matrix::_42;
	matrix["_43"]      = &Matrix::_43;
	matrix["_44"]      = &Matrix::_44;
}
void ScriptingSystem::lua_add_console()
{
	lua["Console"] = sol::new_table();
	lua["Console"]["Info"]    = [](const char* message) { consoleWindow->AddInfoMessage(message);    };
	lua["Console"]["Warning"] = [](const char* message) { consoleWindow->AddWarningMessage(message); };
	lua["Console"]["Error"]   = [](const char* message) { consoleWindow->AddErrorMessage(message);   };
}
void ScriptingSystem::lua_add_time()
{
	lua["Time"] = sol::new_table();
	lua["Time"]["FrameTime"]   = []() { return game->GetFrameTime();   };
	lua["Time"]["DeltaTime"]   = []() { return game->GetDeltaTime();   };
	lua["Time"]["ElapsedTime"] = []() { return game->GetElapsedTime(); };
	lua["Time"]["FrameCount"]  = []() { return game->GetFrameCount();  };
}
void ScriptingSystem::lua_add_input()
{
	lua["Input"] = sol::new_table();

	/* Mouse */
	//lua["Input"]["ShowCursor"] = []() { StarHelpers::ShowCursor(true); };
	//lua["Input"]["HideCursor"] = []() { StarHelpers::ShowCursor(false); };
	lua["Input"]["HideCursor"]     = [](bool value) { game->HideCursor(value); };
	lua["Input"]["LockCursor"]     = [](bool value) { game->LockCursor(value); };
	lua["Input"]["IsCursorHidden"] = []() { return game->IsCursorHidden(); };
	lua["Input"]["IsCursorLocked"] = []() { return game->IsCursorLocked(); };
	lua["Input"]["GetCursorAxis"]  = []() { return game->GetCursorAxis(); };
	lua["Input"]["GetCursorAxisX"] = []() { return game->GetCursorAxis().x; };
	lua["Input"]["GetCursorAxisY"] = []() { return game->GetCursorAxis().y; };

	/* Keyboard */
	lua["Input"]["GetKeyDown"] = [](unsigned char key) { return game->InputGetKey(key); };
	lua["Input"]["KeyCode"] = sol::new_table();

	lua["Input"]["KeyCode"]["Q"] = DIK_Q;
	lua["Input"]["KeyCode"]["W"] = DIK_W;
	lua["Input"]["KeyCode"]["E"] = DIK_E;
	lua["Input"]["KeyCode"]["R"] = DIK_R;
	lua["Input"]["KeyCode"]["T"] = DIK_T;
	lua["Input"]["KeyCode"]["Y"] = DIK_Y;
	lua["Input"]["KeyCode"]["U"] = DIK_U;
	lua["Input"]["KeyCode"]["I"] = DIK_I;
	lua["Input"]["KeyCode"]["O"] = DIK_O;
	lua["Input"]["KeyCode"]["P"] = DIK_P;

	lua["Input"]["KeyCode"]["A"] = DIK_A;
	lua["Input"]["KeyCode"]["S"] = DIK_S;
	lua["Input"]["KeyCode"]["D"] = DIK_D;
	lua["Input"]["KeyCode"]["F"] = DIK_F;
	lua["Input"]["KeyCode"]["G"] = DIK_G;
	lua["Input"]["KeyCode"]["H"] = DIK_H;
	lua["Input"]["KeyCode"]["J"] = DIK_J;
	lua["Input"]["KeyCode"]["K"] = DIK_K;
	lua["Input"]["KeyCode"]["L"] = DIK_L;

	lua["Input"]["KeyCode"]["Z"] = DIK_Z;
	lua["Input"]["KeyCode"]["X"] = DIK_X;
	lua["Input"]["KeyCode"]["C"] = DIK_C;
	lua["Input"]["KeyCode"]["V"] = DIK_V;
	lua["Input"]["KeyCode"]["B"] = DIK_B;
	lua["Input"]["KeyCode"]["N"] = DIK_N;
	lua["Input"]["KeyCode"]["M"] = DIK_M;

	lua["Input"]["KeyCode"]["1"] = DIK_1;
	lua["Input"]["KeyCode"]["2"] = DIK_2;
	lua["Input"]["KeyCode"]["3"] = DIK_3;
	lua["Input"]["KeyCode"]["4"] = DIK_4;
	lua["Input"]["KeyCode"]["5"] = DIK_5;
	lua["Input"]["KeyCode"]["6"] = DIK_6;
	lua["Input"]["KeyCode"]["7"] = DIK_7;
	lua["Input"]["KeyCode"]["8"] = DIK_8;
	lua["Input"]["KeyCode"]["9"] = DIK_9;
	lua["Input"]["KeyCode"]["0"] = DIK_0;

	lua["Input"]["KeyCode"]["F1"] = DIK_F1;
	lua["Input"]["KeyCode"]["F2"] = DIK_F2;
	lua["Input"]["KeyCode"]["F3"] = DIK_F3;
	lua["Input"]["KeyCode"]["F4"] = DIK_F4;
	lua["Input"]["KeyCode"]["F5"] = DIK_F5;
	lua["Input"]["KeyCode"]["F6"] = DIK_F6;
	lua["Input"]["KeyCode"]["F7"] = DIK_F7;
	lua["Input"]["KeyCode"]["F8"] = DIK_F8;
	lua["Input"]["KeyCode"]["F9"] = DIK_F9;
	lua["Input"]["KeyCode"]["F10"] = DIK_F10;
	lua["Input"]["KeyCode"]["F11"] = DIK_F11;
	lua["Input"]["KeyCode"]["F12"] = DIK_F12;

	lua["Input"]["KeyCode"]["Up"] = DIK_UP;
	lua["Input"]["KeyCode"]["Down"] = DIK_DOWN;
	lua["Input"]["KeyCode"]["Left"] = DIK_LEFT;
	lua["Input"]["KeyCode"]["Right"] = DIK_RIGHT;

	lua["Input"]["KeyCode"]["LeftControl"] = DIK_LCONTROL;
	lua["Input"]["KeyCode"]["RightControl"] = DIK_RCONTROL;

	lua["Input"]["KeyCode"]["LeftShift"] = DIK_LSHIFT;
	lua["Input"]["KeyCode"]["RightShift"] = DIK_RSHIFT;

	lua["Input"]["KeyCode"]["LeftAlt"] = DIK_LALT;
	lua["Input"]["KeyCode"]["RightAlt"] = DIK_RALT;
}
void ScriptingSystem::lua_add_bounding_box()
{
	using namespace DirectX;

	sol::usertype<BoundingBox> boundingBox = lua.new_usertype<BoundingBox>(
		"BoundingBox",
		sol::constructors<BoundingBox(), BoundingBox(Vector3, Vector3)>());
}

/* entity */
void ScriptingSystem::lua_add_entity()
{
	sol::usertype<EntityX> entity = lua.new_usertype<EntityX>(
		"Entity");
	entity["CreateEntity"] = &EntityX::CreateEntity;
	entity["AddComponent"] = &EntityX::AddComponent;
	entity["GetComponent"] = &EntityX::GetComponent;
	entity["HasComponent"] = &EntityX::HasComponent;
	entity["RemoveComponent"] = &EntityX::RemoveComponent;
}
void ScriptingSystem::lua_add_engine()
{
	lua["Engine"] = sol::new_table();
	lua["Engine"]["FindByName"] = [](const char* name) { return GetSingleton()->FindByName(name); };
	lua["Engine"]["FindByTag"] = [](const char* tag) { return GetSingleton()->FindByTag(tag); };
	lua["Engine"]["LoadScene"] = [](const char* path) { projectSceneSystem->OpenScene(path); };
}
void ScriptingComponent::AddScript(const char* path)
{
	std::filesystem::path file(path);
	if (file.extension().string() != LUA)
		return;

	for (size_t i = 0; i < scripts.size(); i++)
		if (scripts[i].fileName == file.stem().string())
			return;

	scriptingSystem->CompileScript(path);
	ScriptComponent scriptComponent;

	{
		scriptComponent.checksum = StarHelpers::GetChecksum(path);

		// old
		//scriptBuffer.filePath = path;

		// new
		std::string exe = StarHelpers::GetParent(StarHelpers::GetExecutablePath());
		std::string model = StarHelpers::GetParent(path);
		std::string dir = StarHelpers::GetRelativePath(model, exe);
		std::string full = dir + "\\" + StarHelpers::GetFileNameFromPath(path) + StarHelpers::GetFileExtensionFromPath(path);
		scriptComponent.filePath = full;

		scriptComponent.fileName = file.stem().string();
		scriptComponent.fileNameToUpper = scriptComponent.fileName;
		std::transform(
			scriptComponent.fileNameToUpper.begin(),
			scriptComponent.fileNameToUpper.end(),
			scriptComponent.fileNameToUpper.begin(),
			[](unsigned char c) { return std::toupper(c); });
	}
	scripts.push_back(scriptComponent);
}
void ScriptingComponent::Render()
{
	for (size_t i = 0; i < scripts.size(); i++)
	{
		ImGui::PushID((int)i);
		{
			scripts[i].Render();
		}
		ImGui::PopID();
	}
}
void ScriptingComponent::lua_call_start()
{
	lua_add_entity_from_component();

	for (size_t i = 0; i < scripts.size(); i++)
	{
		if (!scripts[i].activeComponent) continue;
		//if (scripts[i].error) continue;

		std::string buffer = scripts[i].fileName;
		sol::function function = scriptingSystem->GetState()[buffer.c_str()]["Start"];
		if (function)
		{
			sol::protected_function_result result = function();
			if (!result.valid())
			{
				sol::error error = result;
				consoleWindow->AddErrorMessage("%s", error.what());
				game->StopGame();
			}
		}
	}
}
void ScriptingComponent::lua_call_update()
{
	lua_add_entity_from_component();

	for (size_t i = 0; i < scripts.size(); i++)
	{
		if (!scripts[i].activeComponent) continue;
		//if (scripts[i].error) continue;

		std::string buffer = scripts[i].fileName;
		sol::function function = scriptingSystem->GetState()[buffer.c_str()]["Update"];
		if (function)
		{
			sol::protected_function_result result = function();
			if (!result.valid())
			{
				sol::error error = result;
				consoleWindow->AddErrorMessage("%s", error.what());
				game->StopGame();
			}
		}
	}
}
void ScriptingComponent::RecompileScriptsChecksum()
{
	for (size_t i = 0; i < scripts.size(); i++)
	{
		scripts[i].RecompileScriptsChecksum();
	}
}
bool ScriptingSystem::CompileScript(const char* filename)
{
	sol::load_result result = scriptingSystem->GetState().load_file(filename);
	if (!result.valid())
	{
		sol::error error = result;
		consoleWindow->AddErrorMessage("%s", error.what());
		return false;
	}
	else
	{
		//printf("CompileScript()\n");
		scriptingSystem->GetState().script_file(filename);
		return true;
	}

	return false;
}
void ScriptingSystem::CreateScript(const char* filename, const char* name)
{
	std::string buffer = std::string(name) + " = {}\n"
		"\n"
		"-- Use this for initialization\n"
		"function " + std::string(name) + ":Start()\n"
		"end\n"
		"\n"
		"-- Update is called once per frame\n"
		"function " + std::string(name) + ":Update()\n"
		"end";

	std::ofstream script;
	script.open(filename);
	script << buffer;
	script.close();
}
void ScriptingComponent::RecompileScripts()
{
	for (size_t i = 0; i < scripts.size(); i++)
	{
		scripts[i].RecompileScript();
	}
}
void ScriptingComponent::lua_add_entity_from_component()
{
	entt::entity entity = entt::to_entity(ecs->registry, *this);
	EntityX entityX; entityX.entity = entity;
	scriptingSystem->GetState().set("entity", entityX);
}
sol::state& ScriptingSystem::GetState()
{
	return lua;
}

void EntityX::CreateEntity()
{
	entity = ecs->CreateEntity();
	ecs->GetComponent<GeneralComponent>(ecs->root).AddChild(entity);
}
void EntityX::AddComponent(const char* component_name)
{
	if (!ecs->IsValid(entity))
	{
		consoleWindow->AddWarningMessage("Failed to add component because entity is not valid");
		return;
	}

	if (strcmp(component_name, "MeshComponent") == 0)
	{
		if (!ecs->HasComponent<MeshComponent>(entity))
			ecs->AddComponent<MeshComponent>(entity);
	}
	else if (strcmp(component_name, "TextMeshComponent") == 0)
	{
		if (!ecs->HasComponent<TextMeshComponent>(entity))
			ecs->AddComponent<TextMeshComponent>(entity);
	}
	else if (strcmp(component_name, "CameraComponent") == 0)
	{
		if (!ecs->HasComponent<CameraComponent>(entity))
			ecs->AddComponent<CameraComponent>(entity);
	}
	else if (strcmp(component_name, "RigidbodyComponent") == 0)
	{
		if (!ecs->HasComponent<RigidBodyComponent>(entity))
		{
			ecs->AddComponent<RigidBodyComponent>(entity);
			ecs->GetComponent<RigidBodyComponent>(entity).CreateActor();
		}
	}
	else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);
}
sol::object EntityX::GetComponent(const char* component_name)
{
	sol::object component;

	if (!ecs->IsValid(entity))
	{
		consoleWindow->AddWarningMessage("Failed to get component because entity is not valid");
		return component;
	}

	if (strcmp(component_name, "GeneralComponent") == 0)
	{
		if (ecs->HasComponent<GeneralComponent>(entity))
		{
			auto& entt_comp = ecs->GetComponent<GeneralComponent>(entity);
			component = sol::make_object(scriptingSystem->GetState(), &entt_comp);
		}
		else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);
	}
	else if (strcmp(component_name, "TransformComponent") == 0)
	{
		if (ecs->HasComponent<TransformComponent>(entity))
		{
			auto& entt_comp = ecs->GetComponent<TransformComponent>(entity);
			component = sol::make_object(scriptingSystem->GetState(), &entt_comp);
		}
		else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);
	}
	else if (strcmp(component_name, "MeshComponent") == 0)
	{
		if (ecs->HasComponent<MeshComponent>(entity))
		{
			auto& entt_comp = ecs->GetComponent<MeshComponent>(entity);
			component = sol::make_object(scriptingSystem->GetState(), &entt_comp);
		}
		else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);
	}
	else if (strcmp(component_name, "TextMeshComponent") == 0)
	{
		if (ecs->HasComponent<TextMeshComponent>(entity))
		{
			auto& entt_comp = ecs->GetComponent<TextMeshComponent>(entity);
			component = sol::make_object(scriptingSystem->GetState(), &entt_comp);
		}
		else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);
	}
	else if (strcmp(component_name, "CameraComponent") == 0)
	{
		if (ecs->HasComponent<CameraComponent>(entity))
		{
			auto& entt_comp = ecs->GetComponent<CameraComponent>(entity);
			component = sol::make_object(scriptingSystem->GetState(), &entt_comp);
		}
		else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);
	}
	else if (strcmp(component_name, "RigidbodyComponent") == 0)
	{
		if (ecs->HasComponent<RigidBodyComponent>(entity))
		{
			auto& entt_comp = ecs->GetComponent<RigidBodyComponent>(entity);
			component = sol::make_object(scriptingSystem->GetState(), &entt_comp);
		}
		else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);
	}
	else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);

	return component;
}
bool EntityX::HasComponent(const char* component_name)
{
	if (!ecs->IsValid(entity))
	{
		consoleWindow->AddWarningMessage("Failed to check if entity has component because entity is not valid");
		return false;
	}

	if (strcmp(component_name, "MeshComponent") == 0)
		return ecs->HasComponent<MeshComponent>(entity);
	else if (strcmp(component_name, "TextMeshComponent") == 0)
		return ecs->HasComponent<TextMeshComponent>(entity);
	else if (strcmp(component_name, "CameraComponent") == 0)
		return ecs->HasComponent<CameraComponent>(entity);
	else if (strcmp(component_name, "RigidbodyComponent") == 0)
		return ecs->HasComponent<RigidBodyComponent>(entity);
	else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);

	return false;
}
void EntityX::RemoveComponent(const char* component_name)
{
	if (!ecs->IsValid(entity))
	{
		consoleWindow->AddWarningMessage("Failed to remove component because entity is not valid");
		return;
	}

	if (strcmp(component_name, "MeshComponent") == 0)
		ecs->RemoveComponent<MeshComponent>(entity);
	else if (strcmp(component_name, "TextMeshComponent") == 0)
		ecs->RemoveComponent<TextMeshComponent>(entity);
	else if (strcmp(component_name, "CameraComponent") == 0)
		ecs->RemoveComponent<CameraComponent>(entity);
	else if (strcmp(component_name, "RigidbodyComponent") == 0)
		ecs->RemoveComponent<RigidBodyComponent>(entity);
	else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);
}

void ScriptingComponent::SerializeComponent(YAML::Emitter& out)
{
	if (scripts.empty())
		return;

	out << YAML::Key << "Scripts" << YAML::Value << YAML::BeginSeq;
	for (size_t i = 0; i < scripts.size(); i++)
	{
		scripts[i].SerializeComponent(out);
	}
	out << YAML::EndSeq;
}
void ScriptingComponent::DeserializeComponent(YAML::Node& in)
{
	YAML::Node scripts = in["Scripts"];
	if (scripts)
	{
		for (const auto& script : scripts)
		{
			YAML::Node scriptNode = script["Script"];
			if (scriptNode)
			{
				AddScript(scriptNode["Path"].as<std::string>().c_str());
			}
		}
	}
}

EntityX ScriptingSystem::FindByName(const char* name)
{
	printf("FindByName() %s\n", name);
	EntityX entityX;

	auto view = ecs->registry.view<GeneralComponent>();
	for (auto entity : view)
	{
		if (entity == ecs->root) continue; // skip root entity
		auto& gc = ecs->GetComponent<GeneralComponent>(entity);

		if (!gc.GetName().compare(name))
		{
			entityX.entity = entity;
			return entityX;
		}
	}

	consoleWindow->AddWarningMessage("Failed to find entity by name %s", name);
	return entityX;
}
EntityX ScriptingSystem::FindByTag(const char* tag)
{
	EntityX entityX;

	auto view = ecs->registry.view<GeneralComponent>();
	for (auto entity : view)
	{
		if (entity == ecs->root) continue; // skip root entity
		auto& gc = ecs->GetComponent<GeneralComponent>(entity);

		if (!gc.GetTag().compare(tag))
		{
			entityX.entity = entity;
			return entityX;
		}
	}

	consoleWindow->AddWarningMessage("Failed to find entity by tag %s", tag);
	return entityX;
}