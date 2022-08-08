#include "sshellModel.h"




void CheckNumOfCommand(SshellInput input){
    int sizeOfArray = input.userInputArray.currentSizeOfArray;
    selfDefinedArray array = input.userInputArray;
    int numOfCommand = 0;

    for(int i = 0; i < sizeOfArray; i++){
        if(arrayAt(&array, i) == '|'){
            numOfCommand += 1;
        }
    }

    input.numOfCommand = numOfCommand;

}

void SplitCommandAndArgument(SshellInput input){
    selfDefinedArray inputArray = input.userInputArray;
    selfDefinedArray tempCommand = arrayCreate(10);
    selfDefinedArray tempArgument = arrayCreate(10);
    
    //if var is 0, var equal to false
    //if var is 1, var equal to true
    int isFindArgument;
    int isFindCommand;

    for(int i = 0; i < inputArray.currentSizeOfArray; i++){

        if(arrayAt(&inputArray, i) == '|'){
            isFindArgument = 0;
            isFindCommand = 0;

            continue;
        }

        if(arrayAt(&inputArray, i) != ' '){
            continue;
        }
        
        

    }
}