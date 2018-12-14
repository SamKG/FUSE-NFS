! fusermount -u /tmp/fusetest 
./client -address `hostname` -port 100000 -mount /tmp/fusetest2/ -f -s
