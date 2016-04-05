
grid=$1

unc="0.20"
pileup="NoPileUp"

DelphesReaderDir=/eliza11/atlas/mhance/Snowmass/DelphesReader
basedir=${DelphesReaderDir}/../data
signaldir=Signal/SUSY/$grid/Delphes-3.0.9.1
summary=${basedir}/${signaldir}/scan_summary_summary.dat

taskindex=$2
batchmode=1
if [[ "$3" == "" ]]; then
    if [[ $2 == "" ]]; then
	taskindex=4
    fi
    batchmode=0
    echo "Running in local mode"
fi

line=""
if [[ $grid == SQ_decoupledGO_* || $grid == GO_decoupledSQ_* ]]; then
    line=$(grep -v '#' ${summary} | head -$taskindex | tail -1)
    if [[ $grid == *_14TeV ]]; then
	energy="14TeV"
	Energy="14TEV"
    elif [[ $grid == *_33TeV ]]; then
	energy="33TeV"
	Energy="33TEV"
    elif [[ $grid == *_100TeV ]]; then
	energy="100TeV"
	Energy="100TEV"
    fi
    line="${line}_${Energy}"
else
    echo "Can't process grid=${grid}"
    exit
fi
echo $line

mGO=$(echo $line | awk '{print $1}' | sed 's/0*//')
mN1=$(echo $line | awk '{print $2}')
prod=$(echo $line | awk '{print $3}')
decay=$(echo $line | awk '{print $4}')
sigma=$(echo $line | awk '{print $5}')
nevents=$(echo $line | awk '{print $6}')
prefix=$(echo $line | awk '{print $7}' | sed s/"${grid}\."/""/g)

if [[ $mN1 != "0" && $useclassified != 1 ]]; then
    mN1=$(echo $line | awk '{print $2}' | sed 's/0*//')
fi

echo "$mGO $mN1"

infile=hist.${grid}.${pileup}.${prefix}.root
echo $infile
if [[ $grid == "SQ_decoupledGO_100TeV" ]]; then
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
elif [[ $grid == GO_decoupledSQ_14TeV* ]]; then
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
elif [[ $grid == GO_decoupledSQ_33TeV* ]]; then
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
fi

echo $mGO
echo $LOCS
echo $sigma

kfact=$(echo "here" | awk -v LOCS=$LOCS -v CS=$sigma '{print CS/LOCS}')
echo "Going to apply a k-factor of $kfact"

if [[ "${batchmode}" == "1" && "1" == "1" ]]; then
    echo "Copying signal file to local disk"
    mkdir -p ${grid}
    cp ${DelphesReaderDir}/${grid}/${infile} ./${grid}

    echo "Linking to BG files on eliza"
    ln -s ${DelphesReaderDir}/backgrounds_${energy} .

elif [[ "${batchmode}" == "1" ]]; then
    echo "Copying signal file to local disk"
    mkdir -p ${grid}
    cp ${DelphesReaderDir}/${grid}/${infile} ./${grid}

    echo "Copying background files to local disk"
    mkdir -p backgrounds_${energy}/
    cp ${DelphesReaderDir}/backgrounds_${energy}/hist.*-4p_${Energy}.NoPileUp.root ./backgrounds_${energy}/
    echo "Done copying bg files"
fi

logfilebase=$(echo $infile | sed s_"\.root"_""_g | sed s_"hist\."_"log\."_g)
logfilebase=${grid}/${logfilebase}

limitinputbase=$(echo $infile | sed s_"\.root"_""_g | sed s_"hist\."_"limit\."_g)
limitinputbase=${grid}/${limitinputbase}

#var1="mT2_leading"
if [[ $var != "" ]]; then
    var1=$var
elif [[ $grid == SQ_decoupledGO_* ]]; then
    var1="MET_mT2"
else
    var1="MET_mT2"
fi
var1="HT_MET"
#var1="MET"

#doUpperCuts=1
doUpperCuts=0
doSuperCuts=0
#doFixedWindow="1.0"
doFixedWindow=""
doFlatUnc=1
forceinclusive=1
onlyinclusive=1
doSingleBGModel=0

for j in "SRhists_old" "SRhists" "SRhists_lownjets" "SRhists_mednjets" "SRhists_highnjets" "SRhists_suprnjets"; do
    # if [[ $var1 != "MET_mT2" && ${j} != "SRhists" ]]; then
    # 	continue
    # fi

    for doSingleBGModel in 0 1; do

	singleBGModelOpts=""
	bgmodeltag=""

	if [[ $doSingleBGModel == 1 ]]; then
	    singleBGModelOpts="--singleBGModel"
	    bgmodeltag="singleBGModel."
	fi

	for i in "htbins"; do # "htmetbins_20" "htmetbins_30" "htmetbins_40" "htmetbins_50"
	    uppercut=""
	    uppercutopts=""
	    if [[ $doUpperCuts == 1 ]]; then
		uppercut="uppercuts."
		uppercutopts="--doUpperCuts"
	    fi
	    supercut=""
	    supercutopts=""
	    if [[ $doSuperCuts == 1 ]]; then
		uppercut="supercuts."
		supercutopts="--doSuperCuts"
	    fi
	    fixedWindow=""
	    fixedWindowOpts=""
	    if [[ $doFixedWindow != "" ]]; then
		fixedWindow="fixedWindow_$doFixedWindow."
		fixedWindowOpts="--fixedWindow $doFixedWindow"
	    fi
	    flatunc=""
	    flatuncopts=""
	    if [[ $doFlatUnc == 1 ]]; then
		flatunc="flatunc."
		flatuncopts="--flatBGUnc"
	    fi
	    onlyincoptions=""
	    if [[ $onlyinclusive == 1 ]]; then
		onlyincoptions="--onlyInclusive"
	    fi

	    logfilename=$logfilebase.${var1}.${uppercut}${fixedWindow}${bgmodeltag}${flatunc}unc_${unc}.SR_${i}.${j}.log
	    limitinput=$limitinputbase.${var1}.${uppercut}${fixedWindow}${bgmodeltag}${flatunc}unc_${unc}.SR_${i}.${j}.input.log
	    limitoutput=$limitinputbase.${var1}.${uppercut}${fixedWindow}${bgmodeltag}${flatunc}unc_${unc}.SR_${i}.${j}.output.log
	    finallimitoutput=limit.${var1}.${uppercut}${fixedWindow}${bgmodeltag}${flatunc}unc_${unc}.SR_${i}.${j}.output.log
	    set -x
	    ./scripts/mt2optimize.py \
		--infile "${grid}/${infile}" \
		--unc ${unc} \
		--SR ${i} \
		--kfact "${kfact}"  \
		--var="${var1}" \
		--energy=$(echo $energy | sed s_"TeV"__g | sed s_"TEV"__g) \
		--collection=${j} \
		${flatuncopts} \
		${uppercutopts} \
		${supercutopts} \
		${onlyincoptions} \
		${fixedWindowOpts} \
		> $logfilename
	    set +x
	    ls $logfilename
	    cat $logfilename
	    if [[ "${batchmode}" == "1" ]]; then
		cp -p $logfilename $DelphesReaderDir/${grid}/
	    fi

	# now, extract the Z_n significances
	    SRbins=13
	    total=0
	    if [[ $onlyinclusive != 1 ]]; then
		for k in $(tail -${SRbins} $logfilename | awk '{print $4}'); do
		    total=$(bc <<< "$total+$k*$k")
		done
		total=$(bc <<< "sqrt($total)")
		echo "$i with collection $j has signif=$total"
		tailnum=$((SRbins+1))
	    else
		tailnum=1
		total=$(tail -1 $logfilename | awk '{print $4}')
	    fi

	    cut1=$(tail -${tailnum} $logfilename | head -1 | awk '{print $5}')
	    sig=$(tail -${tailnum} $logfilename | head -1 | awk '{print $6}')
	    bkg=$(tail -${tailnum} $logfilename | head -1 | awk '{print $7}')
	    preeff=$(tail -${tailnum} $logfilename | head -1 | awk '{print $13}')
	    prerej="0.0"
	    eff=$(tail -${tailnum} $logfilename | head -1 | awk '{print $14}')
	    rej="0.0"
	    sgn="$total"
	    if [[ $var1 == *_* ]]; then
		var1p=$(echo $var1 | sed s/"_"/" "/g | awk '{print $2}')
		var2p=$(echo $var1 | sed s/"_"/" "/g | awk '{print $1}')
		cut2=$(tail -${tailnum} $logfilename | head -1 | awk '{print $15}')
		if [[ $doUpperCuts == 1 || $doSuperCuts == 1 ]]; then
		    cut1=$(tail -${tailnum} $logfilename | head -1 | awk '{print $15}')
		    cut2=$(tail -${tailnum} $logfilename | head -1 | awk '{print $16}')
		    cut1up=$(tail -${tailnum} $logfilename | head -1 | awk '{print $17}')
		    cut2up=$(tail -${tailnum} $logfilename | head -1 | awk '{print $18}')
		fi
	    else
		var1p=$var1
		var2p="none"
		cut2="0"
	    fi	    

	# now, if we're in a region with terrible stats, we may do better just cutting on the inclusive
	# region.  Check for that here.
	    mkdir -p $TMPDIR/${grid}
	    inclusivesgn=$(tail -${tailnum} $logfilename | head -1 | awk '{print $4}')
	    inclusivebetter=$(echo "${inclusivesgn}>${sgn}" | bc)
	    if [[ $onlyinclusive == 1 ]]; then
		tail -1 $logfilename > $TMPDIR/$limitinput
	    elif [[ $inclusivebetter != 0 || $forceinclusive == 1 ]]; then
		tail -${tailnum} $logfilename | head -1 > $TMPDIR/$limitinput	
	    else
		tail -${SRbins} $logfilename > $TMPDIR/$limitinput
	    fi


	# just run the limits
	    set -x
	    ./scripts/make_limits.py --optresults="$TMPDIR/$limitinput" ${singleBGModelOpts} --uncertainty=${unc} ${flatuncopts} > $TMPDIR/$limitoutput
	    set +x
	    m2sig=$(  cat $TMPDIR/$limitoutput | tail -1 | sed s_","_" "_g | sed s_"("_" "_g | awk '{print $5}')
	    m1sig=$(  cat $TMPDIR/$limitoutput | tail -1 | sed s_","_" "_g | sed s_"("_" "_g | awk '{print $8}')
	    median=$( cat $TMPDIR/$limitoutput | tail -1 | sed s_","_" "_g | sed s_"("_" "_g | awk '{print $12}')
	    p1sig=$(  cat $TMPDIR/$limitoutput | tail -1 | sed s_","_" "_g | sed s_"("_" "_g | awk '{print $15}')
	    p2sig=$(  cat $TMPDIR/$limitoutput | tail -1 | sed s_","_" "_g | sed s_"("_" "_g | awk '{print $18}')
	    p0=$(     cat $TMPDIR/$limitoutput | tail -1 | sed s_","_" "_g | sed s_"("_" "_g | awk '{print $21}')
	    p0sig=$(  cat $TMPDIR/$limitoutput | tail -1 | sed s_","_" "_g | sed s_"("_" "_g | awk '{print $22}')

	    echo "Median expected limit with $i with collection $j is $median"

	# echo "prefix  = $prefix  "
	# echo "mGO     = $mGO     "
	# echo "mN1     = $mN1     "
	# echo "{sigma} = ${sigma} "
	# echo "var1    = $var1    "
	# echo "cut1    = $cut1    "
	# echo "sig     = $sig     "
	# echo "bkg     = $bkg     "
	# echo "preeff  = $preeff  "
	# echo "prerej  = $prerej  "
	# echo "eff     = $eff     "
	# echo "rej     = $rej     "
	# echo "sgn     = $sgn     "
	# echo "m2sig   = $m2sig   "
	# echo "m1sig   = $m1sig   "
	# echo "median  = $median  "
	# echo "p1sig   = $p1sig   "
	# echo "p2sig   = $p2sig   "
	# echo "p0      = $p0      "
	# echo "p0sig   = $p0sig   "
	# echo "var2    = $var2    "
	# echo "cut2    = $cut2    "

	    if [[ $var1 == *_* ]]; then
		if [[ $doUpperCuts == 1 || $doSuperCuts == 1 || $doFixedWindow != "" ]]; then
		    printf "%20s    %6d   %6d  %1.4e    %10s      %012.4f  %012.4f   %016.4f   %08.4f   %1.4e   %08.4f   %1.4e   %08.4f   %08.4f   %08.4f  %08.4f   %08.4f  %08.4f  %12.4e %08.4f   %10s   %012.4f   %012.4f   %012.4f\n" \
  			$prefix  $mGO  $mN1 ${sigma} $var1p    $cut1    $sig      $bkg      $preeff  $prerej $eff     $rej    $sgn     $m2sig   $m1sig  $median  $p1sig  $p2sig  $p0    $p0sig   $var2p $cut2 $cut1up $cut2up>> $DelphesReaderDir/${grid}/${finallimitoutput}
		    
		    printf "%20s    %6d   %6d  %1.4e    %10s      %012.4f  %012.4f   %016.4f   %08.4f   %1.4e   %08.4f   %1.4e   %08.4f   %08.4f   %08.4f  %08.4f   %08.4f  %08.4f  %12.4e %08.4f   %10s   %012.4f   %012.4f   %012.4f\n" \
			$prefix  $mGO  $mN1 ${sigma} $var1p    $cut1    $sig      $bkg      $preeff  $prerej $eff     $rej    $sgn     $m2sig   $m1sig  $median  $p1sig  $p2sig  $p0    $p0sig   $var2p $cut2 $cut1up $cut2up
		else
		    printf "%20s    %6d   %6d  %1.4e    %10s      %012.4f  %012.4f   %016.4f   %08.4f   %1.4e   %08.4f   %1.4e   %08.4f   %08.4f   %08.4f  %08.4f   %08.4f  %08.4f  %12.4e %08.4f   %10s   %012.4f\n" \
  			$prefix  $mGO  $mN1 ${sigma} $var1p    $cut1    $sig      $bkg      $preeff  $prerej $eff     $rej    $sgn     $m2sig   $m1sig  $median  $p1sig  $p2sig  $p0    $p0sig   $var2p $cut2 >> $DelphesReaderDir/${grid}/${finallimitoutput}
		    
		    printf "%20s    %6d   %6d  %1.4e    %10s      %012.4f  %012.4f   %016.4f   %08.4f   %1.4e   %08.4f   %1.4e   %08.4f   %08.4f   %08.4f  %08.4f   %08.4f  %08.4f  %12.4e %08.4f   %10s   %012.4f\n" \
			$prefix  $mGO  $mN1 ${sigma} $var1p    $cut1    $sig      $bkg      $preeff  $prerej $eff     $rej    $sgn     $m2sig   $m1sig  $median  $p1sig  $p2sig  $p0    $p0sig   $var2p $cut2
		fi
	    else
		printf "%20s    %6d   %6d  %1.4e    %10s      %012.4f  %012.4f   %016.4f   %08.4f   %1.4e   %08.4f   %1.4e   %08.4f   %08.4f   %08.4f  %08.4f   %08.4f  %08.4f  %12.4e %08.4f\n" \
  		    $prefix  $mGO  $mN1 ${sigma} $var1p    $cut1    $sig      $bkg      $preeff  $prerej $eff     $rej    $sgn     $m2sig   $m1sig  $median  $p1sig  $p2sig  $p0    $p0sig >> $DelphesReaderDir/${grid}/${finallimitoutput}
		
		printf "%20s    %6d   %6d  %1.4e    %10s      %012.4f  %012.4f   %016.4f   %08.4f   %1.4e   %08.4f   %1.4e   %08.4f   %08.4f   %08.4f  %08.4f   %08.4f  %08.4f  %12.4e %08.4f\n" \
		    $prefix  $mGO  $mN1 ${sigma} $var1p    $cut1    $sig      $bkg      $preeff  $prerej $eff     $rej    $sgn     $m2sig   $m1sig  $median  $p1sig  $p2sig  $p0    $p0sig
	    fi	    

	    if [[ "${batchmode}" == "0" ]]; then
		exit
	    fi
	done
    done
done



