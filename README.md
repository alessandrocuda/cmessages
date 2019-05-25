# cmessage
A client-server and P2P messaging application written in C

## Description

TBW
## Stack
- [log.c](https://github.com/rxi/log.c): A simple logging library implemented in C99
- **pthread**: POSIX Threads system call
- **socket api**: create an endpoint for communication linux system call

## Usage
TBW

## BUILD
```bash
git clone https://github.com/alessandrocuda/cmessages
cd cmessages
make
```
### Run
```bash
#if you want some help just type:
./msg_client.out -h
./msg_server.out -h

#run cmessages client
./msg_client.out -s client_ip:client_port -d server_ip:server_port 
#run cmessages server
./msg_server.out  -p server_port     
```

## Support

Reach out to me at one of the following places!

- Website at <a href="https://alessandrocudazzo.it" target="_blank">Alessandro Cudazzo</a>.
- Twitter at <a href="http://twitter.com/alessandrocuda" target="_blank">`@alessandrocuda`</a>

## License
[![License](http://img.shields.io/:license-mit-blue.svg?style=flat-square)](http://badges.mit-license.org)

This library is free software; you can redistribute it and/or modify it under
the terms of the MIT license. 

- **[MIT license](LICENSE)**
- Copyright 2019 © <a href="https://alessandrocudazzo.it" target="_blank">Alessandro Cudazzo</a>.