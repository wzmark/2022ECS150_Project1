#include "sshell.h"

void PrintMessage(SshellInput *shell){
        //+ completed 'echo Hello world | grep Hello|wc -l' [0][0][0]
        //shell->listOfCommand[0].isSuccess = 0;
        char* result = (char*)malloc(COMMAND_MAX_LEN * sizeof(char));
        strcpy(result,  "+ completed '");
        strcat(result, shell->savedCommand);
        strcat(result, "' ");
        for(int i = 0; i < shell->numOfCommand; i++){
                if(shell->listOfCommand[i].isSuccess == 1){
                        strcat(result, "[0]");
                }else{
                        strcat(result, "[1]");
                }
        }
        strcat(result, "\n");
        printf("%s", result);
}

//printf any error message, using errorType to determine the error type
int ErrorHandler(int errorType){
        switch(errorType){
                case 1:
                        //can not open output file
                        fprintf(stderr, 
				"Error: cannot open output file\n");
                        break;
                case 2:
                        //can not cd
                        fprintf(stderr, 
				"Error: cannot cd file\n");
                        break;
                case 3:
                        fprintf(stderr, 
				"Error: cannot open name of file\n");
                        //invalid name
                        break;
                case 4:
                        fprintf(stderr, 
				"Error: missing command\n");
                        //missing command
                        break;
                case 5:
                        fprintf(stderr, 
				"Error: enter too much argument\n");
                        //too much argument
                        break;
                case 6:
                        fprintf(stderr, 
				"Error: missing output file\n");
                        //missing output file
                        break;
                case 7:
                        //mislocated output redirection
                        break;

                case 8:
                        fprintf(stderr, 
				"Error: invalid variable name\n");

                        
                        break;
                default:
                        fprintf(stderr, 
				"Error: unknown error\n");
                        break;
                        
        }
        return 0;
}

//detect whether user type in exit, if detect, print end message and return 1
int ExitHandler(char* userInput){
        if(strstr(userInput, "exit") != NULL){
                fprintf(stderr,"Bye...\n+ completed '%s' [0]\n", userInput);
		return 1;

        }else{
                return 0;
        }
}


int ExecuteDefinedCommand(CommandAndArgument *singleCommand){
        
        RedirectionOutput(singleCommand);
        int ret;
        
        ret = execvp(singleCommand->command, singleCommand->argument);
        
        exit(EXIT_FAILURE);
        return ret;
}





void ExecutePipelineCommands(SshellInput *shell){


        
        int pids[COMMAND_MAX_NUM];
        int fd[COMMAND_MAX_NUM][2];
        
        
        //if we have three commands, we only need to two pipeline
        for (int i = 0; i < shell->numOfCommand - 1; i++){
                if(pipe(fd[i]) < 0){
                        exit(1);
                }
        }
                

        for (int i = 0; i < shell->numOfCommand; i++)
        {
                int pid = fork();
                if (pid == 0){
                
                        if (i != 0){
                                dup2(fd[i - 1][0], STDIN_FILENO); 
                        }

                        if (i != shell->numOfCommand - 1)
                        {
                                dup2(fd[i][1], STDOUT_FILENO);   
                        }

                        
                        for (int j = 0; j < shell->numOfCommand - 1; j++)    
                        {
                                close(fd[j][0]);
                                close(fd[j][1]);
                        }
                        
                        
                        ExecuteCommand(&shell->listOfCommand[i]);
                        
                        exit(EXIT_FAILURE);
                }else{
                        pids[i] = pid;
                }
                        
        }

        for (int i = 0; i < shell->numOfCommand - 1; i++)       
        {
                close(fd[i][0]);
                close(fd[i][1]);
        }
        int status;
        for(int i = 0; i < shell->numOfCommand; i++){
                waitpid(pids[i],&status, 0);
                printf("%d\n", status);
                if(status != 0){
                        shell->listOfCommand[i].isSuccess = 0;
                }else{
                        printf("success");
                        shell->listOfCommand[i].isSuccess = 1;
                }
        }
        
        
        
}



void RedirectionOutput(CommandAndArgument *singleCommand){

        int fd;
        char fileName[PATH_MAX_LEN];
        if(singleCommand->isRedirect == 0){
                return;
        }else{
                int indexOfRedirectSign;
                for(int i = singleCommand->numOfArgument; i > 0; i--){
                        if(strcmp(singleCommand->argument[i], ">") == 0){
                                indexOfRedirectSign = i;
                        }
                }
                if(singleCommand->argument[indexOfRedirectSign + 1] == NULL){
                        //missing file name
                        return;
                }

                strcpy(fileName, singleCommand->argument[indexOfRedirectSign + 1]);
                singleCommand->argument[indexOfRedirectSign] = NULL;
                singleCommand->argument[indexOfRedirectSign + 1] = NULL;
                
        }
        

        fd = open(fileName, O_RDWR|O_CREAT);
        if(fd != -1){
                
                dup2(fd, STDOUT_FILENO);
                close(fd);
        }else{
                fprintf(stderr, "Error: cannot open output file\n");
        }
}

//if function find these command "pwd", "cd" return 1
//this function run the build in command, like cd pwd
//return 0 if do not execute any command else return 1 return -1 if do not success
int ExecuteBuildInCommand(CommandAndArgument *singleCommand){
        //RedirectionOutput(singleCommand);
        char *buffer = (char*)malloc(PATH_MAX_LEN);
        getcwd(buffer, PATH_MAX_LEN);
        if(strstr(singleCommand->command, "pwd") != NULL){
                //getcwd(buffer, PATH_MAX_LEN);
                fprintf(stdout, "%s\n", buffer);
                return 1;
        }else if(strstr(singleCommand->command, "cd") != NULL){
                if(singleCommand->numOfArgument == 0){
                        strcpy(singleCommand->argument[1], "HOME");
                }
                
                char* temp = (char*)malloc(COMMAND_MAX_LEN * sizeof(char));
                strcat(temp, buffer);
                strcat(temp, "/");
                strcat(temp, singleCommand->argument[1]);
                strcpy(singleCommand->argument[1], temp);


                printf("%s", singleCommand->argument[1]);
                if(chdir(singleCommand->argument[1]) < 0){
                        //can not cd file
                        ErrorHandler(2);
                        return -1;
                }
                return 1;
        }
        free(buffer);
        return 0;
}

//this function set value to argument in command
//find and retrieve value from contentOfVariable
int SetVariable(VariableDictionary *listOfVariable, SshellInput *shell){
        
        for(int i = 0; i < shell->numOfCommand; i++){
                if(strcmp(shell->listOfCommand[i].command, "set") == 0 && shell->listOfCommand[i].numOfArgument == 2){
                        //retrieve number of variable for setting variable into list
                        int numberOfVariable = listOfVariable->numOfVariables;

                        listOfVariable->nameOfVariable[numberOfVariable] = (char*)malloc(VARIABLE_MAX_NUM  * sizeof(char));
                        listOfVariable->contentOfVariable[numberOfVariable] = (char*)malloc(VARIABLE_MAX_NUM  * sizeof(char));
                        strcpy(listOfVariable->nameOfVariable[numberOfVariable], shell->listOfCommand[i].argument[0]);
                        strcpy(listOfVariable->contentOfVariable[numberOfVariable], shell->listOfCommand[i].argument[1]);
                        numberOfVariable = numberOfVariable + 1;
                        listOfVariable->numOfVariables = numberOfVariable;
                }else if(strcmp(shell->listOfCommand[i].command, "set") == 0 && shell->listOfCommand[i].numOfArgument == 0){
                        ErrorHandler(8);
                        //TODO:
                }
        }
        return 0;
}

int FindVariable(VariableDictionary *listOfVariable, char *tempOfVarName){
        for(int i = 0; i < listOfVariable->numOfVariables; i++){
                if(strcmp(listOfVariable->nameOfVariable[i], tempOfVarName) == 0){
                        return i;
                }
        }
        return -1;
}

int RetrieveVariable(VariableDictionary *listOfVariable, SshellInput *shell){
        char *tempOfVarName;
        int tempOfPosition;
        for(int i = 0; i < shell->numOfCommand; i++){
                //for command 
                if(strstr(shell->listOfCommand[i].command, "$") != NULL){
                        tempOfVarName = strtok(shell->listOfCommand[i].command, "$");
                        tempOfPosition = FindVariable(listOfVariable, tempOfVarName);
                        if(tempOfPosition == -1){

                        }else{
                                strcpy(shell->listOfCommand[i].command, listOfVariable->contentOfVariable[tempOfPosition]);
                        }
                        //TODO: error handling in not finding position

                        
                }

                //for argument
                for(int j = 0; j < shell->listOfCommand[i].numOfArgument; j++){
                        if(strstr(shell->listOfCommand[i].argument[j], "$") != NULL){
                                tempOfVarName = strtok(shell->listOfCommand[i].argument[j], "$");
                                tempOfPosition = FindVariable(listOfVariable, tempOfVarName);
                                if(tempOfPosition == -1){
                                        //for not finding right value
                                        strcpy(shell->listOfCommand[i].argument[j], " ");
                                }else{
                                        strcpy(shell->listOfCommand[i].argument[j], listOfVariable->contentOfVariable[tempOfPosition]);
                                }
                                //TODO: error handling in not finding position

                                
                        }
                        
                }
        }
        return 0;
}


void ExecuteCommand(CommandAndArgument *singleCommand){

        //command + list of argument to command + {command + argument +NULL}
        if(singleCommand->numOfArgument >= 1){
                char *newArgument = (char*)malloc(ARGUMENT_MAX_LEN  * sizeof(char));
                strcpy(newArgument, singleCommand->command);
                
                char *tempArgument = (char*)malloc(ARGUMENT_MAX_LEN  * sizeof(char));
                for(int i = 0; i < COMMAND_MAX_NUM; i++){
                        if(i < singleCommand->numOfArgument){

                                strcpy(tempArgument, singleCommand->argument[i]);
                                strcpy(singleCommand->argument[i], newArgument);
                                strcpy(newArgument, tempArgument);
                                
                        }else if(i == singleCommand->numOfArgument){
                                singleCommand->argument[i] = (char*)malloc(
                                        ARGUMENT_MAX_LEN  * sizeof(char));
                                strcpy(singleCommand->argument[i], newArgument);
                                
                        }else{
                                singleCommand->argument[i] = (char*)malloc(
                                        ARGUMENT_MAX_LEN  * sizeof(char));
                                singleCommand->argument[i] = NULL;
                        }  
                }
        }
        char *tempCommand = (char*)malloc(ARGUMENT_MAX_LEN  * sizeof(char));
        strcpy(tempCommand, "");
        
        strcat(tempCommand, singleCommand->command);

        strcpy(singleCommand->command, tempCommand);
        
        
        if(ExecuteBuildInCommand(singleCommand) == 0){
                ExecuteDefinedCommand(singleCommand);
        }

        
        
}


void ViewStart(){
        char userInput[CMD_MAX_LEN];
        SshellInput shell;
        //CommandAndArgument listOfCommand[COMMAND_MAX_NUM];
        VariableDictionary listOfVariable;
        listOfVariable.numOfVariables = 0;
        printf("sshell@ucd$ ");
        fflush(stdout);
        while(fgets(userInput, CMD_MAX_LEN, stdin) != NULL){
                
                //find postion of \n and set it to 0 for deleting 
                userInput[strcspn(userInput, "\n")] = 0;

                //TODO: may need fix
                if(ExitHandler(userInput) == 1){
                        exit(0);
                }

                //backup user input for final print result
                shell.savedCommand = (char*)malloc(COMMAND_MAX_LEN * sizeof(char));
                strcpy(shell.savedCommand, userInput);

                //split input to struct CommandAndArgument
                SplitInput(userInput, shell.listOfCommand, &shell.numOfCommand);
                
                if(SetVariable(&listOfVariable, &shell) == 0){
                        RetrieveVariable(&listOfVariable, &shell);
                
                        ExecutePipelineCommands(&shell);
                }
                PrintMessage(&shell);

                printf("sshell@ucd$ ");
                fflush(STDIN_FILENO);
                setbuf(stdin, NULL);
                
        }
}

void SplitInput(char userInput[CMD_MAX_LEN], CommandAndArgument *listOfCommand, int *numOfCommand){
        
        //store commands from echo 123|tr 2 1 to "echo 123", "tr 2 1"
        char listOfTempString[COMMAND_MAX_NUM][COMMAND_MAX_LEN];
        char* splitString;
        
        char splitChar[2] = "|";
        //if find command set to 1, reset to zero after meet '|'
        int isFindCommand = 0;
        //if command is redirect, set to 1
        //int isRedirect = 0;

        //split whole pipeline in to seperate command
        int index = 0;
        splitString = strtok(userInput, splitChar);
        while(splitString != NULL){
                strcpy(listOfTempString[index],splitString);
                
                index++;
                splitString = strtok(NULL, splitChar);
        }
        //set num of command from how many times while loop go
        *numOfCommand = index;

        //split single combo of command and argument
        *splitChar = ' ';
        int numOfArgument = 0;
        //first loop is for number of combos
        for(int i = 0; i < index; i++){
                
                if(RedirectionCommandHandler(listOfTempString[i]) == 1){
                        listOfCommand[i].isRedirect = 1;
                }else{
                        listOfCommand[i].isRedirect = 0;
                }
                splitString = strtok(listOfTempString[i], splitChar);
                
                while(splitString != NULL){
                        
                        if(isFindCommand == 0){
                                isFindCommand = 1; //find command
                                //tempCommand = (char*)malloc(COMMAND_MAX_LEN * sizeof(char));
                                listOfCommand[i].command = (char*)malloc(COMMAND_MAX_LEN * sizeof(char));
                                strcpy(listOfCommand[i].command, splitString);
                                
                                
                        }else{
                                
                                //tempArgument[i] = (char*)malloc(ARGUMENT_MAX_LEN  * sizeof(char));
                                listOfCommand[i].argument[numOfArgument] = (char*)malloc(ARGUMENT_MAX_LEN  * sizeof(char));
                                strcpy(listOfCommand[i].argument[numOfArgument], splitString);
                                numOfArgument++; 
                        }
                        
                        splitString = strtok(NULL, splitChar);
                }
                
                listOfCommand[i].numOfArgument = numOfArgument;
                isFindCommand = 0; //reset isFindCommand for next loop
                numOfArgument = 0;
                //tempCommand = NULL;
                
        }
        
}

//check whether exist redirection
int RedirectionCommandHandler(char *splitString){
        //handle special case xx>xxx, xx> xx ,xx >xx
        //transform them to the xx > xx
        //return 1 if find > else return 0

        //initialize var for front part and back part whether they exist
        char *finalString = (char*)malloc(ARGUMENT_MAX_LEN  * sizeof(char));     
        char *subString;
        if(strchr(splitString, '>') == NULL){
                //check whether can find >, if do not find return
                return 0;
        }else if(strlen(strchr(splitString, '>')) == 1){
                //find >, but not need to separate
                return 1;
        }else{
                //echo 123>file to echo 123 > file
                subString = strtok(splitString, ">");
                int flag = 0; //first loop flag
                while(subString != NULL){
                        strcat(finalString, subString);
                        if(flag == 0){
                                strcat(finalString, " > ");
                                flag = 1;
                        }
                        subString = strtok(NULL, ">");
                }
                strcpy(splitString, finalString);
                return 1;
        }


        
        
}





int main(){
        
        
        ViewStart();
        
}
