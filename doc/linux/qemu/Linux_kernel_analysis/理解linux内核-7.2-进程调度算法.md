The scheduling algorithm used in earlier versions of Linux was quite simple and
straightforward: at every process switch the kernel scanned the list of runnable processes,
computed their priorities, and selected the “best” process to run. The main
drawback of that algorithm is that the time spent in choosing the best process
depends on the number of runnable processes; therefore, the algorithm is too
costly—that is, it spends too much time—in high-end systems running thousands of
processes.
The scheduling algorithm of Linux 2.6 is much more sophisticated. By design, it
scales well with the number of runnable processes, because it selects the process to
run in constant time, independently of the number of runnable processes. It also
scales well with the number of processors because each CPU has its own queue of
runnable processes. Furthermore, the new algorithm does a better job of distinguishing
interactive processes and batch processes. As a consequence, users of heavily
loaded systems feel that interactive applications are much more responsive in
Linux 2.6 than in earlier versions.
The scheduler always succeeds in finding a process to be executed; in fact, there is
always at least one runnable process: the swapper process, which has PID 0 and executes
only when the CPU cannot execute other processes. As mentioned in Chapter 3,
every CPU of a multiprocessor system has its own swapper process with PID equal to 0.
Every Linux process is always scheduled according to one of the following scheduling
classes:

* SCHED_FIFO

    A First-In, First-Out real-time process. When the scheduler assigns the CPU to
the process, it leaves the process descriptor in its current position in the runqueue
list. If no other higher-priority real-time process is runnable, the process
continues to use the CPU as long as it wishes, even if other real-time processes
that have the same priority are runnable.

* SCHED_RR

    A Round Robin real-time process. When the scheduler assigns the CPU to the
process, it puts the process descriptor at the end of the runqueue list. This policy
ensures a fair assignment of CPU time to all SCHED_RR real-time processes that
have the same priority.

* SCHED_NORMAL

    A conventional, time-shared process.

The scheduling algorithm behaves quite differently depending on whether the process
is conventional or real-time.

## Scheduling of Conventional Processes

Every conventional process has its own static priority, which is a value used by the
scheduler to rate the process with respect to the other conventional processes in the
system. The kernel represents the static priority of a conventional process with a
number ranging from 100 (highest priority) to 139 (lowest priority); notice that static
priority decreases as the values increase.

A new process always inherits the static priority of its parent. However, a user can
change the static priority of the processes that he owns by passing some “nice values”
to the nice() and setpriority() system calls (see the section “System Calls
Related to Scheduling” later in this chapter).

### Base time quantum


