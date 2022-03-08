#include "Serializer.h"

#include "Environment.h"
#include "TextureManager.h"
#include "EntryPoint.h"
#include "TextureAtlasManager.h"
#include "../OpenGL/Texture.h"
#include "../Components/BoxCollider2D.h"
#include "../Components/Rigidbody2D.h"
#include "../Components/Animator2D.h"

#include <fstream>
#include <string>

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

YAML::Emitter& operator<<(YAML::Emitter& out, glm::vec4& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
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

	glm::vec2 size = data["Size"].as<glm::vec2>();
	std::string textureFilePath = data["Texture"].as<std::string>();

	Texture* texture = TextureManager::GetInstance().Get(Utils::GetNameFromFilePath(textureFilePath));
	if (!texture)
	{
		texture = TextureManager::GetInstance().Get("White");
	}

	TextureAtlas* textureAtlas = TextureAtlasManager::GetInstance().Create(Utils::GetNameFromFilePath(filePath, 2), size, texture);
	textureAtlas->m_FilePath = filePath;
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
	out << YAML::BeginSeq;

	const size_t texturesSize = animation->m_Textures.size();
	for (size_t i = 0; i < texturesSize; i++)
	{
		out << animation->m_Textures[i]->GetFilePath();
	}
	
	out << YAML::EndSeq;
	out << YAML::EndMap;
	
	animation->m_FilePath = std::string("Source/Animations2D/" + animation->m_Name + ".anim");
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

	auto textures = data["Textures"];
	if (textures)
	{
		for (auto texture : textures)
		{
			animation->m_Textures.push_back(TextureManager::GetInstance().Create(texture.as<std::string>()));
		}
	}

	return animation;
}

void Serializer::SerializePrefab(const std::string& filePath, GameObject& gameObject)
{
	YAML::Emitter out;

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

	std::ofstream fout(std::string(filePath + "/" + gameObject.GetName() + ".prefab"));
	fout << out.c_str();

	Logger::Success("has been serialized!", "Prefab", gameObject.GetName().c_str());
}

void Serializer::DeserializePrefab(const std::string filePath)
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

	std::unordered_map<GameObject*, std::vector<size_t>> childs;

	auto& gameObjects = data["GameObjects"];
	if (gameObjects)
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
}

void Serializer::SerializeScene(const Scene& scene)
{
	YAML::Emitter out;

	out << YAML::BeginMap;

	out << YAML::Key << "Title" << YAML::Value << scene.GetTitle().c_str();
	
	SerializeCamera(out, Environment::GetInstance().GetMainCamera());

	SerializeGameObjects(out, scene);

	out << YAML::EndMap;

	std::ofstream fout("Source/Scenes/" + scene.GetTitle() + ".yaml");
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
	scene->Clear();
	scene->m_Title = data["Title"].as<std::string>();

	Environment::GetInstance().GetMainCamera()->Copy(DeserializeCamera(data));

	std::unordered_map<GameObject*, std::vector<size_t>> childs;

	auto& gameObjects = data["GameObjects"];
	if (gameObjects)
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
	auto& cameraIn = in["Camera"];
	Camera camera;

	camera.m_Transform = DeserializeTransform(cameraIn);
	camera.m_Fov = cameraIn["Fov"].as<float>();
	camera.m_ZoomScale = cameraIn["Zoom"].as<float>();
	camera.m_ZoomSensetivity = cameraIn["Zoom Sensetivity"].as<float>();
	camera.m_Znear = cameraIn["Z near"].as<float>();
	camera.m_Zfar = cameraIn["Z far"].as<float>();
	camera.m_Speed = cameraIn["Speed"].as<float>();
	camera.m_Type = (Camera::CameraType)cameraIn["Type"].as<int>();

	camera.UpdateProjection(camera.m_Size);

	return camera;
}

void Serializer::SerializeTransform(YAML::Emitter& out, const Transform& transform)
{
	out << YAML::Key << "Transform";
	out << YAML::BeginMap;
	out << YAML::Key << "Position" << YAML::Value << transform.GetPosition();
	out << YAML::Key << "Rotation" << YAML::Value << transform.GetRotation();
	out << YAML::Key << "Scale" << YAML::Value << transform.GetScale();
	out << YAML::EndMap;
}

Transform Serializer::DeserializeTransform(YAML::Node& in)
{
	auto& transformIn = in["Transform"];

	glm::vec3 position = transformIn["Position"].as<glm::vec3>();
	glm::vec3 rotation = transformIn["Rotation"].as<glm::vec3>();
	glm::vec3 scale = transformIn["Scale"].as<glm::vec3>();

	return Transform(position, scale, rotation);
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
	
	SerializeGameObjectChilds(out, gameObject);
	SerializeTransform(out, gameObject.m_Transform);
	SerializeRenderer2D(out, gameObject.m_ComponentManager);
	SerializeBoxCollider2D(out, gameObject.m_ComponentManager);
	SerializeRigidbody2D(out, gameObject.m_ComponentManager);
	SerializeAnimator2D(out, gameObject.m_ComponentManager);

	out << YAML::EndMap;
}

void Serializer::DeserializeGameObject(YAML::Node& in, Scene& scene, std::unordered_map<GameObject*, std::vector<size_t>>& childs)
{
	GameObject* gameObject = scene.CreateGameObject(in["Name"].as<std::string>(),
		DeserializeTransform(in), UUID(in["GameObject"].as<size_t>()));

	DeserializeGameObjectChilds(in, gameObject, childs);
	DeSerializeRenderer2D(in, gameObject->m_ComponentManager);
	DeSerializeBoxCollider2D(in, gameObject->m_ComponentManager);
	DeSerializeRigidbody2D(in, gameObject->m_ComponentManager);
	DeSerializeAnimator2D(in, gameObject->m_ComponentManager);
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
	auto childsIn = in["Childs"];
	if (childsIn)
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
		
		std::vector<float> uv;
		for (uint32_t i = 0; i < 8; i++)
		{
			uv.push_back(r2d->m_UV[i]);
		}
		out << YAML::Key << "UV" << YAML::Value << uv;
		out << YAML::Key << "Layer" << YAML::Value << r2d->m_Layer;
		out << YAML::EndMap;
	}
}

void Serializer::DeSerializeRenderer2D(YAML::Node& in, ComponentManager& componentManager)
{
	auto& renderer2DIn = in["Renderer2D"];
	if (renderer2DIn)
	{
		Renderer2D* r2d = componentManager.AddComponent<Renderer2D>();
		
		r2d->m_Color = renderer2DIn["Color"].as<glm::vec4>();
		
		std::string textureFilePath = renderer2DIn["Texture"].as<std::string>();
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

		r2d->SetLayer(renderer2DIn["Layer"].as<int>());

		std::vector<float> uv = renderer2DIn["UV"].as<std::vector<float>>();
		for (size_t i = 0; i < 8; i++)
		{
			r2d->m_UV[i] = uv[i];
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
		out << YAML::Key << "Offset" << YAML::Value << bc2d->m_Offset;
		out << YAML::Key << "Size" << YAML::Value << bc2d->m_Size;
		out << YAML::Key << "Density" << YAML::Value << bc2d->m_Density;
		out << YAML::Key << "Friction" << YAML::Value << bc2d->m_Friction;
		out << YAML::Key << "Restitution" << YAML::Value << bc2d->m_Restitution;
		out << YAML::Key << "Restitution threshold" << YAML::Value << bc2d->m_RestitutionThreshold;
		out << YAML::EndMap;
	}
}

void Serializer::DeSerializeBoxCollider2D(YAML::Node& in, ComponentManager& componentManager)
{
	auto& boxCollider2DIn = in["BoxCollider2D"];
	if (boxCollider2DIn)
	{
		BoxCollider2D* bc2d = componentManager.AddComponent<BoxCollider2D>();

		bc2d->m_Offset = boxCollider2DIn["Offset"].as<glm::vec2>();
		bc2d->m_Size = boxCollider2DIn["Size"].as<glm::vec2>();
		bc2d->m_Density = boxCollider2DIn["Density"].as<float>();
		bc2d->m_Friction = boxCollider2DIn["Friction"].as<float>();
		bc2d->m_Restitution = boxCollider2DIn["Restitution"].as<float>();
		bc2d->m_RestitutionThreshold = boxCollider2DIn["Restitution threshold"].as<float>();
	}
}

void Serializer::SerializeRigidbody2D(YAML::Emitter& out, ComponentManager& componentManager)
{
	Rigidbody2D* rb2d = componentManager.GetComponent<Rigidbody2D>();
	if (rb2d != nullptr)
	{
		out << YAML::Key << "Rigidbody2D";
		out << YAML::BeginMap;
		out << YAML::Key << "Type" << YAML::Value << (int)rb2d->m_BodyType;
		out << YAML::Key << "Fixed rotation" << YAML::Value << rb2d->m_FixedRotation;
		out << YAML::EndMap;
	}
}

void Serializer::DeSerializeRigidbody2D(YAML::Node& in, ComponentManager& componentManager)
{
	auto& Rigidbody2DIn = in["Rigidbody2D"];
	if (Rigidbody2DIn)
	{
		Rigidbody2D* rb2d = componentManager.AddComponent<Rigidbody2D>();

		rb2d->m_BodyType = (Rigidbody2D::BodyType)Rigidbody2DIn["Type"].as<int>();
		rb2d->m_FixedRotation = Rigidbody2DIn["Fixed rotation"].as<bool>();
	}
}

void Serializer::SerializeAnimator2D(YAML::Emitter& out, ComponentManager& componentManager)
{
	Animator2D* a2d = componentManager.GetComponent<Animator2D>();
	if (a2d != nullptr)
	{
		out << YAML::Key << "Animator2D";
		out << YAML::BeginMap;
		out << YAML::Key << "Speed" << YAML::Value << a2d->m_Speed;
		out << YAML::Key << "Play" << YAML::Value << a2d->m_Play;

		std::vector<std::string> animations;

		size_t size = a2d->m_Animations.size();
		for (uint32_t i = 0; i < size; i++)
		{
			animations.push_back(a2d->m_Animations[i]->m_FilePath);
		}
		out << YAML::Key << "Animations" << YAML::Value << animations;
		out << YAML::Key << "Current animation" << YAML::Value << (a2d->m_CurrentAnimation ? 
			a2d->m_CurrentAnimation->m_FilePath.c_str() : "Null");

		out << YAML::EndMap;
	}
}

void Serializer::DeSerializeAnimator2D(YAML::Node& in, ComponentManager& componentManager)
{
	auto& Animator2DIn = in["Animator2D"];
	if (Animator2DIn)
	{
		Animator2D* a2d = componentManager.AddComponent<Animator2D>();

		a2d->m_Speed = Animator2DIn["Speed"].as<float>();
		a2d->m_Play = Animator2DIn["Play"].as<bool>();

		std::vector<std::string> AnimationFilePaths = Animator2DIn["Animations"].as<std::vector<std::string>>();
		for (const std::string& animationFilePath : AnimationFilePaths)
		{
			if (Animation2DManager::Animation2D* animation = Animation2DManager::GetInstance().Load(animationFilePath))
			{
				a2d->m_Animations.push_back(animation);
			}
		}

		std::string currentAnimationFilePath = Animator2DIn["Current animation"].as<std::string>();
		a2d->m_CurrentAnimation = Animation2DManager::GetInstance().Get(Utils::GetNameFromFilePath(currentAnimationFilePath, 4));
	}
}
