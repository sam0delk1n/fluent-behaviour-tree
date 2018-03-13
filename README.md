# Fluent Behaviour Tree
It's a C++-port from https://github.com/codecapers/Fluent-Behaviour-Tree.

### How to use
It's a header-only library so just put `fluent-behaviour-tree.hpp` into your cpp file.
```cpp
#include "include/fluent-behaviour-tree.hpp"
...
// Write your code here.
```

### How to build sanity-checks
```bash
git clone https://github.com/sam0delk1n/fluent-behaviour-tree.git
cd fluent-behaviour-tree
mkdir build
cd build
mkdir Debug Release
cd Debug
cmake -DCMAKE_BUILD_TYPE=Debug ../..
make
./fbt-check
cd ../Release
cmake -DCMAKE_BUILD_TYPE=Release ../..
make
./fbt-check
```
