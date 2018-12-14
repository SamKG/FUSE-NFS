! mkdir ~/fservertest/
cd ../serverSNFS
make clean && make
../serverSNFS/serverSNFS -port 16555 -mount ~/fservertest/
