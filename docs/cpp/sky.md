## To change sky
### Get sky pointer
```cpp
static Sky* sky = Sky::GetSingleton();
```
### Now you can set solid color, texture or none
```cpp
sky->SetSolidColor(Vector3(0.5f, 0.5f, 0.5f));
//sky->LoadTexture("texture.hdr");
//sky->SetNone();
```
