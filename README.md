# OS-P3
Project 3 

# USAGE
in clientSNFS:
	compilation:
		make clean && make
	usage:
		./clientSNFS -address <server hostname> -port <port> -mount <directory to mount to>	

in serverSNFS
	compilation:
		make clean && make
	usage:
		./serverSNFS -port <port> -mount <directory to mount to>

# RUNNING TESTS
cd tests
make clean && make

# NOTE
The mount directories must exist *PRIOR* to calling client and/or server!
To create them, use mkdir
