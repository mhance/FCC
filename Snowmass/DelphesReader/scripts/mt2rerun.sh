taskindex=$1
grid=$2
pileup=$3
signalgrid=$4
signaltaskindex=$taskindex

DelphesReaderDir=/eliza11/atlas/mhance/Snowmass/DelphesReader
basedir=${DelphesReaderDir}/../data
#signaldir=Signal/SUSY1/$grid/Delphes-3.0.9.1
signaldir=Signal/SUSY/$signalgrid/Delphes-3.0.9.1
cp -a $basedir/$signaldir/scan_summary_summary.dat $TMPDIR/scan_summary.dat

Energy=$(echo $grid | sed s/"_"/" "/g | awk '{print $NF}')
energy=$(echo $Energy | sed s_"TEV"_"TeV"_g)
Energy=$(echo $energy | sed s_"TeV"_"TEV"_g)
sqrts=$(echo $energy | sed s_"TeV"__g)
if [[ $grid == SQ_decoupledGO_* ]]; then
    line=$(grep -v '#' $TMPDIR/scan_summary.dat | head -$taskindex | tail -1)
    echo $line
    prefix=$(echo $line | awk '{print $7}' | sed s/"${grid}\."/""/g)
    prefix="${prefix}_${Energy}."

    inputpath="${DelphesReaderDir}/${grid}"
    outputdir=/eliza11/atlas/mhance/Snowmass/DelphesReader/${grid}/
else
    # must be a background sample
    inputpath="/eliza11/atlas/mhance/Snowmass/DelphesReader/backgrounds_${energy}"
    outputdir=${inputpath}
fi

if [[ $signalgrid == SQ_decoupledGO_* ]]; then
    if [[ $signaltaskindex == "" ]]; then
	signaltaskindex=$taskindex
    fi
    signalline=$(grep -v '#' $TMPDIR/scan_summary.dat | head -$signaltaskindex | tail -1)
    echo $signalline
    signalprefix=$(echo $signalline | awk '{print $7}' | sed s/"${grid}\."/""/g)
    signalprefix="${signalprefix}"
fi



# now, re-run on the input data to produce histogram files with the mt2 and met cuts applied,
# so we can inspect for any additional optimization we may want to do
#
# first, get the mT2 and met cuts
for signalregion in "SRhists" "SRhists_lownjets" "SRhists_mednjets" "SRhists_highnjets"; do 
    output=hist.${grid}.${pileup}.${prefix}optimized.${signalgrid}.${signalprefix}.${signalregion}.root
    signallogfile=${DelphesReaderDir}/${signalgrid}/log.${signalgrid}.${pileup}.${signalprefix}_${Energy}.unc_0.20.SR_htbins.${signalregion}.log
    echo $signallogfile
    
    signifs=""
    for k in $(cat $signallogfile | tail -12 | awk '{print $4}'); do
	if [[ $signifs == "" ]]; then
	    signifs="$k"
	else
	    signifs="$signifs,$k"
	fi
    done
    mt2s=""
    for k in $(cat $signallogfile | tail -12 | awk '{print $5}'); do
	if [[ $mt2s == "" ]]; then
	    mt2s="$k"
	else
	    mt2s="$mt2s,$k"
	fi
    done
    mets=""
    for k in $(cat $signallogfile | tail -12 | awk '{print $15}'); do
	if [[ $mets == "" ]]; then
	    mets="$k"
	else
	    mets="$mets,$k"
	fi
    done
    
    set -x

    # now re-run
    python \
	-m cProfile \
	./scripts/mt2analysis.py \
	--infile=${inputpath}/hist.${grid}.${pileup}.${prefix}root \
	--outfile=${output} \
	--energy=${sqrts} \
	--signif=${signifs} \
	--mt2cut=${mt2s} \
	--metcut=${mets} \
	--skiptree
    set +x

    cp -p $output ${outputdir}
done

