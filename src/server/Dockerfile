FROM ubuntu:20.04

RUN apt update -y && apt install -y python3 -m pip

WORKDIR /app

COPY ./requirements.txt /app/

RUN pip install -r requirements.txt

RUN apt install -y libntl-dev libgmp-dev

COPY . /app/

CMD python3 server.py