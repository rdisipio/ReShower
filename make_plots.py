#!/usr/bin/env python

import os
import sys

from ROOT import *

gROOT.LoadMacro("AtlasStyle.C")
gROOT.LoadMacro("AtlasUtils.C")
SetAtlasStyle()

gStyle.SetOptStat(0)
gROOT.SetBatch(1)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


def SetHistogramStyle(h, color=kBlack, linewidth=3, linestyle=kSolid, fillcolor=0, fillstyle=0, markerstyle=21, markersize=1.3, fill_alpha=0):
    '''Set the style with a long list of parameters'''

    h.SetLineColor(color)
    h.SetLineWidth(linewidth)
    h.SetLineStyle(linestyle)
    h.SetFillColor(fillcolor)
    h.SetFillStyle(fillstyle)
    h.SetMarkerStyle(markerstyle)
    h.SetMarkerColor(h.GetLineColor())
    h.SetMarkerSize(markersize)
    if fill_alpha > 0:
        h.SetFillColorAlpha(color, fill_alpha)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


def Normalize(h, sf=1.0):
    area = h.Integral()
    h.Scale(sf/area)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


known_obs = [
    "ljet_pt",
    "ljet_eta",
    "ljet_m",
    "ljet_tau21",
    "ljet_tau32",
    "jets_n",
]

f_higgs_low = TFile.Open("histograms/histograms.pid_25.pt_250.root")
f_higgs_mid = TFile.Open("histograms/histograms.pid_25.pt_500.root")
f_higgs_hi = TFile.Open("histograms/histograms.pid_25.pt_800.root")

f_top_low = TFile.Open("histograms/histograms.pid_6.pt_350.root")
f_top_mid = TFile.Open("histograms/histograms.pid_6.pt_500.root")
f_top_hi = TFile.Open("histograms/histograms.pid_6.pt_800.root")

h_higgs = {}
h_top = {}
for obs in known_obs:

    h_higgs[obs] = {
        'low': f_higgs_low.Get(obs).Clone("%s_low" % obs),
        'mid': f_higgs_mid.Get(obs).Clone("%s_mid" % obs),
        'hi': f_higgs_hi.Get(obs).Clone("%s_hi" % obs)
    }
    SetHistogramStyle(h_higgs[obs]['low'], color=kBlack, markersize=0)
    SetHistogramStyle(h_higgs[obs]['mid'], color=kBlue,
                      linestyle=kDotted, markersize=0)
    SetHistogramStyle(h_higgs[obs]['hi'], color=kRed,
                      linestyle=kDashed, markersize=0)

    h_top[obs] = {
        'low': f_top_low.Get(obs).Clone("%s_low" % obs),
        'mid': f_top_mid.Get(obs).Clone("%s_mid" % obs),
        'hi': f_top_hi.Get(obs).Clone("%s_hi" % obs)
    }
    SetHistogramStyle(h_top[obs]['low'], color=kBlack, markersize=0)
    SetHistogramStyle(h_top[obs]['mid'], color=kBlue,
                      linestyle=kDotted, markersize=0)
    SetHistogramStyle(h_top[obs]['hi'], color=kRed,
                      linestyle=kDashed, markersize=0)

c = TCanvas("C", "C", 1200, 800)
c.Divide(3, 2)
# c.Draw()

i = 1
for obs in known_obs:
    c.cd(i)

    for x in ['low', 'mid', 'hi']:
        Normalize(h_higgs[obs][x])

    h_higgs[obs]['low'].Draw("h")
    h_higgs[obs]['mid'].Draw("h same")
    h_higgs[obs]['hi'].Draw("h same")

    h_higgs[obs]['low'].SetMaximum(1.5*h_higgs[obs]['hi'].GetMaximum())

    i += 1
c.SaveAs("plots/higgs.png")

i = 1
for obs in known_obs:
    c.cd(i)

    for x in ['low', 'mid', 'hi']:
        Normalize(h_top[obs][x])

    h_top[obs]['low'].Draw("h")
    h_top[obs]['mid'].Draw("h same")
    h_top[obs]['hi'].Draw("h same")

    h_top[obs]['low'].SetMaximum(1.5*h_top[obs]['hi'].GetMaximum())

    i += 1
c.SaveAs("plots/top.png")
