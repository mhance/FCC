allbgs=""
base="backgrounds_${1}TeV"
task=$2
if [[ $1 == "100" ]]; then
    for i in "tt"; do
	bgfiles="${base}/hist.tt-4p-0-1000_100TEV.NoPileUp${task}.root ${base}/hist.tt-4p-1000-2000_100TEV.NoPileUp${task}.root ${base}/hist.tt-4p-2000-3500_100TEV.NoPileUp${task}.root ${base}/hist.tt-4p-3500-5500_100TEV.NoPileUp${task}.root ${base}/hist.tt-4p-5500-7000_100TEV.NoPileUp${task}.root ${base}/hist.tt-4p-7000-8500_100TEV.NoPileUp${task}.root ${base}/hist.tt-4p-8500-11000_100TEV.NoPileUp${task}.root ${base}/hist.tt-4p-11000-100000_100TEV.NoPileUp${task}.root"
	if [[ $2 == "" ]]; then
	    hadd -T -v 1 -f ${base}/hist.${i}-4p_100TEV.NoPileUp${task}.root ${bgfiles}
	fi
	allbgs="$allbgs ${base}/hist.${i}-4p_100TEV.NoPileUp${task}.root"
    done
    
    for i in "tB" "tj" "ttB" "QCD" "Bj"; do
	if [[ $2 == "" ]]; then
	    hadd -T -v 1 -f ${base}/hist.${i}-4p_100TEV.NoPileUp${task}.root ${base}/hist.${i}-4p-*_100TEV.NoPileUp${task}.root
	fi
	allbgs="$allbgs ${base}/hist.${i}-4p_100TEV.NoPileUp${task}.root"
    done
    hadd -T -v 1 -f ${base}/hist.BG-4p_100TEV.NoPileUp${task}.root ${allbgs}
elif [[ $1 == "33" || $1 == "14" ]]; then
    for i in "tB" "tj" "tt" "ttB" "Bj"; do
	if [[ $2 == "" ]]; then
	    hadd -T -v 1 -f ${base}/hist.${i}-4p_${1}TEV.NoPileUp${task}.root ${base}/hist.${i}-4p-*_${1}TEV.NoPileUp${task}.root
	fi
	allbgs="$allbgs ${base}/hist.${i}-4p_${1}TEV.NoPileUp${task}.root"
    done
    hadd -T -v 1 -f ${base}/hist.BG-4p_${1}TEV.NoPileUp${task}.root ${allbgs}
fi
