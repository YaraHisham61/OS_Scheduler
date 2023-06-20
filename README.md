# OS_Scheduler <img src="https://github.com/YaraHisham61/OS_Scheduler/assets/88517271/cca95ab0-8f61-40b3-aba8-6eb32981a70d" width="30" height="30" />
A simulation of scheduling processes by the operating system using C language and IPCs

## <img align= center width=50px height=50px src="https://thumbs.gfycat.com/HeftyDescriptiveChimneyswift-size_restricted.gif"> Table of Contents

- [Overview](#Overview)
- [System Description](#sysdes) 
- [Data Structure](#ds)
- [How To Run](#run)
- [Output](#out)
- [Contributors](#contributors)

<hr style="background-color: #4b4c60"></hr>

## <img src="https://github.com/YaraHisham61/OS_Scheduler/assets/88517271/3aac4ed8-1e15-42f2-8f0f-210460cb122d" width="30" height="30" /> Overview <a name = "Overview"></a>
This is our project for Operating Systems course at Cairo University. The project simulates the work of the operating system scheduler with the processes in the system using
3 different algorithms to run the processes which are:
1. Non-preemptive Highest Priority First (HPF)
2. Shortest Remaining time Next (SRTN)
3. Round Robin (RR)

While it uses 2 differnet algorithms for memory allocations which are:
1. First Fit.
2. Buddy memory allocation.



## <img src="https://github.com/YaraHisham61/OS_Scheduler/assets/88517271/9da0b3d6-1e82-4100-889c-fa8f1bf72cd8" width="30" height="30" /> System Description <a name = "sysdes"></a>
![image](https://github.com/YaraHisham61/OS_Scheduler/assets/88517271/5eda98f6-8739-4e18-bc32-74071143f693)

## Data Structures <a name = "ds"></a>
* Priority queue
   - Ready queue : To hold the processes PCBs arranged according to the scheduling algorithm.

      | Scheduling algorithm 	| Priority queue key |
      |:----------------------|:-------------------|
      | Highest priority first	| Processes priority |
      | Shortest remaining time next | The remaining time (initially it’s the process runtime)|
      | Round robin |	Arriving time |
      
    - Unallocated Processes : To hold the process that can’t be currently allocated due to memory limits
* Linked list
  - Used in first fit memory management algorithm
  - Hole linked list: To hold the free blocks of memory and their location
  - Processes linked list: To hold the allocated processes address space

* Binary search tree
  - Buddy lists : To hold memory addresses and specify whether they are allocated or not
## How To Run <a name = "run"></a>
1) Clone the project
```
git clone https://github.com/YaraHisham61/OS_Scheduler
 ```
 
2) Go to the project directory
  ```
  cd code
  ```
  
3) Compile your project using the command:
 ```
 make  
 ```
4) Choose the scheduling algorithm by modifying Makefile
 ```
 -sch 1 : HPF
 -sch 2 : SRTN
 -sch 3 : RR
 ```

5) Choose the memory allocation algorithm by modifying Makefile
 ```
 -mem 1 : First fit
 -mem 2 : Buddy
 ```
6) Modify process.txt to the desired test case
7) Run your project using the command:
 ```
make run

 ```
 
 ## Outputs Files <a name = "out"></a>
 * scheduler.log
 * scheduler.perf
 * memory.log
 
  ## Contributors <a name = "contributors"></a>
- [@AhmedSamy02](https://github.com/AhmedSamy02)
- [@kaokab33](https://github.com/kaokab33)
- [@nancyalgazzar](https://github.com/nancyalgazzar)
- [@YaraHisham61](https://github.com/YaraHisham61)
