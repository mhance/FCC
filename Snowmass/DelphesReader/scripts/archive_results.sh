
suffix=$(date +%Y%m%d)

for i in \
    GO_decoupledSQ_14TeV_LHE_QQALL \
    GO_decoupledSQ_33TeV_LHE_QQALL \
    GO_decoupledSQ_100TeV \
    SQ_decoupledGO_14TeV \
    SQ_decoupledGO_33TeV \
    SQ_decoupledGO_100TeV \
    GOSQ_masslessN0_14TeV \
    GOSQ_masslessN0_33TeV \
    GOSQ_masslessN0_100TeV \
    ; do
  mkdir -p ${i}_${suffix}
  cp -p ${i}/limits*.txt ${i}_${suffix}
done
