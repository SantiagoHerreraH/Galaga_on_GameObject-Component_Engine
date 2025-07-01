#pragma once

namespace dae {

	class ISystem {

	public:

		virtual void Update() {}
		virtual void Reset() {}
	};
}