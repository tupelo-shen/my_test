Very few projects require absolutely all of their software to be created from
scratch; the vast majority make use of at least some code that already
exists—at the application level, in the operating system, or both. You may well
find, however, that the existing code you’d like to use in your MIPS system was
originally developed for some other microprocessor family.Of course, at a minimum,
you’ll need to recompile the source code to create a new set of binaries for
MIPS; but as we’ll see, the task may be more complicated than that. Portability
refers to the ease with which a piece of software can be transferred successfully
and correctly to a new environment, particularly a new instruction set. Porting
a substantial body of software is rarely easy, and the level of difficulty tends
to rise sharply if the software in question is (or includes) an OS or OS-related
software such as device drivers.