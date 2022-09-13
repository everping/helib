FROM ubuntu:20.04

WORKDIR /app

ARG DEBIAN_FRONTEND=noninteractive

RUN apt update -y && apt install -y git libopenblas64-pthread-dev gcc mono-mcs cmake build-essential clang patchelf m4 wget

RUN git clone https://github.com/homenc/HElib.git

RUN cd HElib && mkdir build && cd build && cmake -DPACKAGE_BUILD=ON -DCMAKE_INSTALL_PREFIX=/app/helib_install .. && make -j16 && make install

RUN apt install -y libntl-dev libgmp-dev

RUN cd HElib/examples/ && mkdir build

COPY ./helib_source /app/HElib/examples/BGV_country_db_lookup/

COPY ./CMakeLists.txt /app/HElib/examples/CMakeLists.txt

RUN mkdir -p /app/bin/

RUN cd /app/HElib/examples/BGV_country_db_lookup/ && rm -rf BGV_country_db_lookup.cpp && mv server.cpp BGV_country_db_lookup.cpp && rm -rf ../build && mkdir ../build && cd ../build && cmake -Dhelib_DIR=/app/HElib/build/helib_pack/share/cmake/helib .. && make -j16 && mv bin/BGV_country_db_lookup /app/bin/server

RUN cd /app/HElib/examples/BGV_country_db_lookup/ && rm -rf BGV_country_db_lookup.cpp && mv client_dec.cpp BGV_country_db_lookup.cpp && rm -rf ../build && mkdir ../build && cd ../build && cmake -Dhelib_DIR=/app/HElib/build/helib_pack/share/cmake/helib .. && make -j16 && mv bin/BGV_country_db_lookup /app/bin/client_dec

RUN cd /app/HElib/examples/BGV_country_db_lookup/ && rm -rf BGV_country_db_lookup.cpp && mv client_enc.cpp BGV_country_db_lookup.cpp && rm -rf ../build && mkdir ../build && cd ../build && cmake -Dhelib_DIR=/app/HElib/build/helib_pack/share/cmake/helib .. && make -j16 && mv bin/BGV_country_db_lookup /app/bin/client_enc




