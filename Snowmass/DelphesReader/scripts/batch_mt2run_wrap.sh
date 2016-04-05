
grid=$1
test=$2

basedir=/eliza11/atlas/mhance/Snowmass/data

mkdir -p batch_submit_logs
mkdir -p batch_logs

if [[ $grid == "" ]]; then
    echo "Must specify a grid set."
    exit
fi

grid_start=1
grid_end=$(grep -v "#" $basedir/Signal/SUSY/$grid/Delphes-3.0.9.1/scan_summary_summary.dat | wc | awk '{print $1}')

energy=$(echo $grid | sed s/"_"/" "/g | awk '{print $NF}')
Energy=$(echo $grid | sed s/"_"/" "/g | awk '{print $NF}' | sed s_"TeV"_"TEV"_g)

pileups="NoPileUp";

for pileup in ${pileups}; do

    logopts="-j y"
    queueopts=""
    if [[ $test != "" ]]; then
	grid_start=$test
	grid_end=$test
	logopts="-o batch_submit_logs/batch_submit.${grid}.${pileup}.${test}.out -j y"
	rm -f batch_submit_logs/batch_submit.${grid}.${pileup}.${test}.out
	queueopts="-l debug=1"
    fi
    
    # if not testing, then run backgrounds
    sigonly=""
    if [[ $test != "" ]]; then
	sigonly=1
    fi

    if [[ $sigonly == "" ]]; then
	archivedir=backgrounds_${energy}/archive_hists_$(date +%Y%m%d%H%M)
	mkdir -p $archivedir
	mv -f backgrounds_${energy}/hist* ${archivedir}

	for i in "tj" "tB" "Bj" "ttB" "QCD" "tt"; do
	    for j in $(/bin/ls backgrounds_${energy}/delphes.${i}-4p-*_${Energy}.NoPileUp.root); do
		tag=$(echo $j | sed s_"\."_" "_g | sed s="_"=" "=g | awk '{print $3}')
		qsub \
		    -N hists_bg_${tag}_${pileup} \
		    -t 1-1:1 \
		    -v grid="${tag}_${Energy}" \
		    -v pileup=$pileup \
		    -l cvmfs -l h_vmem=4G \
		    -l eliza11io=2 \
		    ${logopts} \
		    scripts/batch_mt2run.sh
	    done
	done
    fi

    # run signal
    bgonly=""
    if [[ $bgonly == "" ]]; then
	if [[ $grid_end != $grid_start ]]; then
	    archivedir=${grid}/archive_hists_$(date +%Y%m%d%H%M)
	    mkdir -p $archivedir
	    mv -f ${grid}/hist* ${archivedir}
	fi

	echo hists_${grid}_${pileup}
	qsub \
	    -N hists_${grid}_${pileup} \
	    -v grid=${grid} \
	    -v pileup=$pileup \
	    -t ${grid_start}-${grid_end}:1 \
	    -l cvmfs -l h_vmem=4G \
	    -l eliza11io=2 \
	    -l h=!mc1311-ib.nersc.gov \
	    ${logopts} \
	    scripts/batch_mt2run.sh
	
	if [[ $test != "" ]]; then
	    exit
	fi
    fi
    
done
