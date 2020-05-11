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
Throw everything into a visual studio project. Make sure you have GLEW and GLFW installed. Ensure that the data and shader directory is
accessible from the executable directory and you're good to go!
**Mac / Linux**
In theory this program should compile if all of the files are compiled and linked, 
however we did no testing on these operating systems so we make no guarantees about 
compiling this project for them.

## Installing GLEW
**Windows**
1. Go to the [glew website](http://glew.sourceforge.net/) and download/extract the 32-bit Windows binaries.
2. Copy the `include\GL` folder into your Visual Studio include path. Usually it's `C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\VS\include`
3. Copy the `.lib` files from `\lib\Release\Win32` into your Visual Studio library path. `C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\VS\lib\x86`
4. Copy the `.dll` file from `\bin\Release\Win32` to your Windows runtime directory. Usually that's `C:\Windows\System32`
5. Add `glew32s.lib` in Visual Studios to `Project->Linker->Input->Additional Dependencies`

## Installing GLFW
**Windows**
1. Go to the [GLFW website](https://www.glfw.org/download.html) and download/extract the 32-bit Windows binaries.
2. Copy the `include\GLFW` folder into your Visual Studio include path. Usually it's `C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\VS\include`
3. Copy the two `.lib` files from the `lib-vc` folder corresponding to your version of Visual Studios into your Visual Studio library path. Usually it's `C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\VS\lib\x86`
4. Copy the `.dll` file to your Windows runtime directory. Usually that's `C:\Windows\System32`
5. Add `glfw3.lib` in Visual Studios to `Project->Linker->Input->Additional Dependencies`

## Controls
Player Movement - *W, A, S, D*

Camera Movement - *Mouse (__Left click to toggle camera movement__)*

Debug Hitbox Toggle - *M*

Quit Game - *Escape Key*


## Todo
- **Develop Enemies**
	- ~~Get enemy PLY models~~
	- Add enemy death animation (Fall over)
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