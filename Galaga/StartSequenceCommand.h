#pragma once

#include "Command.h"
#include "MovementActionSequence.h"

namespace dae {

	class StartSequenceCommand final : public ICommand {

	public:

		virtual void Execute(GameObject& gameObject) {

			CMovementActionSequence* sequence = gameObject.GetComponent<CMovementActionSequence>();

			if (sequence && !sequence->IsActing())
			{
				sequence->StartSequence();
			}
		}
	};
}