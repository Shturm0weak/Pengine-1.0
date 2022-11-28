#include "Raycast3D.h"

#include "GameObject.h"
#include "Mesh.h"
#include "../Components/Renderer3D.h"
#include "../Components/BoxCollider3D.h"

using namespace Pengine;

std::map<float, class GameObject*> Raycast3D::RayCast(std::vector<class GameObject*> gameObjects, const glm::vec3& start,
	const glm::vec3& direction, Hit3D& hit, float length, bool AABB, std::vector<std::string> ignoreMask)
{
	std::map<float, GameObject*> gameObjectsByDistance;
	for (GameObject* gameObject : gameObjects)
	{
		bool hasTag = ignoreMask.empty() ? true : false;
		/*for (size_t j = 0; j < ignoreMask.size(); j++)
		{
			if (gameObject->m_Tag == ignoreMask[j])
			{
				hasTag = true;
				break;
			}
		}*/

		if (!hasTag)
		{
			continue;
		}

		Mesh* mesh = nullptr;
		if (Renderer3D* r3d = gameObject->m_ComponentManager.GetComponent<Renderer3D>())
		{
			if (r3d->m_Mesh)
			{
				mesh = r3d->m_Mesh;
			}
			else
			{
				continue;
			}
		}

		bool isIntersected = false;
		if (AABB)
		{
			isIntersected = IntersectBoxAABB(start, direction, hit, length, mesh->m_BoundingBox, gameObject->m_Transform);
		}
		else
		{
			isIntersected = IntersectBoxOBB(start, direction, hit, length, mesh->m_BoundingBox, gameObject->m_Transform);
		}
		if (isIntersected)
		{
			hit.m_Object = gameObject;
			gameObjectsByDistance.emplace(hit.m_Distance, hit.m_Object);
		}
	}

	if (gameObjectsByDistance.size() > 0)
	{
		hit.m_Object = gameObjectsByDistance.begin()->second;
		hit.m_Point = direction * gameObjectsByDistance.begin()->first + start;
		hit.m_Distance = gameObjectsByDistance.begin()->first;
	}
	else
	{
		hit.m_Object = nullptr;
		hit.m_Point = { 0.0f, 0.0f, 0.0f };
		hit.m_Distance = 0.0f;
	}
	return gameObjectsByDistance;
}


std::map<float, class BoxCollider3D*> Raycast3D::RayCast(std::vector<class BoxCollider3D*> bcs3d, const glm::vec3& start,
	const glm::vec3& direction, Hit3D& hit, float length, bool AABB, std::vector<std::string> ignoreMask)
{
	std::map<float, BoxCollider3D*> bcs3dByDistance;
	for (BoxCollider3D* bc3d : bcs3d)
	{
		GameObject* owner = bc3d->GetOwner();
		bool hasTag = ignoreMask.empty() ? true : false;
		/*for (size_t j = 0; j < ignoreMask.size(); j++)
		{
			if (gameObject->m_Tag == ignoreMask[j])
			{
				hasTag = true;
				break;
			}
		}*/

		if (!hasTag)
		{
			continue;
		}

		bool isIntersected = false;

		Transform transform = owner->m_Transform;
		transform.Translate(transform.GetPosition() + bc3d->GetOffset());
		BoundingBox boundingBox = { -bc3d->GetHalfExtent(), bc3d->GetHalfExtent(), { 0.0f, 0.0f, 0.0f } };

		if (AABB)
		{
			isIntersected = IntersectBoxAABB(start, direction, hit, length, boundingBox, transform);
		}
		else
		{
			isIntersected = IntersectBoxOBB(start, direction, hit, length, boundingBox, transform);
		}
		if (isIntersected)
		{
			hit.m_Object = owner;
			bcs3dByDistance.emplace(hit.m_Distance, bc3d);
		}
	}

	if (bcs3dByDistance.size() > 0)
	{
		hit.m_Object = bcs3dByDistance.begin()->second->GetOwner();
		hit.m_Point = direction * bcs3dByDistance.begin()->first + start;
		hit.m_Distance = bcs3dByDistance.begin()->first;
	}
	else
	{
		hit.m_Object = nullptr;
		hit.m_Point = { 0.0f, 0.0f, 0.0f };
		hit.m_Distance = 0.0f;
	}
	return bcs3dByDistance;
}

bool Raycast3D::IntersectTriangle(const const glm::vec3& start, const glm::vec3& direction, Hit3D& hit,
	float length, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& planeNorm)
{
	glm::vec3 end = direction * length;
	glm::vec3 rayDelta = end - start;
	glm::vec3 rayToPlaneDelta = a - start;
	float vp = glm::dot(rayDelta, planeNorm);
	if (vp >= -0.0001 && vp <= 0.0001)
	{
		return false;
	}

	float wp = glm::dot(rayToPlaneDelta, planeNorm);
	float t = wp / vp;
	glm::vec3 point = rayDelta * t + start;
	hit.m_Point = point;
	hit.m_Distance = glm::distance(start, point);
	glm::vec3 edge0 = b - a;
	glm::vec3 edge1 = c - b;
	glm::vec3 edge2 = a - c;
	glm::vec3 c0 = point - a;
	glm::vec3 c1 = point - b;
	glm::vec3 c2 = point - c;

	if (glm::dot(glm::normalize(point - start), direction) < 0.0f)
	{
		return false;
	}

	if (glm::dot(planeNorm, glm::cross(edge0, c0)) > 0.0f &&
		glm::dot(planeNorm, glm::cross(edge1, c1)) > 0.0f &&
		glm::dot(planeNorm, glm::cross(edge2, c2)) > 0.0f)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Raycast3D::IntersectBoxAABB(const glm::vec3& start, const glm::vec3& direction, Hit3D& hit,
	float length, const BoundingBox& boundingBox, const Transform& transform)
{
	glm::vec3 bMin = transform.GetScaleMat4() * glm::dvec4(boundingBox.m_Min, 1.0f);
	glm::vec3 bMax = transform.GetScaleMat4() * glm::dvec4(boundingBox.m_Max, 1.0f);

	glm::vec3 transformStart = start - transform.GetPosition();

	float txMin = (bMin.x - transformStart.x) / direction.x;
	float txMax = (bMax.x - transformStart.x) / direction.x;
	if (txMax < txMin)
	{
		float temp = txMax;
		txMax = txMin;
		txMin = temp;
	}

	float tyMin = (bMin.y - transformStart.y) / direction.y;
	float tyMax = (bMax.y - transformStart.y) / direction.y;
	if (tyMax < tyMin)
	{
		float temp = tyMax;
		tyMax = tyMin;
		tyMin = temp;
	}

	float tzMin = (bMin.z - transformStart.z) / direction.z;
	float tzMax = (bMax.z - transformStart.z) / direction.z;
	if (tzMax < tzMin)
	{
		float temp = tzMax;
		tzMax = tzMin;
		tzMin = temp;
	}

	float tMin = (txMin > tyMin) ? txMin : tyMin;
	float tMax = (txMax < tyMax) ? txMax : tyMax;

	if (txMin > tyMax || tyMin > txMax) return false;
	if (tMin > tzMax || tzMin > tMax) return false;
	if (tzMin > tMin) tMin = tzMin;
	if (tzMax < tMax) tMax = tzMax;
	hit.m_Distance = tMin;
	hit.m_Point = tMin * direction + start;

	return true;
}

bool Raycast3D::IntersectBoxOBB(const glm::vec3& start, const glm::vec3& direction, Hit3D& hit,
	float length, const BoundingBox& boundingBox, const Transform& transform)
{
	glm::vec3 transformedDirection = direction * length;
	glm::vec3 bMin = glm::vec3(transform.GetScaleMat4() * glm::dvec4(boundingBox.m_Min, 1.0f));
	glm::vec3 bMax = glm::vec3(transform.GetScaleMat4() * glm::dvec4(boundingBox.m_Max, 1.0f));
	const float* transformPtr = glm::value_ptr(transform.GetTransform());
	glm::vec3 axis;
	glm::vec3 bbRayDelta = transform.GetPosition() - start;

	float tMin = 0, tMax = FLT_MAX, nomLen, denomLen, tmp, min, max;
	size_t p = 0;

	for (size_t i = 0; i < 3; i++)
	{
		p = i * 4;
		axis = glm::vec3(transformPtr[p], transformPtr[p + 1], transformPtr[p + 2]);
		axis = glm::normalize(axis);
		nomLen = glm::dot(axis, bbRayDelta);
		denomLen = glm::dot(transformedDirection, axis);
		if (glm::abs(denomLen) > 0.00001f)
		{
			min = (nomLen + bMin[i]) / denomLen;
			max = (nomLen + bMax[i]) / denomLen;

			if (min > max) { tmp = min; min = max; max = tmp; }
			if (min > tMin) tMin = min;
			if (max < tMax) tMax = max;

			if (tMax < tMin) return false;

		}
		else if (-nomLen + bMin[i] > 0 || -nomLen + bMax[i] < 0) return false;
	}

	hit.m_Distance = tMin;
	hit.m_Point = tMin * direction + start;
	return true;
}
