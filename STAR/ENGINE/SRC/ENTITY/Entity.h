#pragma once

#undef min
#undef max

#include "entt/entt.hpp"

class Entity
{
public:
	bool Init();

public:
	entt::registry registry;
	entt::entity selected = entt::null;
	entt::entity root = entt::null;
	entt::entity copy = entt::null;

public:
	entt::entity CreateEntity();
	void SetSelectedEntity(entt::entity entity);
	entt::entity GetSelectedEntity();
	void SetRootEntity(entt::entity entity);
	entt::entity GetRootEntity();
	entt::registry& GetRegistry();
	unsigned int GetFreeNameIndex(const char* name);
	entt::entity FindByName(const char* name, bool print);
	entt::entity FindByTag(const char* tag, bool print);
	void Destroy(entt::entity entity);
	void DestroyChildren(entt::entity entity);
	template<typename T>
	void CopyComponent(entt::entity from, entt::entity to)
	{
		if (HasComponent<T>(from))
		{
			if (!HasComponent<T>(to))
			{
				registry.emplace<T>(to, registry.get<T>(from));
			}
			else
			{
				registry.replace<T>(to, registry.get<T>(from));
			}
		}
	}
	entt::entity CopyEntity(entt::entity _Entity);

private:
	void DestroyAll(entt::entity entity, std::vector<entt::entity>& bin);

public:
	void Cleanup(entt::entity entity);

public:
	void CreateEmptyEntity(entt::entity entity);
	void CreateCubeEntity(entt::entity entity);
	void CreateSphereEntity(entt::entity entity);
	void CreateCapsuleEntity(entt::entity entity);
	void CreatePlaneEntity(entt::entity entity);
	void CreateCameraEntity(entt::entity entity);
	void CreateTextMeshEntity(entt::entity entity);
	void CreateRectangleEntity(entt::entity entity);
	void CreateImageEntity(entt::entity entity);
	void CreateTextEntity(entt::entity entity);
	void CreateButtonEntity(entt::entity entity);

public:
	template <typename T>
	void AddComponent(entt::entity entity)
	{
		registry.emplace<T>(entity);
	}
	template <typename T>
	T& GetComponent(entt::entity entity)
	{
		return registry.get<T>(entity);
	}
	template <typename T>
	bool HasComponent(entt::entity entity)
	{
		return registry.any_of<T>(entity);
	}
	template <typename T>
	void RemoveComponent(entt::entity entity)
	{
		registry.remove<T>(entity);
	}

	bool IsValid(entt::entity entity)
	{
		return registry.valid(entity);
	}

public:
	static Entity* GetSingleton();
};