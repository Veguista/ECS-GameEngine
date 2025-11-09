# ECS GameEngine Mini
A compact 2D Game Engine that implements the Entity Component System (ECS) architecture. The engine's components and configuration can be serialized using the pugixml library, allowing users to alter its levels, components, and memory configuration without needing to recompile.
The project contains a Game Demo for a Pang clone developed using the engine.

# In-Depth Explanation
The engine was built with 3 main goals in mind:
- Implement the Entity Component System (ECS) architecture.
- Reduce its memory usage to a minimum, prioritizing Memory over Performance.
- Create systems to control the engine without recompiling the source code.

The memory constraint immediately affected the engine's architecture. In more basic ECS implementations, memory was reserved for every possible entity and component combination. However, I wanted to be efficient and provide developers with more granular memory control. The solution was having multiple Entity Pools:

An Entity Pool is an array of Entities that share some characteristics. Other engines have only one Entity Pool, and thus every entity can have every Component. This can lead to memory waste or performance hitches due to memory reallocation. My engine allows developers to predefine multiple Entity Pools, each with a selection of allowed components. For example, a user can create a bullet entity pool whose entities can have rendering, collider, and/or bullet components, but not an Animator Component, which greatly optimizes memory usage.

To enable user control without recompilation, I implemented the pugixml library, which allowed me to serialize Levels, Components, and even Entity Pool Configurations. This way, users could create Prefabs of entities, use those prefabs in levels, and make sure that every prefab belonged to a specific Entity Pool, all without recompiling the source code and while using .xml files as a storage method.
