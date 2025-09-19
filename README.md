
# About My Project

I adapted the base functionalities of Minigin, which are described below, and added the following features:

# Game Engine Features

- A Game Loop including
	- Update
	- FixedUpdate
	- Render
	- Input tracking
	- Time tracking
- A Scene / GameObject / Component Framework that lets you
	- Create structured code to interact with the game loop and divide execution into different Scenes, GameObjects and Components
	- Create and Navigate through scenes in a cyclical order or via string id
	- Group Component functionality through GameObjects
	- easily extend functionality through component creation
	- easily interact with other components and your gameobject
	- control spatial object transformations via a scene graph through an extensive Transform class
		- Lets you control rotation, translation, scale
		- Lets you parent and unparent 
		- lets you control which transformation variables you want the Transform to be affected by its parent transformation
			- for instance, have rotation and translation be affected by scene graph, but not scale
- Physics systems
	- 2d collision systems for solid and nonsolid rectangles
	- 2d Rigidbody systems
	- Movement system for physics interaction
- A State Machine that functions as a state graph
	- create various states and links them through conditionals
	- allows one state at a time
- A variadic template Event System with queue functionality as well as instantaneous triggering where you can subscribe std::functions
- A templated Stat Controller with various events that are triggered upon stat modification
- A player controller that receives input from keyboard and gamepad, where you can link Commands to input (through ICommand interface)
- Different rendering systems that include
	- SpriteSheet: for managing grid like textures
	- Animation: animating textures and controlling framerate
	- Simple particle system: where you can control different variables for movable particles
	- Improved ResourceManager for limited loading of resources such as textures
- A ServiceLocator for interacting with and changing different services at runtime depending on needs
- A SceneSingleton templated class for having unique instances of classes per scene and having them automatically update and reset upon scene change
- A TimerSystem for controlling execution through various Events (OnStart, OnUpdate, OnEnd, etc...) in the time of your choice, and that stays lean by keeping active timers contiguous in memory.
- Simple but effective math classes for 
	- Getting random value between range
	- Controlling sine and cosine movement with x direction and time
- A MovementActionSequence system that lets you
	- chain actions one after another and set their duration to be conditional or time dependent
	- Skip action execution through conditions

# Game Features

- System for calling functions on templated data through strings (StringToDataModifier)
	- This was crucial for limiting the amount of data exposed in json format for level creation and level loading
	- I made level creation much easier and simpler
- Implementation of pattens such as
	- Prototype for enemy creation and player creation
	- Builder for enemy creation and CAnimation Component
	- Factory and Command patterns for WeaponType
- Usage of all game engine features in a concrete game, which was Galaga in this case

# GitHub repo Link

https://github.com/SantiagoHerreraH/Exam-Prog4.git

# About the Base Minigin

Minigin is a very small project using [SDL2](https://www.libsdl.org/) and [glm](https://github.com/g-truc/glm) for 2D c++ game projects. It is in no way a game engine, only a barebone start project where everything sdl related has been set up. It contains glm for vector math, to aleviate the need to write custom vector and matrix classes.

[![Build Status](https://github.com/avadae/minigin/actions/workflows/msbuild.yml/badge.svg)](https://github.com/avadae/msbuild/actions)
[![GitHub Release](https://img.shields.io/github/v/release/avadae/minigin?logo=github&sort=semver)](https://github.com/avadae/minigin/releases/latest)

# Goal

Minigin can/may be used as a start project for the exam assignment in the course [Programming 4](https://youtu.be/j96Oh6vzhmg) at DAE. In that assignment students need to recreate a popular 80's arcade game with a game engine they need to program themselves. During the course we discuss several game programming patterns, using the book '[Game Programming Patterns](https://gameprogrammingpatterns.com/)' by [Robert Nystrom](https://github.com/munificent) as reading material. 

# Disclaimer

Minigin is, despite perhaps the suggestion in its name, **not** a game engine. It is just a very simple sdl2 ready project with some of the scaffolding in place to get started. None of the patterns discussed in the course are used yet (except singleton which use we challenge during the course). It is up to the students to implement their own vision for their engine, apply patterns as they see fit, create their game as efficient as possible.

# Use

Either download the latest release of this project and compile/run in visual studio or, since students need to have their work on github too, they can use this repository as a template (see the "Use this template" button at the top right corner). There is no point in forking this project.
