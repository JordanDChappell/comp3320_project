# Farm-Life

Farm-Life is the beginnings of a base for an open world game. It's aim is to be visually impressive while running in real time on gaming computers. This was created for the Univerity of Newcastle COMP3320 course group project in 2019, semester 2.

## How to Run

Open the solution using Visual Studio 2019 and run.

Must have up-to-date GPU drivers which have OpenGL functionality.

Must have at least Windows 32 bit.

## Gameplay

The aim of the game is to find the cat, named William, and put him back in his pen. Do this by finding him and pressing `g`. He will teleport around randomly five times before going into his pen.

Pressing `t` will reset the day/night cycle.

Pressing `SHIFT+\` ` will disable gravity and collision.

## Features

- Phong lighting for models, water and terrain.

- Procedurally generated terrain. Textures determined at run time. Height based on a height map. Trees generated based on image map.

- Water, with reflection and refraction. Uses a du/dv (bump) map. Created using a [tutorial on YouTube by ThinMatrix](https://www.youtube.com/watch?v=HusvGeEDU_U&list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh).

- Grass, made using cards that are rotated to match the terrain. Coloured with a texture. Rotated to give the appearance of grass moving in the wind. Optimised by randomly clipping grass, and clipping any grass that is far away from the camera. Based off a tutorial for waving grass on [Megabyte Softworks](http://www.mbsoftworks.sk/tutorials/opengl3/29-terrain-pt2-waving-grass/).

- Models, made in Blender and imported with ASSIMP.

- Camera/player movement based off [this repo](https://github.com/Bidski/COMP3320) with some added functionality. Added jumping, collision and gravity.

- Audio, implemented with OpenAL using a tutorial by [ThinMatrix on YouTube](https://www.youtube.com/watch?v=BR8KjNkYURk&list=PLRIWtICgwaX2VNpAFjAZdlQw2pA1-5kU8).

# Particle Effects

Opening the ParticleEffects solution will showcase the particle effects in a separate project.

Navigate to the Particle-Effects-Example folder.
Open ParticleSystem.sln, build and run.

# Credits

Music "Acoustic Breeze": https://www.bensound.com
