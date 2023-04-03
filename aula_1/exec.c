#include <stdio.h>
#include <stdlib.h>

int main() {
	int pid = fork();
	int pid_2 = fork();
	if (pid == 0 && pid_2>0){
	fork();
	}
	getchar();
	return 0;
}
