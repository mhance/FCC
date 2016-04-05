#$ -o batch_logs/batch_$JOB_NAME_$TASK_ID.log

shopt -s expand_aliases

taskindex=$SGE_TASK_ID

DelphesReaderDir=/export/share/diskvault/mhance/FCC/DelphesReader
if [[ $taskindex != "" ]]; then
    export ATLAS_LOCAL_ROOT_BASE="/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase"
    source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh >> /dev/null 2>&1
    source ${ATLAS_LOCAL_ROOT_BASE}/packageSetups/atlasLocalROOTSetup.sh --skipConfirm --rootVersion="5.34.05-x86_64-slc5-gcc4.3"
    cd $DelphesReaderDir
else
    taskindex=$1
    grid="GO_decoupledSQ_100TeV_LHE_TTALL"
    unitsopt=""
    doMain=1
    pileup="NoPileUp"
    analysistype="1"
fi

/bin/ls -ltrha

basedir=../data

#signaldir=Signal/SUSY1/$grid/Delphes-3.0.9.1
#scan_summary=$basedir/$signaldir/scan_summary_summary.dat

signaldir=Signal/SUSY/$grid/Delphes-3.0.9.1
scan_summary=$basedir/$signaldir/scan_summary.dat

line=$(grep -v "#" $scan_summary | head -${taskindex} | tail -1)

echo $line

mGO=$(echo $line | awk '{print $1}')
mN1=$(echo $line | awk '{print $2}')
prod=$(echo $line | awk '{print $3}')
decay=$(echo $line | awk '{print $4}')
sigma=$(echo $line | awk '{print $5}')
nevents=$(echo $line | awk '{print $6}')
prefix=$(echo $line | awk '{print $7}' | sed s/"${grid}\."/""/g)

#signalfile=$basedir/$signaldir/$pileup/GOGO_decoupledSQ.GOGO.${prefix}_${pileup}.root
SLACopt=""
if [[ $grid == "GO_decoupledSQ_14TeV_LHE_QQALL" ]]; then
    signalfile=$basedir/$signaldir/$pileup/GOGO_decoupledSQ.GOGO.${prefix}_${pileup}.root
elif [[ $grid == "GO_decoupledSQ_33TeV_LHE_QQALL" ]]; then
    signalfile=$basedir/$signaldir/$pileup/GOGO-decoupledSQ.GOGO.${prefix}_${pileup}.root
elif [[ $grid == "GO_decoupledSQ_100TeV_LHE_TTALL" ]]; then
    signalfile=$basedir/$signaldir/$pileup/GOGO-decoupledSQ.${prefix}_${pileup}.root
elif [[ $grid == "GO_decoupledSQ_100TeV" ]]; then
    signalfile=$basedir/$signaldir/$pileup/GOGO-decoupledSQ.${prefix}.Delphes3.root
    SLACopt="-S"
elif [[ $grid == "SQ_decoupledGO_14TeV_LHE_QQALL" ]]; then
    signalfile=$basedir/$signaldir/$pileup/SQSQB-decoupledGO.${prefix}_${pileup}.root
elif [[ $grid == "SQ_decoupledGO_14TeV" ]]; then
    signalfile=$basedir/$signaldir/$pileup/SQSQB-decoupledGO.${prefix}.Delphes3.root
    SLACopt="-S"
elif [[ $grid == "SQ_decoupledGO_33TeV" ]]; then
    signalfile=$basedir/$signaldir/$pileup/SQSQB-decoupledGO.${prefix}.Delphes3.root
    SLACopt="-S"
elif [[ $grid == "SQ_decoupledGO_100TeV" ]]; then
    signalfile=$basedir/$signaldir/$pileup/SQSQB-decoupledGO.${prefix}.Delphes3.root
    echo "0" > $TMPDIR/LSP_masses.txt
    cat $basedir/$signaldir/scan_summary_summary.dat | awk '{print $2}' | sort -n | uniq >> $TMPDIR/LSP_masses.txt
    SLACopt="-S -m $TMPDIR/LSP_masses.txt"
elif [[ $grid == "T1_decoupledGO_100TeV" ]]; then
    signalfile=$basedir/$signaldir/$pileup/T1T1.${prefix}.Delphes3.root
    SLACopt="-S"
elif [[ $grid == "T1_decoupledGO_100TeV_linearScan_forCompressed" ]]; then
    #pushd $basedir/$signaldir/$pileup/
    #rm -f delphes_T1_decoupledGO_100TeV.NoPileUp.${prefix}_100TEV_1.root 
    #hadd -f delphes_T1_decoupledGO_100TeV.NoPileUp.${prefix}_100TEV_1.root delphes*.${prefix}_100TEV_*.root
    #popd
    signalfile=$basedir/$signaldir/$pileup/delphes_T1_decoupledGO_100TeV.NoPileUp.${prefix}_100TEV_1.root
    SLACopt="-S"
elif [[ $grid == "T1_decoupledGO_100TeV_linearScan" ]]; then
    signalfile=$basedir/$signaldir/$pileup/delphes_T1_decoupledGO_100TeV.NoPileUp.${prefix}_100TEV_1.root
    SLACopt=""
elif [[ $grid == "T1_decoupledGO_100TeV_more" ]]; then
    signalfile=$basedir/$signaldir/$pileup/delphes_T1_decoupledGO_100TeV.NoPileUp.${prefix}_100TEV_1.root
    SLACopt="-S"
fi

echo $signalfile
ls $signalfile

if [[ $doMain != "" ]]; then
    fixedsigma=$(echo $sigma | awk '{print $1*1000.}')
    if [[ $fixedsigma == "" || $fixedsigma == "0" ]]; then
	fixedsigma=1
    fi

    rm -f $grid/delphes.${grid}.${prefix}.root

    if [[ $grid == *33TeV* && $prefix != "*_33TEV" ]]; then
	prefix="${prefix}_33TEV"
    elif [[ $grid == *14TeV* && $prefix != "*_14TEV" ]]; then
	prefix="${prefix}_14TEV"
    elif [[ $grid == *100TeV* && $prefix != "*_100TEV" ]]; then
	prefix="${prefix}_100TEV"
    fi

    if [[ $pileup == "NoPileUpKiel" ]]; then
	SLACopt="-S"
    fi

    set -x

    ./bin/DRMain \
	-f $signalfile \
	-s "$grid" \
	-t "$pileup.$prefix" \
	-c $fixedsigma \
	-o $grid \
	-T ${analysistype} \
	${SLACopt}

    set +x

    # in case we have, for instance, separate simulation files for positive
    # and negative charginos
    sigma2=$(echo $line | awk '{print $8}')
    nevents2=$(echo $line | awk '{print $9}')
    prefix2=$(echo $line | awk '{print $10}' | sed s/"${grid}\."/""/g)
    hasTwoSubsets=0
    if [[ $sigma2 != "" && $nevents2 != "" && $prefix2 != "" ]]; then
	fixedsigma2=$(echo $sigma2 | awk '{print $1*1000.}')

	rm $grid/delphes.${grid}.${prefix2}.root

	echo $basedir/$grid/Decayed/Events/$grid.$prefix.Delphes3.root
	ls $basedir/$grid/Decayed/Events/$grid.$prefix.Delphes3.root

	./bin/DRMain \
	    -f $basedir/$grid/Decayed/Events/$grid.$prefix2.Delphes3.root \
	    -s "$grid" \
	    -t "$prefix2" \
	    -c $fixedsigma2 \
	    -o $grid \
	    -T ${analysistype} \
	    ${unitsopt} \
	    ${SLACopt}

	hadd \
	    -f $grid/delphes.${grid}.${prefix}.tmp.root \
	    $grid/delphes.${grid}.${prefix}.root \
	    $grid/delphes.${grid}.${prefix2}.root \

	mv $grid/delphes.${grid}.${prefix}.tmp.root $grid/delphes.${grid}.${prefix}.root
	rm $grid/delphes.${grid}.${prefix2}.root
    fi

fi


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
