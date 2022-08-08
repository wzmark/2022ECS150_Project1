
#include <stdio.h>
#include <stdlib.h>
#include "array.h"

int main(){
    selfDefinedArray arr = arrayCreate(5);

    arrayAppend(&arr, 'a');
    printf("%c", arrayPopUp(&arr));
    arrayFree(&arr);
}

//This function will initialize array basing on the argument size
selfDefinedArray arrayCreate(int size){
        selfDefinedArray arr;
        arr.array = (char*)malloc(size * sizeof(char));
        arr.maxSizeOfArray = size;
        arr.currentSizeOfArray = 0;
        return arr;

}

void arrayExpand(selfDefinedArray *arr, int size){
    
        //get new array size
        int newSizeOfArray = size + arr->maxSizeOfArray;
        //initialize new array for new size
        char* newArray = (char*)malloc(newSizeOfArray * sizeof(char));

        //move old data in the old array to new array
        for(int i = 0; i < arr->currentSizeOfArray; i++){
            newArray[i] = arr->array[i];
        }

        //free old array space and set new array to it
        free(arr->array);
        arr->maxSizeOfArray = newSizeOfArray;
        arr->array = newArray;



}

void arrayFree(selfDefinedArray *arr){
        //if arr is empty, return without more destroy steps
        if(arr == NULL){
            return;
        }

        //destroy array
        free(arr->array);
        arr->array = NULL;
        arr->maxSizeOfArray = 0;
        arr->currentSizeOfArray = 0;

        return;
}

//return the size which has been used in array
int arraySize(selfDefinedArray *arr){
        return arr->currentSizeOfArray;
}

//append new value to the end of the array
void arrayAppend(selfDefinedArray *arr, char value){

        //check whether array has enough space for the new value
        //if array does not has enough space, expand 10 char space for it
        if(arr->currentSizeOfArray == arr->maxSizeOfArray){
            arrayExpand(arr, 10);
        }

        //append new char to the array
        arr->currentSizeOfArray += 1;
        arr->array[arr->currentSizeOfArray - 1] = value;
        
        return;
}

//return char in array at index position
char arrayAt(selfDefinedArray *arr, int index){
        return arr->array[index];
}

//set position index to empty
void arrayDelete(selfDefinedArray *arr, int index){
        arr->array[index] = ' ';
}

//pop up the last value in the array
char arrayPopUp(selfDefinedArray *arr){
        //get the last value at temporary value
        char lastValue = arrayAt(arr, arr->currentSizeOfArray);
        //remove from the array
        arrayDelete(arr, arr->currentSizeOfArray);
        //decrement current size
        arr->currentSizeOfArray -= 1;
        return lastValue;
}

void arraySet(selfDefinedArray *newArray, selfDefinedArray *oldArray){
    if(newArray->maxSizeOfArray < oldArray->currentSizeOfArray){
        arrayExpand(newArray, 10);
    }

    for(int i = 0; i < oldArray->currentSizeOfArray; i++){
        newArray->array[i] = oldArray->array[i];
    }
}

void arraySplit(selfDefinedArray *array, char symbol, selfDefinedArray *splitArray){
    selfDefinedArray tempArray = arrayCreate(10);

    if(array->currentSizeOfArray == 0){
        return;
    }
    for(int i = 0; i < array->currentSizeOfArray; i++){
        if(array->array[i] != symbol){
            arrayAppend(splitArray, array->array[i]);
        }else{
            break;
        }
        
    }

}

