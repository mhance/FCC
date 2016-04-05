#!/usr/bin/env python
import os,sys,commands,subprocess
import argparse

import ROOT
ROOT.gROOT.SetBatch(True)

#======================================================================
#
class mt2hists:
    def __init__(self, tag, topdir):
        self.topdir=topdir
        self.hists={}
        self.tag=tag
        self.newdir=topdir.mkdir(tag)
        
    def write(self):
        self.newdir.cd()
        for i,k in self.hists.iteritems():
            k.Write()
        self.topdir.cd()
    
    def addhist(self,tag):
        self.newdir.cd()
        self.hists[tag]=ROOT.TH1F("h_"+self.tag+"_"+tag, self.tag+"_"+tag, 200, 0, 20)
        self.topdir.cd()

    def add(self,coll):
        for i,k in self.hists.iteritems():
            if i in coll.hists: k.Add(coll.hists[i])

    def fill(self,event):

        weight=float(event.weight)
        mt2vals=event.mT2
        mt2lspvals=event.mT2LSP

        for i in range(len(mt2lspvals)):
            mt2tag=("lsp_%06d" % mt2lspvals[i])
            if mt2tag not in self.hists:
                self.addhist(mt2tag)
            self.hists[mt2tag].Fill(float(mt2vals[i])/1000.,weight)
        
class jethists:
    def __init__(self, ijet, tag, topdir):
        self.topdir=topdir
        self.hists={}
        self.ijet=ijet

        self.newdir=topdir.mkdir(tag)
        self.newdir.cd()

        for i in ["pT"]: # , "mass", "TrimmedMass"
            self.hists[i]=ROOT.TH1F("h_"+tag+"_"+i, tag+"_"+i, 1000, 0, 50)

        #self.hists["eta"]=ROOT.TH1F("h_"+tag+"_"+"eta", tag+"_"+"eta", 100, -10, 10)

        #for i in ("btag", "Wtag", "Toptag"):
        #    self.hists[i]=ROOT.TH1F("h_"+tag+"_"+i, tag+"_"+i, 2, 0, 2)

        self.topdir.cd()
        
    def write(self):
        self.newdir.cd()
        for i,k in self.hists.iteritems():
            k.Write()
        self.topdir.cd()

    def add(self,coll):
        for i,k in self.hists.iteritems():
            if i in coll.hists: k.Add(coll.hists[i])
    
    def fill(self,event):

        weight=float(event.weight)
        self.hists["pT"]         .Fill(float(event.pTJet[self.ijet]         /1000.),weight)
        #self.hists["eta"]        .Fill(float(event.etaJet[self.ijet]              ),weight)
        #self.hists["mass"]       .Fill(float(event.massJet[self.ijet]       /1000.),weight)
        #self.hists["TrimmedMass"].Fill(float(event.TrimmedMassJet[self.ijet]/1000.),weight)
        #self.hists["btag"]  .Fill(int(event.btagJet  [self.ijet]),weight)
        #self.hists["Toptag"].Fill(int(event.TopTagJet[self.ijet]),weight)
        #self.hists["Wtag"]  .Fill(int(event.WTagJet  [self.ijet]),weight)
#======================================================================

def get_mindX4j(event):
    pTJet=event.pTJet
    etaJet=event.etaJet
    phiJet=event.phiJet
    massJet=event.massJet

    j0=ROOT.TLorentzVector()
    j0.SetPtEtaPhiM(pTJet[0],etaJet[0],phiJet[0],massJet[0])
    
    j1=ROOT.TLorentzVector()
    j1.SetPtEtaPhiM(pTJet[1],etaJet[1],phiJet[1],massJet[1])
    
    j2=ROOT.TLorentzVector()
    if pTJet[2]>100:
        j2.SetPtEtaPhiM(pTJet[2],etaJet[2],phiJet[2],massJet[2])
    else:
        j2.SetPtEtaPhiM(1,0,0,0)
        
    j3=ROOT.TLorentzVector()
    if pTJet[3]>100:
        j3.SetPtEtaPhiM(pTJet[3],etaJet[3],phiJet[3],massJet[3])
    else:
        j3.SetPtEtaPhiM(1,0,0,0)

    mindR4j=abs(float(j0.DrEtaPhi(j1)))
    mindPhi4j=abs(float(j0.DeltaPhi(j1)))

    if j2.Pt()>100:
        j2j0dR=abs(float(j0.DrEtaPhi(j2)))
        if j2j0dR<mindR4j:
            mindR4j=j2j0dR

        j2j1dR=abs(float(j1.DrEtaPhi(j2)))
        if j2j1dR<mindR4j:
            mindR4j=j2j1dR

        j2j0dPhi=abs(float(j0.DeltaPhi(j2)))
        if j2j0dPhi<mindPhi4j:
            mindPhi4j=j2j0dPhi

        j2j1dPhi=abs(float(j1.DeltaPhi(j2)))
        if j2j1dPhi<mindPhi4j:
            mindPhi4j=j2j1dPhi

    if j3.Pt()>100:
        j3j0dR=abs(float(j0.DrEtaPhi(j3)))
        if j3j0dR<mindR4j:
            mindR4j=j3j0dR

        j3j1dR=abs(float(j1.DrEtaPhi(j3)))
        if j3j1dR<mindR4j:
            mindR4j=j3j1dR

        j3j2dR=abs(float(j2.DrEtaPhi(j3)))
        if j3j2dR<mindR4j:
            mindR4j=j3j2dR

        j3j0dPhi=abs(float(j0.DeltaPhi(j3)))
        if j3j0dPhi<mindPhi4j:
            mindPhi4j=j3j0dPhi

        j3j1dPhi=abs(float(j1.DeltaPhi(j3)))
        if j3j1dPhi<mindPhi4j:
            mindPhi4j=j3j1dPhi

        j3j2dPhi=abs(float(j2.DeltaPhi(j3)))
        if j3j2dPhi<mindPhi4j:
            mindPhi4j=j3j2dPhi


    return float(mindR4j),float(mindPhi4j)
    
        
#======================================================================
class allhists:
    def __init__(self, tag, topdir, filljethists, detaillevel=99): 
        self.topdir=topdir
        self.hists={}
        self.tag=tag

        self.filljethists=filljethists

        self.newdir=topdir.mkdir(tag)
        self.newdir.cd()

        self.detaillevel=detaillevel

        for i in ("mT2", "mT2_leading", "MET", "HTjets", "meff", "HTleptons"):
            self.hists[i]=ROOT.TH1F("h_"+tag+"_"+i, tag+"_"+i, 1000, 0, 50)

        for i in ("nJets100", "nJets200", "nJets500", "nJets1000", "nJets2000", "nBtags", "nWtags", "nToptags"):
            self.hists[i]=ROOT.TH1F("h_"+tag+"_"+i, tag+"_"+i, 20, 0, 20)

        self.hists["METsignif"]=ROOT.TH1F("h_"+tag+"_METsignif",tag+"_METsignif",400,0,1)

        self.hists["mindphi"]=ROOT.TH1F("h_"+tag+"_mindphi",tag+"_mindphi",200,-5,5)

        self.hists["HT_MET"]=ROOT.TH2F("h_"+tag+"_HT_MET",tag+"_HT_MET",200,0,40,100,0,20)
        self.hists["METsignif_mT2"]=ROOT.TH2F("h_"+tag+"_METsignif_mT2",tag+"_METsignif_mT2",50,0,0.25,200,0,20)
        self.hists["MET_mT2"]=ROOT.TH2F("h_"+tag+"_MET_mT2",tag+"_MET_mT2",200,0,20,200,0,20)
        self.hists["HT_mT2"]=ROOT.TH2F("h_"+tag+"_HT_mT2",tag+"_HT_mT2",200,0,20,200,0,20)

        self.hists["eventbalance"]=ROOT.TH1F("h_"+tag+"_eventbalance",tag+"_eventbalance",100,0,1)

        self.hists["truth"]=ROOT.TH1F("h_"+tag+"_truth",tag+"_truth",100,0,100)

        self.hists["mjj"]=ROOT.TH1F("h_"+tag+"_mjj",tag+"_mjj",1000,0,50)
        self.hists["dRjj"]=ROOT.TH1F("h_"+tag+"_dRjj",tag+"_dRjj",100,0,10)
        self.hists["dPhijj"]=ROOT.TH1F("h_"+tag+"_dPhijj",tag+"_dPhijj",100,0,10)
        self.hists["dPhijjMET"]=ROOT.TH1F("h_"+tag+"_dPhijjMET",tag+"_dPhijjMET",200,0,5)
        self.hists["mindR4j"]=ROOT.TH1F("h_"+tag+"_mindR4j",tag+"_mindR4j",100,0,10)
        self.hists["mindPhi4j"]=ROOT.TH1F("h_"+tag+"_mindPhi4j",tag+"_mindPhi4j",200,0,5)

        self.hists["j0HT"]=ROOT.TH1F("h_"+tag+"_j0HT",tag+"_j0HT",100,0,1)
        self.hists["j1HT"]=ROOT.TH1F("h_"+tag+"_j1HT",tag+"_j1HT",100,0,1)
        self.hists["j01HT"]=ROOT.TH1F("h_"+tag+"_j01HT",tag+"_j01HT",100,0,1)
        self.hists["j0MET"]=ROOT.TH1F("h_"+tag+"_j0MET",tag+"_j0MET",100,0,1)
        self.hists["j1MET"]=ROOT.TH1F("h_"+tag+"_j1MET",tag+"_j1MET",100,0,1)
        self.hists["j01MET"]=ROOT.TH1F("h_"+tag+"_j01MET",tag+"_j01MET",100,0,1)
        self.hists["j0Meff"]=ROOT.TH1F("h_"+tag+"_j0Meff",tag+"_j0Meff",100,0,1)
        self.hists["j1Meff"]=ROOT.TH1F("h_"+tag+"_j1Meff",tag+"_j1Meff",100,0,1)
        self.hists["j01Meff"]=ROOT.TH1F("h_"+tag+"_j01Meff",tag+"_j01Meff",100,0,1)

        self.collections={}
        if self.filljethists:
            self.collections["jet0"]=jethists(0,tag+"_jet0",self.newdir)
            self.collections["jet1"]=jethists(1,tag+"_jet1",self.newdir)
            self.collections["jet2"]=jethists(2,tag+"_jet2",self.newdir)
            self.collections["jet3"]=jethists(3,tag+"_jet3",self.newdir)
        
        topdir.cd()
        
    def write(self,):
        self.newdir.cd()
        for i,k in self.hists.iteritems():
            k.Write()
        for i,k in self.collections.iteritems():
            k.write()
        self.topdir.cd()

    def add(self,coll):
        for i,k in self.hists.iteritems():
            if i in coll.hists: k.Add(coll.hists[i])
        for i,k in self.collections.iteritems():
            if i in coll.collections: k.add(coll.collections[i])
    
    def fill(self,event):
        weight=float(event.weight)

        MET=event.MET
        HT=event.HTjets

        if "old" in self.tag:
            HT=0
            for j in range(len(event.pTJet)):
                if event.pTJet[j]>30 and abs(event.etaJet[j])<3.5:
                    HT+=event.pTJet[j]


        self.hists["HTjets"]   .Fill(float(HT   /1000.) ,weight)

        if self.detaillevel==0:
            return

        self.hists["eventbalance"] .Fill(float(event.eventbalance/1000.) , weight)
        self.hists["mindphi"]      .Fill(float(event.mindPhiMet4Jet),weight)
        self.hists["MET"]          .Fill(float(MET      /1000.) ,weight)
        self.hists["HT_MET"]       .Fill(float(HT/1000.),float(MET/1000.),weight)

        if self.detaillevel<2:
            return

        self.hists["METsignif"].Fill(float(event.METsignif/1000.) ,weight)
        if self.detaillevel>1:
            self.hists["meff"]     .Fill(float(event.m_eff    /1000.) ,weight)
        if self.detaillevel>2:
            self.hists["HTleptons"].Fill(float(event.HTleptons/1000.) ,weight)

        mT2=0
        mT2_leading=0
        if isinstance(event.mT2,float):
            mT2=event.mT2/1000.
            mT2_leading=event.mT2_leading/1000.
        else:
            mT2=event.mT2[0]/1000.
            mT2_leading=event.mT2_leading[0]/1000.
            
        pTJet=event.pTJet
        nJets200=0
        nJets2000=0
        for pT in pTJet:
            if pT>200: nJets200+=1
            if pT>2000: nJets2000+=1

        self.hists["mT2"]          .Fill(float(mT2        ) ,weight)
        self.hists["mT2_leading"]  .Fill(float(mT2_leading) ,weight)
        self.hists["METsignif_mT2"].Fill(float(event.METsignif/1000.),mT2_leading,weight)
        self.hists["HT_mT2"]       .Fill(float(HT/1000.)             ,mT2_leading,weight)
        self.hists["MET_mT2"]      .Fill(float(MET/1000.)            ,mT2_leading,weight)
        self.hists["nJets100"]     .Fill(int(event.nJets100)          ,weight)
        self.hists["nJets200"]     .Fill(int(nJets200)                ,weight)
        self.hists["nJets500"]     .Fill(int(event.nJets500)          ,weight)
        self.hists["nJets1000"]    .Fill(int(event.nJets1000)         ,weight)
        self.hists["nJets2000"]    .Fill(int(nJets2000)               ,weight)

        etaJet=event.etaJet
        phiJet=event.phiJet
        massJet=event.massJet

        if self.detaillevel>2:

            self.hists["j0HT"].Fill(float(pTJet[0]/HT),weight)
            self.hists["j1HT"].Fill(float(pTJet[1]/HT),weight)
            self.hists["j01HT"].Fill(float((pTJet[0]+pTJet[1])/HT),weight)
            self.hists["j0MET"].Fill(float(pTJet[0]/MET),weight)
            self.hists["j1MET"].Fill(float(pTJet[1]/MET),weight)
            self.hists["j01MET"].Fill(float((pTJet[0]+pTJet[1])/MET),weight)
            self.hists["j0Meff"].Fill(float(pTJet[0]/event.m_eff),weight)
            self.hists["j1Meff"].Fill(float(pTJet[1]/event.m_eff),weight)
            self.hists["j01Meff"].Fill(float((pTJet[0]+pTJet[1])/event.m_eff),weight)

            if pTJet[0]<100 or pTJet[1]<100:
                self.hists["mjj"].Fill(9,weight)
                self.hists["dRjj"].Fill(9,weight)
                self.hists["dPhijj"].Fill(9,weight)
                self.hists["dPhijjMET"].Fill(9,weight)
                self.hists["mindR4j"].Fill(9,weight)
                self.hists["mindPhi4j"].Fill(9,weight)

            else:
                j0=ROOT.TLorentzVector()
                j0.SetPtEtaPhiM(pTJet[0],etaJet[0],phiJet[0],massJet[0])

                j1=ROOT.TLorentzVector()
                j1.SetPtEtaPhiM(pTJet[1],etaJet[1],phiJet[1],massJet[1])

                self.hists["mjj"].Fill(float((j0+j1).M()/1000.),weight)
                self.hists["dRjj"].Fill(abs(float(j0.DrEtaPhi(j1))),weight)
                self.hists["dPhijj"].Fill(abs(float(j0.DeltaPhi(j1))),weight)

                mindR4j,mindPhi4j=get_mindX4j(event)

                self.hists["mindR4j"].Fill(mindR4j,weight)
                self.hists["mindPhi4j"].Fill(mindPhi4j,weight)

                if MET>0:
                    met=ROOT.TLorentzVector()
                    met.SetPtEtaPhiM(MET,0,event.MET_phi,0)
                    self.hists["dPhijjMET"].Fill(abs(float((j0+j1).DeltaPhi(met))),weight)
                else:
                    self.hists["dPhijjMET"].Fill(9,weight)
                

                nBtags=0
                for i in event.btagJet:
                    if int(i)<0:
                        break
                    if (int(i) & 0x1)>0:
                        nBtags+=1
                self.hists["nBtags"].Fill(nBtags,weight)

        # -----------------------------------------------------------
        nemu=0
        ntau=0
        nnu=0
        nW=0
        nZ=0
        nH=0
        nP=0
        nG=0
        nLF=0
        nHF=0
        ntop=0
        pdgs=event.pdgIdTruth
        for pdgiter in range(len(pdgs)):
            if pdgs[pdgiter]==-1: break
            pdg=abs(pdgs[pdgiter])
            if pdg<5: nLF+=1
            elif pdg==5: nHF+=1
            elif pdg==6: ntop+=1
            elif (pdg==11 or pdg==13) and (event.pTTruth[pdgiter]>5): nemu+=1
            elif (pdg==12 or pdg==14 or pdg==16) and (event.pTTruth[pdgiter]>5): nnu+=1
            elif pdg==15: ntau+=1
            elif pdg==21: nG+=1
            elif pdg==22: nP+=1
            elif pdg==23: nZ+=1
            elif pdg==24: nW+=1
            elif pdg==25: nH+=1

        if   nW>0 and ntau>0:  self.hists["truth"].Fill( 0,weight)
        elif nW>0 and nemu>0:  self.hists["truth"].Fill( 1,weight)
        elif nW>0 and nnu>0:   self.hists["truth"].Fill( 2,weight)
        elif nW>0:             self.hists["truth"].Fill( 3,weight)
        elif nZ>0 and ntau>0:  self.hists["truth"].Fill(10,weight)
        elif nZ>0 and nnu>0:   self.hists["truth"].Fill(12,weight)
        elif nZ>0 and nemu>0:  self.hists["truth"].Fill(11,weight)
        elif nZ>0:             self.hists["truth"].Fill(13,weight)
        elif nH>0:             self.hists["truth"].Fill(20,weight)
        elif nP>0:             self.hists["truth"].Fill(30,weight)
        elif ntop>0:           self.hists["truth"].Fill(40,weight)
        else:                  self.hists["truth"].Fill(90,weight)
        # -----------------------------------------------------------

        if False:
            nWtags=0
            nToptags=0
            for i in event.WTagJet:
                if i==1:
                    nWtags+=1
            for i in event.TopTagJet:
                if i==1:
                    nToptags+=1

            self.hists["nWtags"].Fill(int(nWtags),weight)
            self.hists["nToptags"].Fill(int(nToptags),weight)

        for i,k in self.collections.iteritems():
            if self.filljethists:
                k.fill(event)
#======================================================================

#======================================================================
#
class histcollection:
    def __init__(self, tag, topdir, filljethists=False, detaillevel=99): 
        self.topdir=topdir
        self.tag=tag

        self.filljethists=filljethists
        
        self.newdir=topdir.mkdir(tag)

        self.detaillevel=detaillevel

        self.newdir.cd()

        self.collections={}
        
        self.topdir.cd()
        
    def write(self):
        self.newdir.cd()
        for i,k in self.collections.iteritems():
            k.write()
        self.topdir.cd()
    
    def addcollection(self,tag):
        self.newdir.cd()
        self.collections[tag]=allhists(self.tag+"_"+tag,self.newdir,self.filljethists,self.detaillevel)
        self.topdir.cd()

    def add(self,coll):
        for i,k in self.collections.iteritems():
            if i in coll: k.add(coll[i])

    def fill(self,event):
        for i,k in self.collections.iteritems():
            k.fill(event)

    def fill(self,event,tag):
        for i,k in self.collections.iteritems():
            if i==tag:
                k.fill(event)
                break
        
#======================================================================


#======================================================================
#
class SRhists:
    def __init__(self, tag, topdir, filljethists=False, energy=14, detaillevel=99): 
        self.topdir=topdir
        self.tag=tag
        
        self.newdir=topdir.mkdir(tag)
        self.newdir.cd()
        self.energy=int(energy)

        self.htvals=(0.5, 1, 1.5, 2, 3, 4, 5, 6, 8, 10, 12, 15, 20)

        self.collections={}
        self.collections["htbins"]=histcollection("htbins",self.newdir, filljethists, detaillevel)
        self.collections["htbins"].addcollection("inclusive")
        for i in self.htvals:
            self.collections["htbins"].addcollection(str(i))

        self.topdir.cd()
        
    def write(self):
        self.newdir.cd()

        # fill the inclusive hist collections here by adding the constituents, should be faster
        # than doing it in 'fill'
        for i,k in self.collections["htbins"].collections.iteritems():
           if k.tag=="htbins_inclusive": continue
           else:
               self.collections["htbins"].collections["inclusive"].add(k)
            
        for i,k in self.collections.iteritems():
            k.write()
        self.topdir.cd()
    
    def addcollection(self,tag):
        self.newdir.cd()
        self.collections[tag]=allhists(self.tag+"_"+tag,self.topdir,False)
        self.topdir.cd()


    def add(self,coll):
        for i,k in self.collections.iteritems():
            if i in coll: k.add(coll[i])

    def fill(self,event):
        HTfrac=100.*float(((float(event.HTjets)/1000.)/int(self.energy)))

        # binned results
        for thresh_i in range(len(self.htvals)):
            if (((thresh_i < len(self.htvals)-1) and 
                 HTfrac>=self.htvals[thresh_i] and 
                 HTfrac<self.htvals[thresh_i+1]) or 
                ((thresh_i == len(self.htvals)-1) and 
                 HTfrac>=self.htvals[thresh_i])):
                self.collections["htbins"].fill(event,str(self.htvals[thresh_i]))
                break

#======================================================================

#======================================================================
#
def analyze(infile, outfile, debug, fast, energy, skippresel, signif, mt2cut, metcut, skiptree):

    if debug: print "...opening input"

    # open the file
    f=ROOT.TFile(infile,"RO")

    # get the input TTree
    t=f.Get("OutputTree")


    t.SetBranchStatus("Tau*",0)
    t.SetBranchStatus("WTagJet",0)
    t.SetBranchStatus("TopTagJet",0)
    t.SetBranchStatus("TrimmedMassJet",0)
    t.SetBranchStatus("pTBJet",0)
    t.SetBranchStatus("etaBJet",0)
    t.SetBranchStatus("phiBJet",0)
    t.SetBranchStatus("massBJet",0)
    t.SetBranchStatus("dPhiMETB*",0)
    t.SetBranchStatus("pTLep",0)
    t.SetBranchStatus("etaLep",0)
    t.SetBranchStatus("phiLep",0)
    t.SetBranchStatus("flavLep",0)
    t.SetBranchStatus("mTLep",0)
    t.SetBranchStatus("etaTruth",0)
    t.SetBranchStatus("phiTruth",0)
    t.SetBranchStatus("massTruth",0)
    t.SetBranchStatus("M1Truth",0)
    t.SetBranchStatus("M2Truth",0)
    t.SetBranchStatus("D1Truth",0)
    t.SetBranchStatus("D2Truth",0)
    t.SetBranchStatus("statusTruth",0)
    t.SetBranchStatus("nLeps35",0)
    t.SetBranchStatus("mindPhiMETBtag",0)
    
    skimweights=f.Get("TotalProcessWeights")
    skimevents=f.Get("TotalProcessEvents")
    if skimweights==None:
        skimweights=f.Get("TotalSkimWeights")
        skimevents=f.Get("TotalSkimEvents")

    if debug: print "...opening output"

    # create the output
    o=ROOT.TFile(outfile,"RECREATE")

    skimweights.Write("TotalSkimWeights")
    skimevents.Write("TotalSkimEvents")

    t_output=t.CloneTree(0)
    
    if debug: print "...making histograms"

    # create the output histograms
    presel=histcollection("presel",o,False,1)
    presel.addcollection("skim")
    presel.addcollection("oldpresel")
    presel.addcollection("njets")
    presel.addcollection("lepveto")
    presel.addcollection("balance")
    presel.addcollection("mindphi")
    
    srhists_old=SRhists("SRhists_old",o,True,int(energy),3)
    srhists=SRhists("SRhists",o,True,int(energy),3)

    # collections based on number of jets
    srhists_lownjets=SRhists("SRhists_lownjets",o,True,int(energy),3)
    srhists_mednjets=SRhists("SRhists_mednjets",o,True,int(energy),3)
    srhists_highnjets=SRhists("SRhists_highnjets",o,True,int(energy),3)
    srhists_suprnjets=SRhists("SRhists_suprnjets",o,True,int(energy),3)

    minmt2cut=-1
    minmetcut=-1
    if isinstance(mt2cut,list):
        if len(mt2cut) != len(srhists.htvals):
            print "Can't reconcile %d mt2 cuts with %d ht bins.  Aborting." % (len(mt2cut),len(srhists.htvals))
            exit
    else:
        minmt2cut=float(mt2cut)
    if isinstance(metcut,list):
        if len(metcut) != len(srhists.htvals):
            print "Can't reconcile %d met cuts with %d ht bins.  Aborting." % (len(metcut),len(srhists.htvals))
            exit
    else:
        minmetcut=float(metcut)

    signifs=[]
    if isinstance(signif,float):
        for i in range(len(srhists.htvals)):
            signifs.append(signif)
    else:
        signifs=signif

    if debug: print "...starting event loop"

    # loop over the TTree
    eventcount=0
    totalentries=t.GetEntriesFast()
    printoutmod=1000
    if t.GetEntriesFast()>50000:
        printoutmod=t.GetEntriesFast()/200
    for event in t:
        #-------------------------
        # bookkeeping and monitoring
        #
        eventcount+=1
        if (eventcount%printoutmod)==0:
            print "Processed %8d / %8d events (%3.1f %%)" % (eventcount, totalentries, float(100.*eventcount/totalentries))
            sys.stdout.flush()

        if debug and eventcount==1000:
            break
        #-------------------------

        #-------------------------
        # some preliminaries
        #
        HT = event.HTjets
        MET = event.MET
        mT2_leading=0
        if isinstance(event.mT2_leading,float):
            mT2_leading=event.mT2_leading
        else:
            mT2_leading=event.mT2_leading[0]

        # if we're making simple cuts
        if minmt2cut>0:
            if mT2_leading<minmt2cut:
                print "1"
                continue
        if minmetcut>0:
            if MET<minmetcut:
                print "1"
                continue

        HTbin=-1
        # if we're making simple cuts on MET and/or mT2
        for i in range(len(srhists.htvals)):
            if i==len(srhists.htvals)-1 and HT/1000.>srhists.htvals[i]:
                HTbin=i
                break
            if HT/1000. >= srhists.htvals[i] and HT/1000. < srhists.htvals[i+1]:
                HTbin=i
                break

        # if we're making optimized cuts
        if isinstance(mt2cut,list):
            if mT2_leading<float(mt2cut[HTbin]) or float(signifs[HTbin])<1.:
                print "1"
                continue
        if isinstance(metcut,list):
            if MET<float(metcut[HTbin]) or float(signifs[HTbin])<1.:
                print "1"
                continue
        #-------------------------

        #-------------------------
        # define nJets using a pT threshold of 0.5% of sqrt(s)
        #
        nJets=0
        pTJet=event.pTJet
        etaJet=event.etaJet
        HTjets_30_3p5=0
        nJets60_3p5=0
        for j in range(len(pTJet)):
            if pTJet[j]>float(energy)*5.:
                nJets+=1
            if pTJet[j]>30 and abs(etaJet[j])<3.5:
                HTjets_30_3p5+=pTJet[j]
                if pTJet[j]>60:
                    nJets60_3p5+=1
        #-------------------------

        #-------------------------
        # normal preselection cuts
        #
        if not fast: presel.fill(event,"skim")
        #-------------------------

        #-------------------------
        # if we want to re-run the old analysis, do that here.
        #
        if event.MET>100 and nJets60_3p5>3 and event.HTleptons==0:
            if not fast: presel.fill(event,"oldpresel")
            if HTjets_30_3p5>0. and event.HTjets>0 and \
                    event.MET/(event.HTjets**0.5)>=15. and \
                    (pTJet[0]/HTjets_30_3p5)<0.4:
                srhists_old.fill(event)
        #-------------------------

        #-------------------------
        # normal selection cuts
        #
        # number of jets... will bin in this later.
        if nJets < 2:
            continue
        if not fast: presel.fill(event,"njets")

        # lepton veto
        if event.HTleptons_lep20>0:
            continue
        if not fast: presel.fill(event,"lepveto")

        # event balance between MET and hard objects.
        if event.eventbalance>100:
            continue
        if not fast: presel.fill(event,"balance")

        # min dphi between met and 4 leading jets
        if abs(event.mindPhiMet4Jet)<0.6:
            continue
        if not fast: presel.fill(event,"mindphi")
        #-------------------------
        
        #-------------------------
        # signal regions
        srhists.fill(event)
        if not skiptree:
            t.GetEntry(eventcount-1)
            t_output.Fill()
        #-------------------------

        #-------------------------
        # some signal regions with larger numbers of jets.
        # this is one place where things could really be improved,
        # e.g. optimizing these cuts for different corners of the
        # signal grid.
        if nJets>=3:
            srhists_lownjets.fill(event)
            if nJets>=4:
                srhists_mednjets.fill(event)
                if nJets>=5:
                    srhists_highnjets.fill(event)
                    if nJets>=6:
                        srhists_suprnjets.fill(event)
        #-------------------------
        

    if debug: print "...writing data"
    
    presel.write()
    srhists_old.write()
    srhists.write()
    srhists_lownjets.write()
    srhists_mednjets.write()
    srhists_highnjets.write()
    srhists_suprnjets.write()
    if not skiptree:
        t_output.Write()

    if debug: print "...done."
    
#======================================================================


#======================================================================
def main(argv):

    parser = argparse.ArgumentParser(description="Command line arguments")
    parser.add_argument("--infile"       , action='store', default='')
    parser.add_argument("--outfile"      , action='store', default='')
    parser.add_argument("--test"         , action='store_true')
    parser.add_argument("--fast"         , action='store_true')
    parser.add_argument("--energy"       , action='store', default=100)
    parser.add_argument("--skippresel"   , action='store_true')
    parser.add_argument("--signif"      , action='store', default="100")
    parser.add_argument("--mt2cut"      , action='store', default="-1")
    parser.add_argument("--metcut"      , action='store', default="-1")
    parser.add_argument("--skiptree"    , action='store_true')
    args=parser.parse_args()

    if args.test:
        print "Starting analysis"

    if "," in args.signif:
        newsignifs=args.signif.split(",")
    else:
        newsignifs=float(args.signif)

    if "," in args.mt2cut:
        newmt2cuts=args.mt2cut.split(",")
    else:
        newmt2cuts=float(args.mt2cut)

    if "," in args.metcut:
        newmetcuts=args.metcut.split(",")
    else:
        newmetcuts=float(args.metcut)

    analyze(args.infile, args.outfile, args.test, args.fast, args.energy,
            args.skippresel, newsignifs, newmt2cuts, newmetcuts, args.skiptree)

    if args.test:
        print "Done with analysis."


if __name__ == '__main__':
    main(sys.argv[1:])
#======================================================================
