FROM sam0delk1n/ubuntu-bionic-gcc-cmake:latest

WORKDIR /home/app
ADD . .

WORKDIR /home/app/build/Debug
RUN cmake -DCMAKE_BUILD_TYPE=Debug ../.. && make

WORKDIR /home/app/build/Release
RUN cmake -DCMAKE_BUILD_TYPE=Release ../.. && make
