#include "Serializer.h"

#include "Environment.h"
#include "TextureManager.h"
#include "EntryPoint.h"
#include "TextureAtlasManager.h"
#include "../OpenGL/Texture.h"
#include "../Components/BoxCollider2D.h"
#include "../Components/Rigidbody2D.h"
#include "../Components/Animator2D.h"
#include "../Components/ParticleEmitter.h"
#include "../Components/Script.h"

#include <fstream>
#include <string>

using namespace Pengine;

namespace ReflectedProps
{

#define EXPLICIT_DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(_name, type) \
bool deserialize_reflected_##_name##_property(YAML::Node& in, IComponent* component, rttr::property prop) \
{ \
	using namespace glm; \
	using namespace std; \
	if (auto& propData = in[prop.get_name()]) \
	{ \
		if (auto& propTypeData = propData["Type"]) \
		{ \
			if (ReflectedProps::is_##_name(propTypeData.as<std::string>())) \
			{ \
				if (auto& propValueData = propData["Value"]) \
				{ \
					prop.set_value(component, propValueData.as<type>()); \
					return true; \
				} \
			} \
		} \
	} \
	return false; \
}

#define	DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(type) EXPLICIT_DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(type, type)

#define SERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(type) \
if (ReflectedProps::is_##type(prop.get_type().get_name())) \
{ \
	out << YAML::Key << "Type" << YAML::Value << ReflectedProps::get_##type(); \
	out << YAML::Key << "Value" << YAML::Value << (type)prop.get_value(component).get_value<type>(); \
}

#define SERIALIZE_REFLECTED_VECTOR_PROPERTY(type) \
if (Utils::Contains(prop.get_type().get_name(), ReflectedProps::get_vector##type())) \
{ \
	out << YAML::Key << "Type" << YAML::Value << ReflectedProps::get_vector##type(); \
	out << YAML::Key << "Value" << YAML::Value << YAML::BeginSeq; \
\
	vector<type> value = prop.get_value(component).get_value<vector<type>>(); \
\
	for (size_t i = 0; i < value.size(); i++) \
	{ \
		out << value[i]; \
	} \
\
	out << YAML::EndSeq; \
}

DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(float)
DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(double)
DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(string)
DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(uint32_t)
DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(uint64_t)
DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(int)
DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(int64_t)
DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(bool)
DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(vec2)
DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(vec3)
DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(vec4)
DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(ivec2)
DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(ivec3)
DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(ivec4)
DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(dvec2)
DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(dvec3)
DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(dvec4)

EXPLICIT_DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(vectorint, std::vector<int>)
EXPLICIT_DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(vectorint64_t, std::vector<__int64>)
EXPLICIT_DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(vectoruint32_t, std::vector<unsigned int>)
EXPLICIT_DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(vectoruint64_t, std::vector<unsigned __int64>)
EXPLICIT_DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(vectorfloat, std::vector<float>)
EXPLICIT_DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(vectordouble, std::vector<double>)
EXPLICIT_DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(vectorbool, std::vector<bool>)
EXPLICIT_DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(vectorvec2, std::vector<glm::vec2>)
EXPLICIT_DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(vectorvec3, std::vector<glm::vec3>)
EXPLICIT_DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(vectorvec4, std::vector<glm::vec4>)
EXPLICIT_DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(vectorivec2, std::vector<glm::ivec2>)
EXPLICIT_DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(vectorivec3, std::vector<glm::ivec3>)
EXPLICIT_DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(vectorivec4, std::vector<glm::ivec4>)
EXPLICIT_DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(vectordvec2, std::vector<glm::dvec2>)
EXPLICIT_DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(vectordvec3, std::vector<glm::dvec3>)
EXPLICIT_DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(vectordvec4, std::vector<glm::dvec4>)
EXPLICIT_DESERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(vectorstring, std::vector<std::string>)

}

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

	std::ofstream fout(textureAtlas->m_FilePath);
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

	TextureAtlas* textureAtlas = TextureAtlasManager::GetInstance().Create(Utils::GetNameFromFilePath(filePath, 2));
	textureAtlas->m_FilePath = filePath;
	
	if (auto& sizeData = data["Size"])
	{
		textureAtlas->SetSize(sizeData.as<glm::vec2>());
	}

	if (auto& textureData = data["Texture"])
	{
		Texture* texture = TextureManager::GetInstance().Create(textureData.as<std::string>());
		if (!texture)
		{
			texture = TextureManager::GetInstance().Get("White");
		}

		textureAtlas->m_Texture = texture;
	}

	return textureAtlas;
}

void Serializer::SerializeAnimation2D(Animation2DManager::Animation2D* animation)
{
	if (animation->m_Name.empty())
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

	std::ofstream fout(animation->m_FilePath);
	fout << out.c_str();
}

Animation2DManager::Animation2D* Serializer::DeSerializeAnimation2D(const std::string& filePath)
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

	Animation2DManager::Animation2D* animation = new Animation2DManager::Animation2D(filePath, Utils::GetNameFromFilePath(filePath, 4));

	if (auto textures = data["Textures"])
	{
		for (auto texture : textures)
		{
			animation->m_Textures.push_back(TextureManager::GetInstance().Create(texture["FilePath"].as<std::string>()));
			animation->m_UVs.push_back(texture["UVs"].as<std::vector<float>>());
		}
	}

	return animation;
}

void Serializer::SerializePrefab(const std::string& filePath, GameObject& gameObject)
{
	YAML::Emitter out;

	std::string prefabFilePath = filePath + "/" + gameObject.GetName() + ".prefab";
	gameObject.m_PrefabFilePath = prefabFilePath;

	out << YAML::BeginMap;
	out << YAML::Key << "GameObjects";
	out << YAML::Value << YAML::BeginSeq;

	SerializeGameObject(out, gameObject);

	std::vector<GameObject*> childs = gameObject.GetChilds();
	for (auto child : childs)
	{
		SerializeGameObject(out, *child);
	}

	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(prefabFilePath);
	fout << out.c_str();

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

	std::unordered_map<GameObject*, std::vector<size_t>> childs;

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
			GameObject* child = scene->FindGameObject(childUUID);
			if (child)
			{
				gameObject.first->AddChild(child);
			}
		}
	}

	for (auto gameObject : childs)
	{
		gameObject.first->m_UUID = UUID::Generate();
	}

	Logger::Success("has been deserialized!", "Prefab", Utils::GetNameFromFilePath(filePath, 6).c_str());

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

	if (Utils::GetNameFromFilePath(filePath, 4) != scene.m_Title)
	{
		filePath = Utils::ReplaceNameFromFilePath(filePath, scene.m_Title, 4);
	}

	scene.m_FilePath = filePath;

	std::ofstream fout(filePath);
	fout << out.c_str();

	Logger::Success("has been serialized!", "Scene", scene.m_Title.c_str());
}

void Serializer::DeserializeScene(const std::string filePath)
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
		scene->m_Title = Utils::GetNameFromFilePath(filePath, 4);
	}

	scene->m_FilePath = filePath;

	DeserializeEnvironment(data);
	Environment::GetInstance().GetMainCamera()->Copy(DeserializeCamera(data));

	std::unordered_map<GameObject*, std::vector<size_t>> childs;

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
			GameObject* child = scene->FindGameObject(childUUID);
			if (child)
			{
				gameObject.first->AddChild(child);
			}
		}
	}

	for (auto gameObject : childs)
	{
		gameObject.first->m_UUID = UUID::Generate();
	}

	Logger::Success("has been deserialized!", "Scene", scene->m_Title.c_str());
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
			camera.m_Fov = fovData.as<float>();
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
			camera.m_Znear = zNearData.as<float>();
		}

		if (auto& zFarData = cameraIn["Z far"])
		{
			camera.m_Zfar = zFarData.as<float>();
		}

		if (auto& speedData = cameraIn["Speed"])
		{
			camera.m_Speed = speedData.as<float>();
		}

		if (auto& typeData = cameraIn["Type"])
		{
			camera.m_Type = (Camera::CameraType)typeData.as<int>();
		}
	}
	
	camera.UpdateProjection(camera.m_Size);
	
	return camera;
}

void Serializer::SerializeEnvironment(YAML::Emitter& out)
{
	out << YAML::Key << "Environment";
	out << YAML::BeginMap;

	out << YAML::Key << "Global Intensity" << YAML::Value << Environment::GetInstance().GetGlobalIntensity();
	out << YAML::Key << "Blur passes" << YAML::Value << Environment::GetInstance().m_BloomSettings.m_BlurPasses;
	out << YAML::Key << "Brightness threshold" << YAML::Value << Environment::GetInstance().m_BloomSettings.m_BrightnessThreshold;
	out << YAML::Key << "Exposure" << YAML::Value << Environment::GetInstance().m_BloomSettings.m_Exposure;
	out << YAML::Key << "Gamma" << YAML::Value << Environment::GetInstance().m_BloomSettings.m_Gamma;
	out << YAML::Key << "Pixels blured" << YAML::Value << Environment::GetInstance().m_BloomSettings.m_PixelsBlured;
	out << YAML::Key << "IsEnabled" << YAML::Value << Environment::GetInstance().m_BloomSettings.m_IsEnabled;
	out << YAML::Key << "DownSampling" << YAML::Value << Environment::GetInstance().m_BloomSettings.m_DownSampling;
	out << YAML::Key << "UpSampling" << YAML::Value << Environment::GetInstance().m_BloomSettings.m_UpSampling;
	out << YAML::Key << "UpScalingScale" << YAML::Value << Environment::GetInstance().m_BloomSettings.m_UpScalingScale;
	out << YAML::EndMap;
}

void Serializer::DeserializeEnvironment(YAML::Node& in)
{
	if (auto& environmentIn = in["Environment"])
	{
		if (auto& globalIntensityData = environmentIn["Global Intensity"])
		{
			Environment::GetInstance().SetGlobalIntensity(globalIntensityData.as<float>());
		}

		if (auto& blurPassesData = environmentIn["Blur passes"])
		{
			Environment::GetInstance().m_BloomSettings.m_BlurPasses = blurPassesData.as<int>();
		}

		if (auto& brightnessThresholdData = environmentIn["Brightness threshold"])
		{
			Environment::GetInstance().m_BloomSettings.m_BrightnessThreshold = brightnessThresholdData.as<float>();
		}

		if (auto& exposureData = environmentIn["Exposure"])
		{
			Environment::GetInstance().m_BloomSettings.m_Exposure = exposureData.as<float>();
		}

		if (auto& GammaData = environmentIn["Gamma"])
		{
			Environment::GetInstance().m_BloomSettings.m_Gamma = GammaData.as<float>();
		}

		if (auto& pixelsbluredData = environmentIn["Pixels blured"])
		{
			Environment::GetInstance().m_BloomSettings.m_PixelsBlured = pixelsbluredData.as<int>();
		}

		if (auto& isEnabledData = environmentIn["IsEnabled"])
		{
			Environment::GetInstance().m_BloomSettings.m_IsEnabled = isEnabledData.as<bool>();
		}

		if (auto& downSamplingData = environmentIn["DownSampling"])
		{
			Environment::GetInstance().m_BloomSettings.m_DownSampling = downSamplingData.as<bool>();
		}

		if (auto& upSamplingData = environmentIn["UpSampling"])
		{
			Environment::GetInstance().m_BloomSettings.m_UpSampling = upSamplingData.as<bool>();
		}

		if (auto& upScalingScaleData = environmentIn["UpScalingScaleData"])
		{
			Environment::GetInstance().m_BloomSettings.m_UpScalingScale = upScalingScaleData.as<float>();
		}
	}
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

	size_t size = gameObjects.size();
	for (uint32_t i = 0; i < size; i++)
	{
		if (gameObjects[i]->m_IsSerializable)
		{
			SerializeGameObject(out, *gameObjects[i]);
		}
	}

	out << YAML::EndSeq;
}

void Serializer::SerializeGameObject(YAML::Emitter& out, GameObject& gameObject)
{
	out << YAML::BeginMap;
	out << YAML::Key << "GameObject" << YAML::Value << std::to_string(gameObject.GetUUID());
	out << YAML::Key << "Name" << YAML::Value << gameObject.m_Name;
	out << YAML::Key << "Is Enabled" << YAML::Value << gameObject.m_IsEnabled;
	out << YAML::Key << "Is Selectable" << YAML::Value << gameObject.m_IsSelectable;
	out << YAML::Key << "Prefab File Path" << YAML::Value << gameObject.m_PrefabFilePath;

	gameObject.m_UUID.Clear();
	gameObject.m_UUID = UUID::Generate();

	SerializeGameObjectChilds(out, gameObject);
	SerializeTransform(out, gameObject.m_Transform);
	SerializeRenderer2D(out, gameObject.m_ComponentManager);
	SerializeBoxCollider2D(out, gameObject.m_ComponentManager);
	SerializeCircleCollider2D(out, gameObject.m_ComponentManager);
	SerializeRigidbody2D(out, gameObject.m_ComponentManager);
	SerializeAnimator2D(out, gameObject.m_ComponentManager);
	SerializeParticleEmitter(out, gameObject.m_ComponentManager);
	SerializeScript(out, gameObject.m_ComponentManager);
	SerializePointLight(out, gameObject.m_ComponentManager);
	SerializeUserDefinedComponents(out, gameObject.m_ComponentManager);

	out << YAML::EndMap;
}

void Serializer::DeserializeGameObject(YAML::Node& in, Scene& scene, std::unordered_map<GameObject*, std::vector<size_t>>& childs)
{
	GameObject* gameObject = nullptr;

	if (auto& nameData = in["Name"])
	{
		gameObject = scene.CreateGameObject(nameData.as<std::string>(),
			DeserializeTransform(in), UUID(in["GameObject"].as<size_t>()));
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
	DeSerializeBoxCollider2D(in, gameObject->m_ComponentManager);
	DeSerializeCircleCollider2D(in, gameObject->m_ComponentManager);
	DeSerializeRigidbody2D(in, gameObject->m_ComponentManager);
	DeSerializeAnimator2D(in, gameObject->m_ComponentManager);
	DeSerializeParticleEmitter(in, gameObject->m_ComponentManager);
	DeSerializeScript(in, gameObject->m_ComponentManager);
	DeSerializePointLight(in, gameObject->m_ComponentManager);
	DeserializeUserDefinedComponents(in, gameObject->m_ComponentManager);
}

void Serializer::SerializeGameObjectChilds(YAML::Emitter& out, GameObject& gameObject)
{
	std::vector<GameObject*> childs = gameObject.GetChilds();
	std::vector<size_t> childsUUID;
	for (auto& child : childs)
	{
		childsUUID.push_back(child->GetUUID());
	}
	out << YAML::Key << "Childs" << YAML::Value << childsUUID;
}

void Serializer::DeserializeGameObjectChilds(YAML::Node& in, GameObject* gameObject,
	std::unordered_map<GameObject*, std::vector<size_t>>& childs)
{
	if (auto& childsIn = in["Childs"])
	{
		std::pair<GameObject*, std::vector<size_t>> pair = std::make_pair(gameObject, std::vector<size_t>());
		std::vector<size_t> childsUUID = childsIn.as<std::vector<size_t>>();
		for (auto childUUID : childsUUID)
		{
			pair.second.push_back(childUUID);
		}
		
		childs.insert(pair);
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
				r2d->SetTexture(TextureManager::GetInstance().Get("White"));
			}
			else
			{
				TextureManager::GetInstance().AsyncCreate(textureFilePath);
				TextureManager::GetInstance().AsyncGet([=](Texture* texture) {
					r2d->SetTexture(texture);
				}, Utils::GetNameFromFilePath(textureFilePath));
			}
		}

		if (auto& textureData = renderer2DIn["Normal Texture"])
		{
			std::string textureFilePath = textureData.as<std::string>();
			if (textureFilePath == "White")
			{
				r2d->SetNormalTexture(TextureManager::GetInstance().Get("White"));
			}
			else
			{
				TextureManager::GetInstance().AsyncCreate(textureFilePath);
				TextureManager::GetInstance().AsyncGet([=](Texture* texture) {
					r2d->SetNormalTexture(texture);
					}, Utils::GetNameFromFilePath(textureFilePath));
			}
		}

		if (auto& textureData = renderer2DIn["Emissive Mask Texture"])
		{
			std::string textureFilePath = textureData.as<std::string>();
			if (textureFilePath == "White")
			{
				r2d->SetEmissiveMaskTexture(TextureManager::GetInstance().Get("White"));
			}
			else
			{
				TextureManager::GetInstance().AsyncCreate(textureFilePath);
				TextureManager::GetInstance().AsyncGet([=](Texture* texture) {
					r2d->SetEmissiveMaskTexture(texture);
					}, Utils::GetNameFromFilePath(textureFilePath));
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
		out << YAML::Key << "Trigger" << YAML::Value << bc2d->m_IsTrigger;
		out << YAML::Key << "Offset" << YAML::Value << bc2d->m_Offset;
		out << YAML::Key << "Size" << YAML::Value << bc2d->GetSize();
		out << YAML::Key << "Density" << YAML::Value << bc2d->m_Density;
		out << YAML::Key << "Friction" << YAML::Value << bc2d->m_Friction;
		out << YAML::Key << "Restitution" << YAML::Value << bc2d->m_Restitution;
		out << YAML::Key << "Restitution threshold" << YAML::Value << bc2d->m_RestitutionThreshold;
		out << YAML::Key << "Tag" << YAML::Value << bc2d->m_Tag;
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
			bc2d->m_IsTrigger = triggerData.as<bool>();
		}

		if (auto& offsetData = boxCollider2DIn["Offset"])
		{
			bc2d->m_Offset = offsetData.as<glm::vec2>();
		}

		if (auto& sizeData = boxCollider2DIn["Size"])
		{
			bc2d->SetSize(sizeData.as<glm::vec2>());
		}

		if (auto& densityData = boxCollider2DIn["Density"])
		{
			bc2d->m_Density = densityData.as<float>();
		}

		if (auto& frictionData = boxCollider2DIn["Friction"])
		{
			bc2d->m_Friction = frictionData.as<float>();
		}
	
		if (auto& restitutionData = boxCollider2DIn["Restitution"])
		{
			bc2d->m_Restitution = restitutionData.as<float>();
		}
		
		if (auto& restitutionData = boxCollider2DIn["Restitution threshold"])
		{
			bc2d->m_RestitutionThreshold = restitutionData.as<float>();
		}

		if (auto& tagData = boxCollider2DIn["Tag"])
		{
			bc2d->m_Tag = tagData.as<std::string>();
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
		out << YAML::Key << "Trigger" << YAML::Value << cc2d->m_IsTrigger;
		out << YAML::Key << "Offset" << YAML::Value << cc2d->m_Offset;
		out << YAML::Key << "Radius" << YAML::Value << cc2d->GetRadius();
		out << YAML::Key << "Density" << YAML::Value << cc2d->m_Density;
		out << YAML::Key << "Friction" << YAML::Value << cc2d->m_Friction;
		out << YAML::Key << "Restitution" << YAML::Value << cc2d->m_Restitution;
		out << YAML::Key << "Restitution threshold" << YAML::Value << cc2d->m_RestitutionThreshold;
		out << YAML::Key << "Tag" << YAML::Value << cc2d->m_Tag;
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
			cc2d->m_IsTrigger = triggerData.as<bool>();
		}

		if (auto& offsetData = circleCollider2DIn["Offset"])
		{
			cc2d->m_Offset = offsetData.as<glm::vec2>();
		}

		if (auto& radiusData = circleCollider2DIn["Radius"])
		{
			cc2d->SetRadius(radiusData.as<float>());
		}

		if (auto& densityData = circleCollider2DIn["Density"])
		{
			cc2d->m_Density = densityData.as<float>();
		}

		if (auto& frictionData = circleCollider2DIn["Friction"])
		{
			cc2d->m_Friction = frictionData.as<float>();
		}

		if (auto& restitutionData = circleCollider2DIn["Restitution"])
		{
			cc2d->m_Restitution = restitutionData.as<float>();
		}

		if (auto& restitutionData = circleCollider2DIn["Restitution threshold"])
		{
			cc2d->m_RestitutionThreshold = restitutionData.as<float>();
		}

		if (auto& tagData = circleCollider2DIn["Tag"])
		{
			cc2d->m_Tag = tagData.as<std::string>();
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
	if (auto& Rigidbody2DIn = in["Rigidbody2D"])
	{
		Rigidbody2D* rb2d = componentManager.AddComponent<Rigidbody2D>();

		if (auto& typeData = Rigidbody2DIn["Type"])
		{
			rb2d->SetStatic(!((bool)typeData.as<int>()));
		}

		if (auto& fixedRotationData = Rigidbody2DIn["Fixed rotation"])
		{
			rb2d->SetFixedRotation(fixedRotationData.as<bool>());
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

		std::vector<std::string> animations;

		size_t size = a2d->m_Animations.size();
		for (uint32_t i = 0; i < size; i++)
		{
			animations.push_back(a2d->m_Animations[i]->m_FilePath);
		}
		out << YAML::Key << "Animations" << YAML::Value << animations;
		out << YAML::Key << "Current animation" << YAML::Value << (a2d->GetCurrentAnimation() ? 
			a2d->GetCurrentAnimation()->m_FilePath.c_str() : "Null");

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
					a2d->m_Animations.push_back(animation);
				}
			}
		}

		if (auto& currentAnimationData = Animator2DIn["Current animation"])
		{
			std::string currentAnimationFilePath = currentAnimationData.as<std::string>();
			a2d->SetCurrentAnimation(Animation2DManager::GetInstance().Get(Utils::GetNameFromFilePath(currentAnimationFilePath, 4)));
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

		if (auto& textureData = particleEmitterIn["Texture"])
		{
			if (textureData.as<std::string>() == "White")
			{
				particleEmitter->m_Texture = TextureManager::GetInstance().Get("White");
			}
			else
			{
				TextureManager::GetInstance().AsyncCreate(textureData.as<std::string>());
				TextureManager::GetInstance().AsyncGet([=](Texture* texture) {
					particleEmitter->m_Texture = texture; }, Utils::GetNameFromFilePath(textureData.as<std::string>()));
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
	PointLight2D* pointLight2D = componentManager.GetComponent<PointLight2D>();
	if (pointLight2D)
	{
		out << YAML::Key << "Point Light 2D";
		out << YAML::BeginMap;

		out << YAML::Key << "Color" << YAML::Value << pointLight2D->m_Color;
		out << YAML::Key << "Constant" << YAML::Value << pointLight2D->m_Constant;
		out << YAML::Key << "Linear" << YAML::Value << pointLight2D->m_Linear;
		out << YAML::Key << "Quadratic" << YAML::Value << pointLight2D->m_Quadratic;

		out << YAML::EndMap;
	}
}

void Serializer::DeSerializePointLight(YAML::Node& in, ComponentManager& componentManager)
{
	if (auto& pointLight2DIn = in["Point Light 2D"])
	{
		PointLight2D* pointLight2D = componentManager.AddComponent<PointLight2D>();

		if (auto& colorData = pointLight2DIn["Color"])
		{
			pointLight2D->m_Color = colorData.as<glm::vec3>();
		}

		if (auto& constantData = pointLight2DIn["Constant"])
		{
			pointLight2D->m_Constant = constantData.as<float>();
		}

		if (auto& linearData = pointLight2DIn["Linear"])
		{
			pointLight2D->m_Linear = linearData.as<float>();
		}

		if (auto& quadraticData = pointLight2DIn["Quadratic"])
		{
			pointLight2D->m_Quadratic = quadraticData.as<float>();
		}
	}
}

void Serializer::SerializeUserDefinedComponents(YAML::Emitter& out, ComponentManager& componentManager)
{
	using namespace glm;
	using namespace std;

	for (IComponent* component : componentManager.GetComponents())
	{
		if (Utils::IsUserDefinedComponent(component->GetType()))
		{
			out << YAML::Key << component->GetType();
			out << YAML::BeginMap;

			rttr::type componentClass = rttr::type::get_by_name(component->GetType().c_str());
			for (auto& prop : componentClass.get_properties())
			{
				out << YAML::Key << prop.get_name();
				out << YAML::BeginMap;

				if (prop.get_type().is_array())
				{
					SERIALIZE_REFLECTED_VECTOR_PROPERTY(float)
					else SERIALIZE_REFLECTED_VECTOR_PROPERTY(double)
					else SERIALIZE_REFLECTED_VECTOR_PROPERTY(bool)
					else SERIALIZE_REFLECTED_VECTOR_PROPERTY(int)
					else SERIALIZE_REFLECTED_VECTOR_PROPERTY(int64_t)
					else SERIALIZE_REFLECTED_VECTOR_PROPERTY(string)
					else SERIALIZE_REFLECTED_VECTOR_PROPERTY(uint32_t)
					else SERIALIZE_REFLECTED_VECTOR_PROPERTY(uint64_t)
					else SERIALIZE_REFLECTED_VECTOR_PROPERTY(vec2)
					else SERIALIZE_REFLECTED_VECTOR_PROPERTY(vec3)
					else SERIALIZE_REFLECTED_VECTOR_PROPERTY(vec4)
					else SERIALIZE_REFLECTED_VECTOR_PROPERTY(ivec2)
					else SERIALIZE_REFLECTED_VECTOR_PROPERTY(ivec3)
					else SERIALIZE_REFLECTED_VECTOR_PROPERTY(ivec4)
					else SERIALIZE_REFLECTED_VECTOR_PROPERTY(dvec2)
					else SERIALIZE_REFLECTED_VECTOR_PROPERTY(dvec3)
					else SERIALIZE_REFLECTED_VECTOR_PROPERTY(dvec4)
				}
				else if (prop.get_type().is_pointer())
				{
					if (auto& value = prop.get_value(component))
					{
						if (value.can_convert<IAsset*>())
						{
							std::string type = prop.get_type().get_name();
							IAsset* asset = value.get_value<IAsset*>();
							if (asset)
							{
								out << YAML::Key << "Type" << YAML::Value << ReflectedProps::get_asset();
								out << YAML::Key << "Value" << YAML::Value << asset->GetFilePath();
							}
						}
					}
				}
				else
				{
					SERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(float)
					else SERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(double)
					else SERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(bool)
					else SERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(int)
					else SERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(int64_t)
					else SERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(string)
					else SERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(uint32_t)
					else SERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(uint64_t)
					else SERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(vec2)
					else SERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(vec3)
					else SERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(vec4)
					else SERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(ivec2)
					else SERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(ivec3)
					else SERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(ivec4)
					else SERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(dvec2)
					else SERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(dvec3)
					else SERIALIZE_REFLECTED_PRIMITIVE_PROPERTY(dvec4)
				}

				out << YAML::EndMap;
			}

			out << YAML::EndMap;
		}
	}
}

void Serializer::DeserializeUserDefinedComponents(YAML::Node& in, ComponentManager& componentManager)
{
	for (auto& registeredClass : ReflectionSystem::GetInstance().m_RegisteredClasses)
	{
		if (Utils::IsUserDefinedComponent(registeredClass.first))
		{
			if (auto& componentData = in[registeredClass.first])
			{
				registeredClass.second.m_AddComponentCallBack(&componentManager);
				IComponent* component = componentManager.GetComponent(registeredClass.first);
				rttr::type componentClass = rttr::type::get_by_name(registeredClass.first.c_str());

				for (auto& prop : componentClass.get_properties())
				{
					ReflectedProps::deserialize_reflected_int_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_float_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_double_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_uint32_t_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_int64_t_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_uint64_t_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_string_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_bool_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_vec2_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_vec3_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_vec4_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_ivec2_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_ivec3_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_ivec4_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_dvec2_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_dvec3_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_dvec4_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_dvec4_property(componentData, component, prop);

					ReflectedProps::deserialize_reflected_vectorint_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_vectorfloat_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_vectordouble_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_vectorint64_t_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_vectoruint32_t_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_vectoruint64_t_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_vectorstring_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_vectorbool_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_vectorvec2_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_vectorvec3_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_vectorvec4_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_vectorivec2_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_vectorivec3_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_vectorivec4_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_vectordvec2_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_vectordvec3_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_vectordvec4_property(componentData, component, prop);
					ReflectedProps::deserialize_reflected_vectordvec4_property(componentData, component, prop);

					//if (auto& propData = componentData[prop.get_name()])
					//{
					//	if (auto& propTypeData = propData["Type"])
					//	{
					//		if (ReflectedProps::is_asset(propTypeData.as<std::string>()))
					//		{
					//			if (auto& propValueData = propData["Value"])
					//			{
					//				std::string assetFilePath = propValueData.as<std::string>();
					//				if (assetFilePath == "White" || Utils::MatchType(assetFilePath, {"jpeg", "png", "jpg"}))
					//				{
					//					TextureManager::GetInstance().AsyncCreate(assetFilePath);
					//					TextureManager::GetInstance().AsyncGet([=](Texture* texture)
					//						{
					//							prop.set_value(component, texture);
					//						}
					//					, Utils::GetNameFromFilePath(assetFilePath));
					//				}
					//			}
					//		}
					//	}
					//}
				}
			}
		}
	}
}
