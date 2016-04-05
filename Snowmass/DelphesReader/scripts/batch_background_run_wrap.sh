basedir=/eliza11/atlas/mhance/Snowmass/data

if [[ $1 == "14" || $1 == "14TeV" ]]; then
    energy=14TeV
    bgfile=$basedir/CommonBG/HTBinned/background_crosssections.txt
    delphesversion="Delphes-3.0.9.1"
elif [[ $1 == "33" || $1 == "33TeV" ]]; then
    energy=33TeV
    bgfile=$basedir/CommonBG/HTBinned/background_crosssections_33TeV.txt
    delphesversion="Delphes-3.0.9.2"
elif [[ $1 == "100" || $1 == "100TeV" ]]; then
    energy=100TeV
    bgfile=$basedir/CommonBG/HTBinned/background_crosssections_100TeV.txt
    delphesversion="Delphes-3.0.9.2"
fi

testing=""
if [[ $2 != "" ]]; then
    testing=$2
fi

output=backgrounds_${energy}

mkdir -p $output

# specify this if the input files have units of MeV instead of GeV
# if the input file is already in units of GeV, should be enough
# to just comment this line out, or set an empty string.
# unitsopt="-M"


analysistype="1"
#analysistype="2"
grid_start=1
grid_end=$(grep -v '#' ${bgfile} | wc | awk '{print $1}')

if [[ $testing != "" ]]; then
    grid_start=$testing
    grid_end=$testing
fi

doMain="1"
#    "50PileUp" \
#    "NoPileUp" \
#    "140PileUp" \
for pileup in \
    "NoPileUp" \
    ; do
  
  qsub \
      -N bg_${energy}_${pileup} \
      -v unitsopt="" \
      -v bgfile=${bgfile} \
      -v doMain=${doMain} \
      -v analysistype=${analysistype} \
      -v delphesversion=${delphesversion} \
      -v pileup=$pileup \
      -v test=$testing \
      -v output=$output \
      -t ${grid_start}-${grid_end}:1 \
      -l cvmfs -l h_vmem=4G \
      -l eliza11io=1 \
      -j y \
      scripts/batch_background_run.sh
  
  
  if [[ ${testing} != "" ]]; then
      exit
  fi
  
done
