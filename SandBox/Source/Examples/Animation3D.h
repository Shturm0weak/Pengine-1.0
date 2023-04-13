#pragma once

#include "Core/Core.h"
#include "Core/Application.h"

#include "Core/MeshManager.h"
#include "Core/MeshLoader.h"

#include "Core/Visualizer.h"

using namespace Pengine;

class Animation3D : public Application
{
public:

	Mesh* mesh;
	Animation* animation;
	Animator* animator;

	Animation3D(const std::string& title = "Animation3D")
		: Application(title)
	{}

	virtual void OnStart() override 
	{
		Mesh* mesh = MeshManager::GetInstance().Load("Source/Meshes/Anim/Soldier_body-mesh.meta");
		animation = new Animation("Source/Meshes/Anim/Test.dae", mesh);
		animator = new Animator(animation);
	};

	virtual void OnPostStart() override {};

	void TraverseAnimNode(const AssimpNodeData& node)
	{
		auto transforms = animator->GetFinalBoneMatrices();
		auto boneIdMap = animation->GetBoneIDMap();
		for (auto child : node.children)
		{
			auto bone = boneIdMap.find(child.name);
			if (bone != boneIdMap.end())
			{
				//Visualizer::DrawQuad(transforms[bone->second.id]);
				Visualizer::DrawLine(Utils::GetPosition(node.transformation), Utils::GetPosition(child.transformation), Colors::Red());
			}
			TraverseAnimNode(child);
		}
	}

	virtual void OnUpdate() override 
	{
		animator->UpdateAnimation(Time::GetDeltaTime());

		TraverseAnimNode(animation->GetRootNode());

		//for (auto transform : animator->GetFinalBoneMatrices())
		//{
		//	Visualizer::DrawQuad(transform * glm::scale(glm::mat4(1.0f), glm::vec3(0.3f)));
		//}
	};

	virtual void OnGuiRender() override {};

	virtual void OnImGuiRender() override {};

	virtual void OnClose() override {};
};