![](/images/banner_logo.png)
## Introduction
An Advanced C++ DirectX 11 Game Engine.
## Features
- Lua Scripting
- Physics
- Entity Component System
- UI Editor
- Panorama Sphere Sky (HDRI)
- Rendering Modes (Pointlist, Linelist, Trianglelist, Position, Normal)
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
![](/images/image_9_fix.png)
![](/images/engine_8.png)
![](/images/engine_2.png)
![](/images/engine_3.png)
![](/images/engine_5.png)
![](/images/engine_6.png)
![](/images/engine_7.png)
![](/images/1_scan.png)
![](/images/2_scan.png)
![](/images/3_scan.png)
![](/images/4_scan.png)
![](/images/5_scan.png)
![](/images/6_scan.png)
![](https://github.com/user-attachments/assets/1aae93da-721d-4045-9156-c581c54880bb)
## Lua API
```lua

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
IconFontCppHeaders https://github.com/juliettef/IconFontCppHeaders \
zlib https://zlib.net/ \
LuaSocket https://github.com/lunarmodules/luasocket \
ACES Filmic https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/ \
Font Awesome https://fontawesome.com/ \
![](/images/entt.png)
![](/images/physx.png)
![](/images/lua.png)
## License
StarEngine is licensed under the MIT License, see [LICENSE](/LICENSE) for more information.
