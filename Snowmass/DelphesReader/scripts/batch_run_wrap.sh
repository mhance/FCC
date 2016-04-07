grid=$1
if [[ $grid == "" ]]; then
    echo "Must specify a grid set."
    exit
fi

testing=""
if [[ $2 != "" ]]; then
    testing=$1
fi

basedir=/export/home/mhance/FCC/Snowmass/data

mkdir -p batch_submit_logs
mkdir -p ./$grid

grid_start=1
#grid_end=$(grep -v "#" $basedir/Signal/SUSY1/$grid/Delphes-3.0.9.1/scan_summary_summary.dat | wc | awk '{print $1}')
grid_end=$(grep -v "#" $basedir/Signal/SUSY/$grid/Delphes-3.0.9.1/scan_summary.dat | wc | awk '{print $1}')
analysistype="1"

if [[ "$grid" == GOSQ_masslessN0_* ]]; then
    grid_end=$(grep -v "#" $basedir/Signal/SUSY/$grid/Delphes-3.0.9.1/scan_summary.dat | wc | awk '{print $1/4}')
    batchscript=batch_run_GOSQ.sh
else
    batchscript=batch_run.sh
fi


queueopts=""
if [[ $testing != "" ]]; then
    grid_end=1
    queueopts="-l debug=1"
fi

doMain="1"

#    "50PileUp" \
#    "NoPileUpKiel" \
#    "140PileUp" \
for pileup in \
    "NoPileUp" \
    ; do

    if [[ $PBS == 1 ]]; then
	qsub \
	    -N DRmain_${grid}_${pileup} \
	    -v grid=${grid} \
	    -v unitsopt="" \
	    -v doMain=${doMain} \
	    -v pileup=$pileup \
	    -v analysistype=${analysistype} \
	    -t ${grid_start}-${grid_end}:1 \
	    -l cvmfs -l h_vmem=2G \
	    -l eliza11io=1 \
	    -j y \
	    ${queueopts} \
	    scripts/${batchscript}
    else
	# make a submission file
	subfile="batch_submit_logs/DRmain_${grid}_${pileup}_$(date +%Y%m%d%H%M).cfg"
	rm -f $subfile
	echo "universe = vanilla" >> ${subfile}
	echo "executable = scripts/${batchscript}" >> ${subfile}
	echo "getenv = False" >> ${subfile}
	#echo "should_transfer_files = True" >> ${subfile}
	#echo "when_to_transfer_output = ON_EXIT_OR_EVICT" >> ${subfile}
	echo "environment = \"grid=${grid} doMain=$doMain pileup=$pileup analysistype=$analysistype"\" >> ${subfile}
	
	if [[ ${grid_start} != 1 ]]; then
	    for point in $(seq ${grid_start} ${grid_end}); do
		echo "Arguments = $point" >> ${subfile}
		echo "output = DRmain_${grid}_${pileup}_${point}.log" >> ${subfile}
		echo "error = DRmain_${grid}_${pileup}_${point}.err" >> ${subfile}
		echo "Queue" >> ${subfile}
	    done
	else
	    echo "Arguments = \$(Process)" >> ${subfile}
	    echo "output = batch_submit_logs/DRmain_${grid}_${pileup}_\$(Process).log" >> ${subfile}
	    echo "error = batch_submit_logs/DRmain_${grid}_${pileup}_\$(Process).err" >> ${subfile}
	    echo "Queue ${grid_end}" >> ${subfile}
	    echo "" >> ${subfile}
	    echo "" >> ${subfile}
	fi
	    
	cat ${subfile}
	condor_submit ${subfile}
    fi

    if [[ ${testing} != "" ]]; then
	exit
    fi
  
done
