#!/usr/bin/env python
"""
 Author: Sourabh Dube
 Make XML files for one channel, with the right uncertainties
"""

import os,sys,commands,subprocess
import argparse

import ROOT
from ROOT import TH1F,TFile

def SetupWorkspace(backgrounds,
                   sign,
                   data,
                   lumiuncer,
                   discovery,
                   uncertainty):

    if discovery:
        opprefix = "DP_onechan_discovery_"
        rootfile = "counting_exp_data_discovery_DP.root"
        chanfile = "DP_onechan_discovery.xml"
    else:
        opprefix = "DP_onechan_limit_"
        rootfile = "counting_exp_data_limit_DP.root"
        chanfile = "DP_onechan_limit.xml"

    #
    # Write Main Top XML file
    #
    mainXMLdata = """\
  <!DOCTYPE Combination  SYSTEM "../share/HistFactorySchema.dtd">
  
  <Combination OutputFilePrefix="./tmp_limits_results/%s" >
  
  <Input>./tmp_limits/%s</Input>
  
  <Measurement Name="DPLSMM" Lumi="1." LumiRelErr="%f" BinLow="0" BinHigh="2" >
    <POI>mu</POI>
  </Measurement>
    
    
  </Combination>
    
    """ % (opprefix, chanfile, lumiuncer)

    if discovery:
        script = open('tmp_limits/top_discovery.xml','w')
    else:
        script = open('tmp_limits/top_limit.xml','w')
    script.write(mainXMLdata)
    script.close()
    
    #
    # Write Channel XML
    #
    chanXMLdata = """\
<!DOCTYPE Channel  SYSTEM '../share/HistFactorySchema.dtd'>

<Channel Name="channel1" InputFile="./tmp_limits_data/%s">
  <Data HistoName="data" HistoPath="" />
  <Sample Name="signal" HistoPath="" HistoName="signal">
    <NormFactor Name="mu"        High="20."     Low="0."    Val="1." Const="True" />
  </Sample>
""" % rootfile

#     <OverallSys Name="lumi"      High="1.028"  Low="0.972" />
#     <OverallSys Name="PDFacc"    High="1.05"   Low="0.95"  />
#     <OverallSys Name="acc_truth" High="1.15"   Low="0.85"  />

    setupWSfile = TFile("tmp_limits_data/%s" % rootfile,"RECREATE")
        
    doSingleBGModel=False
    if not doSingleBGModel:
        for key,value in backgrounds.iteritems():
            chanXMLdata+="""\
  <Sample Name="%s" HistoPath="" NormalizeByTheory="True" HistoName="%s">
    <OverallSys Name="%s" Low="%f" High="%f"/>
  </Sample>
""" % (key,key,key+"_norm",1.-float(uncertainty),1.+float(uncertainty))
            hist = TH1F(key,key+" hist",1,0,1)
            hist.Fill(0.5,value)
            hist.Write(key)
    else:
        BGtotal=0
        for key,value in backgrounds.iteritems():
            BGtotal+=value
        key="BG"
        hist = TH1F(key,key+" hist",1,0,1)
        hist.Fill(0.5,BGtotal)
        hist.Write(key)
        chanXMLdata+="""\
  <Sample Name="%s" HistoPath="" NormalizeByTheory="True" HistoName="%s">
    <OverallSys Name="%s" Low="%f" High="%f"/>
  </Sample>
""" % (key,key,key+"_norm",1.-float(uncertainty),1.+float(uncertainty))
  
    
    chanXMLdata+="""\
</Channel>
  """

    script = open('tmp_limits/'+chanfile,'w')
    script.write(chanXMLdata)
    script.close()
    
    hist = TH1F("signal", "signal hist", 1,0,1)
    hist.Fill(0.5,sign)
    hist.Write("signal")

    hist = TH1F("data", "data hist", 1,0,1)
    hist.Fill(0.5,data)
    hist.Write("data")

    setupWSfile.Close()

    if discovery:
        os.system("hist2workspace tmp_limits/top_discovery.xml > tmp_limits/setup_discovery.log 2>&1")
    else:
        os.system("hist2workspace tmp_limits/top_limit.xml > tmp_limits/setup_limit.log 2>&1")
    

def run_limit(line,
              backgrounds,
              lumiuncer,
              toys,
              points,
              mulow,
              muhigh,
              uncertainty):

    cleanup = """\
    mkdir -p tmp_limits
    mkdir -p tmp_limits_data
    mkdir -p tmp_limits_results
    rm -f tmp_limits/*
    rm -f tmp_limits_data/*
    rm -f tmp_limits_results/*
    """
    os.system(cleanup)

    fullcls = 0
    if toys>0:
        fullcls = 1

    # figure out how much signal we have
    words_list = line.split()
    label=words_list[0]
    sign=float(words_list[1])

    # and how much background
    totalbg=0
    for key,value in backgrounds.iteritems():
        totalbg = totalbg+value
    data=totalbg


    # quick check to see if we should even bother with limits.
    if sign <= 1.:
        if (fullcls==0):
            print "%s : -2sig = %1.4f, -1sig = %1.4f, Median Exp = %1.4f, +1sig = %1.4f, +2sig = %1.4f, p0 = %1.3e (%1.4f sigma)" % (label,
                                                                                                                                     10,
                                                                                                                                     10,
                                                                                                                                     10,
                                                                                                                                     10,
                                                                                                                                     10,
                                                                                                                                     10,
                                                                                                                                     0);
        else:
            print data,sign,'==RESFRQ==',10,10,10,10,10,10

        return
            

    scale=1.
    # This does nasty things in the WinoBino grid...  found it necessary for
    # the GMSB signals, but not so much here.
    if False:
        if sign > 1000*totalbg:
            scale = 3000.
        elif sign > 100*totalbg:
            scale = 300.
        elif sign > 10*totalbg:
            scale = 30.
        elif sign > totalbg:
            scale = 3.
    sign = sign/scale

    print "setting up workspace with %f signal events %f background events." % (sign,totalbg)
    SetupWorkspace(backgrounds,sign,data,lumiuncer,False,uncertainty)
    SetupWorkspace(backgrounds,sign,data+sign,lumiuncer,True,uncertainty)

    cmd2 = """\
  ./bin/runCEws -f %i -t %i -p %i -l %f -h %f >& tmp_limits/limit.log
  """ % (fullcls,toys,points,mulow,muhigh)

    #print cmd2
    os.system(cmd2)

    cmd3 = """\
  grep "==RESULT==" tmp_limits/limit.log
  """
    cmd4 = """\
   grep "computed upper limit" tmp_limits/limit.log | awk '{print $6}'
  """
    cmd5 = """\
   grep "expected limit (median) " tmp_limits/limit.log | awk '{print $4}'
  """
    cmd6 = """\
   grep "expected limit (+1 sig) " tmp_limits/limit.log | awk '{print $5}'
  """
    cmd7 = """\
   grep "expected limit (-1 sig) " tmp_limits/limit.log | awk '{print $5}'
  """
    cmd8 = """\
   grep "expected limit (+2 sig) " tmp_limits/limit.log | awk '{print $5}'
  """
    cmd9 = """\
   grep "expected limit (-2 sig) " tmp_limits/limit.log | awk '{print $5}'
  """

    if (fullcls==0):
        # os.system(cmd3)
        p = os.popen(cmd3)
        res = p.readline()
        ressplit = res.split()
        p.close()
        
        printEventLimits=True
        if not printEventLimits:
            scale=(1/scale)*100.
        else:
            scale=(1/scale)

        p = os.popen("grep \"DISCOVERY\" tmp_limits/limit.log" )
        res2 = p.readline()
        res2split = res2.split()
        p.close()

        if len(res2split) > 2:
            if float(res2split[1]) < 1e-20:
                res2split[2] = "10"

        if len(res2split) > 2 and len(ressplit) > 6:
            print "%s : -2sig = %1.4f, -1sig = %1.4f, Median Exp = %1.4f, +1sig = %1.4f, +2sig = %1.4f, p0 = %1.3e (%1.4f sigma)" % (label,
                                                                                                                                     scale*float(ressplit[6]),
                                                                                                                                     scale*float(ressplit[4]),
                                                                                                                                     scale*float(ressplit[2]),
                                                                                                                                     scale*float(ressplit[3]),
                                                                                                                                     scale*float(ressplit[5]),
                                                                                                                                     float(res2split[1]),
                                                                                                                                     float(res2split[2]));
    else:
        p = os.popen(cmd4)
        res1 = (p.readline()).rstrip()
        p.close()
        p = os.popen(cmd5)
        res2 = (p.readline()).rstrip()
        p.close()
        p = os.popen(cmd6)
        res3 = (p.readline()).rstrip()
        p.close()
        p = os.popen(cmd7)
        res4 = (p.readline()).rstrip()
        p.close()
        p = os.popen(cmd8)
        res5 = (p.readline()).rstrip()
        p.close()
        p = os.popen(cmd9)
        res6 = (p.readline()).rstrip()
        p.close()
        print data,sign,'==RESFRQ==',res1,res2,res3,res4,res5,res6


def SetupWorkspaceOpt(optresults,
                      lumiuncer,
                      discovery,
                      uncertainty,
                      flatBGUnc,
                      useSingleBGModel):

    if discovery:
        opprefix = "susy_discovery_"
    else:
        opprefix = "susy_limit_"

    #
    # Write Main Top XML file
    #
    if discovery:
        script = open('tmp_limits/top_discovery.xml','w')
    else:
        script = open('tmp_limits/top_limit.xml','w')

    script.write("""\
  <!DOCTYPE Combination  SYSTEM "../share/HistFactorySchema.dtd">
  
  <Combination OutputFilePrefix="./tmp_limits_results/%s" >
""" % opprefix)

    # --------------------------------------------------------
    # parse optresults
    goodchannels=0
    forcetoys=False
    for line in open(optresults,"r"):
        l=line.split()

        if float(l[3])<0.5:
            continue
        else:
            goodchannels=goodchannels+1

        rootfile=opprefix+"_chan_"+l[2]+".root"
        chanfile="chan_%s.xml" % l[2]
        script.write("""  
  <Input>./tmp_limits/%s</Input>
""" % chanfile)
        
        # write the channel data
        chan=open("./tmp_limits/%s" % chanfile, 'w')
        chan.write("""\
<!DOCTYPE Channel  SYSTEM '../share/HistFactorySchema.dtd'>

<Channel Name="channel_%s" InputFile="./tmp_limits_data/%s">
  <Data HistoName="data" HistoPath="" />
  <Sample Name="signal" HistoPath="" HistoName="signal">
    <NormFactor Name="mu"        High="20."     Low="0."    Val="1." Const="True" />
  </Sample>
""" % (l[2],rootfile))

        setupWSfile = TFile("tmp_limits_data/%s" % rootfile,"RECREATE")
        
        bglabels=["Bj", "tt", "tB", "tj", "ttB"]
        if "100TeV" in optresults:
            bglabels+=["QCD"]
        totalbg=0.
        for i in range(len(bglabels)):
            # only do this if the backgrounds are non-zero
            if float(l[i+7]) > 0.00:
                bgval=float(l[i+7])
                totalbg+=bgval

                if useSingleBGModel:
                    continue
                hist = TH1F(bglabels[i],bglabels[i]+" hist",1,0,1)
                if bgval<0.001:
                    bgval=0.001
                #hist.Fill(0.5,bgval)
                hist.SetBinContent(1,bgval)
                hist.Write(bglabels[i])

                chan.write("""\
  <Sample Name="%s" HistoPath="" NormalizeByTheory="True" HistoName="%s">
""" % (bglabels[i],bglabels[i]))
                if bglabels[i]!="Bj" or flatBGUnc:
                    chan.write("""\
    <OverallSys Name="%s" Low="%f" High="%f"/>
""" % (bglabels[i]+"_norm",1.-float(uncertainty),1.+float(uncertainty)))
                else:
                    reluncZll=((bgval*0.5)**0.5)/(bgval*0.5)
                    
                    if ((reluncZll**2.+(float(uncertainty)/2.)**2.)**0.5)<float(uncertainty):
                        chan.write("""\
    <OverallSys Name="%s_bin_%s" Low="%f" High="%f"/>
    <OverallSys Name="%s" Low="%f" High="%f"/>
""" % (bglabels[i]+"_Zll",l[2],1.-reluncZll,1.+reluncZll, bglabels[i]+"_norm",1.-float(uncertainty)/2.,1.+float(uncertainty)/2.))
                    else:
                        chan.write("""\
    <OverallSys Name="%s" Low="%f" High="%f"/>
""" % (bglabels[i]+"_norm",1.-float(uncertainty),1.+float(uncertainty)))
                        
                chan.write("""\
  </Sample>
""")
        if useSingleBGModel:
            hist = TH1F("BG","BG"+" hist",1,0,1)
            hist.SetBinContent(1,totalbg)
            hist.Write("BG")

            chan.write("""\
  <Sample Name="%s" HistoPath="" NormalizeByTheory="True" HistoName="%s">
""" % ("BG","BG"))
            chan.write("""\
    <OverallSys Name="%s" Low="%f" High="%f"/>
""" % ("BG"+"_norm",1.-float(uncertainty),1.+float(uncertainty)))
            chan.write("""\
  </Sample>
""")
            
                    
        hist = TH1F("signal", "signal hist", 1,0,1)
        #hist.Fill(0.5,float(l[5]))
        hist.SetBinContent(1,float(l[5]))
        hist.Write("signal")

        hist = TH1F("data", "data hist", 1,0,1)
        if not discovery:
            #hist.Fill(0.5,totalbg)
            hist.SetBinContent(1,totalbg)
            hist.SetBinError(1,totalbg**0.5)
        else:
            #hist.Fill(0.5,totalbg+float(l[5]))
            hist.SetBinContent(1,(totalbg+float(l[5])))
            hist.SetBinError(1,(totalbg+float(l[5]))**0.5)
        hist.Write("data")
        setupWSfile.Close()

        chan.write("""\
</Channel>
  """)
        chan.close()
        if float(l[3])>1.0 and (float(l[5])<5 or totalbg<5):
            forcetoys=True
    # --------------------------------------------------------

    script.write("""  
  <Measurement Name="DPLSMM" Lumi="1." LumiRelErr="%f" BinLow="0" BinHigh="2" >
    <POI>mu</POI>
  </Measurement>
    
  </Combination>
    """ % (lumiuncer))

    script.close()
    

    if discovery:
        os.system("hist2workspace tmp_limits/top_discovery.xml > tmp_limits/setup_discovery.log 2>&1")
    else:
        os.system("hist2workspace tmp_limits/top_limit.xml > tmp_limits/setup_limit.log 2>&1")
    
    return goodchannels,forcetoys
    

def run_limit_opt(optresultsfile,
                  lumiuncer,
                  toys,
                  points,
                  mulow,
                  muhigh,
                  uncertainty,
                  flatBGUnc,
                  useSingleBGModel):

    cleanup = """\
    mkdir -p tmp_limits
    mkdir -p tmp_limits_data
    mkdir -p tmp_limits_results
    rm -f tmp_limits/*
    rm -f tmp_limits_data/*
    rm -f tmp_limits_results/*
    """
    os.system(cleanup)

    goodchannels,forcetoys=SetupWorkspaceOpt(optresultsfile,lumiuncer,False,uncertainty,flatBGUnc,useSingleBGModel)
    SetupWorkspaceOpt(optresultsfile,lumiuncer, True,uncertainty,flatBGUnc,useSingleBGModel)

    fullcls = 0
    if forcetoys and toys<1000 and False:
        toys=1000
        if points>20:
            points=20
    if toys>0:
        fullcls = 1

    if goodchannels>0:
        cmd2 = """\
  ./bin/runCEws -f %i -t %i -p %i -l %f -h %f -L tmp_limits_results/susy_limit__combined_DPLSMM_model.root -D tmp_limits_results/susy_discovery__combined_DPLSMM_model.root -n combined >& tmp_limits/limit.log
  """ % (fullcls,toys,int(points),mulow,muhigh)
        
        print cmd2
        os.system(cmd2)
    else:
        cmd2="echo \"==RESULT== 0.0  0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0\" > tmp_limits/limit.log"
        os.system(cmd2)
        cmd2="echo \"==DISCOVERY== 0.5 0.0\" >> tmp_limits/limit.log"
        os.system(cmd2)

    cmd3 = """\
  grep "==RESULT==" tmp_limits/limit.log
  """
    cmd4 = """\
   grep "computed upper limit" tmp_limits/limit.log | awk '{print $6}'
  """
    cmd5 = """\
   grep "expected limit (median) " tmp_limits/limit.log | awk '{print $4}'
  """
    cmd6 = """\
   grep "expected limit (+1 sig) " tmp_limits/limit.log | awk '{print $5}'
  """
    cmd7 = """\
   grep "expected limit (-1 sig) " tmp_limits/limit.log | awk '{print $5}'
  """
    cmd8 = """\
   grep "expected limit (+2 sig) " tmp_limits/limit.log | awk '{print $5}'
  """
    cmd9 = """\
   grep "expected limit (-2 sig) " tmp_limits/limit.log | awk '{print $5}'
  """

    # os.system(cmd3)
    p = os.popen(cmd3)
    res = p.readline()
    ressplit = res.split()
    p.close()
    
    scale=1.
    printEventLimits=True
    if not printEventLimits:
        scale=(1/scale)*100.
    else:
        scale=(1/scale)

    p = os.popen("grep \"DISCOVERY\" tmp_limits/limit.log" )
    res2 = p.readline()
    res2split = res2.split()
    p.close()
    
    if len(res2split) > 2:
        if float(res2split[1]) < 1e-20:
            res2split[2] = "10"

    if len(res2split) > 2 and len(ressplit) > 6:
        print "%s : -2sig = %1.4f, -1sig = %1.4f, Median Exp = %1.4f, +1sig = %1.4f, +2sig = %1.4f, p0 = %1.3e (%1.4f sigma)" % ("dummy",
                                                                                                                                 scale*float(ressplit[6]),
                                                                                                                                 scale*float(ressplit[4]),
                                                                                                                                 scale*float(ressplit[2]),
                                                                                                                                 scale*float(ressplit[3]),
                                                                                                                                 scale*float(ressplit[5]),
                                                                                                                                 float(res2split[1]),
                                                                                                                                 float(res2split[2]));
        
        

def main(argv):

    parser = argparse.ArgumentParser(description="Command line arguments")
    parser.add_argument("--background"   , action='store', default="")
    parser.add_argument("--toys"         , action='store', default=0)
    parser.add_argument("--signal"       , action='store', default="")
    parser.add_argument("--mulow"        , action='store', default=0)
    parser.add_argument("--muhigh"       , action='store', default=5)
    parser.add_argument("--points"       , action='store', default=100)
    parser.add_argument("--lumiUnc"      , action='store', default=.028)
    parser.add_argument("--uncertainty"  , action='store', default=0.20)
    parser.add_argument("--prefix"       , action='store', default="test")
    parser.add_argument("--optresults"   , action='store', default="")
    parser.add_argument("--flatBGUnc"    , action='store_true')
    parser.add_argument("--singleBGModel", action='store_true')
    args=parser.parse_args()
    
    if args.optresults != "":
        run_limit_opt(args.optresults,
                      args.lumiUnc,
                      args.toys,
                      args.points,
                      args.mulow,
                      args.muhigh,
                      args.uncertainty,
                      args.flatBGUnc,
                      args.singleBGModel)
    else:
        backgrounds={}
        bgfile = open(args.background)
        for bg in bgfile.xreadlines():
            bgsplit = bg.split()
            if len(bgsplit) < 2:
                continue
            backgrounds[bgsplit[0]] = float(bgsplit[1])

        sigfile = open(args.signal)
        for line in sigfile.xreadlines():
            run_limit(line,
                      backgrounds,
                      args.lumiUnc,
                      args.toys,
                      args.points,
                      args.mulow,
                      args.muhigh,
                      args.uncertainty)


if __name__ == '__main__':
    main(sys.argv[1:])
