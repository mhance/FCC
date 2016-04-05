
grid=$1
signalpoint=$2

basedir=/eliza11/atlas/mhance/Snowmass/data

mkdir -p batch_submit_logs

if [[ $grid == "" ]]; then
    echo "Must specify a grid set."
    exit
fi

vars=()
vars=("MET___HTjets_30_3p5")
if [[ $grid == GO_decoupledSQ_* ]]; then
    vars=("MET___HTjets_30_3p5" "m_eff")
fi

grid_start=1
if [[ $grid == SQ_decoupledGO_* ]]; then
    grid_end=$(grep -v "#" $basedir/Signal/SUSY/$grid/Delphes-3.0.9.1/scan_summary_summary.dat | wc | awk '{print $1}')
elif [[ $grid == T1_* ]]; then
    grid_end=$(grep -v "#" $basedir/Signal/SUSY/$grid/Delphes-3.0.9.1/scan_summary_summary.dat | wc | awk '{print $1}')
fi

energy=$(echo $grid | sed s/"_"/" "/g | awk '{print $NF}')
Energy=$(echo $grid | sed s/"_"/" "/g | awk '{print $NF}' | sed s_"TeV"_"TEV"_g)

outputs=""
if [[ $outputs != "" ]]; then
    outputsopt="-v outputs='$outputs'"
fi


var="htbins"
varstr=${var}
pileup="NoPileUp"

logopts="-j y"
queueopts=""
if [[ $signalpoint != "" ]]; then
    grid_start=$signalpoint
    grid_end=$signalpoint
fi

bgonly=""
if [[ $bgonly == "" ]]; then
    echo hists_opt_${grid}_${pileup}_${varstr}
    qsub \
	-N hists_opt_${grid}_${pileup}_${varstr} \
	-v grid=${grid} \
	-v variable="${var}" \
	-v pileup=$pileup \
	-v signalgrid=${grid} \
	${outputsopt} \
	-t ${grid_start}-${grid_end}:1 \
	-l cvmfs -l h_vmem=4G \
	-l eliza11io=2 \
	${logopts} \
	scripts/batch_mt2rerun.sh

fi

# if not testing, then also run backgrounds
sigonly=""
if [[ $sigonly == "" ]]; then
    for i in "tj" "tB" "Bj" "ttB" "QCD" "tt"; do
	for j in $(/bin/ls backgrounds_${energy}/hist.${i}-4p_${Energy}.NoPileUp.root); do
	    tag=$(echo $j | sed s_"\."_" "_g | sed s="_"=" "=g | awk '{print $3}')
	    qsub \
		-N hists_opt_bg_${energy}_${tag}_${pileup}_${varstr} \
		-v grid="${tag}_${Energy}" \
		-v variable="${var}" \
		-v pileup=$pileup \
		-v signalgrid=SQ_decoupledGO_${energy} \
		-v signaltaskindex=${signalpoint} \
		${outputsopt} \
		-t ${grid_start}-${grid_end}:1 \
		-l cvmfs -l h_vmem=4G \
		-l eliza11io=2 \
		${logopts} \
		scripts/batch_mt2rerun.sh
	done
    done
fi

