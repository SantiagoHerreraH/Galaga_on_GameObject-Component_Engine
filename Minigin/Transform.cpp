#include "Transform.h"
#include "GameObject.h"

#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp> 

#include <queue>
#include <algorithm> 


dae::Transform::Transform(GameObject& self) : m_Self(self)
{
}

void dae::Transform::MakeRootNode()
{
	if (HasParent())
	{
		m_LocalTransform = GetWorldTransform();

		Transform& pastParentTransform = m_Self.Transform();
		std::erase(pastParentTransform.m_Children, &m_Self);

		m_Parent = nullptr;
		
	}
}

bool dae::Transform::SetParent(GameObject& parent, ETransformReparentType transformReparentType)
{
	if (&m_Self != &parent)
	{

		Transform& newParentTransform = parent.Transform();

		if (IsDescendant(parent))
		{
			return false;
		}

		if (m_Parent != nullptr)
		{
			Transform& pastParentTransform = m_Self.Transform();
			std::erase(pastParentTransform.m_Children, &m_Self);
		}

		newParentTransform.m_Children.push_back(&m_Self);
		m_Parent = &parent;

		switch (transformReparentType)
		{
		case dae::ETransformReparentType::KeepWorldTransform:

			glm::mat4 parentWorldInverse = glm::inverse(newParentTransform.GetWorldMatrix());
			glm::mat4 newLocalMatrix = parentWorldInverse * GetWorldMatrix();
			m_LocalTransform = TransformDataFromMatrix(newLocalMatrix);

			break;
		case dae::ETransformReparentType::KeepLocalTransform:

			NeedsToRecalculateWorldTransform();
			break;
		default:
			break;
		}

		return true;
	}
	return false;
}

const std::vector<dae::GameObject*>& dae::Transform::GetChildren() const
{
	return m_Children;
}

bool dae::Transform::IsDescendant(const GameObject& descendant)
{
	const Transform* parentTransform = &descendant.TransformConst();

	while (parentTransform)
	{
		if (parentTransform->HasParent())
		{
			parentTransform = &parentTransform->GetParent()->TransformConst();

			if (parentTransform == this)
			{
				return true;
			}
		}
		else
		{
			break;
		}

	}

	return false;
}



bool dae::Transform::HasParent() const
{
	return m_Parent != nullptr;
}

bool dae::Transform::HasChildren() const
{
	return m_Children.size() > 0;
}

const dae::GameObject* dae::Transform::GetParent() const
{
	return m_Parent;
}

void dae::Transform::SetLocalScaleX(float scale)
{
	NeedsToRecalculateWorldTransform();
	m_LocalTransform.Scale.x = scale;
}

void dae::Transform::SetLocalScaleY(float scale)
{
	NeedsToRecalculateWorldTransform();
	m_LocalTransform.Scale.y = scale;
}

void dae::Transform::SetLocalScaleZ(float scale)
{
	NeedsToRecalculateWorldTransform();
	m_LocalTransform.Scale.z = scale;
}

void dae::Transform::SetLocalScale(const glm::vec3& scale)
{
	NeedsToRecalculateWorldTransform();
	m_LocalTransform.Scale = scale;
}

void dae::Transform::SetLocalPositionX(float pos)
{
	NeedsToRecalculateWorldTransform();
	m_LocalTransform.Position.x = pos;
}

void dae::Transform::SetLocalPositionY(float pos)
{
	NeedsToRecalculateWorldTransform();
	m_LocalTransform.Position.y = pos;
}

void dae::Transform::SetLocalPositionZ(float pos)
{
	NeedsToRecalculateWorldTransform();
	m_LocalTransform.Position.z = pos;
}

void dae::Transform::SetLocalPosition(const glm::vec3& pos)
{
	NeedsToRecalculateWorldTransform();
	m_LocalTransform.Position = pos;
}

void dae::Transform::MoveLocalPositionX(float pos)
{
	NeedsToRecalculateWorldTransform();
	m_LocalTransform.Position.x += pos;
}

void dae::Transform::MoveLocalPositionY(float pos)
{
	NeedsToRecalculateWorldTransform();
	m_LocalTransform.Position.y += pos;
}

void dae::Transform::MoveLocalPositionZ(float pos)
{
	NeedsToRecalculateWorldTransform();
	m_LocalTransform.Position.z += pos;
}

void dae::Transform::MoveLocalPosition(const glm::vec3& pos)
{
	NeedsToRecalculateWorldTransform();
	m_LocalTransform.Position += pos;
}

void dae::Transform::SetLocalRotationX(float rotation)
{
	NeedsToRecalculateWorldTransform();
	m_LocalTransform.Rotation.x = rotation;
}

void dae::Transform::SetLocalRotationY(float rotation)
{
	NeedsToRecalculateWorldTransform();
	m_LocalTransform.Rotation.y = rotation;
}

void dae::Transform::SetLocalRotationZ(float rotation)
{
	NeedsToRecalculateWorldTransform();
	m_LocalTransform.Rotation.z = rotation;
}

void dae::Transform::SetLocalRotation(const glm::vec3& rotation)
{
	NeedsToRecalculateWorldTransform();
	m_LocalTransform.Rotation = rotation;
}

void dae::Transform::MoveLocalRotationX(float rotation)
{
	NeedsToRecalculateWorldTransform();
	m_LocalTransform.Rotation.x += rotation;
}

void dae::Transform::MoveLocalRotationY(float rotation)
{
	NeedsToRecalculateWorldTransform();
	m_LocalTransform.Rotation.y += rotation;
}

void dae::Transform::MoveLocalRotationZ(float rotation)
{
	NeedsToRecalculateWorldTransform();
	m_LocalTransform.Rotation.z += rotation;
}

void dae::Transform::MoveLocalRotation(const glm::vec3& rotation)
{
	NeedsToRecalculateWorldTransform();
	m_LocalTransform.Rotation += rotation;
}

void dae::Transform::SetLocalTransform(const TransformData& transformData)
{
	NeedsToRecalculateWorldTransform();
	m_LocalTransform = transformData;
}

const dae::TransformData& dae::Transform::GetWorldTransform() const
{
	CalculateWorldTransform();

	return m_WorldTransform;
}

const dae::TransformData& dae::Transform::GetLocalTransform() const
{
	return m_LocalTransform;
}

const dae::TransformData& dae::Transform::GetViewTransform(const glm::mat4& inverseCameraMatrix) const
{
	m_ViewTransform = TransformDataFromMatrix(inverseCameraMatrix * GetWorldMatrix());

	return m_ViewTransform;
}

const glm::mat4& dae::Transform::GetWorldMatrix() const
{
	CalculateWorldTransform();

	return m_WorldMatrix;
}

glm::vec3 dae::Transform::GetForwardVector() const
{
	CalculateWorldTransform(); 
	return -glm::normalize(glm::vec3(m_WorldMatrix[2]));
}

glm::vec3 dae::Transform::GetUpVector() const
{
	CalculateWorldTransform();
	return glm::normalize(glm::vec3(m_WorldMatrix[1]));  // Y-axis
}

glm::vec3 dae::Transform::GetRightVector() const
{
	CalculateWorldTransform();
	return glm::normalize(glm::vec3(m_WorldMatrix[0]));  // X-axis
}

glm::vec3 dae::Transform::LocalToWorldVec(const glm::vec3& localVec) const
{
	const glm::mat4& worldMatrix = GetWorldMatrix();
	return glm::vec3(worldMatrix * glm::vec4(localVec, 1.0f));
}

glm::vec2 dae::Transform::LocalToWorldVec(const glm::vec2& localVec) const
{
	const glm::mat4& worldMatrix = GetWorldMatrix();
	glm::vec4 result = worldMatrix * glm::vec4(localVec, 0.0f, 1.0f);
	return glm::vec2(result);
}

glm::vec3 dae::Transform::WorldToLocalVec(const glm::vec3& worldVec) const
{
	glm::mat4 inverseWorld = glm::inverse(GetWorldMatrix());
	return glm::vec3(inverseWorld * glm::vec4(worldVec, 1.0f));
}

glm::vec2 dae::Transform::WorldToLocalVec(const glm::vec2& worldVec) const
{
	glm::mat4 inverseWorld = glm::inverse(GetWorldMatrix());
	glm::vec4 result = inverseWorld * glm::vec4(worldVec, 0.0f, 1.0f);
	return glm::vec2(result);
}

void dae::Transform::OverrideWorldScaleWithLocalScale(bool override)
{
	NeedsToRecalculateWorldTransform();
	m_OverrideWorldScaleWithLocalScale = override;
}

void dae::Transform::OverrideWorldPositionWithLocalPosition(bool override)
{
	NeedsToRecalculateWorldTransform();
	m_OverrideWorldPositionWithLocalPosition = override;
}

void dae::Transform::OverrideWorldRotationWithLocalRotation(bool override)
{
	NeedsToRecalculateWorldTransform();
	m_OverrideWorldRotationWithLocalRotation = override;
}

void dae::Transform::NeedsToRecalculateWorldTransform()
{
	if (!m_NeedToRecalculateWorldTransform)
	{
		m_NeedToRecalculateWorldTransform = true;

		if (HasChildren())
		{
			Transform* currentParentTransform{ nullptr };
			Transform* currentChildTransform{ nullptr };

			std::queue<Transform*> transforms;
			transforms.push(this);

			while (!transforms.empty())
			{
				currentParentTransform = transforms.front();
				transforms.pop();

				const std::vector<GameObject*>& children = currentParentTransform->GetChildren();

				for (size_t i = 0; i < children.size(); i++)
				{
					currentChildTransform = &children[i]->Transform();

					if (currentChildTransform && !currentChildTransform->m_NeedToRecalculateWorldTransform)
					{
						currentChildTransform->m_NeedToRecalculateWorldTransform = true;
						transforms.push(currentChildTransform);
					}
				}
			}
		}
		
	}
	
}

glm::mat4 dae::Transform::MatrixFromTransformData(const TransformData& transformData)const
{
	glm::mat4 currentMatrix{ glm::identity<glm::mat4>() };

	// Correct order: T * R * S
	currentMatrix = glm::translate(currentMatrix, transformData.Position);
	currentMatrix = glm::rotate(currentMatrix, glm::radians(transformData.Rotation.x), glm::vec3{ 1.f, 0.f, 0.f });
	currentMatrix = glm::rotate(currentMatrix, glm::radians(transformData.Rotation.y), glm::vec3{ 0.f, 1.f, 0.f });
	currentMatrix = glm::rotate(currentMatrix, glm::radians(transformData.Rotation.z), glm::vec3{ 0.f, 0.f, 1.f });
	currentMatrix = glm::scale(currentMatrix, transformData.Scale);

	return currentMatrix;
}

dae::TransformData dae::Transform::TransformDataFromMatrix(const glm::mat4& matrix)const
{
	TransformData transformData{};
	transformData.Position = glm::vec3(matrix[3]);

	transformData.Scale.x = glm::length(glm::vec3(matrix[0])); // X-axis scale
	transformData.Scale.y = glm::length(glm::vec3(matrix[1])); // Y-axis scale
	transformData.Scale.z = glm::length(glm::vec3(matrix[2])); // Z-axis scale

	glm::mat3 rotationMatrix = glm::mat3(matrix); // Extract 3x3 rotation matrix
	rotationMatrix[0] /= transformData.Scale.x; // Normalize X axis
	rotationMatrix[1] /= transformData.Scale.y; // Normalize Y axis
	rotationMatrix[2] /= transformData.Scale.z; // Normalize Z axis

	glm::quat rotation = glm::quat_cast(rotationMatrix); // Convert to quaternion
	transformData.Rotation = glm::eulerAngles(rotation); // Extract Euler angles (in radians)
	transformData.Rotation = glm::degrees(transformData.Rotation); // Convert to degrees if needed

	return transformData;
}

void dae::Transform::CalculateWorldTransform()const
{
	if (m_NeedToRecalculateWorldTransform)
	{
		if (HasParent())
		{
			m_WorldMatrix = MatrixFromTransformData(m_LocalTransform);
			Transform* currentParentTransform{ &m_Parent->Transform()};

			while (currentParentTransform)
			{
				//parent should always have transform
				if (currentParentTransform->m_NeedToRecalculateWorldTransform)
				{
					m_WorldMatrix = MatrixFromTransformData(currentParentTransform->m_LocalTransform) * m_WorldMatrix;
				
					if (currentParentTransform->HasParent())
					{
						currentParentTransform = &currentParentTransform->m_Parent->Transform();
					}
					else
					{
						currentParentTransform = nullptr;
					}
				}
				else
				{
					m_WorldMatrix = currentParentTransform->m_WorldMatrix * m_WorldMatrix;
					currentParentTransform = nullptr;
				}

			}

			m_WorldTransform = TransformDataFromMatrix(m_WorldMatrix);

			if (m_OverrideWorldScaleWithLocalScale)
			{
				m_WorldTransform.Scale = m_LocalTransform.Scale;
			}
			if (m_OverrideWorldPositionWithLocalPosition)
			{
				m_WorldTransform.Position = m_LocalTransform.Position;
			}
			if (m_OverrideWorldRotationWithLocalRotation)
			{
				m_WorldTransform.Rotation = m_LocalTransform.Rotation;
			}
		}
		else
		{
			m_WorldMatrix = MatrixFromTransformData(m_LocalTransform);
			m_WorldTransform = m_LocalTransform;
		}

		
	}

	m_NeedToRecalculateWorldTransform = false;
	
}

