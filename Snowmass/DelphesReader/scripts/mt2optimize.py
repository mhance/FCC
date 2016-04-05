#!/usr/bin/env python
import os,sys,commands,subprocess
import argparse
import math

import ROOT
ROOT.gROOT.SetBatch(True)

from ROOT import TFile,TDirectory,TH1F,TCanvas,TH2F
from ROOT import RooStats

from collections import OrderedDict

#======================================================================
def analyze(infile, outfile, test, lumi, kfact, var, unc, histcoll, collection, energy, doZllCR, doUpperCuts, doSuperUpperCuts, onlyInclusive, fixedWindow):
    # get the input file
    sigfile=TFile(infile,"RO")

    sigscale=lumi*kfact
    bgscale=lumi


    cutflowsteps=("skim","njets","lepveto","balance","mindphi") # "tauveto" would go after lepveto, "metsignif" was removed
    cutflow=OrderedDict()
    for i in cutflowsteps:
        cutflow[i]=sigfile.Get("presel/presel_%s/h_presel_%s_HTjets" % (i,i)).GetSumOfWeights()*sigscale


    gensig=sigfile.Get("TotalSkimWeights").GetVal()*sigscale
    for i in range(0,len(cutflowsteps)):
        cfstep=cutflowsteps[i]
        nev=cutflow[cfstep]
        nevprev=nev
        if i>0:
            nevprev=cutflow[cutflowsteps[i-1]]
            
        print "Cut flow %10s: %20.2f (%6.2f%% from prev step, %6.2f%% total)" % (cfstep,
                                                                                 nev,
                                                                                 nev/nevprev*100,
                                                                                 nev/gensig*100)
    M1=int(infile.split('.')[3][2:])
    M2=int(infile.split('.')[5][1:6])
    M1TEV=float(M1/1000.)

    bghistsall=OrderedDict()
    bgfiles=OrderedDict()
    backgrounds=("Bj", "tt", "tB", "tj", "ttB")
    if energy==100:
        backgrounds.append("QCD")
    bgstring=""
    for i in backgrounds:
        print i
        bgfiles[i]=TFile("backgrounds_"+str(energy)+"TeV/hist."+i+"-4p_"+str(energy)+"TEV.NoPileUp.root","RO")
        bghistsall[i]=OrderedDict()
        bgstring+="%14s" % i

        for j in cutflowsteps:
            if bgfiles[i].IsOpen():
                cutflow[j]=bgfiles[i].Get("presel/presel_%s/h_presel_%s_HTjets" % (j,j)).GetSumOfWeights()*bgscale
            else:
                cutflow[j]=0
            
        if bgfiles[i].IsOpen():
            genbg=bgfiles[i].Get("TotalSkimWeights").GetVal()*bgscale
        else:
            genbg=1
        for j in range(0,len(cutflowsteps)):
            cfstep=cutflowsteps[j]
            nev=0
            if cutflow[cfstep]!=0:
                nev=cutflow[cfstep]
            nevprev=nev
            if j>0 and cutflow[cfstep]!=0:
                nevprev=cutflow[cutflowsteps[j-1]]
            else:
                nevprev=genbg
            
            print "Cut flow %10s: %20.2f (%6.2f%% from prev step, %6.2f%% total)" % (cfstep,
                                                                                     nev,
                                                                                     nev/nevprev*100,
                                                                                     nev/genbg*100)

    print "done"
    if int(var.find('_'))>0:
        if doUpperCuts or doSuperUpperCuts or fixedWindow>0:
            print "%8s %8s %20s %7s %10s %13s %13s %s %7s %7s %10s %10s %10s %10s" % ("M1",
                                                                                      "M2",
                                                                                      "SignalRegion",
                                                                                      "Z_n",
                                                                                      "ignore",
                                                                                      "Signal",
                                                                                      "Total BG",
                                                                                      bgstring,
                                                                                      "AccEff",
                                                                                      "mt2Eff",
                                                                                      var[int(var.find('_'))+1:]+"low",
                                                                                      var[0:int(var.find('_'))]+"low",
                                                                                      var[int(var.find('_'))+1:]+"up",
                                                                                      var[0:int(var.find('_'))]+"up")
        else:
            print "%8s %8s %20s %7s %10s %13s %13s %s %7s %7s %10s" % ("M1",
                                                                       "M2",
                                                                       "SignalRegion",
                                                                       "Z_n",
                                                                       var[int(var.find('_'))+1:],
                                                                       "Signal",
                                                                       "Total BG",
                                                                       bgstring,
                                                                       "AccEff",
                                                                       "mt2Eff",
                                                                       var[0:int(var.find('_'))])
    else:
        print "%8s %8s %20s %7s %10s %13s %13s %s %7s %7s" % ("M1",
                                                              "M2",
                                                              "SignalRegion",
                                                              "Z_n",
                                                              var,
                                                              "Signal",
                                                              "Total BG",
                                                              bgstring,
                                                              "AccEff",
                                                              "mt2Eff")

    SRdirname=collection

    sighists=OrderedDict()
    bghists=OrderedDict()
    for topdir in sigfile.GetListOfKeys():
        if topdir.GetName() != SRdirname:
            continue
        for hdir in topdir.ReadObj().GetListOfKeys():
            if hdir.GetName() != histcoll:
                continue
            for SRdir in hdir.ReadObj().GetListOfKeys():
                # if "_inclusive" in SRdir.GetName():
                #     continue
                if onlyInclusive and "_inclusive" not in SRdir.GetName():
                    continue
                for hist in SRdir.ReadObj().GetListOfKeys():
                    if hist.GetName() != "h_"+SRdir.GetName()+"_"+var:
                        continue

                    h=hist.ReadObj()
                    h.Scale(sigscale)
                    sighists[SRdir.GetName()]=h

                    for i,j in bgfiles.iteritems():
                        bghistloc=SRdirname+"/"+histcoll+"/"+h.GetDirectory().GetName()+"/"+h.GetName()
                        if bgfiles[i].IsOpen():
                            if isinstance(h,ROOT.TH2F):
                                bghistsall[i][SRdir.GetName()]=TH2F(bgfiles[i].Get(bghistloc))
                            else:
                                bghistsall[i][SRdir.GetName()]=TH1F(bgfiles[i].Get(bghistloc))
                        else:
                            if isinstance(h,ROOT.TH2F):
                                bghistsall[i][SRdir.GetName()]=TH2F(bgfiles["Bj"].Get(bghistloc))
                            else:
                                bghistsall[i][SRdir.GetName()]=TH1F(bgfiles["Bj"].Get(bghistloc))
                            bghistsall[i][SRdir.GetName()].Reset()

                        # keep one histogram with a running total
                        if SRdir.GetName() not in bghists:
                            if isinstance(h,ROOT.TH2F):
                                bghists[SRdir.GetName()]=TH2F(bgfiles["Bj"].Get(bghistloc))
                            else:
                                bghists[SRdir.GetName()]=TH1F(bgfiles["Bj"].Get(bghistloc))
                        elif bgfiles[i].IsOpen():
                            bghists[SRdir.GetName()].Add(j.Get(bghistloc))
                            
                    bghists[SRdir.GetName()].Scale(bgscale)

    for i,j in sighists.iteritems():
        sumweights=j.GetSumOfWeights()
        if False:
            print "%10s %10d %10f %10d %10f" % (i,
                                                j.GetEntries(),
                                                sumweights,
                                                bghists[i].GetEntries(),
                                                bghists[i].GetSumOfWeights())

        totbins=j.GetNbinsX()
        bincount=0
        sigtotalatmaxsignif=0
        bkgtotalatmaxsignif=0
        signif=0
        maxsignif=-999999
        maxsignifbin=-1
        capsignif=True
        nxbins=0
        nybins=0
        maxsignifbins={}
        if isinstance(j,ROOT.TH1F) and sumweights>2.:
            nxbins=j.GetXaxis().GetNbins()
            for xbin in range(nxbins,0,-1):
                sig_sum=j.Integral(xbin,nxbins)
                bkg_sum=bghists[i].Integral(xbin,nxbins)

                if sig_sum<1.:
                    continue

                # account for more nuanced systematics
                totuncert=float(unc)*bkg_sum
                if doZllCR:
                    Bj_sum=bghistsall["Bj"][i].Integral(xbin,nxbins)*float(bgscale)
                    if Bj_sum>0:
                        Bj_ext_reluncert=((Bj_sum*0.2)**0.5)/(Bj_sum*0.2)
                        Bj_unc_reluncert=0.10
                        Bj_relunc=(Bj_ext_reluncert**2.+Bj_unc_reluncert**2.)**0.5
                        newtotuncert=float(unc)*(bkg_sum-Bj_sum) + Bj_relunc*Bj_sum
                        if newtotuncert<totuncert:
                            totuncert=newtotuncert
                        # print "Old uncert=%1.3f, new=%1.3f, Bj_relunc=%1.2f, Bj_sum=%f" % 
                        #       (float(unc), totuncert/bkg_sum, Bj_relunc, Bj_sum)

                if bkg_sum<0.001:
                    bkg_sum=0.001
                signif = ROOT.RooStats.NumberCountingUtils.BinomialExpZ(sig_sum, bkg_sum, totuncert/bkg_sum)
                if math.isinf(signif) and bkg_sum>1: 
                    signif=0
                elif math.isinf(signif) and sig_sum>1 and bkg_sum<1:
                    signif=1000
                if signif>=maxsignif:
                    maxsignif=signif
                    maxsignifbins[0]=-1
                    maxsignifbins[1]=xbin
                    maxsignifbin=xbin
                    sigtotalatmaxsignif=sig_sum
                    bkgtotalatmaxsignif=bkg_sum
        elif isinstance(j,ROOT.TH2F) and sumweights>2.:
            nxbins=j.GetXaxis().GetNbins()
            nybins=j.GetYaxis().GetNbins()

            sig_int={}
            bkg_int={}
            Bj_int={}
            for xbin in range(0,nxbins+1):
                sig_int[xbin]={}
                bkg_int[xbin]={}
                Bj_int[xbin]={}
                for ybin in range(0,nybins+1):
                    sig_int[xbin][ybin]=j.Integral(xbin,nxbins,ybin,nybins)
                    bkg_int[xbin][ybin]=bghists[i].Integral(xbin,nxbins,ybin,nybins)
                    Bj_int[xbin][ybin]=bghistsall["Bj"][i].Integral(xbin,nxbins,ybin,nybins)*float(bgscale)

            for xbin in range(nxbins,0,-1):
                prev_sig_sum=0
                for ybin in range(nybins,0,-1):
                    sig_sum=sig_int[xbin][ybin]
                    bkg_sum=bkg_int[xbin][ybin]

                    if sig_sum<1. or sig_sum==prev_sig_sum:
                        continue
                    
                    # account for more nuanced systematics
                    totuncert=float(unc)*bkg_sum
                    if doZllCR:
                        Bj_sum=Bj_int[xbin][ybin]
                        if Bj_sum>0:
                            Bj_ext_reluncert=((Bj_sum*0.2)**0.5)/(Bj_sum*0.2)
                            Bj_unc_reluncert=0.10
                            Bj_relunc=(Bj_ext_reluncert**2.+Bj_unc_reluncert**2.)**0.5
                            newtotuncert=float(unc)*(bkg_sum-Bj_sum) + Bj_relunc*Bj_sum
                            if newtotuncert<totuncert:
                                totuncert=newtotuncert
                            
                    if bkg_sum<0.001:
                        bkg_sum=0.001
                    signif = ROOT.RooStats.NumberCountingUtils.BinomialExpZ(sig_sum, bkg_sum, totuncert/bkg_sum)
                    if math.isinf(signif) and bkg_sum>1: 
                        signif=0
                    elif math.isinf(signif) and sig_sum>0. and bkg_sum<0.002:
                        signif=1000
                    if signif>=maxsignif:
                        maxsignif=signif
                        maxsignifbins[0]=ybin
                        maxsignifbins[1]=xbin
                        sigtotalatmaxsignif=sig_sum
                        bkgtotalatmaxsignif=bkg_sum

                    prev_sig_sum=sig_sum

                    if doSuperUpperCuts:
                        # check if we can find a better result by placing upper bounds on the two variables.
                        #xbin=maxsignifbins[1]
                        #ybin=maxsignifbins[0]
                        #maxsignifbins[2]=ybin
                        #maxsignifbins[3]=xbin
                        for xbin2 in range(xbin+1,nxbins+1):
                            prev_sig_sum2=prev_sig_sum
                            for ybin2 in range(ybin+1,nybins+1):
                                bkg_sum2=bkg_int[xbin][ybin]-bkg_int[xbin2][ybin]-bkg_int[xbin][ybin2]+bkg_int[xbin2][ybin2]
                                sig_sum2=sig_int[xbin][ybin]-sig_int[xbin2][ybin]-sig_int[xbin][ybin2]+sig_int[xbin2][ybin2]

                                if sig_sum2<1. or sig_sum2==prev_sig_sum2 or sig_sum2==prev_sig_sum:
                                    continue

                                # account for more nuanced systematics
                                totuncert2=float(unc)*bkg_sum2
                                if doZllCR:
                                    Bj_sum2=Bj_int[xbin][ybin]-Bj_int[xbin2][ybin]-Bj_int[xbin][ybin2]+Bj_int[xbin2][ybin2]
                                    if Bj_sum2>0:
                                        Bj_ext_reluncert=((Bj_sum2*0.2)**0.5)/(Bj_sum2*0.2)
                                        Bj_unc_reluncert=0.10
                                        Bj_relunc=(Bj_ext_reluncert**2.+Bj_unc_reluncert**2.)**0.5
                                        newtotuncert=float(unc)*(bkg_sum-Bj_sum) + Bj_relunc*Bj_sum
                                        if newtotuncert<totuncert2:
                                            totuncert2=newtotuncert

                                if bkg_sum2<0.001:
                                    bkg_sum2=0.001
                                signif2 = ROOT.RooStats.NumberCountingUtils.BinomialExpZ(sig_sum2, bkg_sum2, totuncert2/bkg_sum2)
                                if math.isinf(signif2) and bkg_sum2>1: 
                                    signif2=0
                                elif math.isinf(signif2) and sig_sum2>5. and bkg_sum2<0.002:
                                    signif2=1000
                                elif math.isinf(signif2):
                                    signif2=0
                                if signif2>=maxsignif:
                                    # print "x=%2.1f, y=%2.1f, sigsum=%6.3f, bkgsum=%6.3f, x2=%2.1f, y2=%1.1f, sigsum2=%6.3f, bkgsum2=%6.3f, signif=%1.3f" % (j.GetXaxis().GetBinLowEdge(xbin), j.GetYaxis().GetBinLowEdge(ybin), 
                                    #                                                                                                                         sig_sum, bkg_sum, 
                                    #                                                                                                                         j.GetYaxis().GetBinLowEdge(xbin2), j.GetYaxis().GetBinLowEdge(ybin2),
                                    #                                                                                                                         sig_sum2, bkg_sum2, 
                                    #                                                                                                                         signif2)
                                    maxsignif=signif2
                                    maxsignifbins[0]=ybin
                                    maxsignifbins[1]=xbin
                                    maxsignifbins[2]=ybin2
                                    maxsignifbins[3]=xbin2
                                    sigtotalatmaxsignif=sig_sum2
                                    bkgtotalatmaxsignif=bkg_sum2

                                prev_sig_sum2=sig_sum2

            if doUpperCuts:
                # check if we can find a better result by placing upper bounds on the two variables.
                ybin=maxsignifbins[0]
                xbin=maxsignifbins[1]
                maxsignifbins[2]=nybins
                maxsignifbins[3]=nxbins

                xlow=j.GetXaxis().GetBinLowEdge(xbin)
                ylow=j.GetYaxis().GetBinLowEdge(ybin)
                
                if fixedWindow>0:
                    xbin2=int((xlow+M1TEV*float(fixedWindow))/j.GetXaxis().GetBinWidth(xbin))
                    #print ybin
                    #print ylow
                    #print float(fixedWindow)
                    #print M1TEV
                    #print j.GetYaxis().GetBinWidth(ybin)
                    #print float((ylow+M1TEV*float(fixedWindow))/j.GetYaxis().GetBinWidth(ybin))
                    ybin2=math.ceil((ylow+M1TEV*float(fixedWindow))/j.GetYaxis().GetBinWidth(ybin))+1
                    #print ybin2
                    #print j.GetYaxis().GetBinLowEdge(ybin2)

                    bkg_sum2=bkg_int[xbin][ybin]-bkg_int[xbin2][ybin]-bkg_int[xbin][ybin2]+bkg_int[xbin2][ybin2]
                    sig_sum2=sig_int[xbin][ybin]-sig_int[xbin2][ybin]-sig_int[xbin][ybin2]+sig_int[xbin2][ybin2]

                    totuncert2=float(unc)*bkg_sum2
                    if doZllCR:
                        Bj_sum2=Bj_int[xbin][ybin]-Bj_int[xbin2][ybin]-Bj_int[xbin][ybin2]+Bj_int[xbin2][ybin2]
                        if Bj_sum2>0:
                            Bj_ext_reluncert=((Bj_sum2*0.2)**0.5)/(Bj_sum2*0.2)
                            Bj_unc_reluncert=0.10
                            Bj_relunc=(Bj_ext_reluncert**2.+Bj_unc_reluncert**2.)**0.5
                            newtotuncert=float(unc)*(bkg_sum-Bj_sum) + Bj_relunc*Bj_sum
                            if newtotuncert<totuncert2:
                                totuncert2=newtotuncert

                    if bkg_sum2<0.001:
                        bkg_sum2=0.001
                    signif2 = ROOT.RooStats.NumberCountingUtils.BinomialExpZ(sig_sum2, bkg_sum2, totuncert2/bkg_sum2)

                    maxsignif=signif2
                    maxsignifbins[0]=ybin
                    maxsignifbins[1]=xbin
                    maxsignifbins[2]=ybin2
                    maxsignifbins[3]=xbin2
                    sigtotalatmaxsignif=sig_sum2
                    bkgtotalatmaxsignif=bkg_sum2
                    
                else:
                    for xbin2 in range(xbin+1,nxbins+1):
                        for ybin2 in range(ybin+1,nybins+1):

                            # only scan from 0.1*mass to 2.5*mass in either axis, to prevent
                            # over-optimization and allow for easier checks
                            dx=j.GetXaxis().GetBinLowEdge(xbin2)-xlow
                            dy=j.GetYaxis().GetBinLowEdge(ybin2)-ylow
                            if dx<0.2*M1TEV: # or dx>2.0*M1TEV: 
                                #print "(%4d, %4d) --> (%4d, %4d): Too wide in x" % (xbin,ybin,xbin2,ybin2)
                                continue
                            if dy<0.2*M1TEV: # or dy>2.0*M1TEV: 
                                #print "(%4d, %4d) --> (%4d, %4d): Too wide in y" % (xbin,ybin,xbin2,ybin2)
                                continue

                            bkg_sum2=bkg_int[xbin][ybin]-bkg_int[xbin2][ybin]-bkg_int[xbin][ybin2]+bkg_int[xbin2][ybin2]
                            sig_sum2=sig_int[xbin][ybin]-sig_int[xbin2][ybin]-sig_int[xbin][ybin2]+sig_int[xbin2][ybin2]

                            if sig_sum2<1.: 
                                #print "(%4d, %4d) --> (%4d, %4d): Too little signal" % (xbin,ybin,xbin2,ybin2)
                                continue

                            # account for more nuanced systematics
                            totuncert2=float(unc)*bkg_sum2
                            if doZllCR:
                                Bj_sum2=Bj_int[xbin][ybin]-Bj_int[xbin2][ybin]-Bj_int[xbin][ybin2]+Bj_int[xbin2][ybin2]
                                if Bj_sum2>0:
                                    Bj_ext_reluncert=((Bj_sum2*0.2)**0.5)/(Bj_sum2*0.2)
                                    Bj_unc_reluncert=0.10
                                    Bj_relunc=(Bj_ext_reluncert**2.+Bj_unc_reluncert**2.)**0.5
                                    newtotuncert=float(unc)*(bkg_sum-Bj_sum) + Bj_relunc*Bj_sum
                                    if newtotuncert<totuncert2:
                                        totuncert2=newtotuncert

                            if bkg_sum2<0.001:
                                bkg_sum2=0.001
                            signif2 = ROOT.RooStats.NumberCountingUtils.BinomialExpZ(sig_sum2, bkg_sum2, totuncert2/bkg_sum2)
                            if math.isinf(signif2) and bkg_sum2>1: 
                                signif2=0
                            elif math.isinf(signif2) and sig_sum2>1 and bkg_sum2<1:
                                signif2=1000
                            if signif2>=maxsignif:
                                #print "(%4d, %4d) --> (%4d, %4d): Good!" % (xbin,ybin,xbin2,ybin2)
                                maxsignif=signif2
                                maxsignifbins[0]=ybin
                                maxsignifbins[1]=xbin
                                maxsignifbins[2]=ybin2
                                maxsignifbins[3]=xbin2
                                sigtotalatmaxsignif=sig_sum2
                                bkgtotalatmaxsignif=bkg_sum2
                            #else:
                            #    print "(%4d, %4d) --> (%4d, %4d): No good, best=%f, this=%f" % (xbin,ybin,xbin2,ybin2,maxsignif,signif2)
                            
                                
                    
        if maxsignif<0:
            maxsignif=0
            maxsignifbin=1
            maxsignifbins[0]=1
            maxsignifbins[1]=1
            maxsignifbins[2]=1
            maxsignifbins[3]=1
            sigtotal=sumweights
            bkgtotal=bghists[i].GetSumOfWeights()
            sigtotalatmaxsignif=sigtotal
            bkgtotalatmaxsignif=bkgtotal

        bgprintstring=""
        for k,l in bghistsall.iteritems():
            if isinstance(j,ROOT.TH1F):
                bgprintstring+=" %13.2f" % float(float(bgscale)*(l[i].Integral(maxsignifbin,totbins)))
            elif isinstance(j,ROOT.TH2F):
                if doUpperCuts or doSuperUpperCuts:
                    #print k+"____"+l[i].GetName()
                    #print float(float(bgscale)*(l[i].Integral(maxsignifbins[1],maxsignifbins[3],maxsignifbins[0],maxsignifbins[2])))
                    #print float(bgscale)*l[i].GetSumOfWeights()
                    bgprintstring+=" %13.2f" % float(float(bgscale)*(l[i].Integral(maxsignifbins[1],maxsignifbins[3],maxsignifbins[0],maxsignifbins[2])))
                else:
                    bgprintstring+=" %13.2f" % float(float(bgscale)*(l[i].Integral(maxsignifbins[1],nxbins,maxsignifbins[0],nybins)))
        acceff=float(sigtotalatmaxsignif/gensig)
        if sumweights>0:
            mt2eff=float(sigtotalatmaxsignif/sumweights)
        else:
            mt2eff=0

        if maxsignif>99:
            maxsignif=99
        if isinstance(j,ROOT.TH1F):
            print "%8d %8d %20s %7.4f %10d %13.2f %13.2f %s  %6.5f %6.5f" % (M1, 
                                                                             M2, 
                                                                             i,
                                                                             maxsignif,
                                                                             1000.*(j.GetBinLowEdge(maxsignifbin)), 
                                                                             sigtotalatmaxsignif, 
                                                                             bkgtotalatmaxsignif, 
                                                                             bgprintstring, 
                                                                             acceff,
                                                                             mt2eff)
        elif isinstance(j,ROOT.TH2F):
            if not doUpperCuts and not doSuperUpperCuts and not fixedWindow>0:
                print "%8d %8d %20s %7.4f %10d %13.2f %13.2f %s  %6.5f %6.5f %10d"  % (M1, 
                                                                                       M2, 
                                                                                       i,
                                                                                       maxsignif,
                                                                                       1000.*(j.GetYaxis().GetBinLowEdge(maxsignifbins[0])), 
                                                                                       sigtotalatmaxsignif, 
                                                                                       bkgtotalatmaxsignif, 
                                                                                       bgprintstring, 
                                                                                       acceff,
                                                                                       mt2eff,
                                                                                       1000.*(j.GetXaxis().GetBinLowEdge(maxsignifbins[1]))
                                                                                       )
            else:
                print "%8d %8d %20s %7.4f %10d %13.2f %13.2f %s  %6.5f %6.5f %10d %10d %10d %10d"  % (M1, 
                                                                                                      M2, 
                                                                                                      i,
                                                                                                      maxsignif,
                                                                                                      0, 
                                                                                                      sigtotalatmaxsignif, 
                                                                                                      bkgtotalatmaxsignif, 
                                                                                                      bgprintstring, 
                                                                                                      acceff,
                                                                                                      mt2eff, 
                                                                                                      1000.*(j.GetYaxis().GetBinLowEdge(maxsignifbins[0])), 
                                                                                                      1000.*(j.GetXaxis().GetBinLowEdge(maxsignifbins[1])),
                                                                                                      1000.*(j.GetYaxis().GetBinLowEdge(maxsignifbins[2])),
                                                                                                      1000.*(j.GetXaxis().GetBinLowEdge(maxsignifbins[3])),
                                                                                                      )
#======================================================================




#======================================================================
def main(argv):

    parser = argparse.ArgumentParser(description="Command line arguments")
    parser.add_argument("--infile"       , action='store', default='')
    parser.add_argument("--outfile"      , action='store', default='')
    parser.add_argument("--test"         , action='store_true')
    parser.add_argument("--lumi"         , action='store', default='3000')
    parser.add_argument("--kfact"        , action='store', default='0.95')
    parser.add_argument("--var"          , action='store', default="mT2_leading")
    parser.add_argument("--unc"          , action='store', default="0.20")
    parser.add_argument("--SR"           , action='store', default="htbins")
    parser.add_argument("--energy"       , action='store', default='100')
    parser.add_argument("--collection"   , action='store', default="SRhists")
    parser.add_argument("--flatBGUnc"    , action='store_true')
    parser.add_argument("--doUpperCuts"  , action='store_true')
    parser.add_argument("--doSuperCuts"  , action='store_true')
    parser.add_argument("--onlyInclusive", action='store_true')
    parser.add_argument("--fixedWindow"  , action='store', default=-1)
    args=parser.parse_args()

    doZllCR=False if args.flatBGUnc else True

    analyze(args.infile,
            args.outfile,
            args.test,
            float(args.lumi),
            float(args.kfact),
            args.var,
            args.unc,
            args.SR,
            args.collection,
            args.energy,
            doZllCR,
            args.doUpperCuts,
            args.doSuperCuts,
            args.onlyInclusive,
            args.fixedWindow)


if __name__ == '__main__':
    main(sys.argv[1:])
#======================================================================
