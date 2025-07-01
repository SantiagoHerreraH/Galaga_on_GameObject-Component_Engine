#pragma once
#include <string>
#include <functional>

namespace dae
{

	//temp variables
	inline int g_WindowWidth{ 640 };
	inline int g_WindowHeight{ 480 };

	class Minigin
	{
	public:
		explicit Minigin(const std::string& dataPath);
		~Minigin();
		void Run(const std::function<void()>& load);

		Minigin(const Minigin& other) = delete;
		Minigin(Minigin&& other) = delete;
		Minigin& operator=(const Minigin& other) = delete;
		Minigin& operator=(Minigin&& other) = delete;
	};
}