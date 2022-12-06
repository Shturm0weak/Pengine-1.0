#include "Raycast2D.h"

#include "Scene.h"
#include "../Components/CircleCollider2D.h"
#include "../Components/BoxCollider2D.h"
#include "../Components/Transform.h"

using namespace Pengine;

bool Raycast2D::LineIntersect(const glm::vec2& from, const glm::vec2& to, const glm::vec2& start, const glm::vec2& end, glm::vec2& where)
{
	glm::vec2 r = (to - from);
	glm::vec2 s = (end - start);
	float d = r.x * s.y - r.y * s.x;
	float u = ((start.x - from.x) * r.y - (start.y - from.y) * r.x) / d;
	float t = ((start.x - from.x) * s.y - (start.y - from.y) * s.x) / d;
	if (0 <= u && u <= 1 && 0 <= t && t <= 1)
	{
		where = from + t * r;
		return true;
	}
	return false;
}

bool Raycast2D::CircleIntersect(const glm::vec2& from, const glm::vec2& to, const glm::vec2& circlePosition, float radius, glm::vec2& where)
{
	glm::vec2 originToCircle = circlePosition - from;
	glm::vec2 direction = Utils::Direction(from, to);
	float radiusSquared = radius * radius;
	float originToCircleLengthSquared = Utils::GetSquaredLength(originToCircle);
	
	float a = glm::dot(originToCircle, direction);
	float bSquared = originToCircleLengthSquared - (a * a);
	
	if (radiusSquared - bSquared < 0.0f)
	{
		return false;
	}

	float f = glm::sqrt(radiusSquared - bSquared);
	float t = 0.0f;

	if (originToCircleLengthSquared < radiusSquared)
	{
		t = a + f;
	}
	else
	{
		t = a - f;
	}

	if (t < 0.0f)
	{
		return false;
	}

	glm::vec2 newDirection = direction * t;

	if (Utils::GetLength(newDirection) < Utils::GetLength(from - to))
	{
		where = from + newDirection;

		return true;
	}
	else
	{
		return false;
	}
}

void Raycast2D::Raycast(Scene* scene, const glm::vec2& from, const glm::vec2& to, Hit2D& hit,
	std::vector<std::string>& ignoreMaskByTag, std::vector<ICollider2D*>& ignoreMaskByCollider)
{
	std::vector<glm::vec2> corners;
	std::multimap<ICollider2D*, glm::vec2> points;

	size_t boxColliders2DSize = scene->m_BoxColliders2D.size();
	for (size_t i = 0; i < boxColliders2DSize; i++)
	{
		BoxCollider2D* bc2d = scene->m_BoxColliders2D[i];

		if (bc2d->IsTrigger()) continue;

		bool ignoreFlag = false;
		size_t ignoreMaskByTagSize = ignoreMaskByTag.size();
		for (size_t i = 0; i < ignoreMaskByTagSize; i++)
		{
			if (ignoreMaskByTag[i] == bc2d->GetTag())
			{
				ignoreFlag = true;
			}
		}
		
		if (ignoreFlag == true)
		{
			ignoreFlag = false;
			continue;
		}

		ignoreFlag = false;
		size_t ignoreMaskByTagCollider = ignoreMaskByCollider.size();
		for (size_t i = 0; i < ignoreMaskByTagCollider; i++)
		{
			if (ignoreMaskByCollider[i] == bc2d)
			{
				ignoreFlag = true;
			}
		}

		if (ignoreFlag == true)
		{
			ignoreFlag = false;
			continue;
		}

		Transform& transform = bc2d->GetOwner()->m_Transform;

		if (bc2d->GetOwner()->IsEnabled())
		{
			glm::vec2 size = bc2d->GetSize();

			glm::mat4 transformMat4 = glm::translate(glm::mat4(1.0f), 
				glm::vec3(bc2d->GetPosition(), 0.0f)) * 
				transform.GetRotationMat4() *
				transform.GetScaleMat4();

			corners.clear();
			corners.push_back(glm::vec2(transformMat4 * glm::vec4(-size.x, -size.y, 0.0f, 1.0f)));
			corners.push_back(glm::vec2(transformMat4 * glm::vec4( size.x, -size.y, 0.0f, 1.0f)));
			corners.push_back(glm::vec2(transformMat4 * glm::vec4( size.x,  size.y, 0.0f, 1.0f)));
			corners.push_back(glm::vec2(transformMat4 * glm::vec4(-size.x,  size.y, 0.0f, 1.0f)));

			glm::vec2 result = { 0.0f, 0.0f };
			if (LineIntersect(corners[3], corners[0], from, to, result))
			{
				points.insert(std::make_pair(bc2d, result));
			}
			if (LineIntersect(corners[0], corners[1], from, to, result))
			{
				points.insert(std::make_pair(bc2d, result));
			}
			if (LineIntersect(corners[1], corners[2], from, to, result))
			{
				points.insert(std::make_pair(bc2d, result));
			}
			if (LineIntersect(corners[2], corners[3], from, to, result))
			{
				points.insert(std::make_pair(bc2d, result));
			}
		}
	}

	size_t circlesColliders2DSize = scene->m_CircleColliders2D.size();
	for (size_t i = 0; i < circlesColliders2DSize; i++)
	{
		CircleCollider2D* cc2d = scene->m_CircleColliders2D[i];

		if (cc2d->IsTrigger()) continue;

		bool ignoreFlag = false;
		size_t ignoreMaskByTagSize = ignoreMaskByTag.size();
		for (size_t i = 0; i < ignoreMaskByTagSize; i++)
		{
			if (ignoreMaskByTag[i] == cc2d->GetTag())
			{
				ignoreFlag = true;
			}
		}

		if (ignoreFlag == true)
		{
			ignoreFlag = false;
			continue;
		}

		ignoreFlag = false;
		size_t ignoreMaskByTagCollider = ignoreMaskByCollider.size();
		for (size_t i = 0; i < ignoreMaskByTagCollider; i++)
		{
			if (ignoreMaskByCollider[i] == cc2d)
			{
				ignoreFlag = true;
			}
		}

		if (ignoreFlag == true)
		{
			ignoreFlag = false;
			continue;
		}

		Transform& transform = cc2d->GetOwner()->m_Transform;

		if (cc2d->GetOwner()->IsEnabled())
		{
			glm::vec2 result = { 0.0f, 0.0f };
			if (CircleIntersect(from, to, cc2d->GetPosition(), cc2d->GetRadius(), result))
			{
				points.insert(std::make_pair(cc2d, result));
			}
		}
	}

	size_t pointsSise = points.size();
	if (pointsSise > 0)
	{
		float distance = 0;
		float min = 10000000;
		for (auto point : points)
		{
			distance = glm::distance(point.second, (glm::vec2)from);

			if (distance <= min)
			{
				min = distance;
				hit.m_Position = point.second;
				hit.m_Collider = point.first;
			}
		}
		points.clear();
	}
}
