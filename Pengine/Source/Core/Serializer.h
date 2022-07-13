#pragma once

#include "Core.h"
#include "Scene.h"
#include "Animation2DManager.h"
#include "../yaml-cpp/yaml.h"

#include <vector>

namespace Pengine
{

	class PENGINE_API Serializer
	{
	public:

		static void SerializeTextureAtlas(class TextureAtlas* textureAtlas);
		
		static class TextureAtlas* DeSerializeTextureAtlas(const std::string& filePath);
		
		static void SerializeAnimation2D(Animation2DManager::Animation2D* animation);
		
		static Animation2DManager::Animation2D* DeSerializeAnimation2D(const std::string& filePath);
		
		static void SerializePrefab(const std::string& filePath, GameObject& gameObject);
		
		static GameObject* DeserializePrefab(const std::string filePath);
		
		static void SerializeScene(const Scene& scene);
		
		static void DeserializeScene(const std::string filePath);
		
		static void SerializeCamera(YAML::Emitter& out, const std::shared_ptr<class Camera>& camera);
		
		static Camera DeserializeCamera(YAML::Node& in);

		static void SerializeEnvironment(YAML::Emitter& out);

		static void DeserializeEnvironment(YAML::Node& in);
		
		static void SerializeTransform(YAML::Emitter& out, const Transform& transform);
		
		static Transform DeserializeTransform(YAML::Node& in);
		
		static void SerializeGameObjects(YAML::Emitter& out, const Scene& scene);
		
		static void SerializeGameObject(YAML::Emitter& out, GameObject& gameObject);
		
		static void DeserializeGameObject(YAML::Node& in, Scene& scene, std::unordered_map<GameObject*, std::vector<size_t>>& childs);
		
		static void SerializeGameObjectChilds(YAML::Emitter& out, GameObject& gameObject);
		
		static void DeserializeGameObjectChilds(YAML::Node& in, GameObject* gameObject, std::unordered_map<GameObject*, std::vector<size_t>>& childs);
		
		static void SerializeRenderer2D(YAML::Emitter& out, ComponentManager& componentManager);
		
		static void DeSerializeRenderer2D(YAML::Node& in, ComponentManager& componentManager);
		
		static void SerializeBoxCollider2D(YAML::Emitter& out, ComponentManager& componentManager);
		
		static void DeSerializeBoxCollider2D(YAML::Node& in, ComponentManager& componentManager);
		
		static void SerializeCircleCollider2D(YAML::Emitter& out, ComponentManager& componentManager);
		
		static void DeSerializeCircleCollider2D(YAML::Node& in, ComponentManager& componentManager);
		
		static void SerializeRigidbody2D(YAML::Emitter& out, ComponentManager& componentManager);
		
		static void DeSerializeRigidbody2D(YAML::Node& in, ComponentManager& componentManager);
		
		static void SerializeAnimator2D(YAML::Emitter& out, ComponentManager& componentManager);
		
		static void DeSerializeAnimator2D(YAML::Node& in, ComponentManager& componentManager);
		
		static void SerializeParticleEmitter(YAML::Emitter& out, ComponentManager& componentManager);
		
		static void DeSerializeParticleEmitter(YAML::Node& in, ComponentManager& componentManager);
		
		static void SerializeScript(YAML::Emitter& out, ComponentManager& componentManager);
		
		static void DeSerializeScript(YAML::Node& in, ComponentManager& componentManager);

		static void SerializePointLight(YAML::Emitter& out, ComponentManager& componentManager);

		static void DeSerializePointLight(YAML::Node& in, ComponentManager& componentManager);

		static void SerializeUserDefinedComponents(YAML::Emitter& out, ComponentManager& componentManager);

		static void DeserializeUserDefinedComponents(YAML::Node& in, ComponentManager& componentManager);

	};

}
