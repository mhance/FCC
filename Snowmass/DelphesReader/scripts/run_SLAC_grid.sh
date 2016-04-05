
grid=$1

basedir=/eliza18/atlas/mhance/Snowmass/data

postfix=".Delphes3"

mkdir -p ./$grid

# need to do following:
# -- get file for point
# -- get CS for that point
# -- run point

while read mGO mN1 prod decay sigma nevents prefix; do
    if [[ $mGO = "#" ]]; then
	continue
    fi
    
    fixedsigma=$(echo $sigma | awk '{print $1*1000.}')
    
    echo "Running grid $grid, point $prefix, with cross section = $fixedsigma fb."

    ./bin/DRMain \
	-f $basedir/$grid/Decayed/Events/$prefix$postfix.root \
	-s "$grid" \
	-t "$prefix" \
	-S \
	-c $fixedsigma\
	-o $grid \
	-T 0

done < $basedir/$grid/scan_summary.dat
    

