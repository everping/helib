FROM ubuntu:22.04

WORKDIR /app

ARG DEBIAN_FRONTEND=noninteractive

RUN apt update -y && apt install -y git libopenblas64-pthread-dev gcc mono-mcs

RUN apt-get -y install cmake

RUN apt install -y build-essential clang

RUN apt install -y  patchelf

RUN apt install -y  m4

RUN git clone https://github.com/homenc/HElib.git

RUN cd HElib && mkdir build && cd build && cmake -DPACKAGE_BUILD=ON -DCMAKE_INSTALL_PREFIX=/app/helib_install .. && make -j16

RUN apt install -y wget 

RUN cd /tmp && wget https://gmplib.org/download/gmp/gmp-6.2.1.tar.xz && tar -xf gmp-6.2.1.tar.xz && cd gmp-6.2.1 && ./configure && make && make install

RUN cd /tmp && wget https://libntl.org/ntl-11.5.1.tar.gz && tar -xf ntl-11.5.1.tar.gz && cd ntl-11.5.1/src/ && ./configure NTL_GMP_LIP=on SHARED=on NTL_THREADS=on NTL_THREAD_BOOST=on && make && make install

RUN cd HElib/examples/ && mkdir build && cmake -Dhelib_DIR=/app/HElib/build/helib_pack/share/cmake/helib .. && make -j16

COPY . /app

RUN cd HElib/examples/build && cp /app/HElib/examples/BGV_country_db_lookup/CMakeLists.txt . && cp /app/helib_source/client_enc.cpp BGV_country_db_lookup.cpp && make

# CMD ["/bin/bash"]

