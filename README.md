# GCode Vision – GCode-Based 3D printer Visualizer (C++ / OpenGL)

This project is a real-time 3D printing simulation based on GCode parsing. It is implemented in C++ using OpenGL for rendering and Crow as a lightweight HTTP server. The application simulates the 3D printing process, visualizing filament deposition based on GCode instructions. It includes a complete 3D model of the printer and offers runtime configuration through a REST API.

## Overview

The simulation runs on two separate threads:
- The first handles GCode parsing and a REST API server (built with Crow).
- The second runs the OpenGL rendering loop responsible for the simulation display.

The project includes a custom rendering engine with a scene graph system, allowing hierarchical transformations (parent-child structure) similar to standard game engines. All models, including the printer and printed object, are rendered using GLSL shaders, including support for the Phong lighting model.

## Key Features

- **GCode Parsing**
  - Supports movement and extrusion commands (e.g., G0, G1).
  - Converts GCode into motion paths and extrusion data.

- **REST API (Crow)**
  - Exposes endpoints for controlling simulation parameters.
  - Allows configuration of speed, camera, step-by-step playback, etc.
  - Designed to run in a separate thread from the rendering engine.

- **Custom OpenGL Engine**
  - Built from scratch using OpenGL 3.3+ and GLSL.
  - Supports 3D model loading, transformations, lighting, and real-time updates.
  - Scene graph system allows complex object hierarchies with local/global transforms.

- **Shader-Based Rendering**
  - Custom shaders using the Phong lighting model.
  - Real-time rendering of the extruded filament and printer components.

- **Simulation Control**
  - Full runtime control via REST API (start, pause, step).
  - Adjustable print speed and camera position.

## Technologies Used

- C++17
- OpenGL 3.3+
- GLSL (for shaders)
- Crow (HTTP server for REST API)
- GLFW (window/context management)
- GLM (math library for graphics)

## Project Structure

- `opengl/` – Rendering engine, scene graph, camera system, shaders etc.
- `rest/` – REST API server using Crow.
- `gcode/` – GCode parsing logic.
- `main.cpp` – Entry point, thread handling, initialization.

## Installation Guide (Windows – Visual Studio + vcpkg)

This project is intended to be built using Visual Studio (MSVC) on Windows, with dependency management handled via vcpkg.
There is no need for CMake or manual Makefiles – all dependencies are managed and integrated automatically through Visual Studio.

### Step 1: Download and install Visual Studio

1. Download and install **Visual Studio**.
2. During installation, select **Desktop development with C++**.

### Step 2: Install vcpkg

#### Clone the vcpkg Repository

Open **Command Prompt** and run the following command:

```sh
git clone https://github.com/microsoft/vcpkg.git
```

#### Bootstrap vcpkg

Navigate to the cloned vcpkg directory and run the bootstrap script:

```sh
cd .\vcpkg
bootstrap-vcpkg.bat
```

This will build vcpkg and set up the necessary environment.

### Step 3: Integrate vcpkg with Visual Studio

Run the following command to integrate vcpkg with Visual Studio:

```sh
vcpkg integrate install
```

This allows Visual Studio to automatically detect and use libraries installed via vcpkg.
