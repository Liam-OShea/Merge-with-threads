/*
 * Author: Liam O'Shea
 * Date: October 24 2019
 * Description: This program reads a list of integers from a file, sorts them using two threads, merges the data
 * using a separate thread, and outputs the result to another text file.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define MAXINTS 500

int integerArray[MAXINTS];
int sortedIntegerArray[MAXINTS];


// Index Struct
typedef struct{
    int start_ind;
    int end_ind;
} params;

// Merging struct
typedef struct{
    int start_ind;
    int end_ind;
    int mid_ind;

} mergeParams;

// Method signatures
void *sorter(void *params);
void *merger(void *params);

int numberOfInts = 0;

char** tokenizeInput(char * in){
    int numTok = 0;           //Number of separate tokens in command
    char ** args = malloc(20 * sizeof(char*));
    char* tok = "nil";
    while ( tok != NULL ) {
        if ( numTok == 0 ) {
            tok = strtok( in, "," );
        }
        else {
            tok = strtok( NULL, "," );
        }
        args[numTok] = tok;
        if ( tok != NULL ) {
            numTok++;
        }
    }

    numberOfInts = numTok;

    return args;
}

int main() {

    // Open file, read comma delimited integers into array
    FILE *intFile;
    //char * fileName = "/Users/Liam/Desktop/OS/OSA2/IntegerList.txt";
    char * fileName = "IntegerList.txt";

    char line[MAXINTS];
    char ** integers;

    intFile = fopen(fileName, "r");
    if(intFile == NULL){
        printf("Could not open file.");
        return 1;
    }

    // Read content of file
    fgets(line, MAXINTS*4, intFile);

    fclose(intFile);

    // Tokenize to separate integers from comma delimited format
    integers = tokenizeInput(line);

    // Convert string type integers to integer type
    for(int i = 0; i < numberOfInts; i++){
        integerArray[i] = atoi(integers[i]);
    }


    // Determine how array will be split
    int splitInd;
    params *N1 = (params *)malloc(sizeof(params));
    params *N2 = (params *)malloc(sizeof(params));
    mergeParams *N3 = (mergeParams *)malloc(sizeof(mergeParams));

    splitInd = numberOfInts/2 - 1;

    N1->start_ind = 0;
    N1->end_ind = splitInd;
    N2->start_ind = splitInd + 1;
    N2->end_ind = numberOfInts - 1;

    // Index information for merging
    N3->start_ind = 0;
    N3->mid_ind = splitInd + 1;
    N3->end_ind = numberOfInts - 1;

    // Print original array for observation
    printf("Unsorted array:\n");
    for(int i = 0; i < numberOfInts; i++){
        printf("%d ", integerArray[i]);
    }
    printf("\n");


    // Create worker threads
    pthread_t worker1;
    pthread_t worker2;

    pthread_create(&worker1, NULL, sorter, N1);
    pthread_create(&worker2, NULL, sorter, N2);

    // Wait for threads to finish
    pthread_join(worker1, NULL);
    pthread_join(worker2, NULL);

    // Create and run merge thread
    pthread_t mergeThread;

    pthread_create(&mergeThread, NULL, merger, N3);

    // Wait for merge thread to finish
    pthread_join(mergeThread, NULL);

    // Free structs
    free(N1);
    free(N2);
    free(N3);

    printf("Array after each half has been sorted by individual threads:\n");
    for(int i = 0; i < numberOfInts; i++){
        printf("%d ", integerArray[i]);
    }
    printf("\nSorted array after merging:\n");
    for(int i = 0; i < numberOfInts; i++){
        printf("%d ", sortedIntegerArray[i]);
    }

    //Open file for output

    FILE * outFile;
    //outFile = fopen("/Users/Liam/Desktop/OS/OSA2/SortedIntegerList.txt", "w");
    outFile = fopen("SortedIntegerList.txt", "w");
    // Write sorted array to outputfile
    for(int i = 0; i < numberOfInts - 1;i++){
        fprintf (outFile, "%d,", sortedIntegerArray[i]);
    }
    // Write last integer without comma separation
    fprintf(outFile, "%d", sortedIntegerArray[numberOfInts-1]);

    /* close the file*/
    fclose (outFile);
    return 0;

}

// Bubble Sort
void *sorter(void *data){
    // We will use bubble sort. It is not efficient but it is easy to write.
    params * paramPointer = (params *) data;
    int tmp;
    for(int i = paramPointer->start_ind; i < paramPointer->end_ind; i++){
        for(int j = paramPointer->start_ind; j < paramPointer->end_ind; j++){
            if (integerArray[j+1] < integerArray[j]){
                tmp = integerArray[j+1];
                integerArray[j+1] = integerArray[j];
                integerArray[j] = tmp;
            }
        }
    }
    pthread_exit(0);
}

void *merger(void *params){
    mergeParams * mparams = (mergeParams *) params;
    int index1 = 0;
    int index2 = mparams->mid_ind;
    int sortedArrayIndex = 0;

    while(index1 < mparams->mid_ind && index2 <= mparams->end_ind){
        if(integerArray[index1] < integerArray[index2]){
            sortedIntegerArray[sortedArrayIndex] = integerArray[index1];
            index1++;
            sortedArrayIndex++;
        } else{
            sortedIntegerArray[sortedArrayIndex] = integerArray[index2];
            index2++;
            sortedArrayIndex++;
        }
    }

    // First conditional is case in which left half finishes first and we must append
    // remainder of right half to sorted list
    if(index1 > mparams->mid_ind - 1){
        while(index2 <= mparams->end_ind){
            sortedIntegerArray[sortedArrayIndex] = integerArray[index2];
            index2++;
            sortedArrayIndex++;
        }
    } else{
        while(index1 < mparams->mid_ind){
            sortedIntegerArray[sortedArrayIndex] = integerArray[index1];
            index1++;
            sortedArrayIndex++;
        }

    }
    pthread_exit(0);
}