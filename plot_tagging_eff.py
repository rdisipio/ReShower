#!/usr/bin/env python

import os, sys, glob
from ROOT import *

pid = 6
if len(sys.argv) > 1: pid = int(sys.argv[1])

if pid == 6:
  hname = "ljet_topTag"
elif pid == "25":
  hname = "ljet_higgsTag"

files = glob.glob("histograms/histograms.pid_%s.*.root" % pid)
files.sort()

g = TGraphErrors()
ipoint = 0
for filename in files:
  f = TFile.Open( filename )
  h = f.Get( hname )
  pt = float( filename.split('.')[2].split('_')[1] )
  eff = h.GetBinContent(2)
  unc = h.GetBinError(2)
  g.SetPoint( ipoint, pt, eff )
  g.SetPointError( ipoint, 0., unc )
  ipoint += 1
  f.Close()

g.Draw("alp")
