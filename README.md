# Virtual-Memory-Simulator
An extension of the LC-3b simulator that handles virtual memory in addition to interrupts/exceptions.

It also handles a new exception -- page fault exception. The protection exception is modified to be triggered upon the attempted access of a protected page by the user (excluding the execution of TRAP instructions in user mode), as opposed to detecting the explicit physical address in system space that the user tried to access.
