#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
	int pid;
	scanf("%d", &pid);
	setuid(pid + 31337);
}
