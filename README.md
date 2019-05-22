# ReShower
Run Parton Shower on the same event multiple times

```
mkdir -p histograms
```

Top quarks, t->Wb->qqb
```
./reshower 6 350 # top quark, pT = 350 GeV
./reshower 6 500 # top quark, pT = 500 GeV
./reshower 6 800 # top quark, pT = 800 GeV
```

Higgs bosons, h->bb
```
./reshower 25 250 # Higgs, pT = 250 GeV
./reshower 25 500 # Higgs, pT = 500 GeV
./reshower 25 800 # Higgs, pT = 800 GeV
```
