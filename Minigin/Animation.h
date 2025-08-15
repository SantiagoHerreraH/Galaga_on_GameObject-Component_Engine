#pragma once

#include "Component.h"
#include "Texture.h"
#include "Rect.h"
#include "EventSystem.h"

namespace dae {

	/*
		Each animation for now has:
		- a character
			- 
		- a state
		- 
		- The animation manager is just the interface 
		to change the animation state -> the actual change in state will be managed by the event system
		- There will be an event listener that will change the current animation corresponding its state
	*/

	

	class CAnimation final : public Component
	{

	public:

		void Update()override;
		void Render()const override;
		void End()override;

		void CanLoop(bool canLoop); //default is true
		void SetFramesPerSecond(unsigned int numberOfFrames);
		void AddFrame(const CTextureHandle& frame, unsigned int numberOfFrames);

		void ClearFrames();

		int LoopNumber();

		void Reset();
		void Play();
		void Pause();

		const CTextureHandle& GetConstTextureRef(int frameNumber)const;
		const CTextureHandle& GetCurrentFrame()const;

		void SubscribeOnEnd(const std::function<void()>& func);

	private:
		int m_NumberOfLoops{ 0 };
		bool m_CanLoop{ true };
		bool m_IsPlaying{ true };
		double m_CurrentTime{0};
		int m_CurrentFrameIndex{ 0 };
		float m_SecondsPerFrame{ 1 };
		Event<> m_OnEnd{};

		struct AnimationFrameInfo {
			CTextureHandle TextureHandle;
			unsigned int NumberOfFrames;
			float TimeToGoToTheNextFrame;
		};

		std::vector<AnimationFrameInfo> m_Frames;
	};
}