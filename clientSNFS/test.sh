! fusermount -u /tmp/fusetest 
./client -address `hostname` 6555 -mount /tmp/fusetest/ #-f -s
