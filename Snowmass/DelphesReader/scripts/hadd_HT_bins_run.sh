#$ -o batch_logs/batch_$JOB_NAME_$TASK_ID.log

shopt -s expand_aliases

export ATLAS_LOCAL_ROOT_BASE="/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase"
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh >> /dev/null 2>&1
source ${ATLAS_LOCAL_ROOT_BASE}/packageSetups/atlasLocalROOTSetup.sh --skipConfirm

taskindex=$SGE_TASK_ID

DelphesReaderDir=/eliza11/atlas/mhance/Snowmass/DelphesReader
cd $DelphesReaderDir

outputpoint=$(echo $outputs | awk -v taskindex=$taskindex '{print $taskindex}')
echo $outputpoint

rm $grid/${outputbase}.${outputpoint}${outputpost}.root

inputfiles=""
for i in $(/bin/ls $grid/${outputbase}.${outputpoint}*${outputpost}*.root); do
    inputfiles="${inputfiles} $i"
done

echo $inputfiles

hadd $grid/${outputbase}.${outputpoint}${outputpost}.root ${inputfiles}

echo "done."

