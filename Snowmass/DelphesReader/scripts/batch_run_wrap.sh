grid=$1
if [[ $grid == "" ]]; then
    echo "Must specify a grid set."
    exit
fi

testing=""
if [[ $2 != "" ]]; then
    testing=$1
fi

basedir=/eliza11/atlas/mhance/Snowmass/data

mkdir -p batch_submit_logs
mkdir -p ./$grid

grid_start=1
#grid_end=$(grep -v "#" $basedir/Signal/SUSY1/$grid/Delphes-3.0.9.1/scan_summary_summary.dat | wc | awk '{print $1}')
grid_end=$(grep -v "#" $basedir/Signal/SUSY/$grid/Delphes-3.0.9.1/scan_summary.dat | wc | awk '{print $1}')
analysistype="1"

if [[ "$grid" == GOSQ_masslessN0_* ]]; then
    grid_end=$(grep -v "#" $basedir/Signal/SUSY/$grid/Delphes-3.0.9.1/scan_summary.dat | wc | awk '{print $1/4}')
    batchscript=batch_run_GOSQ.sh
else
    batchscript=batch_run.sh
fi


queueopts=""
if [[ $testing != "" ]]; then
    grid_end=1
    queueopts="-l debug=1"
fi

doMain="1"

#    "50PileUp" \
#    "NoPileUpKiel" \
#    "140PileUp" \
for pileup in \
    "NoPileUp" \
    ; do
    
  qsub \
      -N DRmain_${grid}_${pileup} \
      -v grid=${grid} \
      -v unitsopt="" \
      -v doMain=${doMain} \
      -v pileup=$pileup \
      -v analysistype=${analysistype} \
      -t ${grid_start}-${grid_end}:1 \
      -l cvmfs -l h_vmem=2G \
      -l eliza11io=1 \
      -j y \
      ${queueopts} \
      scripts/${batchscript}

  if [[ ${testing} != "" ]]; then
      exit
  fi
  
done
