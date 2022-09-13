FROM ubuntu:20.04

RUN apt update -y && apt install -y curl

WORKDIR /app

COPY . /app/

RUN curl -s https://deb.nodesource.com/setup_18.x | bash && apt install -y nodejs

RUN npm install

CMD npm start