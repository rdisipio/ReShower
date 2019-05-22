#!/bin/bash

nevents=1000000

./reshower 6 350 ${nevents} & # top quark, pT = 350 GeV
./reshower 6 500 ${nevents} & # top quark, pT = 500 GeV
./reshower 6 800 ${nevents} & # top quark, pT = 800 GeV

./reshower 25 250 ${nevents} & # Higgs, pT = 250 GeV
./reshower 25 500 ${nevents} & # Higgs, pT = 500 GeV
./reshower 25 800 ${nevents} & # Higgs, pT = 800 GeV

wait
echo "Done."
