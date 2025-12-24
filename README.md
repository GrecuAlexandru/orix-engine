# Orix Engine

A high-performance voxel engine built with C++20, OpenGL, and SDL2, featuring Steamworks integration.

---

## 🚀 Quick Start

Follow these steps to build and run the engine on Windows.

### 1. Install Dependencies
Ensure you have [vcpkg](https://vcpkg.io) installed. Run the following to install required libraries:

```powershell
vcpkg install sdl2:x64-windows
```

### 2. Configure CMake
Initialize the build system and point it to your vcpkg toolchain:

```powershell
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=C:/Users/Alex/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
```

### 3. Build the Project
Compile the source code into an executable:

```powershell
cmake --build build --config Release
```

### 4. Run
Before running, ensure **Steam** is open. Create a `steam_appid.txt` file in the same directory as the executable with the content `480` (SpaceWar test ID), or run it directly:

```powershell
./build/Release/BlockGame.exe
```

---

## 🛠 Tech Stack
- **Language:** C++20
- **Graphics:** OpenGL
- **Windowing:** SDL2
- **Platform:** Steamworks SDK
- **Build System:** CMake + vcpkg
