# SpellFire 
##### By Kevin Naranjo and Will Rusk

Spellfire is a 3D arena-figher game.  You are placed in the middle of an inescapable
arena and surrounded by bloodthirsty monsters.  Your only hope for survival is
to use your trusty fireball spell.  Defeat enemies to gain points and collect
items to restore your stats.  See how long you can last!

The majority of the focus of SpellFire was on rendering all aspects of the game
using GLSL shaders and VBOs.  We modified the ply reader to parse normal data and 
texture coordinates from ply models we exported from 3D modeling tools (along with
their textures).  The code refers to COWs and BUNNYs as the two types of enemies; 
that is a placeholder from when we used the cow and bunny ply models as the enemies.

## Compilation Instructions
**Windows**
Throw everything into a visual studio project.  Ensure that the data directory is
accessible from the executable directory and you're good to go!
**Mac / Linux**
In theory this program should compile if all of the files are compiled and linked, 
however we did no testing on these operating systems so we make no guarantees about 
compiling this project for them.

## Controls
Player Movement - *W, A, S, D*

Camera Movement - *Mouse (__Left click to toggle camera movement__)*

Debug Hitbox Toggle - *M*

Quit Game - *Escape Key*


## Todo
- **Develop Enemies**
	- ~~Get enemy PLY models~~
	- Apply textures
	- Add movement animations (Basic is up and down bobbing)
	- Make different types of enemies
		- Archers
		- Enemy mages 


- **Scenery**
	- ~~Implement the complete Phong lighting model (Ambient, Diffuse, Specular)~~
	- ~~Add sky textures~~


- **Game Mechanics**
	- ~~Add collision between player and enemy~~
	- ~~Allow player to shoot projectile along long vector with click~~
	- ~~Add collision between enemy and projectile~~
	- Add sounds
	- Add particle effects to projectiles
	- Add a staff or hand for the player
	- Add movement bob up and down
	- Improve collisions so players and enemies not allowed to walk through each other
	- Implement progressive difficulty 
	- Improve player stats when leveling up
	- ~~Add points for player~~
	- ~~Add mana restricton for player~~
	- Add spell selection
		- Add ice bolt spell
	- ~~Make different enemies~~
	- ~~Add maximum mana~~
	- ~~Add pickups~~
	


- **Code improvement**
	- Switch over from FLTK to GLFW for window (Makes for smoother gameplay)
	- Improve enemy storage
	- Improve the textured and untextured sprite difference

- **GUI**
	- ~~Add crosshair~~
	- ~~Health bar~~
	- Add a restart button
	- Add a start menu
	- Figure out best way to add text to screen