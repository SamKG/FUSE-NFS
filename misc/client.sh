! fusermount -u /tmp/client16555 
! mkdir /tmp/client16555
cd ../clientSNFS
make clean && make
../clientSNFS/clientSNFS -address `hostname` -port 16555 -mount /tmp/client16555/
