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

cd $TMPDIR

pwd
/bin/ls -ltrh

taskindex=$SGE_TASK_ID

if [[ $grid == SQ_decoupledGO* ]]; then
    signaltaskindex=$taskindex
fi

set -x
./scripts/mt2rerun.sh $taskindex $grid $pileup $signalgrid $signaltaskindex
set +x
