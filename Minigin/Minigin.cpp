#include <stdexcept>
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "Minigin.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include <chrono>
#include <thread>
#include "GameTime.h"
#include "AudioManager.h"
#include "ServiceLocator.h"
#include "Settings.h"
#include "EventSystem.h"
#include <memory>

SDL_Window* g_window{};

void PrintSDLVersion()
{
	SDL_version version{};
	SDL_VERSION(&version);
	printf("We compiled against SDL version %u.%u.%u ...\n",
		version.major, version.minor, version.patch);

	SDL_GetVersion(&version);
	printf("We are linking against SDL version %u.%u.%u.\n",
		version.major, version.minor, version.patch);

	SDL_IMAGE_VERSION(&version);
	printf("We compiled against SDL_image version %u.%u.%u ...\n",
		version.major, version.minor, version.patch);

	version = *IMG_Linked_Version();
	printf("We are linking against SDL_image version %u.%u.%u.\n",
		version.major, version.minor, version.patch);

	SDL_TTF_VERSION(&version)
	printf("We compiled against SDL_ttf version %u.%u.%u ...\n",
		version.major, version.minor, version.patch);

	version = *TTF_Linked_Version();
	printf("We are linking against SDL_ttf version %u.%u.%u.\n",
		version.major, version.minor, version.patch);
}

dae::Minigin::Minigin(const std::string &dataPath)
{
	PrintSDLVersion();
	
	if (SDL_Init(SDL_INIT_VIDEO) != 0) 
	{
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}
	
	g_window = SDL_CreateWindow(
		"Programming 4 assignment",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		g_WindowWidth,
		g_WindowHeight,
		SDL_WINDOW_OPENGL
	);
	if (g_window == nullptr) 
	{
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	}

	Renderer::GetInstance().Init(g_window);

	ResourceManager::GetInstance().Init(dataPath);
}

dae::Minigin::~Minigin()
{
	Renderer::GetInstance().Destroy();
	SDL_DestroyWindow(g_window);
	g_window = nullptr;
	SDL_Quit();
}

void dae::Minigin::Run(const std::function<void()>& load)
{
	IAudioService* audioService = ServiceLocator::GetInstance().GetService<IAudioService>();

	if (!audioService)
	{
		audioService = ServiceLocator::GetInstance().SetService<IAudioService>(std::make_unique<AudioManager>());
	}

	load();
	
	srand((unsigned int)std::time(nullptr));

	auto& renderer = Renderer::GetInstance();
	auto& sceneManager = SceneManager::GetInstance();
	auto& eventSystem = EventSystem::GetInstance();

	

	std::thread audioThread(&IAudioService::Update, audioService);

	// todo: this update loop could use some work.
	const float fixedTimeStep{ 0.02f };
	const long long millisecondsPerFrame = long long(1000/60.f) ;
	bool doContinue = true;
	auto lastTime = std::chrono::high_resolution_clock::now();
	float lag = 0.0f;
	Time::GetInstance().m_FixedTimeStep = fixedTimeStep;

	sceneManager.Start(); //if you need delta seconds, call it inside loop

	while (doContinue)
	{
		const auto currentTime = std::chrono::high_resolution_clock::now();
		const float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
		Time::GetInstance().m_ElapsedSeconds = deltaTime;
		lastTime = currentTime;
		lag += deltaTime;

		while (lag >= fixedTimeStep)
		{
			sceneManager.FixedUpdate();
			lag -= fixedTimeStep;
		}

		eventSystem.Update();
		sceneManager.Update();
		renderer.Render();

		const auto sleepTime = 
			currentTime + 
			std::chrono::milliseconds(millisecondsPerFrame) -
			std::chrono::high_resolution_clock::now();

		std::this_thread::sleep_for(sleepTime);
	}

	audioThread.join();
}
