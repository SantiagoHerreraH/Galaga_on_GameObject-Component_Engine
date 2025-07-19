#pragma once

#include <glm.hpp>
#include <memory>
#include <vector>
namespace dae
{
	struct TransformData final
	{
		glm::vec3 Position{ 0.f, 0.f, 0.f};
		glm::vec3 Scale{ 1.f, 1.f, 1.f };
		glm::vec3 Rotation{ 0.f,0.f, 0.f };

		TransformData& operator+=(const TransformData& other) {
			Position += other.Position;
			Scale *= other.Scale;
			Rotation += other.Rotation;
			return *this;
		}

		
	};

	inline TransformData operator+(const TransformData& one, const TransformData& two) {
		
		return TransformData{one.Position + two.Position, one.Scale * two.Scale, one.Rotation + two.Rotation};
	}

	enum class ETransformReparentType {
		KeepWorldTransform,
		KeepLocalTransform
	};

	class GameObject;
	using GameObjectHandle = std::shared_ptr<GameObject>;

	
	class Transform final
	{
	public:
		Transform(GameObject& self);

		bool HasParent()const;
		bool HasChildren()const;
		const GameObject* GetParent()const;
		void MakeRootNode();
		bool SetParent(GameObject& parent, ETransformReparentType transformReparentType);
		const std::vector<dae::GameObject*>& GetChildren()const;
		bool IsDescendant(const GameObject& descendant);

		void SetLocalScaleX(float scale);
		void SetLocalScaleY(float scale);
		void SetLocalScaleZ(float scale);
		void SetLocalScale(const glm::vec3& scale);

		void SetLocalPositionX(float pos);
		void SetLocalPositionY(float pos);
		void SetLocalPositionZ(float pos);
		void SetLocalPosition(const glm::vec3& pos);

		void MoveLocalPositionX(float pos);
		void MoveLocalPositionY(float pos);
		void MoveLocalPositionZ(float pos);
		void MoveLocalPosition(const glm::vec3& pos);

		void SetLocalRotationX(float rotation);
		void SetLocalRotationY(float rotation);
		void SetLocalRotationZ(float rotation);
		void SetLocalRotation(const glm::vec3& rotation);

		void MoveLocalRotationX(float rotation);
		void MoveLocalRotationY(float rotation);
		void MoveLocalRotationZ(float rotation);
		void MoveLocalRotation(const glm::vec3& rotation);

		void SetLocalTransform(const TransformData& transformData);

		const TransformData& GetWorldTransform()const;
		const TransformData& GetLocalTransform()const;
		const TransformData& GetViewTransform(const glm::mat4& inverseCameraMatrix)const;
		const glm::mat4& GetWorldMatrix()const ;

		glm::vec3 GetForwardVector()const; 
		glm::vec3 GetUpVector()const;
		glm::vec3 GetRightVector()const;

		glm::vec3 LocalToWorldVec(const glm::vec3& localVec) const;
		glm::vec2 LocalToWorldVec(const glm::vec2& localVec) const;
		glm::vec3 WorldToLocalVec(const glm::vec3& worldVec) const;
		glm::vec2 WorldToLocalVec(const glm::vec2& worldVec) const;

		//Does the transformation, keeps the world matrix, but overrides the world transform.
		//Of course, this effect can only be noticed if the transform is child since all of the 
		//root local transforms are by default world transforms
		void OverrideWorldScaleWithLocalScale(bool override);
		void OverrideWorldPositionWithLocalPosition(bool override);
		void OverrideWorldRotationWithLocalRotation(bool override);

	private:

		//because of dirty pattern, some values have to be mutable and const

		void NeedsToRecalculateWorldTransform();
		glm::mat4 MatrixFromTransformData(const TransformData& transformData)const;
		TransformData TransformDataFromMatrix(const glm::mat4& matrix)const;

		void CalculateWorldTransform()const;

		mutable TransformData m_ViewTransform{};
		mutable TransformData m_WorldTransform{};
		TransformData m_LocalTransform{};
		mutable glm::mat4 m_WorldMatrix{};

		GameObject& m_Self;
		GameObject* m_Parent{nullptr};
		std::vector<GameObject*> m_Children{};

		mutable bool m_NeedToRecalculateWorldTransform{true};

		bool m_OverrideWorldScaleWithLocalScale			{false};
		bool m_OverrideWorldPositionWithLocalPosition	{false};
		bool m_OverrideWorldRotationWithLocalRotation	{false};

	};
}
