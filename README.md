# Fluent Behaviour Tree
It's a C++ implementation of Fluent Behaviour Tree from https://github.com/codecapers/Fluent-Behaviour-Tree.

## How to use
It's a header-only library so just put `fluent-behaviour-tree.hpp` into your `.cpp` file.
```cpp
#include "include/fluent-behaviour-tree.hpp"
```
FBT has its own namespace.
```cpp
using namespace smd::fbt;
```
Next define what types are using in the code.
```cpp
using timeData_t = float;
using nodePtr_t  = shared_ptr< node_i< timeData_t > >;
using builder_t  = builder_c< timeData_t >;
```
Do some defines to make our code more clarity.
```cpp
#define DO( name, code )        .mDo( name, []( timeData_t ) code )
#define CONDITION( name, code ) .mCondition( name, []( timeData_t timeData ) code )
#define SEQUENCE( name )        .mSequence( name )
#define PARALLEL( name )        .mParallel( name, 0, 0 )
#define END                     .mEnd()
#define BUILD                   .mBuild()
```
Create FBT with app-logic at startup.
```cpp
nodePtr_t startUp( void ) {
    builder_t builder;

    nodePtr_t pTree {
        builder
            SEQUENCE( "My-Sequence" )
                DO( "Action-1", {
                    cout << "Do Action-1" << endl;
                    return status_t::SUCCESS;
                } )
                DO( "Action-2", {
                    cout << "Do Action-2" << endl;
                    return status_t::SUCCESS;
                } )

                PARALLEL( "Make Parallel" )
                    SEQUENCE( "Seq1" )
                        CONDITION( "Condition-for-Action-3", {
                            return timeData < 0.2f;
                        } )
                        DO( "Action-3", {
                            cout << "Do Action-3" << endl;
                            return status_t::SUCCESS;
                        } )
                    END

                    SEQUENCE( "Seq2" )
                        CONDITION( "Condition-for-Action-4", {
                            return timeData > 0.2f;
                        } )
                        DO( "Action-4", {
                            cout << "Do Action-4" << endl;
                            return status_t::SUCCESS;
                        } )
                    END
                END
            END
            BUILD
    };

    return pTree;
}
```
The update function may invokes in the loops or many times when app running.
```cpp
void update( nodePtr_t pTree, timeData_t timeData ) {
    pTree->mTick( timeData );
}
```
In this example we called the update function two times with different arguments. In the main function create the tree and update it.
```cpp
nodePtr_t pTree { startUp() };

update( pTree, 0.1f );
update( pTree, 0.3f );
```
The output should be as follows.
```
Do Action-1
Do Action-2
Do Action-3
Do Action-1
Do Action-2
Do Action-4
```
You can see this example in details at `examples/main.cpp`.

## How to build and run Sanity-check
### Clone the repo
```bash
git clone https://github.com/sam0delk1n/fluent-behaviour-tree.git
cd fluent-behaviour-tree
```

### Linux
Create `build/Debug/` and `build/Release/` folders in the repo root folder.
```bash
mkdir build
cd build
mkdir Debug Release
```
Generate `Makefile` with Debug options and build it. You need CMake 3.10.2 and GCC-7/g++-7.
```bash
cd Debug
cmake -DCMAKE_BUILD_TYPE=Debug ../..
make
```
Run the test.
```bash
./fbt-check
```
Then do the same for Release.
```bash
cd ../Release
cmake -DCMAKE_BUILD_TYPE=Release ../..
make
./fbt-check
```

### Linux with Docker
What is Docker please read here: https://docs.docker.com/.  

If you don't have required compilers or tools, you can build and run this project with Docker. Before start, please delete `build` folder if you've already create it.
```bash
docker build -t fbt-check .
```
Docker downloads Linux Bionic image with GCC-7/g++-7 and CMake that are already installed. Then Docker builds the project into `/home/app/build/Debug/` and `home/app/build/Release/` (inside the image) respectively and containerizes into the image with name `fbt-check`. Now you can run FBT tests.
```bash
docker run fbt-check /home/app/build/Debug/fbt-check
docker run fbt-check /home/app/build/Release/fbt-check
```

### Windows and MS Visual Studio 2017
Create `build\msvs\` in the repo root folder.
```bat
mkdir build\msvs
```
Generate `Project.sln`. You need CMake 3.10.2 and MSVS 2017.
```bat
cd build\msvs
cmake -G"Visual Studio 15 2017 Win64" ..\..
```
Now you can open `Project.sln` in MSVS and build Debug, Relese, etc.  
The project named `fbt-check` set as StartUp Project to run in Studio. Or run `fbt-check.exe` in the terminal window and you can see the program's output.

### Windows and MinGW-W64
Create `build\mingw\Debug\` and `build\mingw\Release\` in the repo root folder.
```bat
mkdir build\mingw\Debug build\mingw\Release
```
Generate `Makefile` with Debug options and build it. You need CMake 3.10.2 and MinGW-W64 with GCC-7/g++-7.
```bat
cd build\mingw\Debug
cmake -G"MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug ..\..\..
mingw32-make
```
Run the test.
```bat
fbt-check
```
Then do the same for Release.
```bat
cd ..\Release
cmake -G"MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..\..\..
mingw32-make
fbt-check
```
