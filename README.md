# Virtual-Memory-Simulator
An extension of the LC-3b simulator that handles virtual memory in addition to interrupts/exceptions. The simulator is thus augmented to support virtual to physical address translation. 

It also handles a new exception -- page fault exception.

The simulator is also modified to trigger a protection exception upon the attempted access of a protected page by the user (excluding the execution of TRAP instructions in user mode), as opposed to detecting the explicit physical address in system space that the user tried to access.

readme.pdf contains more info. 
