Pixel Dust Game Engine Node Plugin
==================================

The Pixel Dust Game Engine (PDG) is an open source, professional quality 
2D game engine. It provides system independent abstractions of common 
elements needed by game developers. 

This is the node.js plugin portion of it for game servers. 


Features
--------
- integrated with Node.js
- high performance Javascript via Google's V8 engine
- event driven
- cross platform
- 2D OpenGL hardware accelerated graphics
- resource and file management
- user input handling
- timers
- networking
- sound
- animation including jQuery-like easing functions
- sprites
- work directly with Spriter files
- tile based maps
- physics support using Chipmunk Physics
- works with node inspector for Javascript debugging
- interactive Javascript console mode


Design Goals
------------

PDG is focused on making a broad array of capabilities available to developers 
regardless of the OS platform they are targeting. Currently it works on Mac OS X, 
Windows, and iOS. The server side Node.js plugin works on Linux and Mac OS X.

The PDG API is designed to make simple things easy to do, without adding undue 
complexity to more complicated problems.


Node Plugin
-----------

The node plugin version is aimed at game servers, so it leaves out the graphics
and sound systems. However, it still includes all the Sprite and Physics capabilities
for server side simulation.


Documentation
-------------

Man Pages are included, and more API documentation can be found at:

http://ezavada.com/pdg/javascript/html/

