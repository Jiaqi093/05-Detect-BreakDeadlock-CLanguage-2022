#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <stdint.h>
#include "utility.h"

/* task 1, read file and count number of processes and files. */
uint32_t* read_file(char* filename, uint32_t* length) {

    FILE *fp;
    uint32_t process, block, wait, count_process = 0, count_file = 0;
    fp = fopen(filename,"r");
    uint32_t* file = malloc(3*sizeof(uint32_t));


    // only scan the second and third numbers (files) and store in dynamic array
    if (fp){
        while (fscanf(fp, "%d %d %d ", &process, &block, &wait) != EOF) {
            *length+=3;
            file = (uint32_t*)realloc(file, *length * sizeof(uint32_t));
            count_process++;  // where we count the number of processes
            file[*length-3] = process;
            file[*length-2] = block;
            file[*length-1] = wait;
        }

        // count the total number of unique files.
        for(uint32_t i=1; i<*length; i++) {
            if(i % 3 != 0) { //not consider the "processes" number
                for(uint32_t j=0; j<i; j++) {
                    if(j % 3 != 0) { // not consider the "processes" number 
                        if (file[i] == file[j]) { // find repeated value, means not unique, so disregard
                            break;
                        }
                    }
                    if (i == j+1) { // loop through out the array and did not find the repetitive value, so it is unique.
                        count_file++;                         
                    }
                }            
            }
        }
        printf("Processes %d\n", count_process);
        printf("Files %d\n", count_file);

    }else{
        printf("Unable to open the file. Please check the input.\n");
    }

    return file;
}



void deadlock_output(uint32_t* pid_list, uint32_t* deadlock_num) {
    if (*deadlock_num > 0) {
        printf("Deadlock detected\n");
        printf("Terminate");


        // sort pid_list
        uint32_t temp = 0;
        for (uint32_t i = 0; i < *deadlock_num; i++) {     
            for (uint32_t j = i+1; j < *deadlock_num; j++) {     
                if(pid_list[i] > pid_list[j]) {    
                    temp = pid_list[i];    
                    pid_list[i] = pid_list[j];    
                    pid_list[j] = temp;    
                }     
            }     
        
        }

        

        for(uint32_t i=0; i<*deadlock_num; i++) {
            printf(" %d", pid_list[i]);
        }
        printf("\n");
    } else {
        printf("No deadlocks\n");
    }
}