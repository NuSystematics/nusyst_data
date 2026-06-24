import ROOT

Models = [
"hA2018",
"hN2018",
"INCL++",
"Geant4",
]

Particles = [
"piPlus",
"piMinus",
"pi0",
"protonPlus",
"neutron",
]

HistTypes = [
"Mult",
"Diff",
]

OutName = "FSI_KOAbs_reweight_template.root"

f_out = ROOT.TFile(OutName, "RECREATE")

for HistType in HistTypes:

  InName = f"FSI_KOAbs_{HistType}_reweight_template.root"

  f_in = ROOT.TFile(InName)

  for Particle in Particles:
    for Model in Models:
      hName = f"{Model}_{Particle}_Nucl{HistType}"

      h = f_in.Get(hName)

      f_out.cd()
      h.Write(hName)

  f_in.Close()

f_out.Close()
