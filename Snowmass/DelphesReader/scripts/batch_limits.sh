shopt -s expand_aliases

pushd /eliza18/atlas/mhance/D3PDReaderAnalyses_8TeV
source setup.sh MultiLepton
popd

DelphesReaderDir=/eliza18/atlas/mhance/Snowmass/DelphesReader
cd $DelphesReaderDir

cp -a scripts $TMPDIR
cp -a bin $TMPDIR
cp -a share $TMPDIR
cp -a ${grid}/${background} $TMPDIR
cp -a ${grid}/${signal} $TMPDIR

cd $TMPDIR

pwd
/bin/ls -ltrh

allevents=""
allsignal=""
while read signal events; do
    let gridpoints=$((gridpoints+1))
    if [[ $allevents == "" ]]; then
	allevents=$events
	allsignal=$signal
    else
	allevents="$allevents $events"
	allsignal="$allsignal $signal"
    fi
done < ${signal}

taskindex=$(($SGE_TASK_ID))

sig=$(echo $allsignal | awk -v taskindex=$taskindex '{print $taskindex}')
evt=$(echo $allevents | awk -v taskindex=$taskindex '{print $taskindex}')

echo "$sig   $evt" > signal.txt

cat signal.txt

./scripts/make_limits.py \
    --background ${background} \
    --signal signal.txt \
    --lumi=${lumi} >> $DelphesReaderDir/${grid}/${output}

echo "Printing setup.log"
cat tmp_limits/setup.log

echo "Printing limit.log"
cat tmp_limits/limit.log


