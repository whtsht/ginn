# Ginn

Ginn is not nginx.

## What is Ginn?

Ginn is a web server inspired by [nginx](https://www.nginx.com/). It employs an event-driven approach to address [C10k problems](http://www.kegel.com/c10k.html) and achieves scaling by reloading configuration files without dropping client requests.

Note. _This was created for educational purposes and is not suitable for real production._

## Getting start

Please try it out with Docker first!

```
$ docker container run --rm -d -p 4700:80 whtsht/ginn
```

and open [http://localhost:4700](http://localhost:4700).

## Installation

First, clone this repository and go to the directory.

### Install Local

Build Ginn locally and place it in the appropriate directory.

```
$ make && sudo make install
```

Start Ginn with root permission.

```
$ sudo ginn start
```

and open [http://localhost](http://localhost).

Ginn uses a well-known 80 port by default. If you don't like this behavior, edit `/etc/ginn.conf`.

```
- # port 4700;
+ port 4700;
```

### Using Docker

Start Ginn using docker-compose.

```
$ docker compose up -d
```

and open [http://localhost:4700](http://localhost:4700).

## Unit tests

This project uses [CUnit Test Framework](https://cunit.sourceforge.net/). Please install it.

Run all tests with the following command.

```
$ make test
```

## Benchmarking

You can test and benchmark using [siege](https://github.com/JoeDog/siege).

Note. _Start Ginn on port number 4700._

```
$ siege -c 200 -t 5S -b -i --log=/tmp/siege.log -f ./urls.txt
```
