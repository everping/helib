FROM helib2

RUN apt update -y && apt install -y python3 -m pip

WORKDIR /app

COPY ./requirements.txt /app/

RUN pip install -r requirements.txt

RUN apt install -y libntl-dev libgmp-dev

COPY . /app/

CMD python3 service.py