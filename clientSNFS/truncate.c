#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(){
	truncate("/tmp/client16555/testfile.txt", 150);
	return 0;
}
