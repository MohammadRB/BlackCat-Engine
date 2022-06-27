# BlackCat-Engine
BlackCat Engine is an in-house developed 3D game engine written in C++ with multi-platform support in mind. 

![Win32](https://img.shields.io/static/v1?label=win32&message=%E2%9C%94%EF%B8%8F&color=green&style=flat&logo=windows) ![DirectX11](https://img.shields.io/static/v1?label=dx11&message=%E2%9C%94%EF%B8%8F&color=green&style=flat&logo=nutanix)
###### Currently only Windows and DirectX 11 are implemented.

## Features

 - Parallel update/render loops
 - Rich task based parallel programming 
 - Messaging APIs including events and queries
 - Asset and Content management pipeline with offline processing and streaming features
 - Powerful Actor-Component model
 - Entity system to empower actor-component model with actor definitions in json files
 - Client/Server network system with powerful messaging framework
 - 3d Physics simulation powered by PhysX
 - Skinned animation 
 - Ragdoll animation
 - Sound system powered by Fmod
 - Scripting system powered by ChakraCore engine
 - Deferred rendering 
 - Deferred decal rendering
 - Vegetable animation
 - Cascaded shadow maps
 - Particle system

## Workspace directory
 - **3rdParty/** 3rdParty used libraries.
 - **Solution/**  Development IDE solutions.
 - **Src/** Engine source code. The source code is separated into physical like folders instead of logical folders. Below is source code structure based on build dependencies.
    - **BlackCat.Core.Platform/** Platform specific functionalities required by `BlackCat.Core` modules.
	 - **BlackCat.Core.Platform.Win32/** Windows implementation of `BlackCat.Core.Platform` module.
	 - **BlackCat.Core/** Core functionalities of the Engine.
	 - **BlackCat.Platform/** Platform specific functionalities.
	 - **BlackCat.Platform.Win32/** Windows implementation of `BlackCat.Platform` module.
	 - **BlackCat.Graphics/** Graphic API abstractions.
	 - **BlackCat.Graphics.DirectX11/** DirectX 11 implementation of `BlackCat.Graphics` module.
	 - **BlackCat.Physics/** Physics functionality abstractions
	 - **BlackCat.Physics.PhysX/** [PhysX](https://developer.nvidia.com/physx-sdk) implementation of `BlackCat.Physics` module.
	 - **BlackCat.Sound/** Sound functionality abstractions.
	 - **BlackCat.Sound.Fmod/** [Fmod](https://www.fmod.com/) implementation of `BlackCat.Sound` module.
	 - **BlackCat.Game/** Contains main functionalities of engine including animation, input, network, physics, rendering, scripting, sound systems and game play support systems.
	 - **BlackCat.App/** Engine default implementations including `content loaders` and `render passes`.
	 - **BlackCat.Editor/** Engine editor.
- **BattleOfXbots/** A complete third-person multiplayer shooter game developed with the engine with 3 executable projects in following folders.
	- **Box/** Game client version.
	- **Box.Server/** Game server version.
	- **Box.Editor/** Game editor.

