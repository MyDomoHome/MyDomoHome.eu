FROM python:alpine

RUN pip install cheetah
RUN git clone --depth 1 https://github.com/SickRage/SickRage.git /sickbeard

VOLUME /config /downloads /tv
expose 8081

ENTRYPOINT python3 /sickbeard/SickBeard.py --datadir=/config