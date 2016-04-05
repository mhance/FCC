#!/usr/bin/env python

import os,sys,commands,subprocess
import argparse
import glob

def GetBestLine(gridpoint,input):
    bestline=""
    bestsigma=0
    limit=9999999
    for file in input:
        filehandle=open(file)
        for line in filehandle.xreadlines():
            myline = line
            myline = myline.replace(","," ")
            myline = myline.replace("("," ")
            myline = myline.replace(")"," ")
            
            gridpointresults = myline.split()
            if len(gridpointresults) < 2 or gridpointresults[0] == '#':
                continue
            if gridpointresults[0] != gridpoint:
                continue
            # sigma is 21, limit is 11
            if len(gridpointresults)<20:
                print line
                exit(0)
            if line=="" or gridpointresults[19] > bestsigma:
                bestline=line.rstrip()+" "+file
                bestsigma = gridpointresults[19]
                continue
        filehandle.close()
    return bestline
            

def main(argv):
    parser = argparse.ArgumentParser(description="Command line arguments")
    parser.add_argument("--input"   , action='append')
    args=parser.parse_args()

    inputfiles=[]
    for i in args.input:
        if '*' in i:
            inputfiles+=glob.glob(i)
        else:
            inputfiles.append(i)

    print inputfiles
            
    # use first file as template
    firstfile=open(inputfiles[0])
    for line in firstfile.xreadlines():
        gridpointresults = line.split()
        if len(gridpointresults) < 2 or gridpointresults[0] == '#':
            continue
        bestline = GetBestLine(gridpointresults[0],inputfiles)
        print "%s" % bestline.rstrip()
    

if __name__ == '__main__':
    main(sys.argv[1:])
