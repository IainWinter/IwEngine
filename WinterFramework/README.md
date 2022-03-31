I think it would be cool to split the engine from the framework. There is a confusion between tools / data structures and what makes use of those data structures.
For example, the System/Application layout vs the PhysicsSystem/EntityCleanupSystem ect...

This header should be mainly header only, except for optional extensions like FMODAudio.

Focusing on making it as small and understandable as possible. I have a feeling that each of the libraries from before could just be a single file.
Most of the functionality gets included no matter what, so a single file makes it simpler to use. Also, a single file lets you know the level of abstraction as you get deeper
into the file as things need to be defined before they can be used. Looking at a folder of many files does not give you this info at a glance.

| Pieces of framework| Description |
| --- | --- |
| Audio| Mainly just an interface to talks to an underlying audio engine through handles. |
| Event | An events system based on composing std::functions to member functions. |
| Entity |An archetypal entity component system. Uses hashes to store components and std::function to store type information. My main goal was to store everything related in types in concrete classes to minimize the use of templates. This allows much more flexibility at runtime. |
|more coming|..|
