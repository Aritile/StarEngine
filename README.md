![](/images/banner_logo.png)
## Introduction
An Advanced C++ DirectX 11 Game Engine.
## Features
- Lua Scripting
- Physics
- Entity Component System
- UI Editor
- Panorama Sphere Sky (HDRI)
- Rendering Modes (Pointlist, Linelist, Trianglelist)
- Guizmo & Object Picking
- Text To Mesh Text
- Mesh Model With Textures
- Async Loading
- Bounding Box
- Tone Mapping
- DLL Plugin Scripting
- And more...
## Build
- Visual Studio 2022
- Windows SDK Version: 10.0 (latest installed version)
- Platform Toolset: Visual Studio 2022 (v143)
- C++ Language Standard: C++ 2017
- Configuration: Debug or Release
- Platform: x64
## Community
Discord https://discord.gg/sMYwRaR9FG \
YouTube https://www.youtube.com/@starengine
## Gallery
![](/images/engine_1.png)
![](/images/engine_4.png)
![](/images/engine_2.png)
![](/images/engine_3.png)
![](/images/engine_5.png)
![](/images/engine_6.png)
![](/images/engine_7.png)
![](/images/engine_8.png)
![](/images/1_scan.png)
![](/images/2_scan.png)
![](/images/3_scan.png)
![](/images/4_scan.png)
![](/images/5_scan.png)
![](/images/6_scan.png)
![](https://github.com/user-attachments/assets/1aae93da-721d-4045-9156-c581c54880bb)
## Lua API
```lua
Vector2
Vector2.Length()
Vector2.Dot()
Vector2.Cross()
Vector2.Normalize()
Vector2.Clamp()
Vector2.Distance()
Vector2.Min()
Vector2.Max()
Vector2.Lerp()
Vector2.Zero()
Vector2.One()
Vector2.x()
Vector2.y()

Vector3
Vector3.Length()
Vector3.Dot()
Vector3.Cross()
Vector3.Normalize()
Vector3.Clamp()
Vector3.Distance()
Vector3.Min()
Vector3.Max()
Vector3.Lerp()
Vector3.Zero()
Vector3.One()
Vector3.Up()
Vector3.Down()
Vector3.Right()
Vector3.Left()
Vector3.Forward()
Vector3.Backward()
Vector3.x()
Vector3.y()
Vector3.z()

Vector4
Vector4.Length()
Vector4.Dot()
Vector4.Cross()
Vector4.Normalize()
Vector4.Clamp()
Vector4.Distance()
Vector4.Min()
Vector4.Max()
Vector4.Lerp()
Vector4.Zero()
Vector4.One()
Vector4.x()
Vector4.y()
Vector4.z()
Vector4.w()

Quaternion
Quaternion.Identity()
Quaternion.x()
Quaternion.y()
Quaternion.z()
Quaternion.w()

Matrix
Matrix.Up()
Matrix.Down()
Matrix.Right()
Matrix.Left()
Matrix.Forward()
Matrix.Backward()
Matrix.Lerp()
Matrix.Identity()
Matrix._11()
Matrix._12()
Matrix._13()
Matrix._14()
Matrix._21()
Matrix._22()
Matrix._23()
Matrix._24()
Matrix._31()
Matrix._32()
Matrix._33()
Matrix._34()
Matrix._41()
Matrix._42()
Matrix._43()
Matrix._44()

Console
Console.Info()
Console.Warning()
Console.Error()

Time
Time.FrameTime()
Time.DeltaTime()
Time.ElapsedTime()
Time.FrameCount()

Input
Input.HideCursor()
Input.LockCursor()
Input.IsCursorHidden()
Input.IsCursorLocked()
Input.GetCursorAxis()
Input.GetCursorAxisX()
Input.GetCursorAxisY()
Input.GetKeyDown()
Input.KeyCode()
Input.KeyCode.Q()
Input.KeyCode.W()
Input.KeyCode.E()
Input.KeyCode.R()
Input.KeyCode.T()
Input.KeyCode.Y()
Input.KeyCode.U()
Input.KeyCode.I()
Input.KeyCode.O()
Input.KeyCode.P()
Input.Input.KeyCode.A()
Input.Input.KeyCode.S()
Input.Input.KeyCode.D()
Input.Input.KeyCode.F()
Input.Input.KeyCode.G()
Input.Input.KeyCode.H()
Input.Input.KeyCode.J()
Input.Input.KeyCode.K()
Input.Input.KeyCode.L()
Input.Input.KeyCode.Z()
Input.Input.KeyCode.X()
Input.Input.KeyCode.C()
Input.Input.KeyCode.V()
Input.Input.KeyCode.B()
Input.Input.KeyCode.N()
Input.Input.KeyCode.M()
Input.Input.KeyCode.1()
Input.Input.KeyCode.2()
Input.Input.KeyCode.3()
Input.Input.KeyCode.4()
Input.Input.KeyCode.5()
Input.Input.KeyCode.6()
Input.Input.KeyCode.7()
Input.Input.KeyCode.8()
Input.Input.KeyCode.9()
Input.Input.KeyCode.0()
Input.Input.KeyCode.F1()
Input.Input.KeyCode.F2()
Input.Input.KeyCode.F3()
Input.Input.KeyCode.F4()
Input.Input.KeyCode.F5()
Input.Input.KeyCode.F6()
Input.Input.KeyCode.F7()
Input.Input.KeyCode.F8()
Input.Input.KeyCode.F9()
Input.Input.KeyCode.F10()
Input.Input.KeyCode.F11()
Input.Input.KeyCode.F12()
Input.Input.KeyCode.Up()
Input.Input.KeyCode.Down()
Input.Input.KeyCode.Left()
Input.Input.KeyCode.Right()
Input.Input.KeyCode.LeftControl()
Input.Input.KeyCode.RightControl()
Input.Input.KeyCode.LeftShift()
Input.Input.KeyCode.RightShift()
Input.Input.KeyCode.LeftAlt()
Input.Input.KeyCode.RightAlt()

BoundingBox

Entity
Entity.CreateEntity()
Entity.AddComponent()
Entity.GetComponent()
Entity.HasComponent()
Entity.RemoveComponent()

GeneralComponent
GeneralComponent.SetName()
GeneralComponent.GetName()
GeneralComponent.SetTag()
GeneralComponent.GetTag()
GeneralComponent.SetActive()
GeneralComponent.GetActive()
GeneralComponent.SetStatic()
GeneralComponent.GetStatic()
GeneralComponent.MoveUp()
GeneralComponent.MoveDown()
GeneralComponent.GetParentEntity()

TransformComponent
TransformComponent.SetBoundingBox()
TransformComponent.GetBoundingBox()
TransformComponent.SetPosition()
TransformComponent.SetRotationYawPitchRoll()
TransformComponent.SetRotationQuaternion()
TransformComponent.SetScale()
TransformComponent.SetTransform()
TransformComponent.AddPosition()
TransformComponent.AddRotationYawPitchRoll()
TransformComponent.AddRotationQuaternion()
TransformComponent.AddScale()
TransformComponent.AddTransform()
TransformComponent.GetPosition()
TransformComponent.GetRotationYawPitchRoll()
TransformComponent.GetRotationQuaternion()
TransformComponent.GetScale()
TransformComponent.GetTransform()
TransformComponent.GetLocalPosition()
TransformComponent.GetLocalRotationYawPitchRoll()
TransformComponent.GetLocalRotationQuaternion()
TransformComponent.GetLocalScale()
TransformComponent.GetLocalTransform()
TransformComponent.LookAt()

CameraComponent
CameraComponent.SetFov()
CameraComponent.SetNear()
CameraComponent.SetFar()
CameraComponent.GetFov()
CameraComponent.GetNear()
CameraComponent.GetFar()
CameraComponent.SetAspect()
CameraComponent.GetAspect()
CameraComponent.SetScale()
CameraComponent.GetScale()

RigidbodyComponent
RigidbodyComponent.SetMass()
RigidbodyComponent.GetMass()
RigidbodyComponent.SetLinearVelocity()
RigidbodyComponent.GetLinearVelocity()
RigidbodyComponent.SetAngularVelocity()
RigidbodyComponent.GetAngularVelocity()
RigidbodyComponent.SetLinearDamping()
RigidbodyComponent.GetLinearDamping()
RigidbodyComponent.SetAngularDamping()
RigidbodyComponent.GetAngularDamping()
RigidbodyComponent.SetGravity()
RigidbodyComponent.GetGravity()
RigidbodyComponent.SetKinematic()
RigidbodyComponent.GetKinematic()
RigidbodyComponent.AddForce()
RigidbodyComponent.AddTorque()
RigidbodyComponent.ClearForce()
RigidbodyComponent.ClearTorque()

MeshComponent
MeshComponent.GetNumVertices()
MeshComponent.GetNumFaces()
MeshComponent.AddVertices()
MeshComponent.AddIndices()
MeshComponent.SetupMesh()

Vertex
Vertex.position()
Vertex.normal()
Vertex.texCoords()

TextMeshComponent
TextMeshComponent.SetText()
TextMeshComponent.GetText()
TextMeshComponent.SetQuality()
TextMeshComponent.GetQuality()
TextMeshComponent.SetFont()
TextMeshComponent.GetFont()
TextMeshComponent.SetAlign()
TextMeshComponent.GetAlign()
TextMeshComponent.SetMeshDepth()
TextMeshComponent.GetMeshDepth()
TextMeshComponent.SetCharSpacing()
TextMeshComponent.GetCharSpacing()
TextMeshComponent.SetLineSpacing()
TextMeshComponent.GetLineSpacing()
TextMeshComponent.SetWordSpacing()
TextMeshComponent.GetWordSpacing()

PlayerPrefs
PlayerPrefs.DeleteAll()
PlayerPrefs.DeleteKey()
PlayerPrefs.HasKey()
PlayerPrefs.GetFloat()
PlayerPrefs.GetInt()
PlayerPrefs.GetString()
PlayerPrefs.SetFloat()
PlayerPrefs.SetInt()
PlayerPrefs.SetString()
PlayerPrefs.Save()

Scene
Scene.LoadScene()
```
## Credits
Dear ImGui https://github.com/ocornut/imgui \
assimp https://github.com/assimp/assimp \
EnTT https://github.com/skypjack/entt \
Lua https://github.com/lua/lua \
PhysX https://github.com/NVIDIAGameWorks/PhysX \
sol2 https://github.com/ThePhD/sol2 \
yaml-cpp https://github.com/jbeder/yaml-cpp \
ttf2mesh https://github.com/fetisov/ttf2mesh \
ImGuizmo https://github.com/CedricGuillemet/ImGuizmo \
ImGuiWidgets https://github.com/HODAKdev/ImGuiWidgets \
DirectXTex https://github.com/microsoft/DirectXTex \
STRDX https://github.com/HODAKdev/STRDX \
zlib https://zlib.net/ \
ACES Filmic https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/ \
Font Awesome https://fontawesome.com/ \
![](/images/entt.png)
![](/images/physx.png)
![](/images/lua.png)
## License
StarEngine is licensed under the MIT License, see [LICENSE](/LICENSE) for more information.
