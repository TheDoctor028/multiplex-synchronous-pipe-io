# Multiplex synchronous pipe I/O

---

## Original Task

_Create an application: that opens 2 pipes -> 1 to **read**, 1 to **write**. Continuously read the input pipe and write 
out anything that comes to the std_out. At the same time read continuously from the std_in as well and write the incoming date from there to the output pipe. If any of the pipes closes the application should exit._

_For synchronous behavior use ```poll``` or ```select``` to multiplex reading. The output(writing) can be asynchronous._

## My implementation

- Using ```poll```
- Buffer size 255
- Mapping function to remove memory garbage from the buffer (only if there is a ```\n``` that indicants the EOL)
- Synchro output(writing) with ```fork```

## Requirements
- C11
- CMAKE
- GCC
- 2 pipe (exmpl.: Linux: ```mknod input p```, Mac: ```mkfifo input```)

## How to build & run
1. ```cmake -S . -B cmake-build```
2. ```cd cmake-build```
3. ```make```
4. ```mknod input p && mknod output p``` 
5. ```./main```