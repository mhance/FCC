
#     GO_decoupledSQ_14TeV_LHE_QQALL \
#     GOSQ_masslessN0_14TeV \
#     GO_decoupledSQ_33TeV_LHE_QQALL \
#     GOSQ_masslessN0_33TeV \
#     GO_decoupledSQ_100TeV \
#     SQ_decoupledGO_100TeV \
#     GOSQ_masslessN0_100TeV \
for i in \
    SQ_decoupledGO_100TeV \
    SQ_decoupledGO_33TeV \
    SQ_decoupledGO_14TeV \
    ; do
    echo $i
    ./scripts/batch_run_wrap.sh $i
done

# exit

#     GO_decoupledSQ_14TeV_LHE_QQALL \
#     GOSQ_masslessN0_14TeV \
#     GO_decoupledSQ_33TeV_LHE_QQALL \
#     GOSQ_masslessN0_33TeV \
#     GO_decoupledSQ_100TeV \
#     GOSQ_masslessN0_100TeV \
#for i in \
#    SQ_decoupledGO_14TeV \
#    SQ_decoupledGO_33TeV \
#    SQ_decoupledGO_100TeV \
#    ; do
#   echo $i
#   ./scripts/batch_optlimits_wrap.sh $i
#done

exit
