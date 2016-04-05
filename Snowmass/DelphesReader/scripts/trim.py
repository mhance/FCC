from ROOT import TTree,TFile

infile=TFile("backgrounds_100TeV/delphes.Bj-4p_100TEV.NoPileUp.002.root","RO")

intree=infile.Get("OutputTree")

intree.SetBranchStatus("*",0)

tokeep=("MET","weight","HTjets","m_eff","pTJet","METsignif","HTleptons")
for i in tokeep:
    intree.SetBranchStatus(i,1)

outfile=TFile("backgrounds_100TeV/delphes.Bj-4p_100TEV.NoPileUp.skim.root","RECREATE")
outtree=intree.CloneTree(0)

nevents=intree.GetEntriesFast()

for i in xrange(nevents):
    if (i%1000000)==0:
        print i
    intree.GetEntry(i)
    outtree.Fill()

outfile.Write()
outfile.Close()

