! fusermount -u /tmp/fusetest 
./client -address `hostname` -port 16555 -mount /tmp/fusetest2/ -f -s
