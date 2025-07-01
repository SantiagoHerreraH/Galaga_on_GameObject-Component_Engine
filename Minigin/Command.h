#pragma once

#include <vector>
#include "GameObject.h"

namespace dae
{
	class ICommand
	{
	public:
		virtual ~ICommand() = default;
		virtual void Execute(GameObject& gameObject) = 0;
	};

}

