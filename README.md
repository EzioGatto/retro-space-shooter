### Retro-space-shooter
It's a simple and lightweight 2D retro space shooting game that runs in the terminal and written entirely in **C**.This project brings back the nostalgic retro game vibe. With 10 levels and 5 different types of enemies this game is simple but endless. It has no save file option staying true to it's hardcore game vibes. Once closed or quitted the user or rather player loses all progress.

This code is specially written for **Linux/MacOS** so the .c file will not be compiled on a Windows system. Read how to run on Windows.


## Features
**1. Smooth Terminal Rendering:** Uses ANSI escape codes for quick screen updates and double-buffered rendering to reduce flicker.

**2. Dynamic Difficulty:** Game difficulty scales automatically with the score, capping at level 10 with rapid enemy spawn rates.

**3. Multiple Enemy Types:** 5 distinct enemy variants (O, T, V, X, Y), each awarding different point values based on difficulty (1,2,3,4,5).

**4. In-Game HUD:** Track Health Points (HP), Score, and Level in real-time at the bottom of the screen.


## How to Play
**1. Basic Controls:** Commonly used WASD for movements.

**2. Spacebar:** Shoot Bullet (|)

**3. Quit Game:** pressing Q will end the program anytime.

**4. Retry:** On the "Game Over" screen, pressing R will restart the game.


## Game Mechanics
**1.** Player is represented by A.

**2.** Every enemy that reaches the bottom of the screen or collides with player disappears.

**3.** Colliding with an enemy costs 10 HP. The game ends when HP drops to 0.

**4.** Every 10 points advances player to the next level, increasing the enemy spawn rate.


## How to run on Windows
 
This program was written for **Linux/MacOS** specifically because of `termios.h` and `sys/select.h` — these headers **do not exist on Windows**, so it will not compile with a plain Windows C compiler (like MSVC) without changes. 
 
# The easiest and recomended: Using WSL — Windows Subsystem for Linux
This runs a real Linux environment inside Windows, so the code compiles completely unmodified.
 
1. Open **PowerShell as Administrator** and run:
```
   wsl --install
```
2. Restart the coputer when prompted.
3. On first launch, WSL will finish installing Ubuntu and will ask to create a Linux username/password. Finish the setup.
4. On Linux terminal prompt, install a C compiler:
```
   sudo apt update
   sudo apt install gcc -y
```
5. Copy the `retro_space_shooter.c` file into the Linux filesystem. The Windows `C:\` drive is visible inside WSL at `/mnt/c/`. So if the file is on the user's Windows Desktop, the file can be reached at something like:
```
   /mnt/c/Users/<User>/Desktop/retro_space_shooter.c
```
6. Compile it:
```
   gcc retro_space_shooter.c -o retro_space_shooter
```
7. Run it:
```
   ./retro_space_shooter
```
