#!/bin/env bash
# We don't know whether we have bash v4

PythonV=$( python -V 2>&1 | awk '{ print $NF }' )
case ${PythonV} in 
	2.7.?) ;; 
	*) echo Please use Python 2.7+ ; exit ;;
esac

mkdir -p externals
pushd externals > /dev/null 2>&1

MadGraphV=1.5.8
echo "Getting Madgraph"
# Assuming the future naming scheme remains coherent
wget --no-check-certificate http://launchpad.net/madgraph5/trunk/${MadGraphV%.*}.0/+download/MadGraph5_v${MadGraphV}.tar.gz
echo -n "...Unpacking tarball"
tar -xf MadGraph5_v1.5.8.tar.gz
ln -sf MadGraph5_v${MadGraphV//./_} MadGraph5
echo "...done"
pushd MadGraph5 > /dev/null 2>&1
echo "...Installing local Delphes and pythia-pgs (this can take a while...)"
./bin/mg5 2>&1 >./install-log.txt <<EOF
install Delphes
install pythia-pgs
quit
EOF

# Todo: implement checks

popd > /dev/null 2>&1

popd > /dev/null 2>&1
echo "Done with adding MadGraph to externals."
