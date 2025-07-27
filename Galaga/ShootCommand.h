#pragma once

#include "Command.h"
#include "Gun.h"

namespace dae {

	class ShootCommand final : public ICommand {

	public:
		virtual void Execute(GameObject& gameObject) {
			
			CGun* gun = gameObject.GetComponent<CGun>();

			if (gun)
			{
				gun->Shoot();
			}
		
		}

	};
}