# OS-P3
Project 3 

# USAGE
in clientSNFS:
	compilation:
		make clean && make
	usage:
		./client -address <server hostname> -port <port> -mount <directory to mount to>	

in serverSNFS
	compilation:
		make clean && make
	usage:
		./server -port <port> -mount <directory to mount to>

# NOTE
The mount directories must exist *PRIOR* to calling client and/or server!
To create them, use mkdir
