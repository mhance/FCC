#$ -o batch_logs/batch_$JOB_NAME_$TASK_ID.log

shopt -s expand_aliases

export ATLAS_LOCAL_ROOT_BASE="/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase"
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh >> /dev/null 2>&1
source ${ATLAS_LOCAL_ROOT_BASE}/packageSetups/atlasLocalROOTSetup.sh --skipConfirm --rootVersion="5.34.05-x86_64-slc5-gcc4.3"

taskindex=$SGE_TASK_ID

DelphesReaderDir=/eliza11/atlas/mhance/Snowmass/DelphesReader
cd $DelphesReaderDir

/bin/ls -ltrha

basedir=../data

line=$(grep -v '#' $bgfile | head -${taskindex} | tail -1)
echo $line

process=$(echo $line | awk '{print $1}')
sigma=$(echo $line | awk '{print $2}')

fixedsigma=$(echo $sigma | awk '{print $1*1000.*.96}')

ls $basedir/CommonBG/HTBinned/$delphesversion/$pileup/${process}/

fileopts=""
for i in $(find $basedir/CommonBG/HTBinned/$delphesversion/$pileup/${process}/ -name "*.root"); do
    fileopts="$fileopts -f $i"
    if [[ $test != "" && $test != $taskindex ]]; then
	break
    fi
done
if [[ $process == QCD* || $process == bb* ]]; then
    fileopts="$fileopts -N"
fi

echo "0" > $TMPDIR/LSP_masses.txt
cat $basedir/Signal/SUSY/SQ_decoupledGO_100TeV/Delphes-3.0.9.1/scan_summary_summary.dat | awk '{print $2}' | sort -n | uniq >> $TMPDIR/LSP_masses.txt

fileopts="${fileopts} -m $TMPDIR/LSP_masses.txt"

set -x

if [[ $doMain != "" ]]; then
    echo "here2"
    ./bin/DRMain \
	-s "${process}" \
	-t "$pileup" \
	-c $fixedsigma \
	-o "$output" \
	-T ${analysistype} \
	${fileopts}

fi

set +x

/bin/ls -ltrha
