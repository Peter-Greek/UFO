# UFO — A 2D Sci-Fi Survival Game

## **Overview**

**UFO** is a 2D top-down sci-fi survival game developed as part of the **CSCI-437** final project at William & Mary. Built using **C++** and **SDL2**, UFO challenges players to explore, survive, and escape from a hostile alien planet. The project emphasizes component-based design, event-driven architecture, and full lifecycle game development — from design to implementation, testing, and presentation.

This game was created by a team of 4 students over the course of the semester, with each member contributing equally to all areas: **design**, **coding**, **art**, **testing**, and **documentation**.

---

## **Game Description**

In **UFO**, you play as a stranded **U**ndercover **F**erret **O**pperative who must navigate an alien spaceship filled with enemies, environmental hazards, and limited resources. The ultimate goal is to **gather enough AT (Alien Technology)** to **escape** the ship, all while managing oxygen, avoiding enemies, and strategically upgrading your gear.

### **Core Features**
- Top-down 2D gameplay using **SDL2** and **C++**
- Modular **Entity-Component System** architecture
- Fully implemented **ChatBox** system for command-line interactions
- Upgrade system with abilities like **shield**, **invisibility**, **AT cannon**
- **Boss battles** with unique minion and projectile mechanics
- Save/load functionality with a custom **SaveSelector UI**
- Configurable settings through the **SettingsMenu**
- **LeaderboardMenu** with multiple sorting criteria
- Visual assets loaded via **TxdLoader** and **AsepriteLoader**
- Audio layered via **AudioLoader**

---

## **Project Structure**

- [`bin/`](./bin) — Main compiled executable  
- [`cmake/`](./cmake) — CMake build configuration  
- [`include/`](./include) — Header files  
- [`resource/`](./resource) — Game assets (textures, audio, fonts, etc.)  
- [`src/`](./src) — All game logic, entities, systems, and UI code


---

## **Build Instructions**

### **Dependencies**

Install the following:

- GCC or MinGW (Windows)
- CMake
- SDL2 libraries: `SDL2`, `SDL2_image`, `SDL2_gfx`, `SDL2_ttf`, `SDL2_mixer`

### **Linux**

Use your package manager to install the dependencies (e.g. `apt`, `dnf`, `pacman`). Then:

```bash
git clone https://github.com/Peter-Greek/UFO.git
cd UFO
mkdir Debug && cd Debug
cmake ..
make
```

### **Windows (MSYS2)**

Install [MSYS2](https://www.msys2.org/) and use the `MINGW64` terminal:

```bash
pacman -S mingw-w64-x86_64-toolchain cmake git \
  mingw-w64-x86_64-SDL2 \
  mingw-w64-x86_64-SDL2_image \
  mingw-w64-x86_64-SDL2_gfx \
  mingw-w64-x86_64-SDL2_ttf \
  mingw-w64-x86_64-SDL2_mixer

git clone https://github.com/Peter-Greek/UFO.git
cd UFO
mkdir Debug && cd Debug
cmake.exe -G "MinGW Makefiles" ..
mingw32-make.exe
```

---

## **License**

This project is licensed under the MIT License. See the [LICENSE](./LICENSE) file for details.

---

## **Contact**

For academic use or questions, contact [Peter Greek](https://github.com/Peter-Greek) or open an issue on [GitHub](https://github.com/Peter-Greek/UFO/issues).
