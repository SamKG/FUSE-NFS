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
ON SERVER: (Starts server and mounts(creates if does not exist) to ~/fservertest/)
	./server.sh
ON CLIENT: (Starts client and mounts(creates if does not exist) to /tmp/client16555
	./client.sh

AFTER STARTING SERVER AND CLIENT: (Runs all the tests)
	./runtests.sh
# NOTE
The mount directories must exist *PRIOR* to calling client and/or server!
To create them, use mkdir
