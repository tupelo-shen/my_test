Signals were introduced by the first Unix systems to allow interactions between User Mode processes; the kernel also uses them to notify processes of system events. Signals have been around for 30 years with only minor changes.

The first sections of this chapter examine in detail how signals are handled by the Linux kernel, then we discuss the system calls that allow processes to exchange signals.

## 1 信号的角色

A signal is a very short message that may be sent to a process or a group of processes.
The only information given to the process is usually a number identifying the
signal; there is no room in standard signals for arguments, a message, or other
accompanying information.

A set of macros whose names start with the prefix SIG is used to identify signals; we
have already made a few references to them in previous chapters. For instance, the
SIGCHLD macro was mentioned in the section “The clone( ), fork( ), and vfork( ) System
Calls” in Chapter 3. This macro, which expands into the value 17 in Linux,
yields the identifier of the signal that is sent to a parent process when a child stops or
terminates. The SIGSEGV macro, which expands into the value 11, was mentioned in
the section “Page Fault Exception Handler” in Chapter 9; it yields the identifier of
the signal that is sent to a process when it makes an invalid memory reference.

Signals serve two main purposes:

1. To make a process aware that a specific event has occurred
2. To cause a process to execute a signal handler function included in its code

Of course, the two purposes are not mutually exclusive, because often a process
must react to some event by executing a specific routine.

Table 11-1 lists the first 31 signals handled by Linux 2.6 for the 80 × 86 architecture
(some signal numbers, such those associated with SIGCHLD or SIGSTOP, are architecture-dependent; furthermore, some signals such as SIGSTKFLT are defined only for specific architectures). The meanings of the default actions are described in the next section.

Besides the regular signals described in this table, the POSIX standard has introduced
a new class of signals denoted as real-time signals; their signal numbers range
from 32 to 64 on Linux. They mainly differ from regular signals because they are
always queued so that multiple signals sent will be received. On the other hand, regular
signals of the same kind are not queued: if a regular signal is sent many times in
a row, just one of them is delivered to the receiving process. Although the Linux kernel
does not use real-time signals, it fully supports the POSIX standard by means of
several specific system calls.

A number of system calls allow programmers to send signals and determine how
their processes respond to the signals they receive. Table 11-2 summarizes these
calls; their behavior is described in detail in the later section “System Calls Related to
Signal Handling.”


