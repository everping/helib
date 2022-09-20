# Requirements
- OS: Ubuntu 20.04
- RAM: 8GB
- CPU: 4 core


# Install
## Install packages

```shell
curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
apt update -y 
apt install -y git libopenblas64-pthread-dev gcc mono-mcs cmake build-essential clang patchelf m4 wget libntl-dev libgmp-dev nodejs python3-pip
```


## Install HElib
```
git clone https://github.com/homenc/HElib.git /tmp/helib
cd /tmp/helib && mkdir build && cd build && cmake -DPACKAGE_BUILD=ON -DCMAKE_INSTALL_PREFIX=/opt/helib .. && make -j16 && make install && cd -
```

# Build
## Build search
```
git clone https://github.com/everping/helib.git patient-search /opt/patient-search/

cd /opt/helib/examples/BGV_country_db_lookup

mv /opt/patient-search/helib_source/server.cpp BGV_country_db_lookup.cpp && rm -rf ../build && mkdir ../build && cd ../build && cmake -Dhelib_DIR=/opt/helib/build/helib_pack/share/cmake/helib .. && make -j16 && mv bin/BGV_country_db_lookup /opt/patient-search/src/server/bin/server

mv /opt/patient-search/helib_source/client_enc.cpp BGV_country_db_lookup.cpp && rm -rf ../build && mkdir ../build && cd ../build && cmake -Dhelib_DIR=/opt/helib/build/helib_pack/share/cmake/helib .. && make -j16 && mv bin/BGV_country_db_lookup /opt/patient-search/src/service/bin/client_enc

mv /opt/patient-search/helib_source/client_dec.cpp BGV_country_db_lookup.cpp && rm -rf ../build && mkdir ../build && cd ../build && cmake -Dhelib_DIR=/opt/helib/build/helib_pack/share/cmake/helib .. && make -j16 && mv bin/BGV_country_db_lookup /opt/patient-search/src/service/bin/client_dec
```

## Create env
```
python3 -m pip install virtualenv
python3 -m virtualenv /opt/patient-search/env
/opt/patient-search/env/bin/pip install -r /opt/patient-search/requirements.txt
cd /opt/patient-search/src/client && npm install
```


# Run
```
/opt/patient-search/env/bin/python3 /opt/patient-search/src/server/server.py &
/opt/patient-search/env/bin/python3 /opt/patient-search/src/server/service.py &
cd /opt/patient-search/src/client && npm start &
```

# Usage
```
http://localhost:3000
```
