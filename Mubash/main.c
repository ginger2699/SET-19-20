
#include "lib_bash.h"


int main(){

	char currentDirectory[COMMANDSIZE];
	char inputCommandLine[COMMANDSIZE];
	command_parser("clear");
	for(;;){
		char* ptrCommand = NULL;
		if(getcwd(currentDirectory,sizeof(currentDirectory)) == NULL) exit(1);

		printf("\033[1m\033[1;32m%s@microBash:\033[0m\033[1;34m%s$\033[0m ", getenv("LOGNAME"),currentDirectory); //stampo la directory corrente

		if(fgets(inputCommandLine,COMMANDSIZE,stdin) == NULL) exit(1);

		ptrCommand = inputCommandLine;
		if (strcmp(ptrCommand,"\n")==0) continue;

		ptrCommand[strlen(ptrCommand)-1] = 0; //sostituisco \n con \0

		if(strcmp(ptrCommand,"exit") == 0)
		exit(EXIT_SUCCESS);

		command_parser(ptrCommand); //chiamo la funzione che si occupa di eseguire i comandi


	}

	return 0;

}
