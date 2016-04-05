for j in 14 33 100; do
    cd SQ_decoupledGO_${j}TeV/
    for i in $(/bin/ls hist.*.root); do
	hadd -T -v 1 $(echo $i | sed s_"\.root"_"\.NoTree\.root"_g) ${i}
    done
    cd ../
done

tar -cvf hists.tar SQ_decoupledGO_14TeV/hist.*.NoTree.root SQ_decoupledGO_33TeV/hist.*.NoTree.root SQ_decoupledGO_100TeV/hist.*.NoTree.root 

tar -cvf bghists.tar backgrounds_100TeV/hist.*-4p_100TEV.*.root  backgrounds_33TeV/hist.*-4p_33TEV.*.root  backgrounds_14TeV/hist.*-4p_14TEV.*.root 
