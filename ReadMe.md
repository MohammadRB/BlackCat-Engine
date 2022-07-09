# BlackCat-Engine
BlackCat Engine is an in-house developed 3D game engine written in C++ with multi-platform support in mind. Currently, engine only supports Windows and DirectX 11 and is under development to have more features.

![Win32](https://img.shields.io/static/v1?label=win32&message=%E2%9C%94%EF%B8%8F&color=green&style=flat&logo=windows) ![DirectX11](https://img.shields.io/static/v1?label=dx11&message=%E2%9C%94%EF%B8%8F&color=green&style=flat&logo=nutanix)

This repository contains the engine source code and a sample project which is a multiplier third-person shooter game developed by the engine called **Battle Of Xbots**.

![BattleOfXbots](https://raw.githubusercontent.com/MohammadRB/img/6ed470482969c21b760866f0e946849612d9524c/BC/box5.png)
## Features

 - Parallel update/render loops
 - Rich task based parallel library
 - Messaging APIs including events and queries
 - Asset and Content management pipeline with offline processing and streaming features
 - Powerful actor-component model
 - Entity system to empower actor-component model with entity definitions in json files
 - Client/Server network system with robust messaging framework
 - 3d Physics simulation powered by PhysX
 - Skinned animations powered by ozz-animation
 - Ragdoll animations
 - Sound system powered by Fmod
 - Scripting system powered by ChakraCore engine
 - Deferred rendering 
 - Deferred decal rendering
 - Vegetable animation
 - Cascaded shadow maps
 - Particle system
 - Terrain system
 
#### Parallel Update/Render Loops
The engine update and render loops run in two different threads. While the Nth frame is being updated the N-1 frame is being rendered. The interaction between render thread and update thread are through query objects. At the start of each frame, render thread register queries to gather all required data for the next frame. During update phase, these queries are resolved by multiple threads to set each query result which will be used by render thread at next frame. 
#### Parallel Programming
Engine is equipped with a thread pool which add/remove threads on the flay .Running a new task is as easy as below:
 ``` C++
auto l_task = core::bc_concurrency::start_task
(
		core::bc_delegate<void()>
		(
			[]()
			{
				// Do pararrel
			}
		)
);
 ```
And waiting for or fetching Tasks result:
``` C++
// Wait for a single task completion
l_task.get(); 
// Wait for multiple tasks completion
core::bc_concurrency::when_all(l_task1, l_task2, l_task3);
```
#### Messaging APIs
Events and Queries are two features to notify and inquiry states in engine's multithreaded environment. 
Events are based on Publish/Subscribe pattern in which a few subscriber can listen to a specific event type. When subscribing to an event, an object of type `core::bc_event_listener_handle`  is returned which is a RAII object meaning when it get destroyed the subscription will be cancelled.
		
``` C++
// Define an event object
class bc_app_event_window_close : public core::bc_app_event
{
	BC_EVENT(app_cls)
public:
		explicit bc_app_event_window_close(bc_window::id p_window_id) noexcept;
    
		bc_app_event_window_close(const bc_app_event_window_close&) = default;
    
		~bc_app_event_window_close() override = default;
    
		bc_app_event_window_close& operator =(const bc_app_event_window_close&) = default;
    
		bc_window::id get_window_id() const noexcept;
    
private:
    bc_window::id m_window_id;
};

// Subscribe to it and hold the subscription handle in a variable
core::bc_event_listener_handle m_event_handle_window_close = l_event_manager->register_event_listener<platform::bc_app_event_window_close>
(
	core::bc_event_manager::delegate_type(*this, &bc_render_application::_app_event)
);

// Consume it in the function which was used in subscription
void bc_render_application::_app_event(core::bci_event& p_event)
{
	if (const auto* l_close_event = core::bci_message::as<platform::bc_app_event_window_close>(p_event))
	{
	}
}
```
On the other hand Queries are like Command pattern in which queries include the code which must be executed when query is fetched. Defining a query object is as easy as an event except that the object has an execute method which will be called by a worker thread when the query is fetched in engine's main update cycle, so the result will be available in asynchronous manner.
``` C++
// Define a query object which holds the result too
class bc_scene_wind_query : public core::bc_query<bc_scene_query_context>
{
	BC_QUERY(sc_wnd)
			
public:
	bc_scene_wind_query();

	bc_scene_wind_query(bc_scene_wind_query&&) noexcept;

	~bc_scene_wind_query() override;
			
	bc_scene_wind_query& operator=(bc_scene_wind_query&&) noexcept;

	core::bc_vector<bc_wind> get_winds() noexcept;

protected:
	void execute(const bc_scene_query_context& p_context) noexcept override;

private:
	core::bc_vector<bc_wind> m_winds;
};

// The result of queuing a query is of type bc_query_result<T>
core::bc_query_result<game::bc_scene_wind_query> m_winds_query;

// Queue query to be executed which will be ready for next frame
m_winds_query = m_query_manager.queue_query(game::bc_scene_wind_query());

// In next frame check to see if the query is executed and result is ready
if(m_winds_query.is_executed())
{
	m_winds_query_result = m_winds_query.get().get_winds();
}
```
In above example the intention is to query the wind objects in the scene and collect them in a vector. This pattern match concurrent update/render loop very well. While the render thread is rendering nth-1 frame, will queue several queries to fetch current state of the game in the next frame. In fact, this fits with game play code too. When an explosion happens in the scene a query can be queued to fetch all the nearby rigid dynamic objects. The result will be ready by one frame delay but that would not be a big deal. The big advantage in this model is batch processing and making use of available hardware threads which usually will be done when main thread is idle and waiting for render thread to finish with GPU in present method.
#### Content management
Assets are imported into engine through content loaders which must be registered inside the engine.
```C++
core::bc_register_loader<bc_mesh, bc_mesh_loader>("mesh", core::bc_make_loader<bc_mesh_loader>());
```
Here the `bc_mesh` is a subclass of `bci_content` and `bc_mesh_loader` is a subclass of `bci_content_loader`. By a registered loader for `bc_mesh`, an instance of mesh can be loaded by a single line of code:
```C++
auto l_mesh = l_content_manager.load<bc_mesh>
(
	path_to_file,
	{},
	core::bc_content_loader_parameter()
);
```
This way, a new asset or content type can be used inside the engine by simply registering a new loader for that specific content. For example to implement the `game mode` and `editor mode` functionality inside the editor a special loader is created to create and save a checkpoint object from the scene and load an existing one from the disk to apply the difference in the scene. (Actually, loaders are used not only for loading but also for saving assets too)
The return type of `load` function is a smart pointer which has an internal reference counter for the loaded content and will unload the content when its reference count reach zero. 
Loading and managing every single asset is hard. The desired way to load assets is to group them and load all of them at once. The content streams is what is needed. Every stream contains a dozen of contents which are defined inside a json file. For example [this file](https://github.com/MohammadRB/BlackCat-Engine/blob/master/Src/BlackCat.App/App/Content/Data/ContentStream.json) defines a couple of different streams which can be loaded by their names:
```C++
stream_manager.load_content_stream("engine_shaders");
```
The load work is spread  over multiple threads to speed up the process. Beside organizing assets, streams can be used to load/unload whatever is needed on the fly.
#### Actor Component Model
Game objects are modeled as actor-component in which actors can contain multiple distinct components. Before using components they must be registered with engine. 
``` C++
game::bc_register_component_types
(
	game::bc_component_register<game::bc_rigid_static_component>("rigid_static"),
	game::bc_component_register<game::bc_rigid_dynamic_component>("rigid_dynamic"),
	game::bc_component_register<game::bc_rigid_controller_component>("rigid_controller"),
);
```
The component model also support abstract component concept which means components can have a common base component and at runtime instead of searching for concrete component type the common base component can be queried inside an actor. In above example three components are registered which all of them have a common base component. In order to be able to access to these three components through their base component we should inform their hierarchy with below line of code.
```C++
game::bc_register_abstract_component_types
(
	game::bc_abstract_component_register<game::bc_rigid_body_component, game::bc_rigid_static_component, game::bc_rigid_dynamic_component, game::bc_rigid_controller_component>(),
);
```
Here `bc_rigid_body_component` is base abstract component of all three other components which can be used to retrieve any of other components at runtime if one of them exist in actor component collection.
```C++
auto l_actor = l_actor_component_manager.create_actor();
l_actor.create_component<game::bc_rigid_dynamic_component>();

// Access to created component directly
auto* l_rigid_dynamic = l_actor.get_component<game::bc_rigid_dynamic_component>();
// Access to created component through its base component. The runtime type of returned component will be bc_rigid_dynamic_component
auto* l_rigid_body = l_actor.get_component<game::bc_rigid_body_component>();
```
#### Entity System
Entity System is built on top of Actor-Component model to move game object definitions from code to json file. Game objects and their components and behaviors are created with json notation. Below is a simple game object which is called `box` with two components. 
`simple_mesh` Component loads the mesh called `box` and make the actor renderable. The second component, `rigid_dynamic` adds rigid dynamic behaviors to the actor.
```JSON
{
  "name": "box",
  "components": [
    {
      "name": "simple_mesh",
      "parameters": {
        "mesh": "box",
        "view_distance": 200.0
      }
    },
    {
      "name": "rigid_dynamic",
      "parameters": {
        "mass": 10.0,
        "collider_materials": {
          "px_cube": "iron"
        }
      }
    }
  ]
}
```
Inheritance is also supported in this notation. [Here](https://github.com/MohammadRB/BlackCat-Engine/blob/9f34c111ca8a9b4e579402633dd40a064b77cd3c/Src/BlackCat.App/App/Content/Data/EntityType.json#L164) an entity which is called `xbot_idle` is defined with a couple of properties. The next two entities are the same entity with overridden properties which assign different render materials to their base entity. They have inherited from `xbot_idle` and defined their own `skinned_mesh` components with one overridden property.
```JSON
{
  "name": "xbot_idle_red",
  "inherit": "xbot_idle",
  "components": [
    {
      "name": "skinned_mesh",
      "parameters": {
        "materials": {
          "Beta_Surface": "red"
        }
      }
    }
  ]
},
{
  "name": "xbot_idle_blue",
  "inherit": "xbot_idle",
  "components": [
    {
      "name": "skinned_mesh",
      "parameters": {
        "materials": {
          "Beta_Surface": "blue"
        }
      }
    }
  ]
},
```
Here is the result of this inheritance which two identical actors only differ in their sub mesh material.

![enter image description here](https://raw.githubusercontent.com/MohammadRB/img/main/BC/xbot-idle.jpg)
#### Network System
The engine network system is a server-client network model which the whole communication is based on a messaging model over UDP sockets. Messages are queued and based on ping time are serialized and sent to remote party. The received messages should be known to the remote party, so all messages must be registered in both server and client.
Messages can be acknowledged and the ones that need acknowledgment are resend periodically until the ack message is received. Out of order and duplicate messages are discarded. Every message is a subclass of `bci_network_message`  type which its core functionality consist of below functions. 
```C++
/**
 * \brief Indicate whether message must be acknowledged by the receiver or not
 * \return
 */
virtual bool need_acknowledgment() const noexcept;

/**
 * \brief After message execution, if acknowledgment is required, this string will be included in acknowledge message
 * \return 
 */
virtual core::bc_string get_acknowledgment_data() const noexcept;

/**
 * \brief Serialize command into json key/value pair which is provided as parameter
 * \param p_context 
 */
void serialize(const bc_network_message_serialization_context& p_context);

/**
 * \brief Deserialize command from json key/value pair which is provided as parameter. 
 * \param p_context
 */
void deserialize(const bc_network_message_deserialization_context& p_context);

/**
 * \brief Execute message logic on the remote part of connection
 * \param p_context
 */
virtual void execute(const bc_network_message_client_context& p_context) noexcept;

/**
 * \brief Execute message logic on the remote part of connection
 * \param p_context
 */
virtual void execute(const bc_network_message_server_context& p_context) noexcept;

/**
 * \brief Execute message logic when message delivery is acknowledged
 * \param p_context 
 */
virtual void acknowledge(const bc_network_message_client_acknowledge_context& p_context) noexcept;

/**
 * \brief Execute message logic when message delivery is acknowledged
 * \param p_context
 */
virtual void acknowledge(const bc_network_message_server_acknowledge_context& p_context) noexcept;
...
```
For `execute` and `achnowledge` two counterpart functions are defined and two different object parameters are passed in. In fact, when the receiver of the message is the server, the server version of `execute` is called and in case of required acknowledgment the `acknowledge` will be called on the sender when the ack message is received. In other case, when the server is the sender, the functions which will be called are the client version of `execute` and server version of `acknowledge`.
Every actor in the scene can be replicated and synced through the network by adding `bc_network_component` to the actor. Network component will register the actor with `bc_network_manager` class and will be serialized and synced automatically. In serialization process every component can override  `load_network_instance` and `write_network_instance` to write/read properties which describe the current state of the object.
This video illustrate engine network system functionality.

[![Multiplayer GamePlay](https://raw.githubusercontent.com/MohammadRB/img/main/BC/box_multiplayer_thumb.png)](https://www.youtube.com/watch?v=Cs4Wk2tWmlo "Multiplayer GamePlay")

#### Skinned Animation 
A wide range of animations are supported by the engine which are powered by ozz-animation library. Complex animations can be created by constructing individual animation jobs and putting them together though animation pipelines. The below animation of the xbot character is created in [this function](https://github.com/MohammadRB/BlackCat-Engine/blob/9f34c111ca8a9b4e579402633dd40a064b77cd3c/Src/BlackCat.App/App/SampleImp/XBot/bcXBotActorController.cpp#L718) by joining multiple animation layers and constructing a single animation pipeline. Animations are queued and executed by multiple threads in a batch model.

[![Character Animation](https://raw.githubusercontent.com/MohammadRB/img/main/BC/bc_animation_thumb.png)](https://www.youtube.com/watch?v=ejBXw3MUgYw "Character Animation")

#### Ragdoll Animations
To add Ragdoll animation to a model, a special version of the model mesh should be created which include mesh colliders and joints with a specific naming convention for each node. This 3d mesh version , which is called mesh collider, can be loaded by `bc_mesh_collider_loader` to add colliders info to the model object.
Below is a XBot mesh with all of its colliders and joins in Blender. 

![Xbot Collider](https://raw.githubusercontent.com/MohammadRB/img/main/BC/xbot-collider.jpg)
Colliders are rendered as wireframe and joints which connect colliders to each other are rendered as small boxes positioned in the right place. By adding `bc_human_ragdoll_component` to an actor, the component will look for these meta info inside the model to construct the Ragdoll data structures. Below is a video to show the Ragdoll functionality inside the engine.

[![Ragdoll Animation](https://raw.githubusercontent.com/MohammadRB/img/main/BC/bc_ragdoll_thumb.png)](https://www.youtube.com/watch?v=lQOtHQVO3vc "Ragdoll Animation")

#### Scripting System
Native functionalities can be easily exposed, to be used by the scripting language of the engine which is JavaScript. For example in BattleOfXbots game to start the server  `server.start(port);` and `server.load_scene("BattleGround");` should be written in the server console. In fact these are JavaScript codes which run in the context of the engine. The code which bind native functionalities for above example is defined as below. 
The code starts with defining functionalities of an object by a builder object. The builder is used to create a prototype object which then is passed to create a script object that has a reference to the native object. Then this object is assigned to a property named `server` on the global object in JavaScript.
```C++
void bx_server_application::bind(platform::bc_script_context& p_context, platform::bc_script_global_prototype_builder& p_global_prototype, bx_server_application& p_instance)
{
	{
		platform::bc_script_context::scope l_scope(p_context);

		auto l_server_prototype_builder = p_context.create_prototype_builder<bx_server_script>();
		l_server_prototype_builder
			.function(L"start", &bx_server_script::start)
			.function(L"load_scene", &bx_server_script::load_scene)
			.function(L"say", &bx_server_script::say);

		auto l_server_prototype = p_context.create_prototype(l_server_prototype_builder);
		const auto l_server_object = p_context.create_object(l_server_prototype, bx_server_script(*p_instance.m_game_system, p_instance, p_instance));
		
		p_instance.m_server_script_context = &p_context;
		p_instance.m_server_script_object.reset(l_server_object);

		platform::bc_script_property_descriptor<platform::bc_script_object> l_server_property(&p_instance.m_server_script_object.get(), false);
		p_global_prototype.property(L"server", l_server_property);
	}
}
```
#### Visual Effects
A couple of rendering effects are currently implemented in the engine which Include **Tile-Based Differed Rendering**,  **Differed Decal Rendering**, **Vegetable animations**, **Cascaded shadow maps**, **GPU based Particle System**, **Tessellated Terrain System**.

##### Deferred Shading
[![Deferred Shading](https://raw.githubusercontent.com/MohammadRB/img/main/BC/bc_deferred_rendering_thumb.png)](https://www.youtube.com/watch?v=WLmt7279mX0 "Deferred Shading")

##### Particle System
[![Particle System](https://raw.githubusercontent.com/MohammadRB/img/main/BC/bc_particle_thumb.png)](https://www.youtube.com/watch?v=F4s30E_KNCY "Particle System")

##### Vegetable Animation
[![Vegetable Animation](https://raw.githubusercontent.com/MohammadRB/img/main/BC/bc_veg_animation_thumb.png)](https://www.youtube.com/watch?v=DlO1ING1-Yg "Vegetable Animation")

##### Deferred Decal Rendering
[![Decal Rendering](https://raw.githubusercontent.com/MohammadRB/img/main/BC/bc_decal_thumb.png)](https://www.youtube.com/watch?v=UeFoA-HHYMU "Decal Rendering")

##### Terrain Engine
[![Terrain](https://raw.githubusercontent.com/MohammadRB/img/main/BC/bc_terrain_thumb.png)](https://www.youtube.com/watch?v=rarZNjdYU3g "Terrain")

## Workspace directory
 - **3rdParty/** 3rdParty libraries.
 - **Solution/**  Development IDE solutions.
 - **Src/** Engine source code. The source code is separated into physical like folders. Below is source code structure based on build dependencies.
    - **BlackCat.Core.Platform/** Platform specific functionalities required by `BlackCat.Core` modules.
	 - **BlackCat.Core.Platform.Win32/** Windows implementation of `BlackCat.Core.Platform` module.
	 - **BlackCat.Core/** Core functionalities of the Engine.
	 - **BlackCat.Platform/** Platform specific functionalities.
	 - **BlackCat.Platform.Win32/** Windows implementation of `BlackCat.Platform` module.
	 - **BlackCat.Graphics/** Graphic API abstractions.
	 - **BlackCat.Graphics.DirectX11/** DirectX 11 implementation of `BlackCat.Graphics` module.
	 - **BlackCat.Physics/** Physics functionality abstractions.
	 - **BlackCat.Physics.PhysX/** [PhysX](https://developer.nvidia.com/physx-sdk) implementation of `BlackCat.Physics` module.
	 - **BlackCat.Sound/** Sound functionality abstractions.
	 - **BlackCat.Sound.Fmod/** [Fmod](https://www.fmod.com/) implementation of `BlackCat.Sound` module.
	 - **BlackCat.Game/** Contains main functionalities of the engine including animation, input, network, physics, rendering, scripting and sound systems and game play support systems.
	 - **BlackCat.App/** Engine default implementations including `content loaders` and `render passes`.
	 - **BlackCat.Editor/** Engine editor.
- **BattleOfXbots/** A complete third-person multiplayer shooter game developed with the engine with 3 executable projects in following folders.
	- **Box/** Game client version.
	- **Box.Server/** Game server version.
	- **Box.Editor/** Game editor.

