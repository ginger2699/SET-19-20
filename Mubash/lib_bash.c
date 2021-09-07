#include "lib_bash.h"

//*****************************************************************************************************
//**     questa funzione si occupa della divisione dei vari comandi e riconoscimento comando cd	     **
//*****************************************************************************************************
void command_parser(char* input_line){


	char* token = NULL;
	char* aux = input_line;


	token = strtok_r(aux, "|", &aux);		//estraggo il primo comando

	if(strncmp(token,"cd",2)==0)			//se il primo è cd controllo che sia l'unico
		if(strcmp(aux,"\0") == 0){
			command_cd(token);
		}
		else{
			printf("errore: cd usato con altri comandi\n");
			return;
		}
	else
			multiple_commands(token,aux);	//altrimenti, se non si tratta di cd, inizio la getione del caso di comandi multipli

}

//*****************************************************************************************************
//**     questa funzione si occupa del riconoscimento sintattico della rederezione		     **
//*****************************************************************************************************

char* look_for_redirection(char* command,char* symbol){			

		char* copy = malloc(strlen(command)+1);								//copia profonda per non modificare la stringa originaria
		char* fileToReturn =NULL;

		strcpy(copy,command);
		char* file_name = strstr(copy, symbol);								//cerco il simbolo dato

	if(!file_name){ //non ho ridirezioni
		free(copy);
		return NULL;
		}

	file_name+=2;

	if((strcmp(file_name,"\0") == 0) || (file_name[0] == ' ') || (file_name[0] == '\t')){			 //controllo di sintassi, non devono esserci spazi dopo la ridirezione e deve contenere il nome di un file
		switch (symbol[1]){
			case '>': printf("Errore: non e' specificato il file per la redirezione dello standard output\n");
					break;
			case '<': printf("Errore: non e' specificato il file per la redirezione dello standard input\n");
					break;
			default:
			free(copy);
			return "error";
		}
		free(copy);
		return "error";
	}
	file_name = strtok(file_name," \t");//tolgo gli argomenti successivi

	fileToReturn = malloc(strlen(file_name)+1); //creo una copia profonda in modo da non avere perdita di byte
	strcpy(fileToReturn,file_name);
	free(copy);

	return fileToReturn;
	}

//*****************************************************************************************************
//**     questa funzione si occupa del controllo della correttezza sintattica di ogni comando	     **
//*****************************************************************************************************
int syntaxControl(char* cm_to_analyze, char* rest, int* count){
		int flag = 0;		
		char* aux = cm_to_analyze;
		char* auxToFree1 = NULL;
		char* auxToFree2 = NULL;

		while (*aux != '\0' && flag == 0) {
			if (!isspace((unsigned char)*aux))
				flag = 1;
			aux++;
		}
		
		if (flag == 0){
			printf("Errore di sintassi: comando vuoto\n");
			return 1;
		}

		if ((strcmp(rest,"\0") == 0)&&(*count == 0)){
			return 0;
		}
		else if (*count == 0){
			if(( auxToFree1 = look_for_redirection(cm_to_analyze," >"))  != NULL)  {
				free(auxToFree1);
				printf("Errore: redirezione dell'output nel primo comando\n");
				return 1;
			}
		}
		else if ((strcmp(rest,"\0") == 0)){
			if((auxToFree1 = look_for_redirection(cm_to_analyze," <")) != NULL){
				free(auxToFree1);
				printf("Errore: redirezione dell'input nell'ultimo comando\n");
				return 1;
			}
		}
		else {
			if(((auxToFree1 = look_for_redirection(cm_to_analyze," <")) != NULL) || ( (auxToFree2 = look_for_redirection(cm_to_analyze," >") ) != NULL) ) {
				free(auxToFree1);
				free(auxToFree2);
				printf("Errore: redirezione di I/O non ammessa\n");
				return 1;
			}
		}
		return 0;
	}

//*****************************************************************************************************
//**     questa funzione si occupa della gestione delle redirezioni di standard I/O		     **
//*****************************************************************************************************
int handle_redirection(char* file_name, char symbol){

	int fd;

	switch (symbol){
	 case '>':
		 if(file_name[0] != '/'){
			 fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR ); //apro un file, in scrittura e se non esiste lo creo
		 }else{
			 fd = fileno(fopen(file_name,"w"));  //Caso in cui si abbia un pathname completo
		 }
		 if(fd<0){
			 printf("Errore: Impossibile accedere al file richiesto\n");
			 return -1;
		 }
		dup2(fd, 1);   // mando lo standard output al file
		dup2(fd, 2);   // mando lo standarderror al file
		return 0;
	case '<':
	if(file_name[0] != '/'){
		fd = open(file_name, O_RDONLY, S_IRUSR); //apro un file, in lettura
	}else{
		fd = fileno(fopen(file_name,"r"));   //Caso in cui si ha un pathname completo
	}
	if(fd<0){
		printf("Errore: Impossibile accedere al file richiesto\n");
		return -1;
	}
		dup2(fd, 0);   // make stdout go to file
		dup2(fd, 2);   // make stderr go to file - you may choose to not do this
		return 0;
	default: return -1;
	 }

	}


//*****************************************************************************************************
//**     questa funzione si occupa dell'esecuzione dei vari comandi insieme ai loro argomenti        **
//*****************************************************************************************************

int execute(char** list, int pos){ //eseguo il comando
	char** arg;
	char* file_name;
	char* aux;
	size_t len;
	if ((file_name = look_for_redirection(list[pos]," >"))){ //se ho delle redirezioni in output
		if (strcmp(file_name,"error") == 0){ //errore di sintassi, non eseguo
			return -1;
		}

		len=strlen(file_name)+1;
		aux=list[pos];
		aux=strstr(aux, ">");
		memmove(aux-1,aux+len,strlen(aux+len)+1);
		if(handle_redirection(file_name, '>') < 0){
			free(file_name);
			return -1;
		} //gestisco l'output
	}
	 if ((file_name = look_for_redirection(list[pos]," <"))){ //se ho delle redirezioni in input
		if (strcmp(file_name,"error") == 0){
			return -1;
		}

			len=strlen(file_name)+1;
			aux=list[pos];
			aux=strstr(aux, "<");
			memmove(aux-1,aux+len,strlen(aux+len)+1);

		if(handle_redirection(file_name, '<') < 0){
			free(file_name);
			return -1;
		}//gestisco l'input
	}

	arg = argumentTokenizer(list[pos]);
	free(file_name);

	if (execvp(arg[0],arg) < 0){
		perror("errore");
		free(arg);
		return -1;
	}
	else{

		free(arg);
		return 0;
	}
	free(arg);
	return -1; //Se si raggiunge questo punto qualcosa di inaspettato è accaduto
}



//*****************************************************************************************************
//**     questa funzione si occupa della preparazione delle varie pipe			             **
//*****************************************************************************************************
void multiple_commands(char* token,char* commands){
	int n_cm = 0;										
	char** list =NULL;
	pid_t pid;
  	int status;
	int i;
	if((list = commandTokenizer(token,commands,&n_cm)) == NULL){				//inserisco in list tutti i comandi con i relativi argomenti
		free(list);
		return;
	}										

	int numPipes = n_cm-1;
	int pipefds[2*numPipes];

	for(i = 0; i < numPipes; i++){								//creo tutti i file descriptor necessari
			 if(pipe(pipefds + i*2) < 0) {
					 perror("Impossibile effettuare la pipe");
					 exit(EXIT_FAILURE);
			 }
	}
	int j = 0;
	for (int pos = 0;pos<n_cm;pos++){							//ripeto la procedutra per tutti i comandi necessari
		pid=fork();									//duplico i processi
												
		if (pid == 0){									//processo figlio
			if (pos != n_cm-1){							//se non è l'ultimo comando

				if (dup2(pipefds[j+1],1) < 0){					//redirezioniamo lo standard output all'input del comando successivo
					perror("dup2");
					free(list);
					exit(EXIT_FAILURE);
				}
			}
			if (j != 0){								//se non è il primo processo
				if(dup2(pipefds[j-2], 0) < 0){					//prendiamo come input il risultato del comando precedente
						 perror("dup2");
						 free(list);
						 exit(EXIT_FAILURE);
				}
			}

			for (i = 0; i < 2*numPipes; i++){					//chiudo i fd originari
				close(pipefds[i]);
			}
			if (execute(list,pos) < 0){						//eseguo i vari comandi
				free(list);
				exit(EXIT_FAILURE);
			}

		}
		else if (pid < 0){								//gestione ipotetico errore con fork
			perror("Error pid");
			free(list);
			exit(EXIT_FAILURE);
		}

		j += 2;
	}
	for(i = 0; i < (2 * numPipes); i++){							//chiusura di tutte le pipe utilizzate
		if(close(pipefds[i]) < 0){
			perror("errore");
			free(list);
			return;
		}
			
	}

	for(i = 0; i < numPipes + 1; i++){							//aspetto che tutti i processi siano terminati
		wait(&status);
		if(WIFEXITED(status) && WEXITSTATUS(status) != 0)
			printf("stato della exit del processo: %d\n",WEXITSTATUS(status));
	}
	free(list);
}


//*****************************************************************************************************
//**     questa funzione si occupa della divisione in stringhe di comandi e loro argomenti           **
//*****************************************************************************************************


char** commandTokenizer(char* firstCommand, char* restOfCommands,int* numberOfCommands){

		char** commands = (char**)malloc(sizeof(char**)*ARG_MAX);					 //alloco memoria per l'array di stringhe di comandi e argomenti
		char* commandToAdd =NULL;

		if (syntaxControl(firstCommand,restOfCommands,numberOfCommands) == 1){				//controllo la sintassi dei vari comandi
			free(commands);
			return NULL;
		}
		commands[*numberOfCommands] = firstCommand;							//inserisco il primo comando

		*numberOfCommands = *(numberOfCommands)+1 ;

		commandToAdd = restOfCommands;

		while(strcmp(restOfCommands,"\0") != 0){							//eseguo per tutti i comandi rimasti
			commandToAdd = strtok_r(restOfCommands, "|", &restOfCommands);
			if (syntaxControl(commandToAdd,restOfCommands,numberOfCommands)==1){			//controllo la sintassi
				free(commands);
				return NULL;
			}
			if(strncmp(commandToAdd," cd ",4) == 0 || strncmp(commandToAdd,"cd ",3) == 0 || strncmp(commandToAdd," cd\0",4) == 0){							//controllo di non avere cd nei comandi successivi
				printf("errore di parsing: il comando cd deve essere usato da solo\n");
				free(commands);
				return NULL;
			}
			commands[*numberOfCommands] = commandToAdd;

			*numberOfCommands = *(numberOfCommands)+1 ;						//aumento il contatore dei comandi
		}
		commands[*numberOfCommands] = NULL;
		return commands;
}

//*****************************************************************************************************
//**	     questa funzione si occupa della divisione tra comando e suoi argomenti		     **
//*****************************************************************************************************

char** argumentTokenizer(char* command){

	char* aux = command;
	int cont = 0;
	char** arguments;
	char* commandToExec;
	
	arguments = (char**)malloc(sizeof(char**)*ARG_MAX);
	commandToExec = strtok_r(command, " \t", &command);			//estraggo il comando

	arguments[cont] = commandToExec;
	++cont;

	aux = command;

	while(strcmp(command,"\0") != 0){					//finche` ho argomenti 
		aux = strtok_r(command, " \t", &command);
		if (aux[0] == '$'){						//caso in cui ci sia una variabile d'ambiente negli argomenti
			memmove(aux, aux+1, strlen(aux));
			aux =  getenv(aux);
		}
		arguments[cont] = aux;
		++cont;
	}
	arguments[cont]=NULL;							//a questo punto arguments sara` composta da una comando piu` i suoi argomenti
	return arguments;
}


//*****************************************************************************************************
//**		  questa funzione e` l-implementazione builtin del comando cd			     **
//*****************************************************************************************************
void command_cd(char* inputToken){
	char* auxToFree1 = NULL;
	char* auxToFree2 = NULL;
	if((auxToFree1 = look_for_redirection(inputToken, " >")) || (auxToFree2 = look_for_redirection(inputToken, " <"))){	//controllo presenza redirezioni
		printf("errore: comando cd non supporta redirezione\n");
		free(auxToFree1);
		free(auxToFree2);
		return;
	}											
	
	free(auxToFree1);
	free(auxToFree2);
	strtok_r(inputToken," \t",&inputToken);								//ignoriamo cd
	
	char* path = strtok_r(inputToken," \t",&inputToken);						//estriamo il path che vogliamo raggiungere
	
	
	if(path == NULL){										//caso base in cui non viene specificata la directory ci si sposta sulla home
		if(chdir(getenv("HOME")) == -1)
			perror("path non valida");
	}
	else{
		if(strcmp(inputToken,"\0") != 0){							//controllo che non ci sia niente oltre all'argomento estratto ed inserito in path
			printf("errore: cd ha un solo argomento\n");
			return;
		}
	 
	 if (chdir(path) == -1)										//cambio directory 
			perror("errore");
	 
	 }
	
}
