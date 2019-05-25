#!/bin/bash

nevents=100000
[ ! -z $1 ] &&nevents=$1

./reshower 25 200 ${nevents} &
./reshower 25 250 ${nevents} &
./reshower 25 300 ${nevents} &
./reshower 25 350 ${nevents} &
./reshower 25 400 ${nevents} &
./reshower 25 450 ${nevents} &
./reshower 25 500 ${nevents} &
./reshower 25 550 ${nevents} &
./reshower 25 600 ${nevents} &
./reshower 25 650 ${nevents} &
./reshower 25 700 ${nevents} &
./reshower 25 750 ${nevents} &
./reshower 25 800 ${nevents} &
./reshower 25 850 ${nevents} &
./reshower 25 900 ${nevents} &
./reshower 25 950 ${nevents} &
./reshower 25 1000 ${nevents} &

