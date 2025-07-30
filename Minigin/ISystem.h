#pragma once

namespace dae {

	class ISystem {

	public:

		virtual void Update() = 0;
		virtual void Reset() = 0;
	};
}