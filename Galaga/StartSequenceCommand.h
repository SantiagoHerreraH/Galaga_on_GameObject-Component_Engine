#pragma once

#include "ICommand.h"
#include "CMovementActionSequence.h"

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