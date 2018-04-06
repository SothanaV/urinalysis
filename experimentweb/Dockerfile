FROM python:3.5
RUN apt-get update 

RUN mkdir /web
WORKDIR /web
ADD ./requirements.txt /web/requirements.txt
RUN pip install -r requirements.txt
ADD . /web
