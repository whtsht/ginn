FROM alpine:latest

WORKDIR /app
COPY . /app/

RUN apk update && \
  apk add --no-cache build-base linux-headers libc6-compat && \
  make && make install

CMD ["ginn", "start", "-daemon-off"]
