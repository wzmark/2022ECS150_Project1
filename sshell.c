#include "sshell.h"


//
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
        fflush(stdout);
        return 0;
}

//detect whether user type in exit, if detect, print end message and return 1
int ExitHandler(char* userInput){
        //check whether user enter exit
        //if find print end message and return 1, else return 0
        if(strstr(userInput, "exit") != NULL){
                fprintf(stderr,"Bye...\n+ completed '%s' [0]\n", userInput);
		return 1;

        }else{
                return 0;
        }
}



//accept a single command struct and return if falure 
int ExecuteDefinedCommand(CommandAndArgument *singleCommand){
        
        //check whether has inverse redirect if yes, call inverse redirect func
        if(singleCommand->isInverseRedirect == 1){
                InverseRedirect(singleCommand);
        }
        
        RedirectionOutput(singleCommand);
        int ret;
        
        //exeute command and command
        ret = execvp(singleCommand->command, singleCommand->argument);
        
        //only reach this line when execute fail
        exit(EXIT_FAILURE);
        return ret;
}



/*
*
*
*/
void ExecutePipelineCommands(SshellInput *shell){

        if(ExecuteBuildInCommand(&shell->listOfCommand[0], shell) == 1){
                return;
        }
        
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
                //printf("%d\n", status);
                if(status != 0){
                        shell->listOfCommand[i].isSuccess = 0;
                }else{
                        
                        shell->listOfCommand[i].isSuccess = 1;
                }
        }
        
        
        
}

void InverseRedirect(CommandAndArgument *singleCommand){
        int fd;
        char fileName[PATH_MAX_LEN];
        if(singleCommand->isInverseRedirect == 0){
                return;
        }else{
                int indexOfRedirectSign;
                for(int i = singleCommand->numOfArgument - 1; i > 0; i--){
                        if(strcmp(singleCommand->argument[i], "<") == 0){
                                indexOfRedirectSign = i;
                        }
                }
                if(indexOfRedirectSign - 1 < 0){
                        return;
                }
                if(singleCommand->argument[indexOfRedirectSign - 1] == NULL){
                        //missing file name
                        return;
                }

                strcpy(fileName, singleCommand->argument[indexOfRedirectSign + 1]);
                singleCommand->argument[indexOfRedirectSign] = NULL;
                singleCommand->argument[indexOfRedirectSign + 1] = NULL;
                
        }
        printf("%s", fileName);
        fd = open(fileName, O_RDWR|O_CREAT);
        if(fd != -1){
                
                dup2(fd, STDIN_FILENO);
                close(fd);
        }else{
                fprintf(stderr, "Error: cannot open intput file\n");
        }

}


void RedirectionOutput(CommandAndArgument *singleCommand){

        int fd;
        char fileName[PATH_MAX_LEN];
        if(singleCommand->isRedirect == 0){
                return;
        }else{
                int indexOfRedirectSign;
                for(int i = singleCommand->numOfArgument - 1; i > 0; i--){
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
int ExecuteBuildInCommand(CommandAndArgument *singleCommand, SshellInput *shell){
        if(strstr(singleCommand->command, "pwd") == NULL && 
                strstr(singleCommand->command, "cd") == NULL &&
                strstr(singleCommand->command, "dirs") == NULL && 
                strstr(singleCommand->command, "pushd") == NULL && 
                strstr(singleCommand->command, "popd") == NULL){

                return 0;

        }
        


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


        //RedirectionOutput(singleCommand);
        char *buffer = (char*)malloc(PATH_MAX_LEN);
        getcwd(buffer, PATH_MAX_LEN);
        if(strstr(singleCommand->command, "pwd") != NULL){
                //getcwd(buffer, PATH_MAX_LEN);
                fprintf(stdout, "%s\n", buffer);
                singleCommand->isSuccess = 1;
                return 1;
        }else if(strstr(singleCommand->command, "cd") != NULL){
                if(singleCommand->numOfArgument == 0){
                        strcpy(singleCommand->argument[1], "HOME");
                }
                
                char* temp = (char*)malloc(COMMAND_MAX_LEN * sizeof(char));
                //strcat(temp, buffer);
                //strcat(temp, "/");
                strcat(temp, singleCommand->argument[1]);
                strcpy(singleCommand->argument[1], temp);


                //printf("%s\n", singleCommand->argument[1]);
                if(chdir(singleCommand->argument[1]) < 0){
                        //can not cd file
                        ErrorHandler(2);
                        return -1;
                }
                singleCommand->isSuccess = 1;
                return 1;
        }
        
        
        if(strstr(singleCommand->command, "dirs") != NULL){
                if(shell->directoryStack.numOfDirectory == 0){
                        return 0;
                }
                //char *result = (char*)malloc(PATH_MAX_LEN * sizeof(char));
                char *result[PATH_MAX_NUM];
                int index = 0;
                Directory *currentDirectory = shell->directoryStack.startDirectory;

                do{
                        result[index] = (char*)malloc(PATH_MAX_LEN * sizeof(char));

                        strcat(result[index], currentDirectory->DirectoryPath);
                        //strcat(result[index], "\n");
                        index += 1;
                        //printf("%s\n", currentDirectory->DirectoryPath);
                        currentDirectory = (Directory*)currentDirectory->nextDirectory;
                }while(currentDirectory == shell->directoryStack.endDirectory);

                for(int i = index - 1; i >= 0; i--){
                        printf("%s\n", result[i]);
                }
                


                singleCommand->isSuccess = 1;
                return 1;

        }else if(strstr(singleCommand->command, "pushd") != NULL){
                char *path = (char*)malloc(PATH_MAX_LEN * sizeof(char));
                strcat(path, buffer);
                strcat(path, "/");
                strcat(path, singleCommand->argument[1]);
                Directory *newDirectory = (Directory*)malloc(sizeof(Directory));
                newDirectory->DirectoryPath = (char*)malloc(PATH_MAX_LEN * sizeof(char));
                strcpy(newDirectory->DirectoryPath, path);
                newDirectory->nextDirectory = NULL;
                
                shell->directoryStack.numOfDirectory += 1;
                if(chdir(path) < 0){
                        //can not cd file
                        ErrorHandler(2);
                        return -1;
                }
                if(shell->directoryStack.numOfDirectory == 0){
                        shell->directoryStack.startDirectory = newDirectory;
                        shell->directoryStack.endDirectory = newDirectory;
                }else{
                        shell->directoryStack.endDirectory->nextDirectory = (struct Directory*)newDirectory;
                        shell->directoryStack.endDirectory = newDirectory;
                        
                }
                singleCommand->isSuccess = 1;
                return 1;

        }else if(strstr(singleCommand->command, "popd") != NULL){
                char *path = (char*)malloc(PATH_MAX_LEN * sizeof(char));
                strcpy(path, shell->directoryStack.startDirectory->DirectoryPath);
                
                shell->directoryStack.startDirectory = (Directory*)shell->directoryStack.startDirectory->nextDirectory;
                shell->directoryStack.numOfDirectory -= 1;
                if(chdir(path) < 0){
                        //can not cd file
                        ErrorHandler(2);
                        return -1;
                }
                singleCommand->isSuccess = 1;
                return 1;

        }

        //free(buffer);
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
        
        ExecuteDefinedCommand(singleCommand);
        

        
        
}


void ViewStart(){
        char userInput[CMD_MAX_LEN];
        SshellInput shell;
        shell.directoryStack.numOfDirectory = 0;
        //CommandAndArgument listOfCommand[COMMAND_MAX_NUM];
        //VariableDictionary listOfVariable;
        //listOfVariable.numOfVariables = 0;
        printf("sshell@ucd$ ");
        fflush(stdout);
        while(fgets(userInput, CMD_MAX_LEN, stdin) != NULL){
                printf("%s", userInput);
                fflush(stdout);
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
                /*
                if(SetVariable(&listOfVariable, &shell) == 0){
                        RetrieveVariable(&listOfVariable, &shell);
                
                        
                }
                */
                
                ExecutePipelineCommands(&shell);
                PrintMessage(&shell);

                printf("sshell@ucd$ ");
                fflush(stdout);
                //setbuf(stdin, NULL);
                
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

                if(InverseRedirectionCommandHandler(listOfTempString[i]) == 1){
                        listOfCommand[i].isInverseRedirect = 1;
                        
                        
                }else{
                        listOfCommand[i].isInverseRedirect = 0;
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
        }else if(strlen(strchr(splitString, '<')) == 1){
                //find >, but not need to separate
                return 1;
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
