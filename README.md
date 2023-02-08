# 05-Detect-BreakDeadlock-CLanguage-2022

This project is fully written in C.

Each line of the file corresponds to a process and consists of a space-separated tuple:
- process-id space file-id space file-id

For example resources.txt with the following information:
0 1 3
1 2 7
describes a system where process 0 has locked file 1 and is waiting for file 3 while process 1 has locked file 2 and waiting for file 7.

If resources.txt has the following information:
0 1 2
1 2 1
then the system has processes 0 and 1, with locks on files 1 and 2, respectively. In addition, process 0 locked file 1 and requested file 2 while process 1 locked file 2 and requested file 1. Hence, there is a deadlock.

My task is to print the excution time if there is no deadlock, or print out the "deadlock detected" & break the deadlock if there is deadlock detected.
Note breaking the deadlock means excute one process first, then excute another one to avoid deadlock.

For the challenge task, I need to break the deadlock if there is deadlock detected. More detailly, I need to allocate the processes into different execution order, but with minimum number of execution times. 

For example, given resources.txt with the following information:
0 1 2
1 2 3
2 3 4
3 4 1
./detect -f resources.txt -c could print:
0 0 1,2\n
0 2 3,4\n
1 1 2,3\n
1 3 4,1\n
Simulation time 2\n

In this case, at execution time 0, process 0 (with file 1 and 2) and process 2 (with file 3 and 4) are executed at the same time with no deadlock, and then at execution time 1, process 1 (with file 2 and 3) and process 3 (with file 4 and 1) are executed at the same time. There are no deadlock in each execution time and 2 execution time is the minimum number of execution times.

