#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(){
	printf("Truncating /tmp/client16555/testfile.txt to size 250\n");
	truncate("/tmp/client16555/testfile.txt", 250);
	return 0;
}
