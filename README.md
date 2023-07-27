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
  - `clang` and `clang++` (at least `15.0.0`) is included in the system's PATH environment variable.

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
Open the `CG2023.sln` file (inside the `build/vs` directory) with Visual Studio and build the solution. Alternatively 
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