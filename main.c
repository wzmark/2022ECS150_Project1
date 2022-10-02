#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h> 
#include <stdio.h>

int main(void){
    pid_t pid;
        
        int fdNew[2];
        int fdOld[2];
        int status;
        pipe(fdOld);

        int temp = 3;
        
        for(int i = 0; i < temp; i++){
                if(i + 1 < temp){
                        pipe(fdNew);
                        
                }
                
                pid = fork();
                
                if(pid == 0){
                        
                        if(i - 1 >= 0){
                                printf("flag 1\n");
                                close(fdOld[1]);
                                dup2(fdOld[0], STDIN_FILENO);
                                close(fdOld[0]);
                                
                                

                        }
                        if(i + 1 < temp){
                                printf("flag 2\n");
                                close(fdNew[0]);
                                dup2(fdNew[1], STDOUT_FILENO);
                                close(fdNew[1]);
                        }
                        
                        printf("why%d\n", i);
                        exit(0);
                        
                }else if(pid > 0){
                        
                        
                        int status;
                        wait(&status);
                        
                        
                        
                        
                }else { 
                        perror("fork");
                        exit(1); 
                }
                if(temp > 0){
                        close(fdOld[1]);
                        close(fdOld[0]);
                }
        }
        printf("end\n");
}