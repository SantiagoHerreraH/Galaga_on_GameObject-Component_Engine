#pragma once

#include "WeaponType.h"
#include "StateMachine.h"
#include "Movement.h"

namespace dae {

	class IdleState : public IState {

	public:
		IdleState(float speed) : m_Speed(speed) {}
		static std::string Name() { return "IdleState"; }
		virtual void Start(GameObject& actor)override;
		virtual void Update(GameObject& actor)override{}
		virtual void End(GameObject& actor) override{}


	private:
		float m_Speed;
	};

	class DescendState : public IState {

	public:
		DescendState(float speed) : m_Speed(speed){}
		static std::string Name() { return "DescendState"; }
		virtual void Start(GameObject& actor);
		virtual void Update(GameObject& actor);
		virtual void End(GameObject& actor){}

	private:
		float m_Speed;

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
		AscendState(float speed) : m_Speed(speed) {}
		static std::string Name() { return "AscendState"; }
		virtual void Start(GameObject& actor);
		virtual void Update(GameObject& actor);
		virtual void End(GameObject& actor){}


	private:
		float m_Speed;
	};


	class CaptureZoneWeaponType : public WeaponType {

	public:

		CaptureZoneWeaponType(const GameObjectHandle& target, float speed, const std::string& triggerName = "Act");
		void Create(const GameObjectHandle& gameObject) override;
		void Execute(GameObject& obj) override;

	private:
		float m_Speed;
		GameObjectHandle m_Target;
		std::string m_TriggerName;
	};
}