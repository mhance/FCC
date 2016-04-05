for i in \
    GO_decoupledSQ_14TeV_LHE_QQALL \
    GO_decoupledSQ_33TeV_LHE_QQALL; do 
    scripts/batch_optlimits_wrap.sh $i
done
