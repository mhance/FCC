
grid=$1
test=$2

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

useclassified=0
grid_start=1
if [[ $grid == "GOGO_decoupledSQ" ]]; then
    grid_start=2
    grid_end=$(/bin/ls -1 $basedir/$grid/Decayed/Events | wc | awk '{print $1}')
elif [[ $grid == "LHE_GOGO_SQ" ]]; then
    grid_end=$(grep -v "#" $basedir/Signal/SUSY1/$grid/Delphes-3.0.9.1/scan_summary_summary.dat | wc | awk '{print $1}')
elif [[ $grid == GOSQ_masslessN0_* ]]; then
    grid_end=$(grep -v "#" $basedir/Signal/SUSY/$grid/Delphes-3.0.9.1/scan_summary_summary.dat | wc | awk '{print $1/4}')
elif [[ $grid == "GO_decoupledSQ_14TeV_LHE_QQALL" ]]; then
    grid_end=$(grep -v "#" $basedir/Signal/SUSY/$grid/Delphes-3.0.9.1/scan_summary_summary.dat | wc | awk '{print $1}')
elif [[ $grid == "GO_decoupledSQ_33TeV_LHE_QQALL" ]]; then
    grid_end=$(grep -v "#" $basedir/Signal/SUSY/$grid/Delphes-3.0.9.1/scan_summary_summary.dat | wc | awk '{print $1}')
elif [[ $grid == "GO_decoupledSQ_100TeV" ]]; then
    grid_end=$(grep -v "#" $basedir/Signal/SUSY/$grid/Delphes-3.0.9.1/scan_summary_summary.dat | wc | awk '{print $1}')
elif [[ $grid == "SQ_decoupledGO_14TeV_LHE_QQALL" ]]; then
    grid_end=$(grep -v "#" $basedir/Signal/SUSY/$grid/Delphes-3.0.9.1/scan_summary_summary.dat | wc | awk '{print $1}')
elif [[ $grid == SQ_decoupledGO_* ]]; then
    grid_end=$(grep -v "#" $basedir/Signal/SUSY/$grid/Delphes-3.0.9.1/scan_summary_summary.dat | wc | awk '{print $1}')
elif [[ $grid == T1_* ]]; then
    grid_end=$(grep -v "#" $basedir/Signal/SUSY/$grid/Delphes-3.0.9.1/scan_summary_summary.dat | wc | awk '{print $1}')
else
    grid_start=999000
    grid_end=999298
    vars=("BDTScore" "MET" "Beate4a" "Beate4b" "Beate4c")
    useclassified=1
fi

outputs=""
if [[ $outputs != "" ]]; then
    outputsopt="-v outputs='$outputs'"
fi


for var in ${vars[@]}; do
    minimum=""
    if [[ $var == "MET" ]]; then
	minimum=100
    fi
    
    lumis="3000";
    if [[ $grid == *14TeV* ]]; then
	lumis="${lumis} 300" # 30 3
    fi
    for lumi in ${lumis}; do

	pileups="NoPileUp";
	if [[ $grid == "GO_decoupledSQ_14TeV_LHE_QQALL" ]]; then
	    pileups="${pileups} 140PileUp"
	fi

	for pileup in ${pileups}; do

	    leadJetPtCuts=(0)
	    if [[ $grid == SQ_decoupledGO* ]]; then
		leadJetPtCuts=(0 1)
	    fi

	    for doLeadJetPtCut in ${leadJetPtCuts[@]}; do

		metsignifs="15"
		if [[ $grid == GO_decoupledSQ_33TeV* ]]; then
		    metsignifs="${metsignifs} 23"
		elif [[ $grid == "GO_decoupledSQ_100TeV" ]]; then
		    metsignifs="${metsignifs} 23 44"
		fi
		
		for METsignifcut in ${metsignifs}; do
		    
		    uncertainties="0.20"
		    if [[ $grid == GO_decoupledSQ_* ]]; then
			uncertainties="${uncertainties} 0.30 0.10 0.05"
		    fi
		    
		    for unc in ${uncertainties}; do
			
			varstr=$(echo $var | sed s=","="__"=g)
			
			#output=limits_opt.${varstr}.lumi${lumi}.${pileup}.unc_${unc}.txt
			output=limits_opt.${varstr}.lumi${lumi}.${pileup}.unc_${unc}.leadjetcut_${doLeadJetPtCut}.metsignif_${METsignifcut}.txt
			
			rm -f ${grid}/$output
			
			logopts="-j y"
			queueopts=""
			if [[ $test != "" ]]; then
			    grid_start=$test
			    grid_end=$test
			    logopts="-o batch_submit_logs/batch_submit.${grid}.${lumi}.${pileup}.${varstr}.${unc}.${doLeadJetPtCut}.${METsignifcut}.out -j y"
			    rm -f batch_submit_logs/batch_submit.${grid}.${lumi}.${pileup}.${varstr}.${unc}.${doLeadJetPtCut}.${METsignifcut}.out
			    queueopts="-l debug=1"
			fi
			
			echo limit_${grid}_${lumi}_${pileup}_${varstr}_${unc}_${doLeadJetPtCut}_${METsignifcut}

			qsub \
			    -N limit_${grid}_${lumi}_${pileup}_${varstr}_${unc}_${doLeadJetPtCut}_${METsignifcut} \
			    -v lumi=${lumi} \
			    -v output=${output} \
			    -v grid=${grid} \
			    -v uncertainty=${unc} \
			    -v variable="${var}" \
			    -v minimum=${minimum} \
			    -v pileup=$pileup \
			    -v useclassified=${useclassified} \
			    -v METsignifcut=${METsignifcut} \
			    -v doLeadJetPtCut=${doLeadJetPtCut} \
			    ${outputsopt} \
			    -t ${grid_start}-${grid_end}:1 \
			    -l cvmfs -l h_vmem=4G \
			    -l eliza11io=2 \
			    ${logopts} \
			    scripts/batch_optlimits.sh
			
			if [[ $test != "" ]]; then
			    exit
			fi
			
		    done
		done
	    done
	done
    done
done

