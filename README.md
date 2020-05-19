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

## Demo
![Spellfire Demo](data/demo.gif)

## How to Start
If you just want to get straight to the game, place `Spellfire.exe` in the same directory as the `data` and `shader` folders. Then simply run it to play. If you wanna compile from source, follow the instructions below.

## Controls
Player Movement - *W, A, S, D*

Camera Movement - *Mouse*

Shoot Fireball - *Left Mouse Button*

Restart Game - *R*

Toggle Fullscreen - *F*

Toggle Mouse Visibility - *Tab*

Quit Game - *Escape Key*


## Compilation Instructions
**Windows**
Create a Visual Studio Project and add all the `.h` and `.cpp` files. Make sure to follow the instructions below to install OpengGL, glew, GLFW, and irrKlang. Make sure you are in x86 mode. To compile hit the `Local Windows Debugger` button and enjoy. 

**Mac / Linux**
In theory this program should compile if all of the files are compiled and linked, 
however we did no testing on these operating systems so we make no guarantees about 
compiling this project for them.

## Installing OpenGL
**Windows**
1. The OpenGL library `opengl32.lib` comes with the Microsoft SDK which is
installed by default when you install Visual Studios. 
2. Add `opengl32.lib` in Visual Studios to `Project->Linker->Input->Additional Dependencies`. Make sure you are in x86 mode.

## Installing GLEW
**Windows**
1. Go to the [glew website](http://glew.sourceforge.net/) and download/extract the 32-bit Windows binaries.
2. Copy the `include\GL` folder into your Visual Studio include path. Usually it's `C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\VS\include`
3. Copy the `.lib` files from `\lib\Release\Win32` into your Visual Studio library path. `C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\VS\lib\x86`
4. Copy the `.dll` file from `\bin\Release\Win32` to your Windows runtime directory. Usually that's `C:\Windows\System32`
5. Add `glew32s.lib` in Visual Studios to `Project->Linker->Input->Additional Dependencies`. Make sure you are in x86 mode.

## Installing GLFW
**Windows**
1. Go to the [GLFW website](https://www.glfw.org/download.html) and download/extract the 32-bit Windows binaries.
2. Copy the `include\GLFW` folder into your Visual Studio include path. Usually it's `C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\VS\include`
3. Copy the two `.lib` files from the `lib-vc` folder corresponding to your version of Visual Studios into your Visual Studio library path. Usually it's `C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\VS\lib\x86`
4. Copy the `.dll` file to your Windows runtime directory. Usually that's `C:\Windows\System32`
5. Add `glfw3.lib` in Visual Studios to `Project->Linker->Input->Additional Dependencies`. Make sure you are in x86 mode.

## Installing IrrKlang
**Windows**
1. Go to the [irrKlang website](https://www.ambiera.com/irrklang/downloads.html) and download/extract 32 bit binaries of for v1.6
2. Copy the files in the `include` folder into your Visual Studio include path. Usually it's `C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\VS\include\irrKlang`. *Note: You will have to create the irrKlang folder*.
3. Copy the `.lib` file from the `lib\Win32-visualStudio` folder into your Visual Studio library path. `C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\VS\lib\x86`
4. The `.dll` files should already be in the git repo. Make sure they are in the same folder as the .exe file.
5. Add `irrKlang.lib` in Visual Studios to `Project->Linker->Input->Additional Dependencies`. Make sure you are in x86 mode.

## Credit
Music and effects from https://www.bensound.com and https://www.zapsplat.com/music/

## Todo
- **Develop Enemies**
	- ~~Get enemy PLY models~~
	- Add death animation (Fall over instead of disappear)
	- Apply textures
	- Add movement animations (Basic is up and down bobbing)
	- Make different types of enemies
		- Archers
		- Enemy mages 
	- Tweek enemy stats


- **Scenery**
	- ~~Implement diffuse lighting~~
	- ~~Add sky textures~~
	- Implement ambient lighting
	- Implement specular lighting


- **Game Mechanics**
	- ~~Add collision between player and enemy~~
	- ~~Allow player to shoot projectile along long vector with click~~
	- ~~Add collision between enemy and projectile~~
	- ~~Make different enemies~~
	- ~~Add maximum mana~~
	- ~~Add pickups~~
	- ~~Add points for player~~
	- ~~Add mana restricton for player~~
	- Add sounds
	 	- Add start menu music
	 	- Add pause screen music
	 	- Add click sounds to buttons
	 	- ~~Add loading screen music~~	
		- ~~Add music over game~~
		- ~~Add enemy sound when hit for each enemy~~
		- ~~Add enemy call sound for each enemy~~
		- ~~Add spell cast sound for each spell~~
		- ~~Add death screen music~~
		- ~~Add potion pickup sound~~
		- ~~Add player damage sound~~
	- Improve pickups
		- ~~Add duration~~
		- ~~Add animation~~
		- Add flashing when they are about to despawn
		- Add more potion types
			- Exp potion
			- Invisibility potion
			- Time stop potion
	- Add particle effects to projectiles
	- Add a staff or hand for the player
	- Add movement bob up and down when walking
	- Improve collisions so players and enemies not allowed to walk through each other
	- Implement progressive difficulty (maybe through waves of enemies?)
	- Improve player stats when leveling up
	- Add spell selection
		- Add ice bolt spell
		- Add blink teleport spell
		- Add thunderwave spell (Close quarters AOE)


- **Code improvement**
	- ~~Switch over from FLTK to GLFW for better window (Makes for smoother gameplay)~~
	- ~~Improve enemy storage~~
	- ~~Improve the textured and untextured sprite difference~~
	- ~~Stagger the loading of shaders and PLY to avoid blackscreen (Seems like the lag comes from loading skeleton and arena ppms so put up loading screen before that)~~


- **GUI**
	- ~~Add crosshair~~
	- ~~Health bar~~
	- ~~Figure out best way to add text to screen (Through textures)~~
	- ~~Add a restart button~~
	- ~~Cursors change while over buttons~~

	- Add a start menu
	- Add a pause screen
	- Add collectibles menu
	- Add button to show controls list
	- Add options menu
		- Add option to let user change graphics setting (Low, medium, high texture)
		- Add option to change mouse sensitivity
		- Add option to change brightness
		- Add option to change sound volume
	- Add a way to save options locally (Maybe read a file, if empty use default values)
	- Add cool cursors

- **Story**
	- Add plot
	- Add collectibles

