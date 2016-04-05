
grid=$1

mkdir -p batch_submit_logs

if [[ $grid == "" ]]; then
    echo "Must specify a grid set."
    exit
fi

doSubmit=0
for lumi in 3 30 300 3000; do
    results=""
    for postfix in \
	"_cut_MET_gt_0100" \
	"_cut_MET_gt_0200" \
	"_cut_MET_gt_0300" \
	"_cut_MET_gt_0400" \
	"_cut_MET_gt_0500" \
	"_cut_MET_gt_0600" \
	"_cut_MET_gt_0700" \
	"_cut_MET_gt_0800" \
	"_cut_MET_gt_0900" \
	"_cut_MET_gt_1000" \
	"_cut_optimal_50" \
	"_cut_optimal_100" \
	"_cut_optimal_150" \
	"_cut_optimal_200" \
	"_cut_optimal_250" \
	"_cut_optimal_300" \
	"_cut_optimal_350" \
	"_cut_optimal_400"; do
	output=limits${postfix}.lumi${lumi}.txt
	if [[ $doSubmit != 0 ]]; then
	    rm -f ${grid}/$output
	    rm -f batch_submit_logs/batch_submit.${grid}.${postfix}.${lumi}.out
	    
	    gridpoints=$(wc ${grid}/events${postfix}.txt | awk '{print $1}')

	    qsub \
		-v background=backgrounds${postfix}.txt \
		-v signal=events${postfix}.txt \
		-v lumi=${lumi} \
		-v output=${output} \
		-v grid=${grid} \
		-t 1-${gridpoints}:1 \
		-l cvmfs -l h_vmem=2G \
		-l eliza18io=0 \
		-l h_cpu=200 \
		-o batch_submit_logs/batch_submit.${grid}.${postfix}.${lumi}.out \
		-j y \
		scripts/batch_limits.sh
	else
	    sort ${grid}/$output -o ${grid}/$output
	    results="$results --input ${grid}/${output}"
	fi
    done
    if [[ $doSubmit == 0 ]]; then
	./scripts/make_best_limits.py $results > ${grid}/limits_best.lumi${lumi}.txt
    fi
done

