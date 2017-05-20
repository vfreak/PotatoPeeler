#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void HidePID(int PID){
	int process_id;
	 
}

void ShowPID(int PID){
	int process_id;

}

int main(int argc, char *argv[]){
	if(argc != 3){
		printf("Uages: ./FrenchFry show/hide <PID>");
		return 0;
	}
	else{
		if(strcmp("hide", argv[1]) == 0){
			HidePID(argv[2]);
		}
		if(strcmp("show", argv[1]) == 0){
			ShowPID(argv[2]);
		}
		else{
			printf("Unknown command, correct usage show/hide.")
			return 0;
		}
	}
	return 0;
}
