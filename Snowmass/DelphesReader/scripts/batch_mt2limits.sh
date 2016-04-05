#$ -o batch_logs/batch_$JOB_NAME_$TASK_ID.log

shopt -s expand_aliases

DelphesReaderDir=/eliza11/atlas/mhance/Snowmass/DelphesReader
cd $DelphesReaderDir

cd ../

export ATLAS_LOCAL_ROOT_BASE="/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase"
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh >> /dev/null 2>&1
source ${ATLAS_LOCAL_ROOT_BASE}/packageSetups/atlasLocalROOTSetup.sh --skipConfirm --rootVersion="5.34.05-x86_64-slc5-gcc4.3"
source ${ATLAS_LOCAL_ROOT_BASE}/packageSetups/atlasLocalPythonSetup.sh 2.7.4-x86_64-slc6-gcc48
export D3PDANALYSIS_BASE=${PWD}
export PYTHONPATH=$PYTHONPATH:${D3PDANALYSIS_BASE}/externals/argparse

cd DelphesReader

workdir=$TMPDIR/$grid/$SGE_TASK_ID
mkdir -p $workdir
rm -f $workdir/*

cp -a scripts $workdir
cp -a bin $workdir
cp -a share $workdir

cd $workdir

pwd
/bin/ls -ltrh

./scripts/mt2optimize_wrap.sh ${grid} ${SGE_TASK_ID} "batch"

