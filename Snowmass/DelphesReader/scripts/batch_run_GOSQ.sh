#$ -o batch_logs/batch_$JOB_NAME_$TASK_ID.log

shopt -s expand_aliases

export ATLAS_LOCAL_ROOT_BASE="/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase"
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh >> /dev/null 2>&1
source ${ATLAS_LOCAL_ROOT_BASE}/packageSetups/atlasLocalROOTSetup.sh --skipConfirm


DelphesReaderDir=/eliza11/atlas/mhance/Snowmass/DelphesReader
cd $DelphesReaderDir

/bin/ls -ltrha

basedir=../data

#signaldir=Signal/SUSY1/$grid/Delphes-3.0.9.1
#scan_summary=$basedir/$signaldir/scan_summary_summary.dat

signaldir=Signal/SUSY/$grid/Delphes-3.0.9.1
scan_summary=$basedir/$signaldir/scan_summary.dat

point=""
echo $SGE_TASK_ID
for taskindex in $(echo $SGE_TASK_ID | awk '{print (($1-1)*4)+1 " " (($1-1)*4)+2 " " (($1-1)*4)+3 " " (($1-1)*4)+4}'); do
    echo $taskindex

    line=$(grep -v "#" $scan_summary | head -${taskindex} | tail -1)
    
    echo $line
    
    mGO=$(echo $line | awk '{print $1}')
    mN1=$(echo $line | awk '{print $2}')
    prod=$(echo $line | awk '{print $3}')
    decay=$(echo $line | awk '{print $4}')
    sigma=$(echo $line | awk '{print $5}')
    nevents=$(echo $line | awk '{print $6}')
    prefix=$(echo $line | awk '{print $7}' | sed s/"${grid}\."/""/g)
    
    signalfile=$basedir/$signaldir/$pileup/$prefix.q.N00001.Delphes3.root
    echo $signalfile
    ls $signalfile
    
    if [[ $doMain != "" ]]; then
	fixedsigma=$(echo $sigma | awk '{print $1*1000.}')
	
	rm $grid/delphes.${grid}.${prefix}.root
	
	if [[ $grid == *33TeV* && $prefix != "*_33TEV" ]]; then
	    prefix="${prefix}_33TEV"
	elif [[ $grid == *14TeV* && $prefix != "*_14TEV" ]]; then
	    prefix="${prefix}_14TEV"
	elif [[ $grid == *100TeV* && $prefix != "*_100TEV" ]]; then
	    prefix="${prefix}_100TEV"
	fi
	
	SLACopt="-S"
	
	set -x
	
	./bin/DRMain \
	    -f $signalfile \
	    -s "$grid" \
	    -t "$pileup.$prefix.${prod}" \
	    -c $fixedsigma \
	    -o $grid \
	    -T ${analysistype} \
	    ${SLACopt}
	
	set +x

    fi

    point=$prefix
done

point=$(echo $point | sed s_"\."_" "_g | awk '{print $2 "." $3}')

# now add all the output files together
hadd -f ${grid}/delphes.${grid}.${pileup}.${point}.root ${grid}/delphes.${grid}.${pileup}.*.${point}.*.root

MVA_variables=""
MVA_spectators=""
if [[ $grid == "WinoBino" ]]; then
    MVA_variables="-v MET -v HTjets -v m_eff -v pt_l0 -v pt_l2 -v Z_pt -v W_mT"
    MVA_spectators="-p isoLepJet -p lowMll -p nBtags -p nJets -p nLeptons"
    if (( "${massgap}" < "95" )); then
	MVA_variables="${MVA_variables} -v Z_M"
    fi
fi

# can also run the TMVA classification/training if we've already done the background
if [[ $background != "" ]]; then
    rm $grid/tmva.${prefix}.root
    rm weights/MVA_${prefix}_BDT.weights.xml
    ./bin/train_MVA \
	-s $grid/delphes.${grid}.${prefix}.root \
	-b ${background} \
	-o $grid/tmva.${prefix}.root \
	-t ${prefix} \
	-c "${metcut}&&${masscuts}" \
	${MVA_variables} \
	${MVA_spectators}

    rm 	$grid/${prefix}.classified.signal.root
    ./bin/classify \
	-s $grid/delphes.${grid}.${prefix}.root \
	-o $grid/${prefix}.classified.signal.root \
	-m "BDT" \
	-w weights/MVA_${prefix}_BDT.weights.xml \
	${MVA_variables} \
	${MVA_spectators}
    
    rm 	$grid/${prefix}.classified.background.root
    ./bin/classify \
	-s ${background} \
	-o $grid/${prefix}.classified.background.root \
	-m "BDT" \
	-w weights/MVA_${prefix}_BDT.weights.xml \
	${MVA_variables} \
	${MVA_spectators}
fi

/bin/ls -ltrha
