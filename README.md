
# About My Project

A fully self-designed and implemented game engine running a self coded and slightly modified version of Galaga.

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
