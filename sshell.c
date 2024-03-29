#include "sshell.h"


int main(){
        //program start 
        ViewStart();  
}


void ViewStart(){
        //initialize required variable
        char userInput[CMD_MAX_LEN];
        SshellInput shell;

        //create steak of directory
        shell.directoryStack = (DirectoryList*)malloc(sizeof(DirectoryList));
        //get current path
        char *buffer = (char*)malloc(PATH_MAX_LEN);
        getcwd(buffer, PATH_MAX_LEN);
        //push current path to the steak of directory
        Directory *newDirectory = (Directory*)malloc(sizeof(Directory));
        newDirectory->DirectoryPath = 
                (char*)malloc(PATH_MAX_LEN * sizeof(char));
        strcpy(newDirectory->DirectoryPath, buffer);
        newDirectory->nextDirectory = NULL; 
        shell.directoryStack->startDirectory = newDirectory;
        shell.directoryStack->endDirectory = newDirectory;
        shell.directoryStack->numOfDirectory = 1;
        
        printf("sshell@ucd$ ");
       
        while(fgets(userInput, CMD_MAX_LEN, stdin) != NULL){
                if (!isatty(STDIN_FILENO))
                        printf("%s", userInput);
                fflush(stdout);
                //find postion of \n and set it to 0 for deleting 
                userInput[strcspn(userInput, "\n")] = 0;

                
                if(ExitHandler(userInput) == 1){
                        
                        exit(0);
                }

                //backup user input for final print result
                shell.savedCommand = 
                        (char*)malloc(COMMAND_MAX_LEN * sizeof(char));
                strcpy(shell.savedCommand, userInput);

                //split input to struct CommandAndArgument
                SplitInput(userInput, shell.listOfCommand, &shell.numOfCommand);
                
                //detect whether occur parsing error
                int isError = 0;
                for(int i = 0; i < shell.numOfCommand; i++){
                        if(shell.listOfCommand[i].isError == 1){
                                isError = 1;
                        }
                }
                
                if(isError == 0){
                        //execute the pipeline or directly execute
                        ExecutePipelineCommands(&shell);
                        //print out completed message
                        PrintMessage(&shell);
                }

                //reset whole data of command and argument
                for(int i = 0; i < shell.numOfCommand; i++){
                        free(shell.listOfCommand[i].command);

                        int minNum;
                        if(ARGUMENT_MAX_NUM < shell.listOfCommand[i].numOfArgument){
                                minNum = ARGUMENT_MAX_NUM;
                        }else{
                                minNum = shell.listOfCommand[i].numOfArgument;
                        }
                        for(int j = 0; j < minNum; j++){
                                free(shell.listOfCommand[i].argument[j]);
                        }
                        shell.listOfCommand[i].numOfArgument = 0; 
                        shell.listOfCommand[i].isRedirect = -1;
                        shell.listOfCommand[i].isSuccess = -1; 
                        shell.listOfCommand[i].isInverseRedirect = -1;
                        shell.listOfCommand[i].isError = -1; 
                }
                
                shell.numOfCommand = 0;
                
                printf("sshell@ucd$ ");
                fflush(stdout);
                
                
        }
}

int ExitHandler(char* userInput){
        
        //check whether contain key word
        if(strstr(userInput, "exit") != NULL){
                //print exit message
                fprintf(stderr,"Bye...\n+ completed '%s' [0]", userInput);
		return 1;

        }else{
                return 0;
        }
}

void SplitInput(char userInput[CMD_MAX_LEN], CommandAndArgument *listOfCommand, 
        int *numOfCommand){
        
        //store commands from echo 123|tr 2 1 to "echo 123", "tr 2 1"
        char listOfTempString[COMMAND_MAX_NUM][COMMAND_MAX_LEN];
        char* splitString;
        
        char splitChar[2] = "|";
        //if find command set to 1, reset to zero after meet '|'
        int isFindCommand = 0;
        int numOfPipeline = 0;
        //if command is redirect, set to 1
        
        for(unsigned long i = 0; i < strlen(userInput); i++){
                char tempCharacter = userInput[i];
                if(tempCharacter == '|'){
                        numOfPipeline += 1;
                }
        }

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
                
                if(listOfTempString[i] == NULL){
                        ErrorHandler(ERR_MISS_COMMAND);
                        listOfCommand[i].isError = 1;
                }
                
                //check whether contain redirection or inverse redirection
                int flag = RedirectionCommandHandler(listOfTempString[i]);
                if(flag == 1){
                        listOfCommand[i].isRedirect = 1;
                }else if(flag == 0){
                        listOfCommand[i].isRedirect = 0;
                }
                flag = InverseRedirectionCommandHandler(listOfTempString[i]);
                if(flag == 1){
                        listOfCommand[i].isInverseRedirect = 1;
                }else if(flag == 0){
                        listOfCommand[i].isInverseRedirect = 0;
                }

                //split the input by empty space
                splitString = strtok(listOfTempString[i], splitChar);         
                while(splitString != NULL){
                        
                        if(isFindCommand == 0){
                                isFindCommand = 1; //find command
                                //put command in the struct
                                listOfCommand[i].command = 
                                        (char*)malloc(COMMAND_MAX_LEN * sizeof(char));
                                strcpy(listOfCommand[i].command, splitString);
                                
                                
                        }else{
                                //put argument in the struct
                                listOfCommand[i].argument[numOfArgument] = 
                                        (char*)malloc(ARGUMENT_MAX_LEN  * sizeof(char));
                                strcpy(listOfCommand[i].argument[numOfArgument], splitString);
                                numOfArgument++; 
                        }
                        
                        splitString = strtok(NULL, splitChar);
                }

                //missing command before or after pipeline
                if(numOfPipeline != (index - 1)){
                        listOfCommand[i].isError = 1;
                        ErrorHandler(ERR_MISS_COMMAND);
                }
                
                //check whether too much arguments
                if(numOfArgument > 10){
                        listOfCommand[i].isError = 1;
                        ErrorHandler(ERR_TOOMANY_ARG);
                }

                //missing command before or after redirection
                if(strcmp(listOfCommand[i].command, ">") == 0 || 
                                strcmp(listOfCommand[i].command, "<") == 0){

                        listOfCommand[i].isError = 1;
                        ErrorHandler(ERR_MISS_COMMAND);
                }

                //check errror about missing output file or mislocate redirection
                for(int j = 0; j < numOfArgument; j++){
                        if(strcmp(listOfCommand[i].argument[j], ">") == 0 || 
                                strcmp(listOfCommand[i].argument[j], "<") == 0){

                                if(j + 2 > numOfArgument){
                                        ErrorHandler(ERR_OUTPUT_FILE);
                                        listOfCommand[i].isError = 1;
                                }
                                if(i != numOfPipeline){
                                        ErrorHandler(ERR_MISLOCATE_REDIR);
                                        listOfCommand[i].isError = 1;
                                }
                                
                        }
                }

                listOfCommand[i].numOfArgument = numOfArgument;
                isFindCommand = 0; //reset isFindCommand for next loop
                numOfArgument = 0;
                
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


int InverseRedirectionCommandHandler(char *splitString){
        //handle special case xx>xxx, xx> xx ,xx >xx
        //transform them to the xx > xx
        //

        //initialize var for front part and back part whether they exist
        char *finalString = (char*)malloc(ARGUMENT_MAX_LEN  * sizeof(char));     
        char *subString;
        if(strchr(splitString, '<') == NULL){
                //check whether can find >, if do not find return
                return 0;
        }else{
                //echo 123>file to echo 123 > file
                subString = strtok(splitString, "<");
                int flag = 0; //first loop flag
                
                while(subString != NULL){
                        
                        strcat(finalString, subString);
                        if(flag == 0){
                                strcat(finalString, " < ");
                                flag = 1;
                        }
                        subString = strtok(NULL, "<");
                }
                      
                strcpy(splitString, finalString);
                return 1;
        } 
        
}

void ExecutePipelineCommands(SshellInput *shell){

        //check whether need execute build in command, if execute direct return
        if(ExecuteBuildInCommand(&shell->listOfCommand[0], shell) == 1){
                return;
        }
        
        //initialize pids and fds
        int pids[COMMAND_MAX_NUM];
        int fds[COMMAND_MAX_NUM][2];
        
        //set value of fds based on the number of command 
        //if we have three commands, we only need to two pipeline
        for (int i = 0; i < shell->numOfCommand - 1; i++){
                if(pipe(fds[i]) < 0){
                        exit(1);
                }
        }
                
        //pipeline start
        for (int i = 0; i < shell->numOfCommand; i++){

                //initialize pid and fork
                int pid = fork();
                if (pid == 0){
                        //child process

                        //read input from previous command
                        if (i - 1 >= 0){
                                dup2(fds[i - 1][0], STDIN_FILENO); 
                        }

                        //put the result into pipeline
                        if (i + 1 <= shell->numOfCommand - 1){
                                dup2(fds[i][1], STDOUT_FILENO);   
                        }

                        //close all the pipelines
                        for (int j = 0; j < shell->numOfCommand - 1; j++){
                                if(close(fds[j][0]) + close(fds[j][1]) != 0){
                                        exit(1);
                                }
                               
                        }
                        
                        //execute command
                        ExecuteCommand(&shell->listOfCommand[i]);
                        exit(EXIT_FAILURE);
                }else if(pid > 0){
                        //store the pid for further wait pid
                        pids[i] = pid;
                }else if(pid == -1){
                        //fork fails
                        return;
                }
                        
        }
        
        //close all the pipelines
        for (int i = 0; i < shell->numOfCommand - 1; i++){
                if(close(fds[i][0]) + close(fds[i][1]) != 0){
                        exit(1);
                }
        }
        
        //wait all the child end and check exit status
        int status;
        for(int i = 0; i < shell->numOfCommand; i++){
                waitpid(pids[i],&status, 0);
                if(status != 0){
                        
                        shell->listOfCommand[i].isSuccess = 0;
                }else{
                        
                        shell->listOfCommand[i].isSuccess = 1;
                }
        }
        
        
        
}

int ExecuteBuildInCommand(CommandAndArgument *singleCommand, SshellInput *shell){

        //check existence of keyword, if not find return
        if(strstr(singleCommand->command, "pwd") == NULL && 
                strstr(singleCommand->command, "cd") == NULL &&
                strstr(singleCommand->command, "dirs") == NULL && 
                strstr(singleCommand->command, "pushd") == NULL && 
                strstr(singleCommand->command, "popd") == NULL){

                return 0;

        }
        
        //relocate the position of command and argument
        if(singleCommand->numOfArgument >= 1){
                char *newArgument = 
                        (char*)malloc(ARGUMENT_MAX_LEN  * sizeof(char));
                strcpy(newArgument, singleCommand->command);
                
                char *tempArgument = 
                        (char*)malloc(ARGUMENT_MAX_LEN  * sizeof(char));
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
        }else if(singleCommand->numOfArgument == 0){
                //move command for condition no argument
                singleCommand->argument[0] = (char*)malloc(
                                        ARGUMENT_MAX_LEN  * sizeof(char));
                singleCommand->argument[1] = (char*)malloc(
                                        ARGUMENT_MAX_LEN  * sizeof(char));
                strcpy(singleCommand->argument[0], singleCommand->command);
                singleCommand->argument[1] = NULL;

        }
        char *tempCommand = (char*)malloc(ARGUMENT_MAX_LEN  * sizeof(char));
        strcpy(tempCommand, "");
        
        strcat(tempCommand, singleCommand->command);

        strcpy(singleCommand->command, tempCommand);


        //get pwd
        char *buffer = (char*)malloc(PATH_MAX_LEN);
        getcwd(buffer, PATH_MAX_LEN);
        if(strstr(singleCommand->command, "pwd") != NULL){
                //print pwd
                fprintf(stdout, "%s\n", buffer);
                singleCommand->isSuccess = 1;
                return 1;
        }else if(strstr(singleCommand->command, "cd") != NULL){
                //if do not have path, cd to the home
                if(singleCommand->numOfArgument == 0){
                        singleCommand->argument[1] = 
                                (char*)malloc(PATH_MAX_LEN * sizeof(char));
                        strcpy(singleCommand->argument[1], getenv("HOME"));
                }

                //cd into directory
                if(chdir(singleCommand->argument[1]) < 0){
                        //can not cd file
                        singleCommand->isError = 1;
                        ErrorHandler(ERR_CD_DIR);
                        return -1;
                }
                //set success flag
                singleCommand->isSuccess = 1;
                return 1;
        }
        
        
        if(strstr(singleCommand->command, "dirs") != NULL){
                //if stack is empty, return 
               
                
                //initialize variable 
                char *result[PATH_MAX_NUM];
                int index = 0;
                //initialize temp node for looop
                Directory *currentDirectory = 
                        shell->directoryStack->startDirectory;
                
                //loop whole stack and push it into result list
                do{
                        result[index] = 
                                (char*)malloc(PATH_MAX_LEN * sizeof(char));
                        strcat(result[index], currentDirectory->DirectoryPath);
                        index += 1;
                        currentDirectory = 
                                (Directory*)currentDirectory->nextDirectory;
                }while(currentDirectory == shell->directoryStack->endDirectory);
                //printf("%s\n", shell->directoryStack->defaultDirectory->DirectoryPath);
                //print result
                for(int i = index - 1; i >= 0; i--){
                        printf("%s\n", result[i]);
                }
                
                singleCommand->isSuccess = 1;
                return 1;

        }else if(strstr(singleCommand->command, "pushd") != NULL){

                //initialize the directory path
                char *path = (char*)malloc(PATH_MAX_LEN * sizeof(char));
                strcat(path, buffer);
                strcat(path, "/");
                strcat(path, singleCommand->argument[1]);

                //initialize directory node
                Directory *newDirectory = (Directory*)malloc(sizeof(Directory));
                newDirectory->DirectoryPath = 
                        (char*)malloc(PATH_MAX_LEN * sizeof(char));
                strcpy(newDirectory->DirectoryPath, path);
                newDirectory->nextDirectory = NULL;
                

                //cd into directory
                if(chdir(path) < 0){
                        //can not cd file
                        singleCommand->isError = 1;
                        ErrorHandler(ERR_CD_DIR);
                        return -1;
                }

                //push node into stack 
                if(shell->directoryStack->numOfDirectory == 0){
                        shell->directoryStack->startDirectory = (Directory*)malloc(sizeof(Directory));
                        shell->directoryStack->endDirectory = (Directory*)malloc(sizeof(Directory));
                        shell->directoryStack->startDirectory = newDirectory;
                        shell->directoryStack->endDirectory = newDirectory;
                }else{
                        shell->directoryStack->endDirectory->nextDirectory = 
                                (struct Directory*)newDirectory;
                        shell->directoryStack->endDirectory = newDirectory;
                        
                }
                shell->directoryStack->numOfDirectory += 1;
                singleCommand->isSuccess = 1;
                return 1;

        }else if(strstr(singleCommand->command, "popd") != NULL){

                if(shell->directoryStack->numOfDirectory == 1){
                        singleCommand->isError = 1;
                        ErrorHandler(ERR_STACK_EMPTY);
                        return 1;
                }

                //pop the node and get path name
                char *path = (char*)malloc(PATH_MAX_LEN * sizeof(char));
                strcpy(path, shell->directoryStack->startDirectory->DirectoryPath);
                //save address for free
                Directory *savedDirectory = shell->directoryStack->startDirectory;
                

                //reset the start node of the list
                shell->directoryStack->startDirectory = 
                        (Directory*)shell->
                                directoryStack->startDirectory->nextDirectory;
                shell->directoryStack->numOfDirectory -= 1;
                //free old node
                free(savedDirectory);
                //cd into directory 
                if(chdir(path) < 0){
                        //can not cd file
                        singleCommand->isError = 1;
                        ErrorHandler(ERR_CD_DIR);
                        return -1;
                }
                singleCommand->isSuccess = 1;
                return 1;

        }

        return 0;
}


void ExecuteCommand(CommandAndArgument *singleCommand){

        //command + list of argument to command + {command + argument +NULL}
        if(singleCommand->numOfArgument >= 1){
                char *newArgument = 
                        (char*)malloc(ARGUMENT_MAX_LEN  * sizeof(char));
                strcpy(newArgument, singleCommand->command);
                
                char *tempArgument = 
                        (char*)malloc(ARGUMENT_MAX_LEN  * sizeof(char));
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
        }else if(singleCommand->numOfArgument == 0){
                //move command for condition no argument
                singleCommand->argument[0] = (char*)malloc(
                                        ARGUMENT_MAX_LEN  * sizeof(char));
                singleCommand->argument[1] = (char*)malloc(
                                        ARGUMENT_MAX_LEN  * sizeof(char));
                strcpy(singleCommand->argument[0], singleCommand->command);
                singleCommand->argument[1] = NULL;

        }


        //printf("%s %s\n", singleCommand->command, singleCommand->argument[0]);
        char *tempCommand = (char*)malloc(ARGUMENT_MAX_LEN  * sizeof(char));
        strcpy(tempCommand, "");
        
        strcat(tempCommand, singleCommand->command);

        strcpy(singleCommand->command, tempCommand);
        
        //call the real execute function
        ExecuteDefinedCommand(singleCommand);
        
}




//accept a single command struct and return if falure 
int ExecuteDefinedCommand(CommandAndArgument *singleCommand){
        int ret;
        //check whether has inverse redirect if yes, call inverse redirect func
        if(singleCommand->isInverseRedirect == 1){
                InverseRedirect(singleCommand);
        }
        
        //check whther need to redirect 
        RedirectionOutput(singleCommand);
        if(singleCommand->isError != 1){
               
        
                //exeute command and command
                ret = execvp(singleCommand->command, singleCommand->argument);
                ErrorHandler(ERR_CMD_NOTFOUND);
        }

        
        
        //only reach this line when execute fail
        exit(EXIT_FAILURE);
        return ret;
}




void PrintMessage(SshellInput *shell){

        //initialize the completed message
        char* result = (char*)malloc(COMMAND_MAX_LEN * sizeof(char));
        strcpy(result,  "+ completed '");
        strcat(result, shell->savedCommand);
        strcat(result, "' ");

        //set error index
        for(int i = 0; i < shell->numOfCommand; i++){
                
                char errorIndex =  (char)((i + 1) + '0');
                //check the isSuccess in the CommandAndArgument
                if(shell->listOfCommand[i].isSuccess == 1){
                        strcat(result, "[0]");
                }else{

                        
                        strcat(result, "[");
                        strcat(result, &errorIndex);
                        strcat(result, "]");
                }
        }
        strcat(result, "\n");
        //print complete message
        fprintf(stderr, "%s", result);
        fflush(stdout);
}


void InverseRedirect(CommandAndArgument *singleCommand){

        //initialize fd and file name
        int fd;
        char fileName[PATH_MAX_LEN];

        //did not contain symbol <, return 
        if(singleCommand->isInverseRedirect == 0){
                return;
        }else{

                //find the argument of file name 
                int indexOfRedirectSign;
                for(int i = singleCommand->numOfArgument - 1; i > 0; i--){
                        if(strcmp(singleCommand->argument[i], "<") == 0){
                                indexOfRedirectSign = i;
                        }
                }

                //missing argument
                if(indexOfRedirectSign - 1 < 0){
                        return;
                }

                //missing file name
                if(singleCommand->argument[indexOfRedirectSign - 1] == NULL){
                        
                        return;
                }

                //set file name
                strcpy(fileName, singleCommand->argument[indexOfRedirectSign + 1]);
                singleCommand->argument[indexOfRedirectSign] = NULL;
                singleCommand->argument[indexOfRedirectSign + 1] = NULL;
                
        }

        //open file and read it into pipeline
        fd = open(fileName, O_RDWR|O_CREAT);
        if(fd != -1){
                
                dup2(fd, STDIN_FILENO);
                close(fd);
        }else{
                ErrorHandler(ERR_OPEN_FILE);
                singleCommand->isError = 1;
                singleCommand->isSuccess = 0;
        }

}


void RedirectionOutput(CommandAndArgument *singleCommand){

        //initialize fd and file name
        int fd;
        char fileName[PATH_MAX_LEN];

        //did not contain symbol <, return 
        if(singleCommand->isRedirect == 0){
                return;
        }else{
                //find the argument of file name 
                int indexOfRedirectSign;
                for(int i = singleCommand->numOfArgument - 1; i > 0; i--){
                        if(strcmp(singleCommand->argument[i], ">") == 0){
                                indexOfRedirectSign = i;
                        }
                }

                //missing 
                if(singleCommand->argument[indexOfRedirectSign + 1] == NULL){
                        //missing file name
                        return;
                }
                //set file name
                strcpy(fileName, singleCommand->argument[indexOfRedirectSign + 1]);
                singleCommand->argument[indexOfRedirectSign] = NULL;
                singleCommand->argument[indexOfRedirectSign + 1] = NULL;
                
        }
        
        //open file and read it into pipeline
        fd = open(fileName, O_RDWR|O_CREAT);
        if(fd != -1){
                
                dup2(fd, STDOUT_FILENO);
                close(fd);
        }else{
                ErrorHandler(ERR_OPEN_FILE);
                singleCommand->isError = 1;
                singleCommand->isSuccess = 0;
        }
}




int ErrorHandler(int errorType){

        //check which error status number
        switch(errorType){
                case ERR_CMD_NOTFOUND:
                        //can not open output file
                        fprintf(stderr, 
				"Error: command not found\n");
                        
                        break;
                case ERR_CD_DIR:
                        //can not cd
                        fprintf(stderr, 
				"Error: cannot cd into directory\n");
                        
                        break;
                case ERR_OPEN_FILE:
                        fprintf(stderr, 
				"Error: cannot open output file\n");
                        //invalid name
                        break;
                case ERR_MISS_COMMAND:
                        fprintf(stderr, 
				"Error: missing command\n");
                        //missing command
                        break;
                case ERR_TOOMANY_ARG:
                        fprintf(stderr, 
				"Error: too many process arguments\n");
                        //too much argument
                        break;
                case ERR_OUTPUT_FILE:
                        fprintf(stderr, 
				"Error: no output file\n");
                        //missing output file
                        break;
                case ERR_MISLOCATE_REDIR:
                        fprintf(stderr,
                                "Error: mislocated output redirection\n");
                        //mislocated output redirection
                        break;

                case ERR_STACK_EMPTY:
                        fprintf(stderr, 
				"Error: directory stack empty\n");

                        
                        break;
                        
                default:
                        fprintf(stderr, 
				"Error: unknown error\n");
                        break;
                        
        }
        fflush(stdout);
        return 0;
}
















