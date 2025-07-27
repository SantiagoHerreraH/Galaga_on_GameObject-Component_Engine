#pragma once

#include "Command.h"
#include "MovementActionSequence.h"

namespace dae {

	class StartSequenceCommand final : public ICommand {

	public:

		virtual void Execute(GameObject& gameObject) {

			MovementActionSequence* sequence = gameObject.GetComponent<MovementActionSequence>();

			if (sequence && !sequence->IsActing())
			{
				sequence->StartSequence();
			}
		}
	};
}