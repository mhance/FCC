
# Get some data files from the Snowmass folks
mkdir -p data/CommonBG
pushd data/CommonBG

version="Delphes-3.0.5-Snowmass-1.0"
version="Delphes-3.0.6.1"

#host="https://dcdoor10.usatlas.bnl.gov:2881/pnfs/usatlas.bnl.gov/osg/snowmass/Delphes-3.0.5-Snowmass-1.0"
host="http://red-gridftp11.unl.edu/Snowmass/${version}"

get_sample(){
    wget -r -l2 --no-parent -nH --cut-dirs=1 "${host}/${2}/${1}/"
}    

for pileup in \
    "NoPileUp" \
    "50PileUp" \
    "140PileUp" \
    ; do
  for process in \
      TTBAR \
      WJETS \
      ZJETS \
      ; do
    # 13TEV
    for energy in \
	33TEV \
	; do
      
      get_sample "${process}_${energy}" "${pileup}"

    done
  done
done

popd
