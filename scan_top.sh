#!/bin/bash

nevents=100000
[ ! -z $1 ] && nevents=$1

./reshower 6 350 ${nevents} &
./reshower 6 400 ${nevents} &
./reshower 6 450 ${nevents} &
./reshower 6 500 ${nevents} &
./reshower 6 550 ${nevents} &
./reshower 6 600 ${nevents} &
./reshower 6 650 ${nevents} &
./reshower 6 700 ${nevents} &
./reshower 6 750 ${nevents} &
./reshower 6 800 ${nevents} &
./reshower 6 850 ${nevents} &
./reshower 6 900 ${nevents} &
./reshower 6 950 ${nevents} &
./reshower 6 1000 ${nevents} &

