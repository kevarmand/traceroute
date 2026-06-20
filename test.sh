#!/bin/bash

make
./ft_traceroute
./ft_traceroute -n -N 4 -q 3 -s 127.0.0.1 -m 30 -f 1 -p 33434 -z 0.7 8.8.8.8
