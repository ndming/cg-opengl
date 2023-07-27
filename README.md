# Computer Graphics Assignment
**Author**: Minh Nguyen  
**Student ID**: 1952092

## Build Instruction
Requires:
- CMake: `3.25` or higher.
- Either: 
  - Ninja (recommended): `1.11.1` or higher.
  - Visual Studio: `17 2022` or higher.
  - Make: `4.3` or higher.
- If not using Visual Studio, make sure either: 
  - `g++` and `gcc` (at least `12.2.0`) is included in the system's PATH environment variable.
  - `clang` and `clang++` (at least `15.0.0`) is included in the system's PATH environment variable 
  (note that on Windows `LLVM` relies on dependencies from Visual Studio)

Inside the directory `cg-opengl`, execute the following commands:

### With Ninja
```commandline
cmake -G Ninja -B build/ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/ninja --target CG2023 -j 10
```
By default, CMake chooses the GNU compilers to integrate with the build. To compile with `LLVM` compilers, run this 
command instead:
```commandline
cmake -G Ninja -B build/ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang
```
When done, run the executable `CG2023` produced by the build to run the program.

### With Visual Studio
```commandline
cmake -G "Visual Studio 17 2022" -B build/vs -DCMAKE_BUILD_TYPE=Release
```
Open the `cg_opengl.sln` file (inside the `build/vs` directory) with Visual Studio and build the solution. Alternatively 
the build can be invoked indirectly through CMake:
```commandline
cmake --build build/vs --target CG2023 --config Release
```
When done, run the executable `CG2023` inside the `Release` directory to run the program.

### With Make
```commandline
cmake -G "Unix Makefiles" -B build/make -DCMAKE_BUILD_TYPE=Release
cmake --build build/make --target CG2023 -j 10
```
By default, CMake chooses the GNU compilers to integrate with the build. To compile with `LLVM` compilers, run this 
command instead:
```commandline
cmake -G "Unix Makefiles" -B build/make -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang
```
When done, run the executable `CG2023` produced by the build to run the program.

## Stochastic Gradient Descent
![](https://hackmd.io/_uploads/SJb5-ETq3.png)

The SGD demonstration employs two render windows (`view`s). The left `view` renders the main SGD visualization on 
3D scene, while the right `contourView` renders the contour (heat map) of the descending process.

**Features and key bindings:**
- `R`: randomize a position of the ball on the mesh
- `SPACE`: hold (or press) to descend the ball to a local minimum
- `LFT MOUSE`: click on the contour map to select the position of the ball
- `MID MOUSE`: scroll on the left `view` to zoom in/out the 3D scene
- `LFT MOUSE`: drag on the left `view` to change the camera angle on both scenes

The descent process uses **plain derivatives** from the objective function and a **convergence rate**. By default, the mesh is rendered with the following objective function:

$$
z = f(x, y) = \frac{x^2 + y^2}{40} + 2\exp({\cos{\frac x2} + \frac{y^2}{4}}) - \cos{\frac{x}{1.5}} - \sin{\frac{y}{1.5}}
$$

Gradient with respect to $x$:

$$
f_x(x, y) = \frac{x}{20} + \exp({-\cos{\frac x2} + \frac{y^2}{4}})*\sin{\frac{x}{2}} + \frac{1}{1.5}\sin{\frac{x}{1.5}}
$$

Gradient with respect to $y$:

$$
f_y(x, y) = \frac{y}{20} + \exp({-\cos{\frac x2} + \frac{y^2}{4}})*y - \frac{1}{1.5}\cos{\frac{y}{1.5}}
$$