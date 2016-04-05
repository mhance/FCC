shopt -s expand_aliases

pushd /eliza18/atlas/mhance/D3PDReaderAnalyses_8TeV
source setup.sh MultiLepton
popd

jobNumber=$(($SGE_TASK_ID-1))

if [[ $dsid == "" ]]; then
    echo "Must set DSID via an environment variable!"
    exit
else
    echo "using input file $mass"
fi

DelphesRunnerDir=/eliza18/atlas/mhance/Snowmass/DelphesRunner
cd $DelphesRunnerDir

topdir=.
pointoutput=mc12_14TeV.${dsid}
finaldelphesoutputdir=$PWD/$topdir/batch_output/delphes/$pointoutput
finallogfiledir=$PWD/$topdir/batch_output/logs/$pointoutput

mkdir -p $finaldelphesoutputdir
mkdir -p $finallogfiledir

cd $topdir

# copy the run cards
cp cards/* $TMPDIR
    
# copy the data
#cp /eliza18/atlas/mhance/athena/AtlasProduction-17.2.7.7/run_herwig_C1N2/batch_output/HEPMC/mc12_14TeV.${dsid}/* $TMPDIR
cp /eliza18/atlas/mhance/athena/AtlasProduction-17.2.7.7/run_herwig_WinoBino/batch_output/HEPMC/mc12_8TeV.${dsid}/* $TMPDIR

pushd $TMPDIR

/bin/ls -ltrha

rm -f input_files.txt
for i in $(/bin/ls -1 events.*.gz); do
    gunzip $i
    echo $i | sed s_".gz"_""_g >> input_files.txt
done
cat input_files.txt

$DelphesRunnerDir/../externals/Delphes/DelphesHepMC config_file.tcl delphes.$dsid.root $(cat input_files.txt)

cp delphes.${dsid}.root        $finaldelphesoutputdir/
cp delphes.${dsid}_run.log     $finallogfiledir/
    
/bin/ls -ltrha
