! fusermount -u /tmp/client16555 
! mkdir /tmp/client16555
./client -address `hostname` -port 16555 -mount /tmp/client16555/ #-f -s
