# SpellFire 
##### By Kevin Naranjo and Will Rusk

Spellfire is a 3D arena-figher game.  You are placed in the middle of an inescapable
arena and surrounded by bloodthirsty monsters.  Your only hope for survival is
to use your trusty fireball spell.  Defeat enemies to gain points and collect
items to restore your stats.  See how long you can last!

The majority of the focus of SpellFire was on rendering all aspects of the game
using GLSL shaders and VBOs.  We modified the ply reader to parse normal data and 
texture coordinates from ply models we exported from 3D modeling tools (along with
their textures).  

## Controls
Player Movement - *W, A, S, D*

Camera Movement - *Mouse (__Left click to toggle camera movement__)*

Debug Hitbox Toggle - *M*

Quit Game - *Escape Key*


## Todo
- **Develop Enemies**
	- ~~Get enemy PLY models~~
	- Apply textures


- **Scenery**
	- ~~Implement the complete Phong lighting model (Ambient, Diffuse, Specular)~~
	- ~~Add sky textures~~


- **Game Mechanics**
	- ~~Add collision between player and enemy~~
	- ~~Allow player to shoot projectile along long vector with click~~
	- ~~Add collision between enemy and projectile~~
	- Add sounds
	- ~~Add points for player~~
	- ~~Add mana restricton for player~~
	- Add spell selection
		- Add ice bolt spell
	- ~~Make different enemies~~
	- ~~Add maximum mana~~
	


- **Code improvement**
	- Switch over from FLTK to GLFW for window (Makes for smoother gameplay)

- **GUI**
	- ~~Add crosshair~~
	- ~~Health bar~~