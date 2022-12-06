#pragma once

#include "Core.h"
#include "Scene.h"
#include "Animation2DManager.h"
#include "yaml-cpp/yaml.h"
#include "Environment.h"

#include <fstream>
#include <vector>

namespace Pengine
{

	class PENGINE_API Serializer
	{
	public:

		static void SerializeTextureAtlas(class TextureAtlas* textureAtlas);
		
		static class TextureAtlas* DeSerializeTextureAtlas(const std::string& filePath);
		
		static void SerializeAnimation2D(Animation2DManager::Animation2D* animation);
		
		static void DeSerializeAnimation2D(const std::string& filePath, Animation2DManager::Animation2D* animation);
		
		static void SerializePrefab(const std::string& filePath, GameObject& gameObject);
		
		static GameObject* DeserializePrefab(const std::string filePath);
		
		static void SerializeScene(Scene& scene);
		
		static Scene* DeserializeScene(const std::string filePath);
		
		static void SerializeCamera(YAML::Emitter& out, const std::shared_ptr<class Camera>& camera);
		
		static Camera DeserializeCamera(YAML::Node& in);

		static void SerializeEnvironment(YAML::Emitter& out);

		static void DeserializeEnvironment(YAML::Node& in);

		static void SerializeShadows(YAML::Emitter& out);

		static Environment::ShadowsSettings DeserializeShadows(YAML::Node& in);

		static std::string GenerateMetaFilePath(const std::string& filePath, const std::string& name);

		static std::string GenerateMetaFilePathFromFilePath(const std::string& filePath);

		static std::string SerializeMeshMeta(Mesh::Meta meta);

		static Mesh::Meta DeserializeMeshMeta(const std::string& filePath);

		static std::string SerializeTextureMeta(Texture::Meta meta);

		static Texture::Meta DeserializeTextureMeta(const std::string& filePath);
		
		static void SerializeTransform(YAML::Emitter& out, const Transform& transform);
		
		static Transform DeserializeTransform(YAML::Node& in);
		
		static void SerializeGameObjects(YAML::Emitter& out, const Scene& scene);
		
		static void SerializeGameObject(YAML::Emitter& out, GameObject& gameObject);
		
		static void DeserializeGameObject(YAML::Node& in, Scene& scene, std::unordered_map<GameObject*, std::vector<std::string>>& childs);
		
		static void SerializeGameObjectChilds(YAML::Emitter& out, GameObject& gameObject);
		
		static void DeserializeGameObjectChilds(YAML::Node& in, GameObject* gameObject, std::unordered_map<GameObject*, std::vector<std::string>>& childs);
		
		static void SerializeRenderer3D(YAML::Emitter& out, ComponentManager& componentManager);

		static void DeSerializeRenderer3D(YAML::Node& in, ComponentManager& componentManager);

		static void SerializeRenderer2D(YAML::Emitter& out, ComponentManager& componentManager);
		
		static void DeSerializeRenderer2D(YAML::Node& in, ComponentManager& componentManager);
		
		static void SerializeBoxCollider2D(YAML::Emitter& out, ComponentManager& componentManager);
		
		static void DeSerializeBoxCollider2D(YAML::Node& in, ComponentManager& componentManager);

		static void SerializeBoxCollider3D(YAML::Emitter& out, ComponentManager& componentManager);

		static void DeSerializeBoxCollider3D(YAML::Node& in, ComponentManager& componentManager);
		
		static void SerializeCircleCollider2D(YAML::Emitter& out, ComponentManager& componentManager);
		
		static void DeSerializeCircleCollider2D(YAML::Node& in, ComponentManager& componentManager);
		
		static void SerializeSphereCollider3D(YAML::Emitter& out, ComponentManager& componentManager);

		static void DeSerializeSphereCollider3D(YAML::Node& in, ComponentManager& componentManager);

		static void SerializeRigidbody2D(YAML::Emitter& out, ComponentManager& componentManager);
		
		static void DeSerializeRigidbody2D(YAML::Node& in, ComponentManager& componentManager);

		static void SerializeRigidbody3D(YAML::Emitter& out, ComponentManager& componentManager);

		static void DeSerializeRigidbody3D(YAML::Node& in, ComponentManager& componentManager);
		
		static void SerializeAnimator2D(YAML::Emitter& out, ComponentManager& componentManager);
		
		static void DeSerializeAnimator2D(YAML::Node& in, ComponentManager& componentManager);
		
		static void SerializeParticleEmitter(YAML::Emitter& out, ComponentManager& componentManager);
		
		static void DeSerializeParticleEmitter(YAML::Node& in, ComponentManager& componentManager);
		
		static void SerializeScript(YAML::Emitter& out, ComponentManager& componentManager);
		
		static void DeSerializeScript(YAML::Node& in, ComponentManager& componentManager);

		static void SerializePointLight(YAML::Emitter& out, ComponentManager& componentManager);

		static void DeSerializePointLight(YAML::Node& in, ComponentManager& componentManager);

		static void SerializeSpotLight(YAML::Emitter& out, ComponentManager& componentManager);

		static void DeSerializeSpotLight(YAML::Node& in, ComponentManager& componentManager);

		static void SerializeDirectionalLight(YAML::Emitter& out, ComponentManager& componentManager);

		static void DeSerializeDirectionalLight(YAML::Node& in, ComponentManager& componentManager);

		static void SerializeSpline(YAML::Emitter& out, ComponentManager& componentManager);

		static void DeSerializeSpline(YAML::Node& in, ComponentManager& componentManager);

		static void SerializeUserDefinedComponents(YAML::Emitter& out, ComponentManager& componentManager);

		static void DeserializeUserDefinedComponents(YAML::Node& in, ComponentManager& componentManager);

		static void SerializeMaterial(const std::string& filePath, Material* material);

		static Material* DeserializeMaterial(const std::string& filePath);

		template<typename T>
		static void SerializeRttrType(YAML::Emitter& out, const rttr::type& type, T* instance);

		template<typename T>
		static void DeserializeRttrType(YAML::Node& in, const rttr::type& type, T* instance);
	};

}
