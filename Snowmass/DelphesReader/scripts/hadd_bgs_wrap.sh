for i in 100 14; do 
    for j in $(/bin/ls -1 SQ_decoupledGO_${i}TeV/*optimized*.root | sed s_"/"_" "_g | awk '{print $NF}' | sed s_"\."_" "_g | sed s_"\_${i}TEV"_""_g | awk -v energy=$i '{print ".optimized.SQ_decoupledGO_" energy "TeV." $4 "." $5 "." $6 "." $12}'); do 
	./scripts/hadd_bgs.sh ${i} ${j}
    done
done
