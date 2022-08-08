#ifndef _ARRAY_H
#define _ARRAY_H

typedef struct{
    int sizeOfType;
    char* array;
    int maxSizeOfArray;
    int currentSizeOfArray;

}selfDefinedArray;


selfDefinedArray arrayCreate(int size);

void arrayFree(selfDefinedArray *arr);

int arraySize(selfDefinedArray *arr);

void arrayDelete(selfDefinedArray *arr, int index);

void arrayAppend(selfDefinedArray *arr, char value);

char arrayAt(selfDefinedArray *arr, int index);

void arrayExpand(selfDefinedArray *arr, int size);

char arrayPopUp(selfDefinedArray *arr);






#endif