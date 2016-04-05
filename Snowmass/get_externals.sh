mkdir -p externals
pushd externals > /dev/null 2>&1

echo "Getting Delphes"
wget http://mhance.scipp.ucsc.edu/Delphes-3_0_9_1.tar.gz
echo "...Unpacking tarball"
tar -zxf Delphes-3_0_9_1.tar.gz
rm -f Delphes
ln -s Delphes-3.0.9.1 Delphes
pushd Delphes > /dev/null 2>&1
echo "...Building (this can take a minute or two....)"
./configure > /dev/null 2>&1
#sed -i s/"LIBS = \$(ROOTLIBS) -lEG \$(SYSLIBS)"/"LIBS = \$(ROOTLIBS) -lEG \$(SYSLIBS) -lz"/g Makefile
make > /dev/null 2>&1
echo "...Checking for library"
if [ ! -f libDelphes.so ]; then
    echo "Building Delphes failed.  Try doing it manually...  type 'make' and see where it dies."
    exit
else
    echo "...Delphes done."
fi

popd > /dev/null 2>&1

pushd HistFitter/src > /dev/null 2>&1
make

echo "Done with externals."
popd > /dev/null 2>&1
