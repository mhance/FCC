#$ -o batch_logs/batch_$JOB_NAME_$TASK_ID.log

shopt -s expand_aliases

DelphesReaderDir=/eliza11/atlas/mhance/Snowmass/DelphesReader
cd $DelphesReaderDir

cd ../
source setup.sh
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

if [[ $grid == "WinoBino" ]]; then
    line=$(grep $taskindex scan_summary.dat)
    energy="14TEV"
elif [[ $grid == "LHE_GOGO_SQ" ]]; then
    line=$(grep -v '#' scan_summary.dat | head -$taskindex | tail -1)
    line="${line}_14TEV"
    energy="14TEV"
elif [[ $grid == "GO_decoupledSQ_14TeV_LHE_QQALL" ]]; then
    line=$(grep -v '#' scan_summary.dat | head -$taskindex | tail -1)
    line="${line}_14TEV"
    energy="14TEV"
elif [[ $grid == "GO_decoupledSQ_33TeV_LHE_QQALL" ]]; then
    line=$(grep -v '#' scan_summary.dat | head -$taskindex | tail -1)
    line="${line}_33TEV"
    energy="33TEV"
elif [[ $grid == "GO_decoupledSQ_100TeV" ]]; then
    line=$(grep -v '#' scan_summary.dat | head -$taskindex | tail -1)
    line="${line}_100TEV"
    energy="100TEV"
elif [[ $grid == "SQ_decoupledGO_14TeV_LHE_QQALL" ]]; then
    line=$(grep -v '#' scan_summary.dat | head -$taskindex | tail -1)
    line="${line}_14TEV"
    energy="14TEV"
elif [[ $grid == "T1_decoupledGO_100TeV" ]]; then
    line=$(grep -v '#' scan_summary.dat | head -$taskindex | tail -1)
    line="${line}_100TEV"
    energy="100TEV"
elif [[ $grid == GOSQ_masslessN0_* ]]; then
    lineno=$(echo $taskindex | awk '{print (($1-1)*4)+1}')
    line=$(grep -v '#' scan_summary.dat | head -$lineno | tail -1 | sed s_"GOGO\."__g)
    if [[ $grid == *_14TeV ]]; then
	line="${line}_14TEV"
	energy="14TEV"
    elif [[ $grid == *_33TeV ]]; then
	line="${line}_33TEV"
	energy="33TEV"
    elif [[ $grid == *_100TeV ]]; then
	line="${line}_100TEV"
	energy="100TEV"
    fi	
    # need to get rest of cross sections for this grid.
    lineno2=$(echo $taskindex | awk '{print (($1-1)*4)+2}')
    lineno3=$(echo $taskindex | awk '{print (($1-1)*4)+3}')
    lineno4=$(echo $taskindex | awk '{print (($1-1)*4)+4}')
    line2=$(grep -v '#' scan_summary.dat | head -$lineno2 | tail -1 | sed s_"GOGO\."__g)
    line3=$(grep -v '#' scan_summary.dat | head -$lineno3 | tail -1 | sed s_"GOGO\."__g)
    line4=$(grep -v '#' scan_summary.dat | head -$lineno4 | tail -1 | sed s_"GOGO\."__g)
elif [[ $grid == SQ_decoupledGO_* ]]; then
    line=$(grep -v '#' scan_summary.dat | head -$taskindex | tail -1)
    if [[ $grid == *_14TeV ]]; then
	line="${line}_14TEV"
	energy="14TEV"
    elif [[ $grid == *_33TeV ]]; then
	line="${line}_33TEV"
	energy="33TEV"
    elif [[ $grid == *_100TeV ]]; then
	line="${line}_100TEV"
	energy="100TEV"
    fi	
else
    line=$line_from_wrapper
fi
echo $line

mGO=$(echo $line | awk '{print $1}' | sed 's/0*//')
mN1=$(echo $line | awk '{print $2}')
prod=$(echo $line | awk '{print $3}')
decay=$(echo $line | awk '{print $4}')
sigma=$(echo $line | awk '{print $5}')
nevents=$(echo $line | awk '{print $6}')
prefix=$(echo $line | awk '{print $7}' | sed s/"${grid}\."/""/g)

if [[ $grid == GOSQ_masslessN0_* ]]; then
    sigma2=$(echo $line2 | awk '{print $5}')
    sigma3=$(echo $line3 | awk '{print $5}')
    sigma4=$(echo $line4 | awk '{print $5}')
    echo "sigma1 = $sigma"
    echo "sigma2 = $sigma2"
    echo "sigma3 = $sigma3"
    echo "sigma4 = $sigma4"
    sigmatot=$(echo $sigma $sigma2 $sigma3 $sigma4 | awk '{print $1+$2+$3+$4}')
    sigma=$sigmatot
    echo "sigma total = $sigmatot = $sigma"
fi

if [[ $minimum != "" ]]; then
    minopts="-m $minimum"
fi

if [[ $mN1 != "0" && $useclassified != 1 ]]; then
    mN1=$(echo $line | awk '{print $2}' | sed 's/0*//')
fi

echo "$mGO $mN1"

if [[ $useclassified == 1 ]]; then
    masscuts='Z_M>=75&&Z_M<110'
    metcut="MET>150"
    massgap=$((mGO-mN1))
    if (( "${massgap}" < "95" )); then
	masscuts='Z_M<75'
	metcut="MET>150"
    fi

    set -x

    if [[ $variable == "Beate4a" ]]; then
	variable=""
	metcut="MET>150&&W_mT>120"
    elif [[ $variable == "Beate4b" ]]; then
	variable=""
	metcut="MET>150&&W_mT>120&&Z_pt>100"
    elif [[ $variable == "Beate4c" ]]; then
	variable=""
	metcut="MET>300&&W_mT>120&&Z_pt>100"
    fi

    varopts=""
    if [[ $variable != "" ]]; then
	varopts="-v ${variable}"
    fi

    ./bin/choose_operating_point \
	-s ${DelphesReaderDir}/${grid}/${prefix}.classified.signal.root \
	-b ${DelphesReaderDir}/${grid}/${prefix}.classified.background.root \
	-l ${lumi} -c "${metcut}&&${masscuts}" -p 'isoLepJet&&!lowMll&&nBtags==0' \
	${varopts} ${minopts} | tail -1 > optimization.$variable.${prefix}.log

    if [[ $variable == "" ]]; then
	variable="RectCut"
    fi
else
    cut="MET>100&&METsignif>${METsignifcut}&&HTleptons==0"

    if [[ $grid == "T1_decoupledGO_100TeV" ]]; then
	cut="nJets60_eta3p5>3&&MET>100&&METsignif>${METsignifcut}&&nBtags>0&&abs(dPhiMETJet0)>0.5&&abs(dPhiMETJet1)>0.5&&abs(dPhiMETJet2)>0.5"
    fi

    if [[ $grid != SQ_* ]]; then
	cut="nJets60_eta3p5>3&&${cut}"
    fi

    if [[ $doLeadJetPtCut != 0 ]]; then
	cut="${cut}&&(leadJetPt/HTjets_30_3p5)<0.4"
    fi

    echo "Copying signal file to local disk"
    cp ${DelphesReaderDir}/${grid}/delphes.${grid}.${pileup}.${prefix}.root .

    if [[ $grid == "GO_decoupledSQ_14TeV_LHE_QQALL" ]]; then
	case $mGO in
	    315)  LOCS=272.441139;;
	    397)  LOCS=75.639013;;
	    500)  LOCS=20.414622;;
	    629)  LOCS=5.182599;;
	    792)  LOCS=1.203079;;
	    997)  LOCS=0.251926;;
	    1255) LOCS=0.046138;;
	    1580) LOCS=0.007035;;
	    1989) LOCS=0.000838;;
	    2489) LOCS=0.000071;;
	    2989) LOCS=0.000006;;
	    3489) LOCS=0.000001;;
	esac
    elif [[ $grid == "GO_decoupledSQ_33TeV_LHE_QQALL" ]]; then
	case $mGO in 
	    500)  LOCS=239.775034;;
	    629)  LOCS=78.816697;;
	    792)  LOCS=34.429771;;
	    997)  LOCS=10.214843;;
	    1255) LOCS=2.830969;;
	    1580) LOCS=0.722849;;
	    1989) LOCS=0.167331;;
	    2504) LOCS=0.034248;;
	    3152) LOCS=0.003534;;
	    3968) LOCS=0.000506;;
	    4968) LOCS=0.000057;;
	    5968) LOCS=0.000006;;
	esac
    elif [[ $grid == "GO_decoupledSQ_100TeV" ]]; then
	LOCS=$sigma
	case $mGO in
	    1000) sigma=2.050000e+02;;
	    1259) sigma=6.930000e+01;;
	    1585) sigma=2.260000e+01;;
	    1995) sigma=7.050000e+00;;
	    2512) sigma=2.090000e+00;;
	    3162) sigma=5.880000e-01;;
	    3981) sigma=1.550000e-01;;
	    5012) sigma=3.780000e-02;;
	    6310) sigma=8.420000e-03;;
	    7944) sigma=1.660000e-03;;
	    9944) sigma=2.950000e-04;;
	    11944) sigma=6.200000e-05;;
	    13944) sigma=1.460000e-05;;
	    15944) sigma=3.660000e-06;;
	esac
    elif [[ $grid == "SQ_decoupledGO_14TeV_LHE_QQALL" ]]; then
	case $mGO in 
	    315)  LOCS=38.398900;;
	    397)  LOCS=10.920700;;
	    500)  LOCS=2.951870;;
	    629)  LOCS=0.753950;;
	    792)  LOCS=0.175791;;
	    997)  LOCS=0.036489;;
	    1255) LOCS=0.006482;;
	    1580) LOCS=0.000939;;
	    1989) LOCS=0.000100;;
	    2489) LOCS=0.000008;;
	    2989) LOCS=0.000001;;
	    3489) LOCS=0.00000008;;
	esac
    elif [[ $grid == "SQ_decoupledGO_14TeV" ]]; then
	LOCS=$sigma
	case $mGO in 
 	    315)  sigma=6.390000e+01;;
	    397)  sigma=1.910000e+01;;
	    500)  sigma=5.380000e+00;;
	    629)  sigma=1.420000e+00;;
	    792)  sigma=3.440000e-01;;
	    997)  sigma=7.460000e-02;;
	    1255) sigma=1.410000e-02;;
	    1580) sigma=2.200000e-03;;
	    1989) sigma=2.680000e-04;;
	    2489) sigma=2.410000e-05;;
	    2989) sigma=2.250000e-06;;
	esac
    elif [[ $grid == "SQ_decoupledGO_33TeV" ]]; then
	LOCS=$sigma
	case $mGO in 
	    500)  sigma=5.840000e+01;;
	    629)  sigma=1.900000e+01;;
	    792)  sigma=5.870000e+00;;
	    997)  sigma=1.720000e+00;;
	    1255) sigma=4.760000e-01;;
	    1580) sigma=1.220000e-01;;
	    1989) sigma=2.850000e-02;;
	    2504) sigma=5.910000e-03;;
	    3152) sigma=1.050000e-03;;
	    3968) sigma=1.530000e-04;;
	    4968) sigma=1.760000e-05;;
	    5968) sigma=2.260000e-06;;
	esac
    elif [[ $grid == "SQ_decoupledGO_100TeV" ]]; then
	LOCS=$sigma
	case $mGO in 
	    1000) sigma=3.060000e+01;;
	    1259) sigma=1.050000e+01;;
	    1585) sigma=3.490000e+00;;
	    1995) sigma=1.110000e+00;;
	    2512) sigma=3.380000e-01;;
	    3162) sigma=9.750000e-02;;
	    3981) sigma=2.630000e-02;;
	    5012) sigma=6.580000e-03;;
	    6310) sigma=1.490000e-03;;
	    7944) sigma=2.980000e-04;;
	    9944) sigma=5.420000e-05;;
	    11944) sigma=1.200000e-05;;
	    13944) sigma=3.000000e-06;;
	    15944) sigma=8.070000e-07;;
	esac
    elif [[ $grid == "T1_decoupledGO_100TeV" ]]; then
	LOCS=$sigma
	case $mGO in 
	    1000) sigma=3.960000e+00;;
	    1259) sigma=1.380000e+00;;
	    1585) sigma=5.360000e-01;;
	    1995) sigma=2.727000e-01;;
	    2512) sigma=8.040000e-02;;
	    3162) sigma=2.247000e-02;;
	    3981) sigma=5.880000e-03;;
	    5012) sigma=1.419000e-03;;
	    6310) sigma=3.090000e-04;;
	    7944) sigma=5.910000e-05;;
	    9944) sigma=9.900000e-06;;
	    11944) sigma=2.001000e-06;;
	esac
    fi

    echo $mGO
    echo $LOCS
    echo $sigma

    if [[ $pileup == "NoPileUpKiel" || $grid == GOSQ_masslessN0_* ]]; then
	kfact="1"
    else
	kfact=$(echo "here" | awk -v LOCS=$LOCS -v CS=$sigma '{print CS/LOCS}')
    fi
    echo "Going to apply a k-factor of $kfact"

    # need to define background samples
    if [[ $pileup == "NoPileUpKiel" ]]; then
	echo "Copying background files to local disk"
	cp ${DelphesReaderDir}/backgrounds/delphes.tt-4p.${energy}.NoPileUp.root .
	cp ${DelphesReaderDir}/backgrounds/delphes.Bj-4p.${energy}.NoPileUp.root .
	echo "Done copying bg files"
	bgopts="-b delphes.tt-4p.${energy}.NoPileUp.root -b delphes.Bj-4p.${energy}.NoPileUp.root";
    else
	echo "Copying background files to local disk"
	cp ${DelphesReaderDir}/backgrounds/delphes.tt-4p.${energy}.${pileup}.root .
	cp ${DelphesReaderDir}/backgrounds/delphes.Bj-4p.${energy}.${pileup}.root .
	echo "Done copying bg files"
	bgopts="-b delphes.tt-4p.${energy}.${pileup}.root -b delphes.Bj-4p.${energy}.${pileup}.root";
    fi

    echo $variable
    echo ${variable} | sed s="___"=" "=g
    varopts=""
    for i in $(echo ${variable} | sed s="___"=" "=g); do
	if [[ $grid == "T1_decoupledGO_100TeV" && $i == "HTjets_30_3p5" ]]; then
	    varopts="${varopts} -v ${i}" # +HTleptons
	else
	    varopts="${varopts} -v ${i}"
	fi
    done
    echo $varopts

    set -x
    ./bin/choose_operating_point \
	-l ${lumi} -c ${cut} -d \
	-k $kfact \
	${varopts} \
	${minopts} \
	-s delphes.${grid}.${pileup}.${prefix}.root \
	-u ${uncertainty} \
	${bgopts} \
	| tee optimization_full.$variable.${prefix}.log 
    tail -1 optimization_full.$variable.${prefix}.log > optimization.$variable.${prefix}.log 
fi

set +x

echo "Printing results of optimization."
cat optimization.$variable.${prefix}.log

grep "Could not find an optimal working point" optimization.$variable.${prefix}.log
success=$?

if [[ $success == 0 ]]; then
    var1=$variable
    cut1=0
    sig=0
    bkg=0
    eff=0
    sgn=0
    rej=0
    cut=0
    preeff=0
    prerej=0
    var2=""
    cut2=""
else
    var1=$(cat optimization.$variable.${prefix}.log | awk '{print $1}')
    cut1=$(cat optimization.$variable.${prefix}.log | awk '{print $2}')
    sig=$(cat optimization.$variable.${prefix}.log | awk '{print $3}')
    bkg=$(cat optimization.$variable.${prefix}.log | awk '{print $4}')
    sgn=$(cat optimization.$variable.${prefix}.log | awk '{print $5}')
    eff=$(cat optimization.$variable.${prefix}.log | awk '{print $6}')
    rej=$(cat optimization.$variable.${prefix}.log | awk '{print $7}')
    preeff=$(cat optimization.$variable.${prefix}.log | awk '{print $8}')
    prerej=$(cat optimization.$variable.${prefix}.log | awk '{print $9}')
    var2=$(cat optimization.$variable.${prefix}.log | awk '{print $10}')
    cut2=$(cat optimization.$variable.${prefix}.log | awk '{print $11}')
fi
    
echo "$prefix   $sig" > signal.txt
echo "Printing signals"
cat signal.txt

if [[ "$bkg" == "0" ]]; then
    bkg="0.1"
fi
echo "WZ        $bkg" > background.txt
echo "Printing backgrounds"
cat background.txt

echo "Running with lumi=$lumi"

set -x

./scripts/make_limits.py \
    --background=background.txt \
    --signal=signal.txt \
    --uncertainty=${uncertainty} > $output

echo $?

set +x

if [[ 1 == 0 ]]; then
    echo "Printing setup_limit.log"
    cat tmp_limits/setup_limit.log
    
    echo "Printing limit.log"
    cat tmp_limits/limit.log
    
    echo "Printing XML file"
    cat tmp_limits/DP_onechan_limit.xml
    
    echo "Printing XML file"
    cat tmp_limits/top_limit.xml
    
    echo "Printing setup_discovery.log"
    cat tmp_limits/setup_discovery.log
    
    echo "Printing limit.log"
    cat tmp_limits/limit.log
    
    echo "Printing XML file"
    cat tmp_limits/DP_onechan_discovery.xml
    
    echo "Printing XML file"
    cat tmp_limits/top_discovery.xml
fi

echo "Printing limits output file $output"
cat $output

m2sig=$(  cat $output | tail -1 | sed s_","_" "_g | sed s_"("_" "_g | awk '{print $5}')
m1sig=$(  cat $output | tail -1 | sed s_","_" "_g | sed s_"("_" "_g | awk '{print $8}')
median=$( cat $output | tail -1 | sed s_","_" "_g | sed s_"("_" "_g | awk '{print $12}')
p1sig=$(  cat $output | tail -1 | sed s_","_" "_g | sed s_"("_" "_g | awk '{print $15}')
p2sig=$(  cat $output | tail -1 | sed s_","_" "_g | sed s_"("_" "_g | awk '{print $18}')
p0=$(     cat $output | tail -1 | sed s_","_" "_g | sed s_"("_" "_g | awk '{print $21}')
p0sig=$(  cat $output | tail -1 | sed s_","_" "_g | sed s_"("_" "_g | awk '{print $22}')

#if [[ $p0sig == "nan" ]]; then
#    p0sig=10
#fi

# now, reformat output to be a bit more useful
#
#printf "prefix  x     y    CS       var       cut      nsig      nbkg      pre-eff  pre-rej effic    rej     signif   -2sig    -1sig   median   +1sig   +2sig   p0     p0sig    var2   cut2\n "
printf "prefix              x     y    CS       var       cut      nsig      nbkg      pre-eff  pre-rej effic    rej     signif   -2sig    -1sig   median   +1sig   +2sig   p0     p0sig    var2   cut2\n "

printf "%20s    %6d   %6d  %1.4e    %20s      %012.4f  %012.4f   %012.4f   %08.4f   %1.4e   %08.4f   %1.4e   %08.4f   %08.4f   %08.4f  %08.4f   %08.4f  %08.4f  %12.4e  %08.4f    %30s   %012.4f\n" \
       $prefix  $mGO  $mN1 ${sigma} $var1     $cut1    $sig      $bkg      $preeff  $prerej $eff     $rej    $sgn     $m2sig   $m1sig  $median  $p1sig  $p2sig  $p0    $p0sig   $var2  $cut2 >> $DelphesReaderDir/${grid}/${output}

printf "%20s    %6d   %6d  %1.4e    %20s      %012.4f  %012.4f   %012.4f   %08.4f   %1.4e   %08.4f   %1.4e   %08.4f   %08.4f   %08.4f  %08.4f   %08.4f  %08.4f  %12.4e %08.4f   %30s   %012.4f\n" \
       $prefix  $mGO  $mN1 ${sigma} $var1     $cut1    $sig      $bkg      $preeff  $prerej $eff     $rej    $sgn     $m2sig   $m1sig  $median  $p1sig  $p2sig  $p0    $p0sig   $var2  $cut2
