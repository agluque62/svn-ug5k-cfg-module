FROM  gcc:5.5 AS build-env
LABEL Name=uv5kigwcfg Version=0.0.1
WORKDIR /app
COPY . /app
RUN make

FROM ubuntu:latest
# RUN apt-get update
# RUN apt-get install gdb
WORKDIR /home/serv
RUN mkdir -p ./logs
COPY --from=build-env /app/appweb /home/serv/appweb/.
# COPY --from=build-env /app/fs-win /home/serv/fs-win/.
COPY --from=build-env /app/uvki-gwser /home/serv/
COPY --from=build-env /app/plog.conf /home/serv/
COPY --from=build-env /app/ug5k.db /home/serv/
COPY --from=build-env /app/u5kweb-config.ini /home/serv/
COPY --from=build-env /app/versiones.json /home/serv/

CMD ["./uvki-gwser"]