#include "Serializer.h"

#include "Environment.h"
#include "TextureManager.h"
#include "EntryPoint.h"
#include "TextureAtlasManager.h"
#include "../EventSystem/EventSystem.h"
#include "../OpenGL/Texture.h"
#include "../OpenGL/Material.h"
#include "../OpenGL/BaseMaterial.h"
#include "../OpenGL/BaseMaterial.h"
#include "../Components/BoxCollider2D.h"
#include "../Components/Rigidbody2D.h"
#include "../Components/Animator2D.h"
#include "../Components/ParticleEmitter.h"
#include "../Components/Script.h"
#include "../Components/Spline.h"

using namespace Pengine;

namespace YAML
{

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::ivec2>
	{
		static Node encode(const glm::ivec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::ivec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<int>();
			rhs.y = node[1].as<int>();
			return true;
		}
	};

	template<>
	struct convert<glm::ivec3>
	{
		static Node encode(const glm::ivec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::ivec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<int>();
			rhs.y = node[1].as<int>();
			rhs.z = node[2].as<int>();
			return true;
		}
	};

	template<>
	struct convert<glm::ivec4>
	{
		static Node encode(const glm::ivec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::ivec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<int>();
			rhs.y = node[1].as<int>();
			rhs.z = node[2].as<int>();
			rhs.w = node[3].as<int>();
			return true;
		}
	};

	template<>
	struct convert<glm::dvec2>
	{
		static Node encode(const glm::dvec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::dvec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<double>();
			rhs.y = node[1].as<double>();
			return true;
		}
	};

	template<>
	struct convert<glm::dvec3>
	{
		static Node encode(const glm::dvec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::dvec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<double>();
			rhs.y = node[1].as<double>();
			rhs.z = node[2].as<double>();
			return true;
		}
	};

	template<>
	struct convert<glm::dvec4>
	{
		static Node encode(const glm::dvec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::dvec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<double>();
			rhs.y = node[1].as<double>();
			rhs.z = node[2].as<double>();
			rhs.w = node[3].as<double>();
			return true;
		}
	};
}

YAML::Emitter& operator<<(YAML::Emitter& out, glm::vec2& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, glm::vec3& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, glm::vec4& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, glm::ivec2& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, glm::ivec3& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, glm::ivec4& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, glm::dvec2& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, glm::dvec3& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, glm::dvec4& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, float* v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq <<
		v[0] << v[1] << v[2] << v[3] <<
		v[4] << v[5] << v[6] << v[7] <<
		v[8] << v[9] << v[10] << v[11] <<
		v[12] << v[13] << v[14] << v[15] <<
		YAML::EndSeq;
	return out;
}

void Serializer::SerializeTextureAtlas(TextureAtlas* textureAtlas)
{
	if (textureAtlas->GetName().empty())
	{
		return;
	}

	YAML::Emitter out;

	out << YAML::BeginMap;
	out << YAML::Key << "Size" << YAML::Value << textureAtlas->GetSize();
	out << YAML::Key << "Texture" << YAML::Value << textureAtlas->m_Texture->GetFilePath();
	out << YAML::EndMap;

	std::ofstream fout(textureAtlas->GetFilePath());
	fout << out.c_str();
}

TextureAtlas* Serializer::DeSerializeTextureAtlas(const std::string& filePath)
{
	if (filePath.empty())
	{
		return nullptr;
	}

	std::ifstream stream(filePath);
	std::stringstream strStream;

	strStream << stream.rdbuf();

	YAML::Node data = YAML::LoadMesh(strStream.str());
	if (!data)
	{
		return nullptr;
	}

	TextureAtlas* textureAtlas = TextureAtlasManager::GetInstance().Create(Utils::GetNameFromFilePath(filePath));
	textureAtlas->GetFilePath() = filePath;
	
	if (auto& sizeData = data["Size"])
	{
		textureAtlas->SetSize(sizeData.as<glm::vec2>());
	}

	if (auto& textureData = data["Texture"])
	{
		TextureManager::GetInstance().AsyncLoad(textureData.as<std::string>(),
			[=](Texture* texture)
		{
			textureAtlas->m_Texture;
		});
	}

	return textureAtlas;
}

void Serializer::SerializeAnimation2D(Animation2DManager::Animation2D* animation)
{
	if (animation->GetName().empty())
	{
		return;
	}

	YAML::Emitter out;

	out << YAML::BeginMap;
	out << YAML::Key << "Textures";
	out << YAML::Value << YAML::BeginSeq;

	const size_t texturesSize = animation->m_Textures.size();
	for (size_t i = 0; i < texturesSize; i++)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "FilePath" << YAML::Value << animation->m_Textures[i]->GetFilePath();

		out << YAML::Key << "UVs";

		out << YAML::Value << animation->m_UVs[i];

		out << YAML::EndMap;
	}

	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(animation->GetFilePath());
	fout << out.c_str();
}

void Serializer::DeSerializeAnimation2D(const std::string& filePath, Animation2DManager::Animation2D* animation)
{
	if (filePath.empty())
	{
		return;
	}

	std::ifstream stream(filePath);
	std::stringstream strStream;

	strStream << stream.rdbuf();

	YAML::Node data = YAML::LoadMesh(strStream.str());
	if (!data)
	{
		return;
	}

	animation->m_Textures.clear();
	animation->m_UVs.clear();

	if (auto textures = data["Textures"])
	{
		for (auto texture : textures)
		{
			std::vector<float> uvs = texture["UVs"].as<std::vector<float>>();
			TextureManager::GetInstance().Create(texture["FilePath"].as<std::string>(),
				[=](Texture* texture)
			{
					animation->m_Textures.push_back(texture);
					animation->m_UVs.push_back(uvs);
			});
		}
	}
}

void Serializer::SerializePrefab(const std::string& filePath, GameObject& gameObject)
{
	YAML::Emitter out;

	std::string prefabFilePath = Utils::Replace(filePath, '\\', '/') + "/" + gameObject.GetName() + ".prefab";
	gameObject.m_PrefabFilePath = prefabFilePath;

	out << YAML::BeginMap;
	out << YAML::Key << "GameObjects";
	out << YAML::Value << YAML::BeginSeq;

	// We need to make a copy of the GameObject, because at this moment the scene has GameObjects
	// with UUIDs of this GameObject and his Childs, so to not make UUIDs collisions we create 
	// a copy of this GameObject, so UUIDs will be regenerated.
	GameObject* serializableGameObject = gameObject.GetScene()->CreateGameObject();
	serializableGameObject->Copy(gameObject);

	SerializeGameObject(out, *serializableGameObject);

	std::vector<GameObject*> childs = serializableGameObject->GetChilds();
	for (auto child : childs)
	{
		SerializeGameObject(out, *child);
	}

	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(prefabFilePath);
	fout << out.c_str();

	serializableGameObject->DeleteLater();

	Logger::Success("has been serialized!", "Prefab", gameObject.GetName().c_str());
}

GameObject* Serializer::DeserializePrefab(const std::string filePath)
{
	if (filePath.empty())
	{
		return nullptr;
	}

	std::ifstream stream(filePath);
	std::stringstream strStream;

	strStream << stream.rdbuf();

	YAML::Node data = YAML::LoadMesh(strStream.str());
	if (!data)
	{
		return nullptr;
	}

	Scene* scene = EntryPoint::GetApplication().GetScene();

	std::unordered_map<GameObject*, std::vector<std::string>> childs;

	if (auto& gameObjects = data["GameObjects"])
	{
		for (auto& gameObject : gameObjects)
		{
			DeserializeGameObject(gameObject, *scene, childs);
		}
	}

	for (auto gameObject : childs)
	{
		for (auto childUUID : gameObject.second)
		{
			GameObject* child = scene->FindGameObjectByUUID(childUUID);
			if (child)
			{
				gameObject.first->AddChild(child);
			}
		}
	}

	Logger::Success("has been deserialized!", "Prefab", Utils::GetNameFromFilePath(filePath).c_str());

	return childs.begin()->first;
}

void Serializer::SerializeScene(Scene& scene)
{
	YAML::Emitter out;

	out << YAML::BeginMap;

	out << YAML::Key << "Title" << YAML::Value << scene.GetTitle().c_str();
	
	SerializeEnvironment(out);
	SerializeCamera(out, Environment::GetInstance().GetMainCamera());

	SerializeGameObjects(out, scene);

	out << YAML::EndMap;

	std::string filePath = scene.m_FilePath == "None" ? "Source/Scenes/" + scene.GetTitle() + ".yaml" : scene.m_FilePath;

	if (Utils::GetNameFromFilePath(filePath) != scene.m_Title)
	{
		filePath = Utils::ReplaceNameFromFilePath(filePath, scene.m_Title, 4);
	}

	scene.m_FilePath = filePath;

	std::ofstream fout(filePath);
	fout << out.c_str();

	Logger::Success("has been serialized!", "Scene", scene.m_Title.c_str());
}

Scene* Serializer::DeserializeScene(const std::string filePath)
{
	if (filePath.empty())
	{
		return nullptr;
	}

	std::ifstream stream(filePath);
	std::stringstream strStream;

	strStream << stream.rdbuf();

	YAML::Node data = YAML::LoadMesh(strStream.str());
	if (!data)
	{
		return nullptr;
	}

	Scene* scene = EntryPoint::GetApplication().GetScene();
	if (!scene)
	{
		scene = EntryPoint::GetApplication().NewScene("Scene");
	}
	else
	{
		scene->Clear();
	}

	if (auto& titleData = data["Title"])
	{
		scene->m_Title = titleData.as<std::string>();
	}
	else
	{
		scene->m_Title = Utils::GetNameFromFilePath(filePath);
	}

	scene->m_FilePath = filePath;

	DeserializeEnvironment(data);
	Environment::GetInstance().GetMainCamera()->Copy(DeserializeCamera(data));

	std::unordered_map<GameObject*, std::vector<std::string>> childs;

	if (auto& gameObjects = data["GameObjects"])
	{
		for (auto& gameObject : gameObjects)
		{
			DeserializeGameObject(gameObject, *scene, childs);
		}
	}

	for (auto gameObject : childs)
	{
		for (auto childUUID : gameObject.second)
		{
			GameObject* child = scene->FindGameObjectByUUID(childUUID);
			if (child)
			{
				gameObject.first->AddChild(child);
			}
		}
	}

	Logger::Success("has been deserialized!", "Scene", scene->m_Title.c_str());

	return scene;
}

void Serializer::SerializeCamera(YAML::Emitter& out, const std::shared_ptr<class Camera>& camera)
{
	out << YAML::Key << "Camera";
	out << YAML::BeginMap;

	SerializeTransform(out, camera->m_Transform);
	
	out << YAML::Key << "Fov" << YAML::Value << camera->m_Fov;
	out << YAML::Key << "Zoom" << YAML::Value << camera->m_ZoomScale;
	out << YAML::Key << "Zoom Sensetivity" << YAML::Value << camera->m_ZoomSensetivity;
	out << YAML::Key << "Z near" << YAML::Value << camera->m_Znear;
	out << YAML::Key << "Z far" << YAML::Value << camera->m_Zfar;
	out << YAML::Key << "Speed" << YAML::Value << camera->m_Speed;
	out << YAML::Key << "Type" << YAML::Value << (int)camera->m_Type;

	out << YAML::EndMap;
}

Camera Serializer::DeserializeCamera(YAML::Node& in)
{
	Camera camera;
	if (auto& cameraIn = in["Camera"])
	{
		camera.m_Transform = DeserializeTransform(cameraIn);

		if (auto& fovData = cameraIn["Fov"])
		{
			camera.SetFov(fovData.as<float>());
		}

		if (auto& zoomData = cameraIn["Zoom"])
		{
			camera.m_ZoomScale = zoomData.as<float>();
		}

		if (auto& zoomSensetivityData = cameraIn["Zoom Sensetivity"])
		{
			camera.m_ZoomSensetivity = zoomSensetivityData.as<float>();
		}

		if (auto& zNearData = cameraIn["Z near"])
		{
			camera.SetZNear(zNearData.as<float>());
		}

		if (auto& zFarData = cameraIn["Z far"])
		{
			camera.SetZFar(zFarData.as<float>());
		}

		if (auto& speedData = cameraIn["Speed"])
		{
			camera.m_Speed = speedData.as<float>();
		}

		if (auto& typeData = cameraIn["Type"])
		{
			camera.SetType((Camera::CameraType)typeData.as<int>());
		}
	}
	
	camera.UpdateProjection();
	
	return camera;
}

void Serializer::SerializeEnvironment(YAML::Emitter& out)
{
	Environment& environment = Environment::GetInstance();

	out << YAML::Key << "Environment";
	out << YAML::BeginMap;

	out << YAML::Key << "Global Intensity" << YAML::Value << environment.GetGlobalIntensity();
	out << YAML::Key << "Directional Light" << YAML::Value << environment.GetGlobalIntensity();
	out << YAML::Key << "Blur passes" << YAML::Value << environment.m_BloomSettings.m_BlurPasses;
	out << YAML::Key << "Brightness threshold" << YAML::Value << environment.m_BloomSettings.m_BrightnessThreshold;
	out << YAML::Key << "Exposure" << YAML::Value << environment.m_BloomSettings.m_Exposure;
	out << YAML::Key << "Gamma" << YAML::Value << environment.m_BloomSettings.m_Gamma;
	out << YAML::Key << "Pixels blured" << YAML::Value << environment.m_BloomSettings.m_PixelsBlured;
	out << YAML::Key << "IsEnabled" << YAML::Value << environment.m_BloomSettings.m_IsEnabled;
	out << YAML::Key << "DepthTest" << YAML::Value << environment.m_DepthTest;

	SerializeShadows(out);

	out << YAML::EndMap;
}

void Serializer::DeserializeEnvironment(YAML::Node& in)
{
	if (auto& environmentIn = in["Environment"])
	{
		Environment& environment = Environment::GetInstance();

		if (auto& globalIntensityData = environmentIn["Global Intensity"])
		{
			environment.SetGlobalIntensity(globalIntensityData.as<float>());
		}

		if (auto& blurPassesData = environmentIn["Blur passes"])
		{
			environment.m_BloomSettings.m_BlurPasses = blurPassesData.as<int>();
		}

		if (auto& brightnessThresholdData = environmentIn["Brightness threshold"])
		{
			environment.m_BloomSettings.m_BrightnessThreshold = brightnessThresholdData.as<float>();
		}

		if (auto& exposureData = environmentIn["Exposure"])
		{
			environment.m_BloomSettings.m_Exposure = exposureData.as<float>();
		}

		if (auto& GammaData = environmentIn["Gamma"])
		{
			environment.m_BloomSettings.m_Gamma = GammaData.as<float>();
		}

		if (auto& pixelsbluredData = environmentIn["Pixels blured"])
		{
			environment.m_BloomSettings.m_PixelsBlured = pixelsbluredData.as<int>();
		}

		if (auto& isEnabledData = environmentIn["IsEnabled"])
		{
			environment.m_BloomSettings.m_IsEnabled = isEnabledData.as<bool>();
		}

		if (auto& depthTestData = environmentIn["DepthTest"])
		{
			environment.SetDepthTest(depthTestData.as<bool>());
		}

		environment.m_ShadowsSettings = DeserializeShadows(environmentIn);
	}
}

void Serializer::SerializeShadows(YAML::Emitter& out)
{
	Environment::ShadowsSettings& shadows = Environment::GetInstance().m_ShadowsSettings;

	out << YAML::Key << "Shadows";
	out << YAML::BeginMap;

	out << YAML::Key << "Bias" << YAML::Value << shadows.m_Bias;
	out << YAML::Key << "IsEnabled" << YAML::Value << shadows.m_IsEnabled;
	out << YAML::Key << "IsVisualized" << YAML::Value << shadows.m_IsVisualized;
	out << YAML::Key << "Pcf" << YAML::Value << shadows.m_Pcf;
	out << YAML::Key << "MaxPointLightShadows" << YAML::Value << shadows.m_MaxPointLightShadows;
	out << YAML::Key << "Fog" << YAML::Value << shadows.m_Fog;
	out << YAML::Key << "Texels" << YAML::Value << shadows.m_Texels;
	out << YAML::Key << "ZFarScale" << YAML::Value << shadows.m_ZFarScale;
	out << YAML::Key << "CascadesDistance" << YAML::Value << shadows.m_CascadesDistance;

	out << YAML::Key << "Blur";
	out << YAML::BeginMap;

	out << YAML::Key << "Passes" << YAML::Value << shadows.m_Blur.m_BlurPasses;
	out << YAML::Key << "PixelsBlured" << YAML::Value << shadows.m_Blur.m_PixelsBlured;

	out << YAML::EndMap;

	out << YAML::EndMap;
}

Environment::ShadowsSettings Serializer::DeserializeShadows(YAML::Node& in)
{
	Environment::ShadowsSettings shadows;

	if (auto& shadowsIn = in["Shadows"])
	{
		if (auto& biasData = shadowsIn["Bias"])
		{
			shadows.m_Bias = biasData.as<float>();
		}

		if (auto& isEnabledData = shadowsIn["IsEnabled"])
		{
			shadows.m_IsEnabled = isEnabledData.as<bool>();
		}

		if (auto& isVisualizedData = shadowsIn["IsVisualized"])
		{
			shadows.m_IsVisualized = isVisualizedData.as<bool>();
		}

		if (auto& pcfData = shadowsIn["Pcf"])
		{
			shadows.m_Pcf = pcfData.as<int>();
		}

		if (auto& maxPointLightShadowsData = shadowsIn["MaxPointLightShadows"])
		{
			shadows.m_MaxPointLightShadows = maxPointLightShadowsData.as<int>();
		}

		if (auto& fogData = shadowsIn["Fog"])
		{
			shadows.m_Fog = fogData.as<float>();
		}

		if (auto& zFarScaleData = shadowsIn["ZFarScale"])
		{
			shadows.m_ZFarScale = zFarScaleData.as<float>();
		}

		if (auto& texelsData = shadowsIn["Texels"])
		{
			shadows.m_Texels = texelsData.as<float>();
		}

		if (auto& cascadesDistanceData = shadowsIn["CascadesDistance"])
		{
			shadows.m_CascadesDistance = cascadesDistanceData.as<std::vector<float>>();
		}

		if (auto& blurData = shadowsIn["Blur"])
		{
			if (auto& blurPasses = blurData["Passes"])
			{
				shadows.m_Blur.m_BlurPasses = blurPasses.as<int>();
			}

			if (auto& pixelsBlured = blurData["PixelsBlured"])
			{
				shadows.m_Blur.m_PixelsBlured = pixelsBlured.as<int>();
			}
		}
	}

	return shadows;
}

std::string Serializer::GenerateMetaFilePath(const std::string& filePath, const std::string& name)
{
	return Utils::GetDirectoryFromFilePath(filePath) + "/" + name + ".meta";
}

std::string Serializer::GenerateMetaFilePathFromFilePath(const std::string& filePath)
{
	return Utils::RemoveResolution(filePath) + ".meta";
}

std::string Serializer::SerializeMeshMeta(Mesh::Meta meta)
{
	std::string metaFilePath = GenerateMetaFilePath(meta.m_FilePath, meta.m_Name);

	YAML::Emitter out;

	out << YAML::BeginMap;

	out << YAML::Key << "Mesh";
	out << YAML::BeginMap;
	
	out << YAML::Key << "Name" << YAML::Value << meta.m_Name;
	out << YAML::Key << "FilePath" << YAML::Value << meta.m_FilePath;
	out << YAML::Key << "CullFace" << YAML::Value << meta.m_CullFace;

	out << YAML::EndMap;
	
	out << YAML::EndMap;

	std::ofstream fout(metaFilePath);
	fout << out.c_str();

	Logger::Success("has been serialized!", "Mesh meta", metaFilePath.c_str());

	return metaFilePath;
}

Mesh::Meta Serializer::DeserializeMeshMeta(const std::string& filePath)
{
	if (filePath.empty() || !Utils::Contains(filePath, ".meta"))
	{
		return {};
	}

	std::ifstream stream(filePath);
	std::stringstream strStream;

	strStream << stream.rdbuf();

	YAML::Node data = YAML::LoadMesh(strStream.str())["Mesh"];
	if (!data)
	{
		return {};
	}

	Mesh::Meta meta;
	
	if (auto& nameData = data["Name"])
	{
		meta.m_Name = nameData.as<std::string>();
	}

	if (auto& filePathData = data["FilePath"])
	{
		meta.m_FilePath = filePathData.as<std::string>();
	}

	if (auto& cullFaceData = data["CullFace"])
	{
		meta.m_CullFace = cullFaceData.as<bool>();
	}

	return meta;
}

std::string Serializer::SerializeTextureMeta(Texture::Meta meta)
{
	std::string metaFilePath = GenerateMetaFilePath(meta.m_FilePath, meta.m_Name);

	YAML::Emitter out;

	out << YAML::BeginMap;

	out << YAML::Key << "Texture";
	out << YAML::BeginMap;

	out << YAML::Key << "Name" << YAML::Value << meta.m_Name;
	out << YAML::Key << "FilePath" << YAML::Value << meta.m_FilePath;
	out << YAML::Key << "Params" << YAML::Value << meta.m_Params;

	out << YAML::EndMap;

	out << YAML::EndMap;

	std::ofstream fout(metaFilePath);
	fout << out.c_str();

	Logger::Success("has been serialized!", "Texture meta", metaFilePath.c_str());

	return metaFilePath;
}

Texture::Meta Serializer::DeserializeTextureMeta(const std::string& filePath)
{
	if (filePath.empty() || !Utils::Contains(filePath, ".meta"))
	{
		return {};
	}

	std::ifstream stream(filePath);
	std::stringstream strStream;

	strStream << stream.rdbuf();

	YAML::Node data = YAML::LoadMesh(strStream.str())["Texture"];
	if (!data)
	{
		return {};
	}

	Texture::Meta meta;

	if (auto& nameData = data["Name"])
	{
		meta.m_Name = nameData.as<std::string>();
	}

	if (auto& filePathData = data["FilePath"])
	{
		meta.m_FilePath = filePathData.as<std::string>();
	}

	if (auto& paramsData = data["Params"])
	{
		meta.m_Params = paramsData.as<std::vector<int>>();
	}

	return meta;
}

void Serializer::SerializeTransform(YAML::Emitter& out, const Transform& transform)
{
	out << YAML::Key << "Transform";
	out << YAML::BeginMap;
	out << YAML::Key << "Follow owner" << YAML::Value << transform.GetFollorOwner();
	out << YAML::Key << "Position" << YAML::Value << transform.GetPosition();
	out << YAML::Key << "Rotation" << YAML::Value << transform.GetRotation();
	out << YAML::Key << "Scale" << YAML::Value << transform.GetScale();
	out << YAML::EndMap;
}

Transform Serializer::DeserializeTransform(YAML::Node& in)
{
	if (auto& transformIn = in["Transform"])
	{
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

		if (auto& positionData = transformIn["Position"])
		{
			position = positionData.as<glm::vec3>();
		}

		if (auto& rotationData = transformIn["Rotation"])
		{
			rotation = rotationData.as<glm::vec3>();
		}

		if (auto& scaleData = transformIn["Scale"])
		{
			scale = scaleData.as<glm::vec3>();
		}

		Transform transform = Transform(position, scale, rotation);

		if (auto& followOwnerData = transformIn["Follow owner"])
		{
			transform.SetFollowOwner(followOwnerData.as<bool>());
		}

		return transform;
	}
	else
	{
		Logger::Error("Is null", "Transform", "Serializer");
		throw "Transform is null";
	}
}

void Serializer::SerializeGameObjects(YAML::Emitter& out, const Scene& scene)
{
	out << YAML::Key << "GameObjects";
	out << YAML::Value << YAML::BeginSeq;

	std::vector<GameObject*> gameObjects = scene.GetGameObjects();

	for (GameObject* gameObject : gameObjects)
	{
		if (gameObject->m_IsSerializable)
		{
			SerializeGameObject(out, *gameObject);
		}
	}

	out << YAML::EndSeq;
}

void Serializer::SerializeGameObject(YAML::Emitter& out, GameObject& gameObject)
{
	out << YAML::BeginMap;
	out << YAML::Key << "GameObject" << YAML::Value << gameObject.m_UUID;
	out << YAML::Key << "Name" << YAML::Value << gameObject.m_Name;
	out << YAML::Key << "Is Enabled" << YAML::Value << gameObject.m_IsEnabled;
	out << YAML::Key << "Is Selectable" << YAML::Value << gameObject.m_IsSelectable;
	out << YAML::Key << "Prefab File Path" << YAML::Value << gameObject.m_PrefabFilePath;

	SerializeGameObjectChilds(out, gameObject);
	SerializeTransform(out, gameObject.m_Transform);
	SerializeRenderer2D(out, gameObject.m_ComponentManager);
	SerializeRenderer3D(out, gameObject.m_ComponentManager);
	SerializeBoxCollider2D(out, gameObject.m_ComponentManager);
	SerializeBoxCollider3D(out, gameObject.m_ComponentManager);
	SerializeCircleCollider2D(out, gameObject.m_ComponentManager);
	SerializeSphereCollider3D(out, gameObject.m_ComponentManager);
	SerializeRigidbody2D(out, gameObject.m_ComponentManager);
	SerializeRigidbody3D(out, gameObject.m_ComponentManager);
	SerializeAnimator2D(out, gameObject.m_ComponentManager);
	SerializeParticleEmitter(out, gameObject.m_ComponentManager);
	SerializeScript(out, gameObject.m_ComponentManager);
	SerializeSpotLight(out, gameObject.m_ComponentManager);
	SerializePointLight(out, gameObject.m_ComponentManager);
	SerializeDirectionalLight(out, gameObject.m_ComponentManager);
	SerializeUserDefinedComponents(out, gameObject.m_ComponentManager);
	SerializeSpline(out, gameObject.m_ComponentManager);

	out << YAML::EndMap;
}

void Serializer::DeserializeGameObject(YAML::Node& in, Scene& scene, std::unordered_map<GameObject*, std::vector<std::string>>& childs)
{
	GameObject* gameObject = nullptr;

	if (auto& nameData = in["Name"])
	{
		UUID previousUuid = UUID(in["GameObject"].as<std::string>());
		
		gameObject = scene.CreateGameObject(nameData.as<std::string>(),
			DeserializeTransform(in), previousUuid);
		for (auto& child : childs)
		{
			for (size_t i = 0; i < child.second.size(); ++i)
			{
				if (previousUuid.Get() != gameObject->GetUUID() && child.second[i] == previousUuid.Get())
				{
					child.second[i] = gameObject->GetUUID();
				}
			}
		}
	}
	else
	{
		Logger::Error("Name is null", "GameObject", "Serializer");
		throw "GameObject cannot be created, name is null";
	}

	if (auto& prefabData = in["Prefab File Path"])
	{
		gameObject->m_PrefabFilePath = prefabData.as<std::string>();
	}

	if (auto& isEnabledData = in["Is Enabled"])
	{
		gameObject->m_IsEnabled = isEnabledData.as<bool>();
	}

	if (auto& isSelectableData = in["Is Selectable"])
	{
		gameObject->m_IsSelectable = isSelectableData.as<bool>();
	}

	DeserializeGameObjectChilds(in, gameObject, childs);
	DeSerializeRenderer2D(in, gameObject->m_ComponentManager);
	DeSerializeRenderer3D(in, gameObject->m_ComponentManager);
	DeSerializeBoxCollider2D(in, gameObject->m_ComponentManager);
	DeSerializeBoxCollider3D(in, gameObject->m_ComponentManager);
	DeSerializeCircleCollider2D(in, gameObject->m_ComponentManager);
	DeSerializeSphereCollider3D(in, gameObject->m_ComponentManager);
	DeSerializeRigidbody2D(in, gameObject->m_ComponentManager);
	DeSerializeRigidbody3D(in, gameObject->m_ComponentManager);
	DeSerializeAnimator2D(in, gameObject->m_ComponentManager);
	DeSerializeParticleEmitter(in, gameObject->m_ComponentManager);
	DeSerializeScript(in, gameObject->m_ComponentManager);
	DeSerializeSpotLight(in, gameObject->m_ComponentManager);
	DeSerializePointLight(in, gameObject->m_ComponentManager);
	DeSerializeDirectionalLight(in, gameObject->m_ComponentManager);
	DeserializeUserDefinedComponents(in, gameObject->m_ComponentManager);
	DeSerializeSpline(in, gameObject->m_ComponentManager);
}

void Serializer::SerializeGameObjectChilds(YAML::Emitter& out, GameObject& gameObject)
{
	std::vector<GameObject*> childs = gameObject.GetChilds();
	std::vector<std::string> childsUUID;
	for (auto& child : childs)
	{
		childsUUID.push_back(child->GetUUID());
	}
	out << YAML::Key << "Childs" << YAML::Value << childsUUID;
}

void Serializer::DeserializeGameObjectChilds(YAML::Node& in, GameObject* gameObject,
	std::unordered_map<GameObject*, std::vector<std::string>>& childs)
{
	if (auto& childsIn = in["Childs"])
	{
		std::pair<GameObject*, std::vector<std::string>> pair = std::make_pair(gameObject, std::vector<std::string>());
		std::vector<std::string> childsUUID = childsIn.as<std::vector<std::string>>();
		for (auto childUUID : childsUUID)
		{
			pair.second.push_back(childUUID);
		}
		
		childs.insert(pair);
	}
}

void Serializer::SerializeRenderer3D(YAML::Emitter& out, ComponentManager& componentManager)
{
	Renderer3D* r3d = componentManager.GetComponent<Renderer3D>();
	if (r3d != nullptr)
	{
		out << YAML::Key << "Renderer3D";

		out << YAML::BeginMap;
		
		if (r3d->m_Mesh)
		{
			out << YAML::Key << "Mesh" << YAML::Value << r3d->m_Mesh->GetFilePath();
		}

		out << YAML::Key << "IsOpaque" << YAML::Value << r3d->m_IsOpaque;

		out << YAML::Key << "DrawShadows" << YAML::Value << r3d->m_DrawShadows;

		out << YAML::Key << "LodsDistance" << YAML::Value << r3d->m_LodsDistance;

		out << YAML::Key << "Material" << YAML::Value << r3d->m_Material->GetFilePath();

		std::vector<std::string> lods;
		lods.reserve(3);
		for (size_t i = 0; i < 3; i++)
		{
			lods.push_back(r3d->m_Lods[i] ? r3d->m_Lods[i]->GetFilePath() : "None");
		}

		out << YAML::Key << "Lods" << YAML::Value << lods;

		out << YAML::EndMap;
	}
}

void Serializer::DeSerializeRenderer3D(YAML::Node& in, ComponentManager& componentManager)
{
	if (auto& renderer3DIn = in["Renderer3D"])
	{
		Renderer3D* r3d = componentManager.AddComponent<Renderer3D>();

		auto loadMesh = [r3d](const std::string& filePath, size_t lod)
		{
			if (Utils::Contains(filePath, ".meta"))
			{
				MeshManager::GetInstance().LoadAsync(filePath,
					[r3d, lod](Mesh* mesh)
				{
					r3d->SetMesh(mesh, lod);
					r3d->m_CurrentLOD = -1;
				});
			}
			else
			{
				MeshManager::GetInstance().LoadAsync(Serializer::GenerateMetaFilePathFromFilePath(filePath),
					[r3d, lod](Mesh* mesh)
				{
					r3d->SetMesh(mesh, lod);
					r3d->m_CurrentLOD = -1;
				});
			}
		};

		if (auto& meshData = renderer3DIn["Mesh"])
		{
			loadMesh(meshData.as<std::string>(), 0);
		}

		if (auto& lodsData = renderer3DIn["Lods"])
		{
			std::vector<std::string> lods = lodsData.as<std::vector<std::string>>();

			for (size_t i = 0; i < lods.size(); i++)
			{
				if (lods[i] != "None")
				{
					loadMesh(lods[i], i);
				}
			}
		}

		if (auto& drawShadowsData = renderer3DIn["DrawShadows"])
		{
			r3d->m_DrawShadows = drawShadowsData.as<bool>();
		}

		if (auto& isOpaqueData = renderer3DIn["IsOpaque"])
		{
			r3d->m_IsOpaque = isOpaqueData.as<bool>();
		}

		bool isInherited = false;
		if (auto& isInheritedData = renderer3DIn["IsInherited"])
		{
			isInherited = isInheritedData.as<bool>();
		}

		if (auto& lodsDistanceData = renderer3DIn["LodsDistance"])
		{
			r3d->m_LodsDistance = lodsDistanceData.as<std::vector<float>>();
		}

		if (auto& MaterialData = renderer3DIn["Material"])
		{
			r3d->SetMaterial(MaterialManager::GetInstance().Load(MaterialData.as<std::string>()));
		}

		if (auto& inheritedData = renderer3DIn["IsInherited"])
		{
			if (inheritedData.as<bool>())
			{
				std::string filePathData = renderer3DIn["m_FilePath"]["Value"].as<std::string>();
				r3d->SetMaterial(MaterialManager::GetInstance().Load(filePathData));
			}
		}
	}
}

void Serializer::SerializeRenderer2D(YAML::Emitter& out, ComponentManager& componentManager)
{
	Renderer2D* r2d = componentManager.GetComponent<Renderer2D>();
	if (r2d != nullptr)
	{
		out << YAML::Key << "Renderer2D";
		out << YAML::BeginMap;
		out << YAML::Key << "Color" << YAML::Value << r2d->m_Color;
		out << YAML::Key << "Texture" << YAML::Value << 
			(r2d->m_Texture != nullptr ? r2d->m_Texture->GetFilePath() : "White");
		
		out << YAML::Key << "Normal Texture" << YAML::Value <<
			(r2d->m_NormalTexture != nullptr ? r2d->m_NormalTexture->GetFilePath() : "White");

		out << YAML::Key << "Emissive Mask Texture" << YAML::Value <<
			(r2d->m_EmissiveMaskTexture != nullptr ? r2d->m_EmissiveMaskTexture->GetFilePath() : "White");

		std::vector<float> uv;
		for (uint32_t i = 0; i < 8; i++)
		{
			uv.push_back(r2d->m_UV[i]);
		}
		out << YAML::Key << "UV" << YAML::Value << uv;
		out << YAML::Key << "Layer" << YAML::Value << r2d->m_Layer;
		out << YAML::Key << "Inner radius" << YAML::Value << r2d->m_InnerRadius;
		out << YAML::Key << "Outer radius" << YAML::Value << r2d->m_OuterRadius;
		out << YAML::Key << "Is Normal used" << YAML::Value << r2d->m_IsNormalUsed;
		out << YAML::Key << "Ambient" << YAML::Value << r2d->m_Ambient;
		out << YAML::Key << "Emmisive Mask Intensity" << YAML::Value << r2d->m_EmmisiveMaskIntensity;
		out << YAML::EndMap;
	}
}

void Serializer::DeSerializeRenderer2D(YAML::Node& in, ComponentManager& componentManager)
{
	if (auto& renderer2DIn = in["Renderer2D"])
	{
		Renderer2D* r2d = componentManager.AddComponent<Renderer2D>();
		
		if (auto& colorData = renderer2DIn["Color"])
		{
			r2d->m_Color = colorData.as<glm::vec4>();
		}
		
		if (auto& textureData = renderer2DIn["Texture"])
		{
			std::string textureFilePath = textureData.as<std::string>();
			if (textureFilePath == "White")
			{
				r2d->SetTexture(TextureManager::GetInstance().White());
			}
			else
			{
				TextureManager::GetInstance().AsyncLoad(textureFilePath,
					[=](Texture* texture)
				{
					r2d->SetTexture(texture);
				});
			}
		}

		if (auto& textureData = renderer2DIn["Normal Texture"])
		{
			std::string textureFilePath = textureData.as<std::string>();
			if (textureFilePath == "White")
			{
				r2d->SetNormalTexture(TextureManager::GetInstance().White());
			}
			else
			{
				TextureManager::GetInstance().AsyncLoad(textureFilePath,
					[=](Texture* texture) 
				{
					r2d->SetNormalTexture(texture);
				});
			}
		}

		if (auto& textureData = renderer2DIn["Emissive Mask Texture"])
		{
			std::string textureFilePath = textureData.as<std::string>();
			if (textureFilePath == "White")
			{
				r2d->SetEmissiveMaskTexture(TextureManager::GetInstance().White());
			}
			else
			{
				TextureManager::GetInstance().AsyncLoad(textureFilePath,
					[=](Texture* texture) 
				{
					r2d->SetEmissiveMaskTexture(texture);
				});
			}
		}

		if (auto& layerData = renderer2DIn["Layer"])
		{
			r2d->SetLayer(layerData.as<int>());
		}

		if (auto& ambientData = renderer2DIn["Ambient"])
		{
			r2d->m_Ambient = ambientData.as<float>();
		}

		if (auto& emissiveMaskIntensityData = renderer2DIn["Emmisive Mask Intensity"])
		{
			r2d->m_EmmisiveMaskIntensity = emissiveMaskIntensityData.as<float>();
		}

		if (auto& innerRadiusData = renderer2DIn["Inner radius"])
		{
			r2d->m_InnerRadius = innerRadiusData.as<float>();
		}

		if (auto& OuterRadiusData = renderer2DIn["Outer radius"])
		{
			r2d->m_OuterRadius = OuterRadiusData.as<float>();
		}

		if (auto& isNormalUsedData = renderer2DIn["Is Normal used"])
		{
			r2d->m_IsNormalUsed = isNormalUsedData.as<bool>();
		}

		if (auto& uvData = renderer2DIn["UV"])
		{
			std::vector<float> uv = uvData.as<std::vector<float>>();
			for (size_t i = 0; i < 8; i++)
			{
				r2d->m_UV[i] = uv[i];
			}
		}
	}
}

void Serializer::SerializeBoxCollider2D(YAML::Emitter& out, ComponentManager& componentManager)
{
	BoxCollider2D* bc2d = componentManager.GetComponent<BoxCollider2D>();
	if (bc2d != nullptr)
	{
		out << YAML::Key << "BoxCollider2D";
		out << YAML::BeginMap;
		out << YAML::Key << "Trigger" << YAML::Value << bc2d->IsTrigger();
		out << YAML::Key << "Offset" << YAML::Value << bc2d->GetOffset();
		out << YAML::Key << "Size" << YAML::Value << bc2d->GetSize();
		out << YAML::Key << "Density" << YAML::Value << bc2d->GetDensity();
		out << YAML::Key << "Friction" << YAML::Value << bc2d->GetFriction();
		out << YAML::Key << "Restitution" << YAML::Value << bc2d->GetRestitution();
		out << YAML::Key << "Restitution threshold" << YAML::Value << bc2d->GetRestitutionThreshold();
		out << YAML::Key << "Tag" << YAML::Value << bc2d->GetTag();
		out << YAML::EndMap;
	}
}

void Serializer::DeSerializeBoxCollider2D(YAML::Node& in, ComponentManager& componentManager)
{
	if (auto& boxCollider2DIn = in["BoxCollider2D"])
	{
		BoxCollider2D* bc2d = componentManager.AddComponent<BoxCollider2D>();

		if (auto& triggerData = boxCollider2DIn["Trigger"])
		{
			bc2d->SetTrigger(triggerData.as<bool>());
		}

		if (auto& offsetData = boxCollider2DIn["Offset"])
		{
			bc2d->SetOffset(offsetData.as<glm::vec2>());
		}

		if (auto& sizeData = boxCollider2DIn["Size"])
		{
			bc2d->SetSize(sizeData.as<glm::vec2>());
		}

		if (auto& densityData = boxCollider2DIn["Density"])
		{
			bc2d->SetDensity(densityData.as<float>());
		}

		if (auto& frictionData = boxCollider2DIn["Friction"])
		{
			bc2d->SetFriction(frictionData.as<float>());
		}
	
		if (auto& restitutionData = boxCollider2DIn["Restitution"])
		{
			bc2d->SetRestitution(restitutionData.as<float>());
		}
		
		if (auto& restitutionData = boxCollider2DIn["Restitution threshold"])
		{
			bc2d->SetRestitutionThreshold(restitutionData.as<float>());
		}

		if (auto& tagData = boxCollider2DIn["Tag"])
		{
			bc2d->SetTag(tagData.as<std::string>());
		}
	}
}

void Serializer::SerializeBoxCollider3D(YAML::Emitter& out, ComponentManager& componentManager)
{
	BoxCollider3D* bc3d = componentManager.GetComponent<BoxCollider3D>();
	if (bc3d != nullptr)
	{
		out << YAML::Key << "BoxCollider3D";
		out << YAML::BeginMap;
		out << YAML::Key << "Offset" << YAML::Value << bc3d->GetOffset();
		out << YAML::Key << "HalfExtent" << YAML::Value << bc3d->GetHalfExtent();
		out << YAML::EndMap;
	}
}

void Serializer::DeSerializeBoxCollider3D(YAML::Node& in, ComponentManager& componentManager)
{
	if (auto& boxCollider3DIn = in["BoxCollider3D"])
	{
		BoxCollider3D* bc3d = componentManager.AddComponent<BoxCollider3D>();

		if (auto& offsetData = boxCollider3DIn["Offset"])
		{
			bc3d->SetOffset(offsetData.as<glm::vec3>());
		}

		if (auto& halfExtentData = boxCollider3DIn["HalfExtent"])
		{
			bc3d->SetHalfExtent(halfExtentData.as<glm::vec3>());
		}
	}
}

void Serializer::SerializeCircleCollider2D(YAML::Emitter& out, ComponentManager& componentManager)
{
	CircleCollider2D* cc2d = componentManager.GetComponent<CircleCollider2D>();
	if (cc2d != nullptr)
	{
		out << YAML::Key << "CircleCollider2D";
		out << YAML::BeginMap;
		out << YAML::Key << "Trigger" << YAML::Value << cc2d->IsTrigger();
		out << YAML::Key << "Offset" << YAML::Value << cc2d->GetOffset();
		out << YAML::Key << "Radius" << YAML::Value << cc2d->GetRadius();
		out << YAML::Key << "Density" << YAML::Value << cc2d->GetDensity();
		out << YAML::Key << "Friction" << YAML::Value << cc2d->GetFriction();
		out << YAML::Key << "Restitution" << YAML::Value << cc2d->GetRestitution();
		out << YAML::Key << "Restitution threshold" << YAML::Value << cc2d->GetRestitutionThreshold();
		out << YAML::Key << "Tag" << YAML::Value << cc2d->GetTag();
		out << YAML::EndMap;
	}
}

void Serializer::DeSerializeCircleCollider2D(YAML::Node& in, ComponentManager& componentManager)
{
	if (auto& circleCollider2DIn = in["CircleCollider2D"])
	{
		CircleCollider2D* cc2d = componentManager.AddComponent<CircleCollider2D>();

		if (auto& triggerData = circleCollider2DIn["Trigger"])
		{
			cc2d->SetTrigger(triggerData.as<bool>());
		}

		if (auto& offsetData = circleCollider2DIn["Offset"])
		{
			cc2d->SetOffset(offsetData.as<glm::vec2>());
		}

		if (auto& radiusData = circleCollider2DIn["Radius"])
		{
			cc2d->SetRadius(radiusData.as<float>());
		}

		if (auto& densityData = circleCollider2DIn["Density"])
		{
			cc2d->SetDensity(densityData.as<float>());
		}

		if (auto& frictionData = circleCollider2DIn["Friction"])
		{
			cc2d->SetFriction(frictionData.as<float>());
		}

		if (auto& restitutionData = circleCollider2DIn["Restitution"])
		{
			cc2d->SetRestitution(restitutionData.as<float>());
		}

		if (auto& restitutionData = circleCollider2DIn["Restitution threshold"])
		{
			cc2d->SetRestitutionThreshold(restitutionData.as<float>());
		}

		if (auto& tagData = circleCollider2DIn["Tag"])
		{
			cc2d->SetTag(tagData.as<std::string>());
		}
	}
}

void Serializer::SerializeSphereCollider3D(YAML::Emitter& out, ComponentManager& componentManager)
{
	SphereCollider3D* sc3d = componentManager.GetComponent<SphereCollider3D>();
	if (sc3d)
	{
		out << YAML::Key << "SphereCollider3D";
		out << YAML::BeginMap;
		out << YAML::Key << "Radius" << YAML::Value << sc3d->GetRadius();
		out << YAML::Key << "Offset" << YAML::Value << sc3d->GetOffset();
		out << YAML::EndMap;
	}
}

void Serializer::DeSerializeSphereCollider3D(YAML::Node& in, ComponentManager& componentManager)
{
	if (auto& sphereCollider3DIn = in["SphereCollider3D"])
	{
		SphereCollider3D* sc3d = componentManager.AddComponent<SphereCollider3D>();

		if (auto& radiusData = sphereCollider3DIn["Radius"])
		{
			sc3d->SetRadius(radiusData.as<float>());
		}

		if (auto& offsetData = sphereCollider3DIn["Offset"])
		{
			sc3d->SetOffset(offsetData.as<glm::vec3>());
		}
	}
}

void Serializer::SerializeRigidbody2D(YAML::Emitter& out, ComponentManager& componentManager)
{
	Rigidbody2D* rb2d = componentManager.GetComponent<Rigidbody2D>();
	if (rb2d != nullptr)
	{
		out << YAML::Key << "Rigidbody2D";
		out << YAML::BeginMap;
		out << YAML::Key << "Type" << YAML::Value << (int)(!rb2d->IsStatic());
		out << YAML::Key << "Fixed rotation" << YAML::Value << rb2d->IsFixedRotation();
		out << YAML::EndMap;
	}
}

void Serializer::DeSerializeRigidbody2D(YAML::Node& in, ComponentManager& componentManager)
{
	if (auto& rigidbody2DIn = in["Rigidbody2D"])
	{
		Rigidbody2D* rb2d = componentManager.AddComponent<Rigidbody2D>();

		if (auto& typeData = rigidbody2DIn["Type"])
		{
			rb2d->SetStatic(!((bool)typeData.as<int>()));
		}

		if (auto& fixedRotationData = rigidbody2DIn["Fixed rotation"])
		{
			rb2d->SetFixedRotation(fixedRotationData.as<bool>());
		}
	}
}

void Serializer::SerializeRigidbody3D(YAML::Emitter& out, ComponentManager& componentManager)
{
	Rigidbody3D* rb3d = componentManager.GetComponent<Rigidbody3D>();
	if (rb3d  != nullptr)
	{
		out << YAML::Key << "Rigidbody3D";
		out << YAML::BeginMap;
		out << YAML::Key << "Mass" << YAML::Value << rb3d->m_Mass;
		out << YAML::Key << "AllowToSleep" << YAML::Value << rb3d->m_AllowToSleep;
		out << YAML::Key << "Restitution" << YAML::Value << rb3d->m_Restitution;
		out << YAML::Key << "Friction" << YAML::Value << rb3d->m_Friction;
		out << YAML::EndMap;
	}
}

void Serializer::DeSerializeRigidbody3D(YAML::Node& in, ComponentManager& componentManager)
{
	if (auto& rigidbody3DIn = in["Rigidbody3D"])
	{
		Rigidbody3D* rb3d = componentManager.AddComponent<Rigidbody3D>();

		if (auto& massData = rigidbody3DIn["Mass"])
		{
			rb3d->m_Mass = massData.as<float>();
		}

		if (auto& restitutionData = rigidbody3DIn["Restitution"])
		{
			rb3d->m_Restitution = restitutionData.as<float>();
		}
		
		if (auto& frictionData = rigidbody3DIn["Friction"])
		{
			rb3d->m_Friction = frictionData.as<float>();
		}

		if (auto& allowToSleepData = rigidbody3DIn["AllowToSleep"])
		{
			rb3d->m_AllowToSleep = allowToSleepData.as<bool>();
		}
	}
}

void Serializer::SerializeAnimator2D(YAML::Emitter& out, ComponentManager& componentManager)
{
	Animator2D* a2d = componentManager.GetComponent<Animator2D>();
	if (a2d != nullptr)
	{
		out << YAML::Key << "Animator2D";
		out << YAML::BeginMap;
		out << YAML::Key << "Speed" << YAML::Value << a2d->GetSpeed();
		out << YAML::Key << "Play" << YAML::Value << a2d->IsPlaying();

		std::vector<std::string> animationsFilePaths;

		std::vector<Animation2DManager::Animation2D*> animations = a2d->GetAnimations();
		for (auto animation : animations)
		{
			animationsFilePaths.push_back(animation->GetFilePath());
		}

		out << YAML::Key << "Animations" << YAML::Value << animationsFilePaths;
		out << YAML::Key << "Current animation" << YAML::Value << (a2d->GetCurrentAnimation() ? 
			a2d->GetCurrentAnimation()->GetFilePath().c_str() : "Null");

		out << YAML::EndMap;
	}
}

void Serializer::DeSerializeAnimator2D(YAML::Node& in, ComponentManager& componentManager)
{
	if (auto& Animator2DIn = in["Animator2D"])
	{
		Animator2D* a2d = componentManager.AddComponent<Animator2D>();

		if (auto& speedData = Animator2DIn["Speed"])
		{
			a2d->SetSpeed(speedData.as<float>());
		}

		if (auto& playData = Animator2DIn["Play"])
		{
			a2d->SetPlay(playData.as<bool>());
		}

		if (auto& animationsData = Animator2DIn["Animations"])
		{
			std::vector<std::string> AnimationFilePaths = animationsData.as<std::vector<std::string>>();
			for (const std::string& animationFilePath : AnimationFilePaths)
			{
				if (Animation2DManager::Animation2D* animation = Animation2DManager::GetInstance().Load(animationFilePath))
				{
					a2d->AddAnimation(animation);
				}
			}
		}

		if (auto& currentAnimationData = Animator2DIn["Current animation"])
		{
			std::string currentAnimationFilePath = currentAnimationData.as<std::string>();
			a2d->SetCurrentAnimation(Animation2DManager::GetInstance().Get(currentAnimationFilePath));
		}
	}
}

void Serializer::SerializeParticleEmitter(YAML::Emitter& out, ComponentManager& componentManager)
{
	ParticleEmitter* particleEmitter = componentManager.GetComponent<ParticleEmitter>();
	if (particleEmitter != nullptr)
	{
		out << YAML::Key << "Particle Emitter";
		out << YAML::BeginMap;
		out << YAML::Key << "Acceleration" << YAML::Value << particleEmitter->m_Acceleration;
		out << YAML::Key << "Color end" << YAML::Value << particleEmitter->m_ColorEnd;
		out << YAML::Key << "Color start" << YAML::Value << particleEmitter->m_ColorStart;
		out << YAML::Key << "Direction X" << YAML::Value << particleEmitter->m_Direction[0];
		out << YAML::Key << "Direction Y" << YAML::Value << particleEmitter->m_Direction[1];
		out << YAML::Key << "Direction Z" << YAML::Value << particleEmitter->m_Direction[2];
		out << YAML::Key << "Intensity" << YAML::Value << particleEmitter->m_Intensity;
		out << YAML::Key << "Gravity" << YAML::Value << particleEmitter->m_Gravity;
		out << YAML::Key << "Loop" << YAML::Value << particleEmitter->m_Loop;
		out << YAML::Key << "Max time to live" << YAML::Value << particleEmitter->m_MaxTimeToLive;
		out << YAML::Key << "Particles size" << YAML::Value << particleEmitter->m_ParticlesSize;
		out << YAML::Key << "Radius X to spawn" << YAML::Value << particleEmitter->m_RadiusToSpawn[0];
		out << YAML::Key << "Radius Y to spawn" << YAML::Value << particleEmitter->m_RadiusToSpawn[1];
		out << YAML::Key << "Radius Z to spawn" << YAML::Value << particleEmitter->m_RadiusToSpawn[2];
		out << YAML::Key << "Scale" << YAML::Value << particleEmitter->m_Scale;
		out << YAML::Key << "Texture" << YAML::Value << particleEmitter->m_Texture->GetFilePath();
		out << YAML::Key << "Time to spawn" << YAML::Value << particleEmitter->m_TimeToSpawn;
		out << YAML::Key << "Facing mode" << YAML::Value << (int)particleEmitter->m_FacingMode;
		out << YAML::EndMap;
	}
}

void Serializer::DeSerializeParticleEmitter(YAML::Node& in, ComponentManager& componentManager)
{
	if (auto& particleEmitterIn = in["Particle Emitter"])
	{
		ParticleEmitter* particleEmitter = componentManager.AddComponent<ParticleEmitter>();

		if (auto& accelarationData = particleEmitterIn["Acceleration"])
		{
			particleEmitter->m_Acceleration = accelarationData.as<float>();
		}

		if (auto& colorEndData = particleEmitterIn["Color end"])
		{
			particleEmitter->m_ColorEnd = colorEndData.as<glm::vec4>();
		}

		if (auto& colorStartData = particleEmitterIn["Color start"])
		{
			particleEmitter->m_ColorStart = colorStartData.as<glm::vec4>();
		}

		if (auto& directionXData = particleEmitterIn["Direction X"])
		{
			particleEmitter->m_Direction[0] = directionXData.as<glm::vec2>();
		}

		if (auto& directionYData = particleEmitterIn["Direction Y"])
		{
			particleEmitter->m_Direction[1] = directionYData.as<glm::vec2>();
		}

		if (auto& directionZData = particleEmitterIn["Direction Z"])
		{
			particleEmitter->m_Direction[2] = directionZData.as<glm::vec2>();
		}

		if (auto& intensityData = particleEmitterIn["Intensity"])
		{
			particleEmitter->m_Intensity = intensityData.as<float>();
		}

		if (auto& gravityData = particleEmitterIn["Gravity"])
		{
			particleEmitter->m_Gravity = gravityData.as<glm::vec3>();
		}

		if (auto& loopData = particleEmitterIn["Loop"])
		{
			particleEmitter->m_Loop = loopData.as<bool>();
		}

		if (auto& maxTimeToLiveData = particleEmitterIn["Max time to live"])
		{
			particleEmitter->m_MaxTimeToLive = maxTimeToLiveData.as<float>();
		}

		if (auto& particleSizeData = particleEmitterIn["Particles size"])
		{
			particleEmitter->m_ParticlesSize = particleSizeData.as<int>();
		}

		if (auto& radiusXToSpawnData = particleEmitterIn["Radius X to spawn"])
		{
			particleEmitter->m_RadiusToSpawn[0] = radiusXToSpawnData.as<glm::vec2>();
		}

		if (auto& radiusYToSpawnData = particleEmitterIn["Radius Y to spawn"])
		{
			particleEmitter->m_RadiusToSpawn[1] = radiusYToSpawnData.as<glm::vec2>();
		}

		if (auto& radiusZToSpawnData = particleEmitterIn["Radius Z to spawn"])
		{
			particleEmitter->m_RadiusToSpawn[2] = radiusZToSpawnData.as<glm::vec2>();
		}

		if (auto& scaleData = particleEmitterIn["Scale"])
		{
			particleEmitter->m_Scale = scaleData.as<glm::vec2>();
		}

		if (auto& timeToSpawnData = particleEmitterIn["Time to spawn"])
		{
			particleEmitter->m_TimeToSpawn = timeToSpawnData.as<glm::vec2>();
		}

		if (auto& facingModeData = particleEmitterIn["Facing mode"])
		{
			particleEmitter->m_FacingMode = (ParticleEmitter::FacingMode)facingModeData.as<int>();
		}

		if (auto& textureData = particleEmitterIn["Texture"])
		{
			if (textureData.as<std::string>() == "White")
			{
				particleEmitter->m_Texture = TextureManager::GetInstance().White();
			}
			else
			{
				TextureManager::GetInstance().AsyncLoad(textureData.as<std::string>(),
					[=](Texture* texture) 
				{
					particleEmitter->m_Texture = texture; 
				});
			}
		}
	}
}

void Serializer::SerializeScript(YAML::Emitter& out, ComponentManager& componentManager)
{
	Script* script = componentManager.GetComponent<Script>();
	if (script != nullptr && script->m_LStates.size() > 0)
	{
		out << YAML::Key << "Script";
		out << YAML::BeginMap;

		std::vector<std::string> states;
		for (auto& state : script->m_LStates)
		{
			states.push_back(state->m_FilePath);
		}

		out << YAML::Key << "FilePaths" << YAML::Value << states;

		out << YAML::EndMap;
	}
}

void Serializer::DeSerializeScript(YAML::Node& in, ComponentManager& componentManager)
{
	if (auto& scriptIn = in["Script"])
	{
		Script* script = componentManager.AddComponent<Script>();

		if (auto& filePathData = scriptIn["FilePaths"])
		{
			std::vector<std::string> states = filePathData.as<std::vector<std::string>>();
			for (auto& state : states)
			{
				script->Assign(state);
			}
		}
	}
}

void Serializer::SerializePointLight(YAML::Emitter& out, ComponentManager& componentManager)
{
	PointLight* pointLight = componentManager.GetComponent<PointLight>();
	if (pointLight)
	{
		out << YAML::Key << "PointLight";
		out << YAML::BeginMap;

		out << YAML::Key << "Color" << YAML::Value << pointLight->m_Color;
		out << YAML::Key << "Constant" << YAML::Value << pointLight->m_Constant;
		out << YAML::Key << "Linear" << YAML::Value << pointLight->m_Linear;
		out << YAML::Key << "Quadratic" << YAML::Value << pointLight->m_Quadratic;
		out << YAML::Key << "ZFar" << YAML::Value << pointLight->m_ZFar;
		out << YAML::Key << "ZNear" << YAML::Value << pointLight->m_ZNear;
		out << YAML::Key << "Fog" << YAML::Value << pointLight->m_Fog;
		out << YAML::Key << "DrawShadows" << YAML::Value << pointLight->IsDrawShadows();

		out << YAML::EndMap;
	}
}

void Serializer::DeSerializePointLight(YAML::Node& in, ComponentManager& componentManager)
{
	if (auto& pointLightIn = in["PointLight"])
	{
		PointLight* pointLight = componentManager.AddComponent<PointLight>();

		if (auto& colorData = pointLightIn["Color"])
		{
			pointLight->m_Color = colorData.as<glm::vec3>();
		}

		if (auto& constantData = pointLightIn["Constant"])
		{
			pointLight->m_Constant = constantData.as<float>();
		}

		if (auto& linearData = pointLightIn["Linear"])
		{
			pointLight->m_Linear = linearData.as<float>();
		}

		if (auto& quadraticData = pointLightIn["Quadratic"])
		{
			pointLight->m_Quadratic = quadraticData.as<float>();
		}

		if (auto& zNearData = pointLightIn["ZNear"])
		{
			pointLight->m_ZNear = zNearData.as<float>();
		}

		if (auto& zFarData = pointLightIn["ZFar"])
		{
			pointLight->m_ZFar = zFarData.as<float>();
		}

		if (auto& fogData = pointLightIn["Fog"])
		{
			pointLight->m_Fog = fogData.as<float>();
		}

		if (auto& drawShadowsData = pointLightIn["DrawShadows"])
		{
			pointLight->SetDrawShadows(drawShadowsData.as<bool>());
		}
	}
}

void Serializer::SerializeSpotLight(YAML::Emitter& out, ComponentManager& componentManager)
{
	SpotLight* spotLight = componentManager.GetComponent<SpotLight>();
	if (spotLight)
	{
		out << YAML::Key << "SpotLight";
		out << YAML::BeginMap;

		out << YAML::Key << "Color" << YAML::Value << spotLight->m_Color;
		out << YAML::Key << "Constant" << YAML::Value << spotLight->m_Constant;
		out << YAML::Key << "Linear" << YAML::Value << spotLight->m_Linear;
		out << YAML::Key << "Quadratic" << YAML::Value << spotLight->m_Quadratic;
		out << YAML::Key << "InnerCutOff" << YAML::Value << spotLight->GetInnerCutOff();
		out << YAML::Key << "OuterCutOff" << YAML::Value << spotLight->GetOuterCutOfff();

		out << YAML::EndMap;
	}
}

void Serializer::DeSerializeSpotLight(YAML::Node& in, ComponentManager& componentManager)
{
	if (auto& spotLighttIn = in["SpotLight"])
	{
		SpotLight* spotLight = componentManager.AddComponent<SpotLight>();

		if (auto& colorData = spotLighttIn["Color"])
		{
			spotLight->m_Color = colorData.as<glm::vec3>();
		}

		if (auto& constantData = spotLighttIn["Constant"])
		{
			spotLight->m_Constant = constantData.as<float>();
		}

		if (auto& linearData = spotLighttIn["Linear"])
		{
			spotLight->m_Linear = linearData.as<float>();
		}

		if (auto& quadraticData = spotLighttIn["Quadratic"])
		{
			spotLight->m_Quadratic = quadraticData.as<float>();
		}

		if (auto& innerCutOffData = spotLighttIn["InnerCutOff"])
		{
			spotLight->SetInnerCutOff(innerCutOffData.as<float>());
		}

		if (auto& outerCutOffData = spotLighttIn["OuterCutOff"])
		{
			spotLight->SetOuterCutOff(outerCutOffData.as<float>());
		}
	}
}

void Serializer::SerializeDirectionalLight(YAML::Emitter& out, ComponentManager& componentManager)
{
	DirectionalLight* directionalLight = componentManager.GetComponent<DirectionalLight>();
	if (directionalLight)
	{
		out << YAML::Key << "Directional Light";
		out << YAML::BeginMap;

		out << YAML::Key << "Color" << YAML::Value << directionalLight->m_Color;
		out << YAML::Key << "Intensity" << YAML::Value << directionalLight->m_Intensity;

		out << YAML::EndMap;
	}
}

void Serializer::DeSerializeDirectionalLight(YAML::Node& in, ComponentManager& componentManager)
{
	if (auto& directionalLightIn = in["Directional Light"])
	{
		DirectionalLight* directionalLight = componentManager.AddComponent<DirectionalLight>();

		if (auto& colorData = directionalLightIn["Color"])
		{
			directionalLight->m_Color = colorData.as<glm::vec3>();
		}

		if (auto& intensityData = directionalLightIn["Intensity"])
		{
			directionalLight->m_Intensity = intensityData.as<float>();
		}
	}
}

void Serializer::SerializeSpline(YAML::Emitter& out, ComponentManager& componentManager)
{
	Spline* spline = componentManager.GetComponent<Spline>();
	if (spline)
	{
		out << YAML::Key << "Spline";
		out << YAML::BeginMap;

		out << YAML::Key << "Speed" << YAML::Value << spline->m_Speed;

		out << YAML::EndMap;
	}
}

void Serializer::DeSerializeSpline(YAML::Node& in, ComponentManager& componentManager)
{
	if (auto& splineIn = in["Spline"])
	{
		Spline* spline = componentManager.AddComponent<Spline>();

		if (auto& speedData = splineIn["Speed"])
		{
			spline->m_Speed = speedData.as<float>();
		}
	}
}

void Serializer::SerializeUserDefinedComponents(YAML::Emitter& out, ComponentManager& componentManager)
{
	for (IComponent* component : componentManager.GetComponents())
	{
		if (Utils::IsUserDefinedComponent(component->GetType()))
		{
			out << YAML::Key << component->GetType();
			out << YAML::BeginMap;

			SerializeRTTR(out, component, component->GetType());

			out << YAML::EndMap;
		}
	}
}

void Serializer::DeserializeUserDefinedComponents(YAML::Node& in, ComponentManager& componentManager)
{
	for (auto& classByType : ReflectionSystem::GetInstance().m_ClassesByType)
	{
		if (Utils::IsUserDefinedComponent(classByType.first))
		{
			if (auto& componentData = in[classByType.first])
			{
				classByType.second.m_AddComponentCallBack(&componentManager);
				IComponent* component = componentManager.GetComponent(classByType.first);
				DeSerializeRTTR(componentData, component, classByType.first);
			}
		}
	}
}

void Serializer::SerializeRTTR(YAML::Emitter& out, void* instance, const std::string typeName)
{
	std::function<void(void*, const std::string&, size_t)> serialize = [&out, &serialize](void* instance, const std::string& typeName, size_t offset)
	{
		ReflectionSystem& reflectionSystem = ReflectionSystem::GetInstance();
		auto classByType = reflectionSystem.m_ClassesByType.find(typeName);
		if (classByType == reflectionSystem.m_ClassesByType.end())
		{
			return;
		}

		for (auto& [name, prop] : classByType->second.m_PropertiesByName)
		{
#define SERIALIZE_PROPERTY(_type) \
if (prop.IsValue<_type>()) \
{ \
	_type value = reflectionSystem.GetValue<_type>(instance, typeName, name); \
	out << YAML::Key << "Type" << prop.m_Type; \
	out << YAML::Key << "Value" << value; \
}

#define SERIALIZE_VECTOR_PROPERTY(_type) \
if (prop.IsValue<_type>()) \
{ \
	_type vector = reflectionSystem.GetValue<_type>(instance, typeName, name); \
	out << YAML::Key << "Type" << prop.m_Type; \
	out << YAML::Key << "Value"; \
	out << YAML::BeginSeq; \
	for (auto& item : vector) \
	{ \
		out << item; \
	} \
	out << YAML::EndSeq; \
}
			out << YAML::Key << name;
			out << YAML::BeginMap;

			SERIALIZE_PROPERTY(bool)
			else SERIALIZE_PROPERTY(float)
			else SERIALIZE_PROPERTY(int)
			else SERIALIZE_PROPERTY(double)
			else SERIALIZE_PROPERTY(std::string)
			else SERIALIZE_PROPERTY(glm::vec2)
			else SERIALIZE_PROPERTY(glm::vec3)
			else SERIALIZE_PROPERTY(glm::vec4)
			else SERIALIZE_PROPERTY(glm::ivec2)
			else SERIALIZE_PROPERTY(glm::ivec3)
			else SERIALIZE_PROPERTY(glm::ivec4)
			else SERIALIZE_VECTOR_PROPERTY(std::vector<bool>)
			else SERIALIZE_VECTOR_PROPERTY(std::vector<float>)
			else SERIALIZE_VECTOR_PROPERTY(std::vector<int>)
			else SERIALIZE_VECTOR_PROPERTY(std::vector<double>)
			else SERIALIZE_VECTOR_PROPERTY(std::vector<std::string>)
			else SERIALIZE_VECTOR_PROPERTY(std::vector<glm::vec2>)
			else SERIALIZE_VECTOR_PROPERTY(std::vector<glm::vec3>)
			else SERIALIZE_VECTOR_PROPERTY(std::vector<glm::vec4>)
			else SERIALIZE_VECTOR_PROPERTY(std::vector<glm::ivec2>)
			else SERIALIZE_VECTOR_PROPERTY(std::vector<glm::ivec3>)
			else SERIALIZE_VECTOR_PROPERTY(std::vector<glm::ivec4>)
			else if (prop.IsValue<Texture*>())
			{
				Texture* value = reflectionSystem.GetValue<Texture*>(instance, typeName, name);
				out << YAML::Key << "Type" << prop.m_Type;
				out << YAML::Key << "Value" << value->GetFilePath();
			}
			else if (prop.IsValue<Shader*>())
			{
				Shader* value = reflectionSystem.GetValue<Shader*>(instance, typeName, name);
				out << YAML::Key << "Type" << prop.m_Type;
				out << YAML::Key << "Value" << value->GetName();
			}
			else if (prop.IsValue<BaseMaterial*>())
			{
				BaseMaterial* value = reflectionSystem.GetValue<BaseMaterial*>(instance, typeName, name);
				SerializeBaseMaterial(value->GetFilePath(), value);
				out << YAML::Key << "Type" << prop.m_Type;
				out << YAML::Key << "Value" << value->GetFilePath();
			}
			else if (prop.IsValue<GameObject*>())
			{
				GameObject* value = reflectionSystem.GetValue<GameObject*>(instance, typeName, name);
				out << YAML::Key << "Type" << prop.m_Type;
				out << YAML::Key << "Value" << value->GetUUID();
			}
			out << YAML::EndMap;
		}

		for (const auto& parent : classByType->second.m_Parents)
		{
			serialize(instance, parent.first, parent.second);
		}
	};

	serialize(instance, typeName, 0);
}

void Serializer::DeSerializeRTTR(YAML::Node& in, void* instance, const std::string typeName)
{
	std::function<void(void*, const std::string&, size_t)> deserialize = [&in, &deserialize](void* instance, const std::string& typeName, size_t offset)
	{
		ReflectionSystem& reflectionSystem = ReflectionSystem::GetInstance();
		auto classByType = reflectionSystem.m_ClassesByType.find(typeName);
		if (classByType == reflectionSystem.m_ClassesByType.end())
		{
			return;
		}

		for (auto& [name, prop] : classByType->second.m_PropertiesByName)
		{
			if (auto& propData = in[name])
			{
				const std::string type = propData["Type"].as<std::string>();
#define DESERIALIZE_PROPERTY(_type) \
if (GetTypeName<_type>() == type) \
{ \
	_type value = propData["Value"].as<_type>(); \
	reflectionSystem.SetValue(value, instance, typeName, name); \
}

				DESERIALIZE_PROPERTY(bool)
				else DESERIALIZE_PROPERTY(float)
				else DESERIALIZE_PROPERTY(int)
				else DESERIALIZE_PROPERTY(double)
				else DESERIALIZE_PROPERTY(std::string)
				else DESERIALIZE_PROPERTY(glm::vec2)
				else DESERIALIZE_PROPERTY(glm::vec3)
				else DESERIALIZE_PROPERTY(glm::vec4)
				else DESERIALIZE_PROPERTY(glm::ivec2)
				else DESERIALIZE_PROPERTY(glm::ivec3)
				else DESERIALIZE_PROPERTY(glm::ivec4)
				else DESERIALIZE_PROPERTY(std::vector<bool>)
				else DESERIALIZE_PROPERTY(std::vector<float>)
				else DESERIALIZE_PROPERTY(std::vector<int>)
				else DESERIALIZE_PROPERTY(std::vector<double>)
				else DESERIALIZE_PROPERTY(std::vector<std::string>)
				else DESERIALIZE_PROPERTY(std::vector<glm::vec2>)
				else DESERIALIZE_PROPERTY(std::vector<glm::vec3>)
				else DESERIALIZE_PROPERTY(std::vector<glm::vec4>)
				else DESERIALIZE_PROPERTY(std::vector<glm::ivec2>)
				else DESERIALIZE_PROPERTY(std::vector<glm::ivec3>)
				else DESERIALIZE_PROPERTY(std::vector<glm::ivec4>)
				else if (GetTypeName<Texture*>() == type)
				{
					std::string value = propData["Value"].as<std::string>();
					TextureManager::GetInstance().AsyncLoad(value,
						[&reflectionSystem, instance, typeName, propName = name](Texture* texture)
					{
						reflectionSystem.SetValue(texture, instance, typeName, propName);
					});
				}
				else if (GetTypeName<Shader*>() == type)
				{
					std::string value = propData["Value"].as<std::string>();
					reflectionSystem.SetValue(Shader::Get(value), instance, typeName, name);
				}
				else if (GetTypeName<BaseMaterial*>() == type)
				{
					std::string value = propData["Value"].as<std::string>();

					BaseMaterial* materal = nullptr;
					if (materal = MaterialManager::GetInstance().GetBase(value))
					{
						reflectionSystem.SetValue(materal, instance, typeName, name);
					}
					else
					{
						materal = DeserializeBaseMaterial(value);
						reflectionSystem.SetValue(materal, instance, typeName, name);
					}
					
				}
				else if (GetTypeName<GameObject*>() == type)
				{
					std::string value = propData["Value"].as<std::string>();
					EventSystem::GetInstance().SendCallbackOnFrame([=, &reflectionSystem]
					{
						if (GameObject* gameObject = EntryPoint::GetApplication().GetScene()->FindGameObjectByUUID(value))
						{
							reflectionSystem.SetValue(gameObject, instance, typeName, name);
						}
					});
				}
			}
		}

		for (const auto& parent : classByType->second.m_Parents)
		{
			deserialize(instance, parent.first, parent.second);
		}
	};

	deserialize(instance, typeName, 0);
}

void Serializer::SerializeMaterial(const std::string& filePath, Material* material)
{
	if (filePath.empty() || filePath == "None")
	{
		Logger::Warning("file path is incorrect!", "Material", filePath.c_str());

		return;
	}

	auto registeredClass = ReflectionSystem::GetInstance().m_ClassesByType.find(GetTypeName<Material>());
	if (registeredClass != ReflectionSystem::GetInstance().m_ClassesByType.end())
	{
		YAML::Emitter out;

		out << YAML::BeginMap;

		out << YAML::Key << "Material";
		out << YAML::BeginMap;

		SerializeRTTR(out, material, GetTypeName<Material>());

		out << YAML::EndMap;

		out << YAML::EndMap;

		std::ofstream fout(filePath);
		fout << out.c_str();

		Logger::Success("has been serialized!", "Material", filePath.c_str());
	}
}

Material* Serializer::DeserializeMaterial(const std::string& filePath)
{
	if (filePath.empty() || filePath == "None" || !Utils::Contains(filePath, "mat"))
	{
		Logger::Warning("file path is incorrect!", "Material", filePath.c_str());

		return nullptr;
	}

	std::ifstream stream(filePath);
	std::stringstream strStream;

	strStream << stream.rdbuf();

	Material* material = new Material();
	material->GenerateFromFilePath(filePath);

	YAML::Node data = YAML::LoadMesh(strStream.str());
	if (!data)
	{
		Logger::Warning("file doesn't exist!", "Material", filePath.c_str());

		return nullptr;
	}

	YAML::Node materialData = data["Material"];
	if (!materialData)
	{
		Logger::Warning("file doesn't contain any material data! Default material is loaded!", "Material", filePath.c_str());

		return material;
	}

	DeSerializeRTTR(materialData, material, GetTypeName<Material>());
	
	Logger::Success("has been deserialized!", "Material", filePath.c_str());

	return material;
}

void Serializer::SerializeBaseMaterial(const std::string& filePath, BaseMaterial* material)
{
	if (filePath.empty() || filePath == "None")
	{
		Logger::Warning("file path is incorrect!", "Material", filePath.c_str());

		return;
	}

	auto registeredClass = ReflectionSystem::GetInstance().m_ClassesByType.find(GetTypeName<BaseMaterial>());
	if (registeredClass != ReflectionSystem::GetInstance().m_ClassesByType.end())
	{
		YAML::Emitter out;

		out << YAML::BeginMap;

		out << YAML::Key << "BaseMaterial";
		out << YAML::BeginMap;

		SerializeRTTR(out, material, GetTypeName<BaseMaterial>());

		out << YAML::Key << "Uniforms";
		out << YAML::BeginMap;

#define SERIALIZE_UNIFORMS(_uniforms, _type) \
for (auto& [name, value] : _uniforms) \
{ \
	out << YAML::Key << name; \
	out << YAML::BeginMap; \
	out << YAML::Key << "Type" << GetTypeName<_type>(); \
	out << YAML::Key << "Value" << value; \
	out << YAML::EndMap; \
} \

		SERIALIZE_UNIFORMS(material->m_FloatUniformsByName, float)
		SERIALIZE_UNIFORMS(material->m_IntUniformsByName, int)
		SERIALIZE_UNIFORMS(material->m_Vec2UniformsByName, glm::vec2)
		SERIALIZE_UNIFORMS(material->m_Vec3UniformsByName, glm::vec3)
		SERIALIZE_UNIFORMS(material->m_Vec4UniformsByName, glm::vec4)
		for (auto& [name, value] : material->m_TextureUniformsByName)
		{
			out << YAML::Key << name;
			out << YAML::BeginMap;
			out << YAML::Key << "Type" << GetTypeName<Texture*>();
			out << YAML::Key << "Value" << value->GetFilePath();
			out << YAML::EndMap;
		}

		out << YAML::EndMap;

		out << YAML::EndMap;

		out << YAML::EndMap;

		std::ofstream fout(filePath);
		fout << out.c_str();

		Logger::Success("has been serialized!", "BaseMaterial", filePath.c_str());
	}
}

BaseMaterial* Serializer::DeserializeBaseMaterial(const std::string& filePath)
{
	if (filePath.empty() || filePath == "None" || !Utils::Contains(filePath, "basemat"))
	{
		Logger::Warning("file path is incorrect!", "BaseMaterial", filePath.c_str());

		return nullptr;
	}

	std::ifstream stream(filePath);
	std::stringstream strStream;

	strStream << stream.rdbuf();

	BaseMaterial* material = new BaseMaterial();
	material->GenerateFromFilePath(filePath);

	YAML::Node data = YAML::LoadMesh(strStream.str());
	if (!data)
	{
		Logger::Warning("file doesn't exist!", "BaseMaterial", filePath.c_str());

		return nullptr;
	}

	YAML::Node materialData = data["BaseMaterial"];
	if (!materialData)
	{
		Logger::Warning("file doesn't contain any material data! Default material is loaded!", "BaseMaterial", filePath.c_str());

		return material;
	}

	DeSerializeRTTR(materialData, material, GetTypeName<BaseMaterial>());

#define DESERIALIZE_UNIFORMS(_uniforms, _type) \
if (GetTypeName<_type>() == typeData.as<std::string>()) \
{ \
	if (auto& valueData = uniformData.second["Value"]) \
	{ \
		_uniforms[uniformData.first.as<std::string>()] = valueData.as<_type>(); \
	} \
} \

	if (auto& uniformsData = materialData["Uniforms"])
	{
		for (auto& uniformData : uniformsData)
		{
			if (auto& typeData = uniformData.second["Type"])
			{
				DESERIALIZE_UNIFORMS(material->m_FloatUniformsByName, float)
				else DESERIALIZE_UNIFORMS(material->m_IntUniformsByName, int)
				else DESERIALIZE_UNIFORMS(material->m_Vec2UniformsByName, glm::vec2)
				else DESERIALIZE_UNIFORMS(material->m_Vec3UniformsByName, glm::vec3)
				else DESERIALIZE_UNIFORMS(material->m_Vec4UniformsByName, glm::vec4)
				else if (GetTypeName<Texture*>() == typeData.as<std::string>())
				{
					std::string value = uniformData.second["Value"].as<std::string>();
					TextureManager::GetInstance().AsyncLoad(value,
						[material, name = uniformData.first.as<std::string>()](Texture* texture)
					{
						material->m_TextureUniformsByName[name] = texture;;
					});
				}
			}
		}
	}

	Logger::Success("has been deserialized!", "BaseMaterial", filePath.c_str());

	return material;
}
