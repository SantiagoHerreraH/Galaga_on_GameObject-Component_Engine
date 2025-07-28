#pragma once

#include "GameObject.h"

namespace dae {

	class ICreate {

	public:
		virtual ~ICreate() = default;
		virtual void Create(const GameObjectHandle& gameObject) = 0;
	};

}