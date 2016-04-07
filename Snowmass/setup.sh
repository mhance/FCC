export ATLAS_LOCAL_ROOT_BASE="/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase"
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh >> /dev/null 2>&1
source ${ATLAS_LOCAL_ROOT_BASE}/packageSetups/atlasLocalROOTSetup.sh --skipConfirm --rootVersion="5.34.25-x86_64-slc6-gcc48-opt"

export D3PDANALYSIS_BASE=$(echo $(readlink -f ${BASH_SOURCE[0]}) | sed s_/setup.sh__)

export PYTHONPATH=$PYTHONPATH:${D3PDANALYSIS_BASE}/externals/argparse

