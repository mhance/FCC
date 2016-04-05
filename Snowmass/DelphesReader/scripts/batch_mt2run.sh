#$ -o batch_logs/batch_$JOB_NAME_$TASK_ID.log

shopt -s expand_aliases

DelphesReaderDir=/eliza11/atlas/mhance/Snowmass/DelphesReader
cd $DelphesReaderDir

cd ../
export ATLAS_LOCAL_ROOT_BASE="/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase"
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh >> /dev/null 2>&1
source ${ATLAS_LOCAL_ROOT_BASE}/packageSetups/atlasLocalROOTSetup.sh --skipConfirm --rootVersion="5.34.05-x86_64-slc5-gcc4.3"
export D3PDANALYSIS_BASE=${PWD}
export PYTHONPATH=$PYTHONPATH:${D3PDANALYSIS_BASE}/externals/argparse
cd DelphesReader

cp -a scripts $TMPDIR
cp -a bin $TMPDIR
cp -a share $TMPDIR

basedir=${DelphesReaderDir}/../data
#signaldir=Signal/SUSY1/$grid/Delphes-3.0.9.1
signaldir=Signal/SUSY/$grid/Delphes-3.0.9.1
cp -a $basedir/$signaldir/scan_summary_summary.dat $TMPDIR/scan_summary.dat

cd $TMPDIR

pwd
/bin/ls -ltrh

taskindex=$SGE_TASK_ID

Energy=$(echo $grid | sed s/"_"/" "/g | awk '{print $NF}')
energy=$(echo $Energy | sed s_"TEV"_"TeV"_g)
Energy=$(echo $energy | sed s_"TeV"_"TEV"_g)
sqrts=$(echo $energy | sed s_"TeV"__g)
if [[ $grid == SQ_decoupledGO_* || $grid == GO_decoupledSQ_100TeV ]]; then
    line=$(grep -v '#' scan_summary.dat | head -$taskindex | tail -1)
    echo $line
    prefix=$(echo $line | awk '{print $7}' | sed s/"${grid}\."/""/g)
    prefix="${prefix}_${Energy}."

    inputpath="${DelphesReaderDir}/${grid}"
    outputdir=/eliza11/atlas/mhance/Snowmass/DelphesReader/${grid}/
elif [[ $grid == GO_decoupledSQ_14* ||  $grid == GO_decoupledSQ_33* ]]; then
    line=$(grep -v '#' scan_summary.dat | head -$taskindex | tail -1)
    echo $line
    prefix=$(echo $line | awk '{print $7}' | sed s/"${grid}\."/""/g)

    inputpath="${DelphesReaderDir}/${grid}"
    outputdir=/eliza11/atlas/mhance/Snowmass/DelphesReader/${grid}/

    # hadd any binned files together
    sumfilename=delphes.${grid}.${pileup}.${prefix}_${Energy}.root
    binfilenames="delphes.${grid}.${pileup}.${prefix}_*_${Energy}.root"
    hadd -f ${inputpath}/$sumfilename ${inputpath}/$binfilenames

    prefix="${prefix}_${Energy}."
else
    # must be a background sample
    inputpath="/eliza11/atlas/mhance/Snowmass/DelphesReader/backgrounds_${energy}"
    outputdir=${inputpath}
fi

output=hist.${grid}.${pileup}.${prefix}root

set -x

python \
    -m cProfile \
    ./scripts/mt2analysis.py \
    --infile=${inputpath}/delphes.${grid}.${pileup}.${prefix}root \
    --outfile=${output} \
    --energy=${sqrts}

set +x

cp -p $output ${outputdir}
