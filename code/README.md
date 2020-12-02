To start the daemon :
gcc daemon.c 
./a.out

To view the log file :
tail -f log.txt

To run the user program :
gcc SystemResources.c
./a.out <Number of times to be polled> <delay in seconds> <cpu threshold> <memory threshold>
  

