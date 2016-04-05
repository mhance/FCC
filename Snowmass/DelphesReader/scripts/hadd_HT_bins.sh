grid=$1

testing=""
if [[ $2 != "" ]]; then
    testing=$2
fi

basedir=/eliza11/atlas/mhance/Snowmass/data

mkdir -p batch_submit_logs

# now, figure out the files that we need to create
outputs=$(grep -v "#" $basedir/Signal/SUSY/$grid/Delphes-3.0.9.1/scan_summary.dat | awk '{print $NF}' | sed s+"_"+" "+g | sed s+"-"+" "+g | awk '{print $1}' | sort -n | uniq)

numoutputs=$(grep -v "#" $basedir/Signal/SUSY/$grid/Delphes-3.0.9.1/scan_summary.dat | awk '{print $NF}' | sed s+"_"+" "+g | sed s+"-"+" "+g | awk '{print $1}' | sort -n | uniq | wc | awk '{print $1}')


if [[ $testing != "" ]]; then
    numoutputs=1
fi

if [[ $grid == *33TeV* ]]; then
    suffix="_33TEV"
elif [[ $grid == *14TeV* ]]; then
    suffix="_14TEV"
fi

#    "50PileUp" \
for pileup in \
    "NoPileUp" \
    "140PileUp" \
    ; do
    
  qsub \
      -N "merge_$pileup" \
      -v pileup=$pileup \
      -v test=$testing \
      -v outputbase="delphes.$grid.$pileup" \
      -v outputpost=${suffix} \
      -v outputs="${outputs}" \
      -v grid=$grid \
      -t 1-${numoutputs}:1 \
      -l cvmfs -l h_vmem=2G \
      -l eliza11io=1 \
      -j y \
      scripts/hadd_HT_bins_run.sh


  if [[ ${testing} != "" ]]; then
      exit
  fi

done
