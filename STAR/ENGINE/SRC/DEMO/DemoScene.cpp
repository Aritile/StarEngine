#include "DemoScene.h"
#include "../ENTITY/COMPONENT/TransformComponent.h"
#include "../ENTITY/COMPONENT/DistanceCullerComponent.h"
#include "../ENTITY/Entity.h"

static Entity* ecs = Entity::GetSingleton();

void DemoScene::CreateDemoGrid(float _GridSize, float _Spacing, bool _UseDistanceCuller, float _Distance, bool _Reverse)
{
    for (int x = 0; x < _GridSize; x++)
    {
        for (int y = 0; y < _GridSize; y++)
        {
            for (int z = 0; z < _GridSize; z++)
            {
                auto entity = ecs->CreateEntity();
                ecs->CreateCubeEntity(entity);

                // get transform component and set position
                auto& tr = ecs->GetComponent<TransformComponent>(entity);
                tr.AddPosition(Vector3(x * _Spacing, y * _Spacing, z * _Spacing));

                if (_UseDistanceCuller)
                {
                    // add culling component
                    ecs->AddComponent<DistanceCullerComponent>(entity);
                    auto& distanceCuller = ecs->GetComponent<DistanceCullerComponent>(entity);
                    distanceCuller.distance = _Distance;
                    distanceCuller.reverse = _Reverse;
                }
            }
        }
    }
}