#pragma once

#include "WeaponType.h"
#include "StateMachine.h"
#include "Movement.h"

namespace dae {

	class IdleState : public IState {

	public:
		static std::string Name() { return "IdleState"; }
		virtual void Start(GameObject& actor)override;
		virtual void Update(GameObject& actor)override{}
		virtual void End(GameObject& actor) override{}
	};

	class FollowState : public IState {

	public:
		FollowState(GameObjectHandle target) : m_Target(target){}
		static std::string Name() { return "FollowState"; }
		virtual void Start(GameObject& actor);
		virtual void Update(GameObject& actor);
		virtual void End(GameObject& actor){}

	private:
		GameObjectHandle m_Target;
	};

	class CaptureState : public IState {

	public:
		static std::string Name() { return "CaptureState"; }
		virtual void Start(GameObject& actor);
		virtual void Update(GameObject& actor){}
		virtual void End(GameObject& actor);
	};

	class AscendState : public IState {

	public:
		static std::string Name() { return "AscendState"; }
		virtual void Start(GameObject& actor);
		virtual void Update(GameObject& actor) = 0;
		virtual void End(GameObject& actor) = 0;
	};


	class CaptureZoneWeaponType : public WeaponType {

	public:

		CaptureZoneWeaponType(const GameObjectHandle& target, const std::string& triggerName = "Act");
		void Create(const GameObjectHandle& gameObject) override;
		void Execute(GameObject& obj) override;

	private:
		GameObjectHandle m_Target;
		std::string m_TriggerName;
	};
}