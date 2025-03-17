# gcode-vision

## Instalation guide

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
