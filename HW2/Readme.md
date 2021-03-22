# Part - 1
## A Gaussian blur filter with SystemC modules connected with SystemC FIFO channels.
- Steps to execute the code: 
1. $ cd Part\ -\ 1/
2. $ mkdir build
3. $ cd build
4. $ cmake ..
5. $ make 
6. $ make run 
### Output
Scanning dependencies of target gauss. 
[ 20%] Building CXX object CMakeFiles/gauss.dir/testbench.cpp.o. 
[ 40%] Linking CXX executable gauss. 
[ 80%] Built target gauss. 
[100%] Generating out.bmp // This is the output of the code (inside the *build* directory with name *out.bmp*). 

        SystemC 2.3.3-Accellera --- Feb 24 2021 15:14:58
        Copyright (c) 1996-2018 by all Contributors,
        ALL RIGHTS RESERVED

Info: /OSCI/SystemC: Simulation stopped by user.  
Simulated time == 1769478 ns. 
[100%] Built target run. 


# Part - 2 
##  Send in batch a row or a column of image pixels from Input to Calculation and compare the number of pixel transfer of the two implementations.
- Steps to execute the code: 
1. $ cd Part\ -\ 2/
2. $ mkdir build
3. $ cd build
4. $ cmake ..
5. $ make 
6. $ make run 

     
