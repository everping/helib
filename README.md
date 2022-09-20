# Requirements
- OS: Ubuntu 20.04
- RAM: 8GB
- CPU: 4 coẻ


# Install
## Install packages

```shell
curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
apt update -y 
apt install -y git libopenblas64-pthread-dev gcc mono-mcs cmake build-essential clang patchelf m4 wget libntl-dev libgmp-dev nodejs python3-pip
```


## Install HElib
```
git clone https://github.com/homenc/HElib.git
cd HElib && mkdir build && cd build && cmake -DPACKAGE_BUILD=ON -DCMAKE_INSTALL_PREFIX=/opt/helib .. && make -j16 && make install && cd -


```

## load node_modules:
- npm install

## start server:
- cd server
- python / python3 server.py

## start service:
- cd service
- python / python3 service.py

## start client:
- cd client
- npm start
