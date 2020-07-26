FROM alpine:latest
RUN apk add --no-cache cmake make automake autoconf ninja
RUN apk add --no-cache clang clang-dev gcc g++ lld musl-dev
RUN apk add --no-cache python3 py3-pip py3-wheel
RUN pip install --ignore-installed conan

COPY CmakeLists.txt /data/source/
COPY src/ /data/source/
COPY test/ /data/source/

ENV CC=/usr/bin/clang CXX=/usr/bin/clang++ LD=/usr/bin/lld
WORKDIR /data/build_clang_debug
RUN ["cmake", "-G", "Ninja", "-DCMAKE_BUILD_TYPE=Debug", "/data/source/"]
RUN ["cmake", "--build", "."]
WORKDIR /data/build_clang_release
RUN ["cmake", "-G", "Ninja", "-DCMAKE_BUILD_TYPE=RelWithDebInfo", "/data/source/"]
RUN ["cmake", "--build", "."]

ENV CC=/usr/bin/gcc CXX=/usr/bin/g++ LD=/usr/bin/ld
WORKDIR /data/build_gcc_debug
RUN ["cmake", "-G", "Ninja", "-DCMAKE_BUILD_TYPE=Debug", "/data/source/"]
RUN ["cmake", "--build", "."]
WORKDIR /data/build_gcc_release
RUN ["cmake", "-G", "Ninja", "-DCMAKE_BUILD_TYPE=RelWithDebInfo", "/data/source/"]
RUN ["cmake", "--build", "."]
WORKDIR /data
