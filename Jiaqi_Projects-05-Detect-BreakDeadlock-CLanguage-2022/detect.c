#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <stdint.h>
#include "utility.h"


void find_excution_time(uint32_t* file_block, uint32_t* file_wait, uint32_t* file_block_length, uint32_t* file_wait_length);
void find_deadlock(uint32_t* file_block, uint32_t* file_wait, uint32_t* file_process, uint32_t* file_status, uint32_t curr_index, uint32_t init_index, uint32_t file_block_length, uint32_t smallest_pid, uint32_t* pid_list, uint32_t* deadlock_num);
void simulation_time(uint32_t* file_process, uint32_t* file_block, uint32_t* file_wait, uint32_t file_length);

int main(int argc, char *argv[]) {
    uint32_t file_length = 0;
    int f_flag = 0, c_flag = 0, e_flag = 0, option;
    char* filename = NULL;

    //getopt function helps to read the command of "f, c, e" in any order and any positions. 
    while ((option = getopt(argc, argv, "ecf:")) != -1) {
        switch (option) {
            case 'f':
                f_flag = 1;
                filename = optarg;
                break;
            case 'c':
                c_flag = 1;
                break;
            case 'e':
                e_flag = 1;
                break;
        }
    }

    /*.....................................................task 1..........................................................*/
    uint32_t* file_array = read_file(filename, &file_length);
    uint32_t* file_process = malloc(sizeof(uint32_t));
    uint32_t* file_block = malloc(sizeof(uint32_t));
    uint32_t* file_wait = malloc(sizeof(uint32_t));
    uint32_t file_block_length=0, file_wait_length=0, file_process_length=0;

    for(uint32_t i=0; i<file_length; i++) {

        if (i % 3 == 1) { 
            file_block_length++;
            file_block = (uint32_t*)realloc(file_block, file_block_length * sizeof(uint32_t));
            file_block[file_block_length-1] = file_array[i];

        } else if (i % 3 == 2) { 
            file_wait_length++;
            file_wait = (uint32_t*)realloc(file_wait, file_wait_length * sizeof(uint32_t));
            file_wait[file_wait_length-1] = file_array[i];
        } else{
            file_process_length++;
            file_process = (uint32_t*)realloc(file_process, file_process_length * sizeof(uint32_t));
            file_process[file_process_length-1] = file_array[i];
        }
    }

    /*.....................................................task 2..........................................................*/
    if (f_flag == 1 && e_flag == 1 && c_flag == 0) {
        find_excution_time(file_block, file_wait, &file_block_length, &file_wait_length);
    }

    /*.....................................................task 3..........................................................*/
   if (f_flag == 1 && e_flag == 0 && c_flag == 0) {
       uint32_t pid_list[file_process_length];
       uint32_t deadlock_num = 0;
       // loop file 2, call the function to detect the deadlock
        for (uint32_t i=0; i<file_wait_length; i++) {
            // create a status file to keep track on visited processes
            uint32_t file_status[file_wait_length];
            for (uint32_t j=0; j<file_wait_length; j++) {
                file_status[j] = 0;
            }
            find_deadlock(file_block, file_wait, file_process, file_status, i, i, file_block_length, file_process[i], pid_list, &deadlock_num);
        }
    
    /*....................................................task 4,5.........................................................*/
        deadlock_output(pid_list, &deadlock_num);
    }

    /*....................................................challenge.........................................................*/
    if (f_flag == 1 && e_flag == 0 && c_flag == 1) {
        simulation_time(file_process, file_block, file_wait, file_process_length);
    }

    free(file_array);
    free(file_block);
    free(file_wait);
    free(file_process);
    return(0);
}




/* task 2 if there is e command, then print out the excution time. Assume this question has no deadlock. 
since we want the best case of execution time, we are focusing on find out the file which is accessed 
the most number of times by the processes. */
void find_excution_time(uint32_t* file_block, uint32_t* file_wait, uint32_t* file_block_length, uint32_t* file_wait_length) {
    uint32_t repetitive_time_file_block = 1, repetitive_time_file_wait = 1;

    // see the most repetitive files among file 1
    for (uint32_t i=0; i<*file_block_length; i++) {
        uint32_t count = 1;
        for(uint32_t j=i+1; j<*file_block_length; j++) {
            if (file_block[i] == file_block[j]) {
                count++;
            }
        }
        if (count > repetitive_time_file_block) {
            repetitive_time_file_block = count;
        }
    }

    // see the most repetitive files among file 2
    for (uint32_t i=0; i<*file_wait_length; i++) {
        uint32_t count = 1;
        for(uint32_t j=i+1; j<*file_wait_length; j++) {
            if (file_wait[i] == file_wait[j]) {
                count++;
            }
        }
        if (count > repetitive_time_file_wait) {
            repetitive_time_file_wait = count;
        }
    }

    // compare the largest repetitive numbers of file 1 and file 2
    if (repetitive_time_file_block >= repetitive_time_file_wait) {
        printf("Execution time %d\n", repetitive_time_file_block + 1);
    } else {
        printf("Execution time %d\n", repetitive_time_file_wait + 1);
    }
}


/*for this question, the logic is to go through file 2, for each file 2, set the corresponding file 1 
(from the same process) as the initial value, then go through file 1 from the current process(line) to afterwards, 
and check if there is a cycle.
for example: if
process 0: a, b
process 1: b, c
we are going to set a as initial point, check the corresponding file b in process 0 and process 1
if they are the same, check whether c matches to a. if not match, go next one; else its already a deadlock.
*/
void find_deadlock(uint32_t* file_block, uint32_t* file_wait, uint32_t* file_process, uint32_t* file_status, uint32_t curr_index, uint32_t init_index, uint32_t file_block_length, uint32_t smallest_pid, uint32_t* pid_list, uint32_t* deadlock_num) {

    uint32_t detect_deadlock = 0;
    uint32_t is_repeated = 0;
    // check whether there is already exist a deadlock.
    for(uint32_t i=0; i<file_block_length; i++) {
        if (file_block[i] == file_wait[curr_index] && file_wait[i] == file_block[init_index]) {
            if (smallest_pid > file_process[i]) {
                    smallest_pid = file_process[i];
            }
            // store the smallest process id into the pid array
            for(uint32_t k = 0; k < *deadlock_num; k++){
                if (pid_list[k] == smallest_pid){
                    is_repeated = 1;
                }
            }
            if(!is_repeated){
                pid_list[*deadlock_num] = smallest_pid;
                is_repeated = 0;
                *deadlock_num += 1;
            }
            detect_deadlock = 1;
            break;
        }
    }

    // if there is no deadlock detected, there is possibly that the deadlock is a large cycle, so use recursion to go deep.
    if(detect_deadlock == 0) {
        for(uint32_t i=0; i<file_block_length; i++) {
            if (file_block[i] == file_wait[curr_index] && file_wait[i] != file_block[init_index] && (i+1 != file_block_length) && (file_status[i] != 1)) { 
                file_status[i] = 1;  // specific process has been visited
                
                // store the smallest process id
                if (smallest_pid > file_process[i]) {
                    smallest_pid = file_process[i];
                }
                // recursion, keep the i as the current index so it will be used to compare waited file and blocked file
                find_deadlock(file_block, file_wait, file_process, file_status, i, init_index, file_block_length, smallest_pid, pid_list, deadlock_num);
            }
            if (i+1 == file_block_length) {
                break;
            }
        }
    } 
}


/*challenge question, see the challenge_explanation txt file */
void simulation_time(uint32_t* file_process, uint32_t* file_block, uint32_t* file_wait, uint32_t file_length) {

    uint32_t file_status[file_length];
    uint32_t run_time = -1, require_new_runtime = 1, first_detected_process = 1;

    // initialize all the elements in file_status to -1, only except the first element.
    file_status[0] = 0;
    for(uint32_t i=1; i<file_length; i++) {
        file_status[i] = -1;
    }


    while (require_new_runtime == 1) { // here we loop the run time from 0.
        run_time++;
        first_detected_process = 1;

        for(uint32_t i=1; i<file_length; i++) {

            // when this specific process "i" has not been blocked/executed yet in any run time
            if (file_status[i] == -1) {

                // the first process (with unblocked files) in each run time never needs to wait for other processes
                if (first_detected_process == 1 && run_time > 0) {
                    file_status[i] = run_time;
                    first_detected_process = 0;
                }

                for (uint32_t j=0; j<i; j++) {
                     // system detected the process "i" want to access the blocked file (blocked by other process)
                     // so automatically skip the process "i" and leaves it to the next run time
                    if (file_status[j] == run_time) {
                        if (file_block[j] == file_block[i] || file_block[j] == file_wait[i] ||
                        file_wait[j] == file_block[i] || file_wait[j] == file_wait[i]) {
                            break;
                        }
                    }
                    // process i has compared all the previous processes (with blocked files) in the same run time and no access confliction
                    if (j == i-1) {
                        file_status[i] = run_time;                        
                    }
                }
            }
        }

        // check whether all of the processes has been blocked once, if so, then end the run time.
        for(uint32_t i=0; i<file_length; i++) {
            if (i == file_length - 1) {
                if (file_status[i] != -1) {
                    require_new_runtime = 0;
                }
            }
            if (file_status[i] == -1) {
                break;
            }      
        }
    }

    // print output
    for(uint32_t j=0; j<run_time+1; j++) {
        for(uint32_t i=0; i<file_length; i++) {
            if (file_status[i] == j) {
                printf("%d %d %d,%d\n", file_status[i], file_process[i], file_block[i], file_wait[i]);
            }
        }   
    }
}