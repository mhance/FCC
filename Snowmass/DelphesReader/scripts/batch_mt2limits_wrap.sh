
grid=$1
test=$2

#var="MET"
#var="MET_mT2"
var="HT_MET"

basedir=/eliza11/atlas/mhance/Snowmass/data

mkdir -p batch_submit_logs

if [[ $grid == "" ]]; then
    echo "Must specify a grid set."
    exit
fi

grid_start=1
if [[ $grid == SQ_decoupledGO_* || $grid == GO_decoupledSQ_* ]]; then
    grid_end=$(grep -v "#" $basedir/Signal/SUSY/$grid/Delphes-3.0.9.1/scan_summary_summary.dat | wc | awk '{print $1}')
fi

archivedir=${grid}/archive_limits_$(date +%Y%m%d%H%M)
mkdir -p $archivedir
mv -f ${grid}/limit*.${var}.* ${archivedir}
mv -f ${grid}/log.*.${var}.* ${archivedir}

logopts="-j y"
queueopts=""
if [[ $test != "" ]]; then
    grid_start=$test
    grid_end=$test
    logopts="-o batch_submit_logs/batch_submit.${grid}.out -j y"
    rm -f batch_submit_logs/batch_submit.${grid}.out
    queueopts="-l debug=1"
fi

qsub \
    -N limit_${grid} \
    -v grid=${grid} \
    -v var=${var} \
    -t ${grid_start}-${grid_end}:1 \
    -l cvmfs -l h_vmem=4G \
    -l eliza11io=1 \
    -l h=!mc1311-ib.nersc.gov \
    ${logopts} \
    scripts/batch_mt2limits.sh
			
