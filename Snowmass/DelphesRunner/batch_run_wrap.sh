
mkdir -p batch_submit_logs
mkdir -p batch_output/delphes
mkdir -p batch_output/logs

#for dsid in $(seq 164274 164323); do
for dsid in $(seq 999000 999298); do
    rm -f batch_submit_logs/batch_submit.${dsid}.out
    rm -f batch_submit_logs/batch_submit.${dsid}.err

    qsub \
	-v dsid=${dsid} \
	-t 1-1:1 \
	-l cvmfs -l h_vmem=2G \
	-l eliza18io=1 \
	-o batch_submit_logs/batch_submit.${dsid}.out \
	-j y \
	batch_run.sh

done
