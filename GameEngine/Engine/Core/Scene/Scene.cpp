#include "Scene.h"
#include "../../Systems/TransformHelper.h"

Scene::Scene()
{

}

void Scene::Initialize()
{   
	

}

void Scene::Update(float deltaSec)
{

}

entt::entity Scene::AddGameObject()
{
	auto id = registry.create();
	registry.emplace<DataComponent>(id, "GameObject");
	registry.emplace<TransformComponent>(id);
	registry.emplace<MeshComponent>(id);
	gameObjects.insert(id);
	return id;
}

entt::entity Scene::AddStaticBox(const SyVector3& position_, const SyVector3& rotation_, const SyVector3 scale_)
{
	auto id = registry.create();
	registry.emplace<DataComponent>(id, "Box");
	registry.emplace<TransformComponent>(id);
	auto& tc = registry.get<TransformComponent>(id);
	tc.localPosition = position_;
	tc.localRotation = rotation_;
	tc.localScale = scale_;
	registry.emplace<MeshComponent>(id);
	SyRBodyBoxShapeDesc boxDesc;
	boxDesc.origin = position_;
	boxDesc.rotation = rotation_;
	boxDesc.halfExt = scale_;
	registry.emplace<SyRBodyComponent>(id, RB_TYPE_STATIC, RB_SHAPE_TYPE_BOX, boxDesc);
	gameObjects.insert(id);
	return id;
}

entt::entity Scene::AddDynamicBox(const SyVector3& position_, const SyVector3& rotation_, const SyVector3 scale_)
{
	auto id = registry.create();
	registry.emplace<DataComponent>(id, "Box");
	registry.emplace<TransformComponent>(id);
	auto& tc = registry.get<TransformComponent>(id);
	tc.localPosition = position_;
	tc.localRotation = rotation_;
	tc.localScale = scale_;
	registry.emplace<MeshComponent>(id);
	SyRBodyBoxShapeDesc boxDesc;
	boxDesc.origin = position_;
	boxDesc.rotation = rotation_;
	boxDesc.halfExt = scale_;
	registry.emplace<SyRBodyComponent>(id, RB_TYPE_DYNAMIC, RB_SHAPE_TYPE_BOX, boxDesc);
	gameObjects.insert(id);
	return id;
}

entt::entity Scene::AddLight(LightType _lightType)
{
	auto id = registry.create();
	registry.emplace<DataComponent>(id,"LightObject");
	if (_lightType == LightType::PointLight)
		registry.emplace<EditorBillboardComponent>(id, "Engine/Assets/Sprites/PointLightSprite.png");
	else
		registry.emplace<EditorBillboardComponent>(id, "Engine/Assets/Sprites/DirLightSprite.png");
	registry.emplace<LightComponent>(id,_lightType);
	registry.emplace<TransformComponent>(id);
	gameObjects.insert(id);
	return id;
}

entt::entity Scene::AddParticleSystem()
{
	auto id = registry.create();
	registry.emplace<DataComponent>(id,"ParticleObject");
	registry.emplace<ParticleComponent>(id);
	registry.emplace<TransformComponent>(id);
	gameObjects.insert(id);
	return id;
}

bool Scene::DestroyGameObject(entt::entity _id)
{
	return true;
	if (registry.get<TransformComponent>(_id).children.size() > 0)
	{
		for (auto childID : registry.get<TransformComponent>(_id).children)
			DestroyGameObject(childID);
	}

	if (registry.get<TransformComponent>(_id).parent!=entt::null)
	{
		RemoveChild(registry.get<TransformComponent>(_id).parent, _id);
	}
	
	if (gameObjects.count(_id))
	{
		registry.destroy(_id);
		gameObjects.erase(_id);
		return true;
	}
	return false;
}


void Scene::SetParent(entt::entity sourceGameObject, entt::entity parentGameObject)
{
	if (parentGameObject == entt::null)
	{
		if (registry.get<TransformComponent>(sourceGameObject).parent != entt::null) // ���� ���� ������
		{
			
			RemoveChild(registry.get<TransformComponent>(sourceGameObject).parent, sourceGameObject);
		}
		TransformHelper::UpdateRelativeToParent(nullptr, registry.get<TransformComponent>(sourceGameObject));
		registry.get<TransformComponent>(sourceGameObject).parent = entt::null;
	}
	else if (!HasHierarchyCycles(sourceGameObject, parentGameObject))
	{

		if (registry.get<TransformComponent>(sourceGameObject).parent != entt::null)
		{
			
			RemoveChild(registry.get<TransformComponent>(sourceGameObject).parent, sourceGameObject);
			
		}
		TransformHelper::UpdateRelativeToParent(nullptr, registry.get<TransformComponent>(sourceGameObject));
		registry.get<TransformComponent>(sourceGameObject).parent = parentGameObject;
		registry.get<TransformComponent>(parentGameObject).children.insert(sourceGameObject);
		TransformHelper::UpdateRelativeToParent(registry.try_get<TransformComponent>(parentGameObject),
			registry.get<TransformComponent>(sourceGameObject));
	}
	/*TransformHelper::UpdateRelativeToParent(registry.try_get<TransformComponent>(parentGameObject),
		registry.get<TransformComponent>(sourceGameObject));*/
}   

void Scene::AddChild(entt::entity parentGameObject, entt::entity childGameObject)
{
	SetParent(childGameObject, parentGameObject);
}

void Scene::RemoveChild(entt::entity parentGameObject, entt::entity childGameObject)
{
	registry.get<TransformComponent>(parentGameObject).children.erase(childGameObject);
}

bool Scene::HasHierarchyCycles(entt::entity sourceGameObject, entt::entity parentGameObject)
{
	if (parentGameObject == sourceGameObject)
		return true;

	bool res = false;
	for (auto& gameObject : registry.get<TransformComponent>(sourceGameObject).children)
	{
		res = res || HasHierarchyCycles(gameObject, parentGameObject);
	}
	return res;
}
