# Fluent Behaviour Tree
It's a C++ implementation of Fluent Behaviour Tree from https://github.com/codecapers/Fluent-Behaviour-Tree.

### How to use
It's a header-only library so just put `fluent-behaviour-tree.hpp` into your cpp file.
```cpp
#include "include/fluent-behaviour-tree.hpp"

using namespace std;
using namespace smd::fbt;

using timeData_t = float;
using nodePtr_t  = shared_ptr< node_i< timeData_t > >;
using builder_t  = builder_c< timeData_t >;

...

nodePtr_t tree;

void startup( void ) {
    builder_t builder;
  
    tree = builder
        .mSequence( "my-sequence" )
            .mDo( "action1", []( timeData_t ) {
                // Action 1.
                return status_t::SUCCESS;
            } )
            .mDo( "action2", []( timeData_t ) {
                // Action 2.
                return status_t::SUCCESS;
            } )
        .mEnd()
        .mBuild();
}

...

void update( timeData_t time ) {
    tree.mTick( 0.1f );
}
```

### How to build sanity-checks
Clone the repository.
```bash
git clone https://github.com/sam0delk1n/fluent-behaviour-tree.git
```
Create `build/Debug` and `build/Release` folders in the repository root folder.
```bash
cd fluent-behaviour-tree
mkdir build
cd build
mkdir Debug Release
```
Build Debug version. You need CMake 3.10.2 and GCC-7/g++-7.
```bash
cd Debug
cmake -DCMAKE_BUILD_TYPE=Debug ../..
make
```
Run test.
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
