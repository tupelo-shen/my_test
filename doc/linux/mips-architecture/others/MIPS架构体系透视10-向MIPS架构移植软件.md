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

High-level software (Linux application code or the like) will typically have
been written with at least some notion of portability and will quite probably
have been used in several environments already, so there’s a reasonable likelihood
that you’ll be able to recompile it without having to make changes. Lowlevel
software—perhaps a large portion of the source code, for some embedded
systems—is more troublesome. Software that has been developed exclusively in
just one particular environment is especially likely to present portability problems,
since its creators may not have recognized any particular need to avoid or
resolve them. The object of this chapter is to draw your attention to areas that
are particularly likely to cause problems when you’re porting software to MIPS.

The parts of a system that drive the lowest-level hardware are inevitably
nonportable; embedded systems are typically subject to significant design
upgrades every couple of years or so, and it’s just not reasonable (and certainly
not cost effective) to insist that the original hardware/software interfaces
be preserved throughout such changes.

# 1 Low-Level Software for MIPS Applications: A Checklist of Frequently Encountered Problems

The following are problems that have come up fairly frequently:

1. Endianness: The computer world is divided into little- and big-endian
camps, and a gulf of incomprehension falls between them. Most
MIPS CPUs can be set up to run either big-endian or little-endian;
but even if you already know which way your MIPS system will be
configured, it’s strongly recommended that you make sure you understand
this issue thoroughly. It’s caught out many experienced developers
before you, and it will catch out some more. Read about it in
section 10.2.

2. Data layout and alignment in memory: Your program may make
unportable assumptions about the memory layout of data declared in
C. It’s almost always unportable to use C struct declarations to map
input files or data received through a communication link, for example.
Danger can lurk in a program that employs multiple views of private
data with differently typed pointers or unions.
However, data layout goes together with a description of other conventions
(for register use, argument passing, and stack handling) and you’ll
find that in the next chapter: If you need to take a peek ahead, it’s in
section 11.1.

3. Need for explicit cache management: You may find that code you’d like to
reuse was developed for a microprocessor that didn’t implement caches
at all, or one that used a CPU with caches that are “invisible” to software
(almost all side effects of caching in PC-compatible processors are
hidden by clever hardware, for instance). But most MIPS CPUs keep
their hardware simple by letting some side effects remain visible and
making software responsible for cache management; we’ll describe what
this means in section 10.3.

4. Memory access ordering and reordering: In many modern embedded or
consumer systems, data moving around the system may pass through a
chain of subsystems as it moves from its source to its final destination.
Those subsystems may themselves encapsulate a lot of complicated
hardware, and may present you with unexpected problems. For example,
pieces of information passed between the CPU and I/O devices
may be forced to wait in queues, incurring variable amounts of delay;
or they may be separated into several independent traffic streams, so
the order in which they arrive at their respective destinations can’t
be guaranteed to match the order in which they were originally sent.
Typical problems and solutions are discussed in section 10.4.

5. Writing it in C: This is not so much a problem as an opportunity. But
there are things you can do in C (and probably should do in preference
to writing assembly code) that are fairly MIPS-specific. This section talks
about inline assembly, using memory-mapped registers, and a ragbag of
possible pitfalls using MIPS.

# 2 Endianness:Words, Bytes, and Bit Order

The word endianness was introduced to computer science by Danny Cohen
(Cohen 1980). In an article of rare humor and readability, Cohen observed that
computer architectures had divided up into two camps, based on an arbitrary
choice of the way in which byte addressing and integer definitions are related
in communications systems.

In Jonathan Swift’s Gulliver’s Travels, the “little-endians” and “big-endians”
fought a war over the correct end at which to start eating a boiled egg. Swift
was satirizing 18th-century religious disputes, and neither of his sides can see
that their difference is entirely arbitrary. Cohen’s joke was appreciated, and the
word has stuck. The problem is not just relevant to communications; it has
implications for portability too.

Computer programs are always dealing with sequence and order of different
types of data: iterating in order over the characters in a string, the words in
an array, or the bits in a binary representation. C programmers live with a pervasive
assumption that all these variables are stored in a memory that is itself
visible as a sequence of bytes—memcpy() will copy any data type. And C’s I/O
system models all I/O operations as bytes; you can also read() and write()
any chunk of memory containing any data type.

So one computer can write out some data, and another computer can read
it; suddenly, we’re interested in whether the second computer can understand
what the first one wrote.

We understand that we need to be careful with padding and alignment
(details in section 11.1). And it’s probably too much to expect that complex
data types like floating-point numberswill always transfer intact. Butwe’d hope
at least to see simple twos complement integers coming across OK; the curse
of endianness is that they don’t. The 32-bit integer whose hexadecimal value
was written as 0x1234.5678 quite often reads in as 0x7856.3412—it’s been
“byte-swapped.” To understand why, let’s go back a bit.

# 2.1 Bits, Bytes,Words, and Integers

A 32-bit binary integer is represented by a sequence of bits, with each bit having
a different significance. The least significant bit is “ones,” then “twos,” then
“fours”—just as a decimal representation is “ones,” “tens,” and “hundreds.”
When your memory is byte-addressable, your 32-bit integer occupies four bytes.
There are two reasonable choices about how the integer and bytewise view tie
up: Some computers put the least significant (LS) bits “first” (that is, in loweraddressed
memory bytes) and some put the most significant (MS) bit first—
and Cohen called them little-endian and big-endian, respectively. When I first
got to know about computers in 1976, DEC’s minicomputers were little-endian
and IBM mainframes were big-endian; neither camp was about to give way.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_1.PNG">

It’s worth stressing that the curse of choice only appeared once you could
address bytes. Pioneering computers through to the late 1960s were generally
organized around a single word size: Instructions, integers, and memory width
were all the same word size. Such a computer has no endianness: It has word
order in memory and bit order inside words, and those are unrelated.

Just as with opening a boiled egg, both camps have good arguments.

We’re used to writing decimals with the most significant digits to the left,
and (reading left to right as usual) we say numbers that way: Shakespeare might
have said “four and twenty,” but we say “twenty-four.” So if you write down
numbers, it’s natural to put the most significant bits first. Bytes first appeared
as a convenient way of packing characters into words, before memory was byte
addressable. A 1970s vintage IBM programmer had spent most of his or her
career poring over vast dump listings, and each set of characters represented a
word, which was a number. Little-endian numbers look ridiculous. They were
instinctive big-endians. But with numbers written MS to the left and byte
addresses increasing in the same direction, it would have been inconsistent to
have numbered the bits from right to left: So IBM labeled the MS bit of a word
bit 0. Their world looked like Figure 10.1.

But it’s also natural to number the bits according to their arithmetic
significance within integer data types—that is, to assign bit number n to
the position that has arithmetic significance 2n. It’s then consistent to store
bits 0–7 in byte 0, and you’ve become a little-endian. Having words appear
backward in dumps is a shame, but the little-endian view made particular
sense to people who’d gotten used to thinking of memory as a big array of
bytes. Intel, in particular, is little-endian. So its words, bytes, and bits look
like Figure 10.2.

You’ll notice that these diagrams have exactly the same contents: only the
MS/LS are interchanged, as well as the order of the fields. IBM big-endians see
words broken into bytes, while little-endians see bytes built into words. Both
these systems seemed unarguably right to different people: There’s lots of merit
in both, but you have to choose.

But let’s get back to our observation about the problem above. Our mangled
word started as 0x1234.5678, which is 00010010 00110100 01010110
01111000 in binary. If you transfer it naively to a system with the opposite
endianness, you’d surely expect to see all the bits reversed. In that case you’d
receive the number 00011110 01101010 00101100 01001000, which is
hex 0x1E6A.2C48. But we said we’d read hex 0x7856.3412.

It’s true that complete bit reversal could arise in some circumstances; there
are communication links that send the MS bit first, and some that send the LS
bit first. But sometime in the 1970s 8-bit bytes emerged as a universal base unit
both inside computers and in computer communications systems (where they
were called “octets”). Typically, communication systems build all their messages
out of bytes, and only the lowest-level hardware engineers know which bit goes
first.

Meanwhile, every microprocessor system got to use 8-bit peripheral controllers
(wider controllers were reserved for the high-end stuff), and all those
peripherals have 8-bit ports numbered 0 through 7, and the most significant
bit is 7. Somehow, without a shot apparently fired, every byte was little-endian,
and has been ever since.

Early microprocessor systems were 8-bit CPUs on 8-bit buses with 8-bit
memory systems, so they had no endianness. Intel’s 8086 was a 16-bit littleendian
system. When Motorola introduced the 68000 microprocessor around
1978, they greatly admired IBM’s mainframe architecture. Either in admiration
for IBM or to differentiate themselves from Intel, they thought they
should be big-endians too. But Motorola couldn’t oppose the prevailing bitswithin-
bytes convention—every 8-bit Motorola peripheral would have had to
be connected to a 68000 with its data bus bit-twisted. As a result, the 68000
family looks like Figure 10.3, with the bits and bytes numbered in opposite
directions.

The 68000 and its successors went on to be used for most successful UNIX
servers and workstations (notably with Sun). When MIPS and other RISCs
emerged in the 1980s, their designers needed to woo system designers with the
right endianness, so they designed CPUs that could swing either way. But from
the 68000 onward, big-endian has meant 68000-style big-endian, with bits and
bytes going opposite ways. When you set up a MIPS CPU to be big-endian, it
looks like Figure 10.3. And that’s where the trouble really starts.

One small difficulty comes when you read hardware manuals for your CPU
and see register diagrams. Everyone’s convinced that registers are (first and
foremost) 32-bit integers, so they’re invariably drawn with the MS bit (bit 31,
remember) first. This has some consequences for programmers and hardware
designers alike. That picture motivates the difference between “shift-left” and
“shift-right” instructions, determines the bit-number arguments of bitfield
instructions, and even refers to the labeling of the bitfields that make up MIPS
instructions.

Once you get over that, there is serious software trouble when porting software
ormoving data between incompatible machines; there is hardware trouble
when connecting incompatible components or buses. We’ll take the software
and hardware problems separately.

# 2.2 Software and Endianness

Here’s a software-oriented definition of endianness: A CPU/compiler system
where the lowest addressed byte of amultibyte integer holds the least significant
bits is called little-endian; a system where the lowest addressed byte of a multibyte
integer holds the most significant bits is called big-endian. You can very
easily find out which sort of CPU you have by running a piece of deliberately
nonportable code:

    #include<stdio.h>
    main ()
    {
        union {
            int as_int;
            short as_short[2];
            char as_char[4];
        } either;
        
        either.as_int = 0x12345678;
        if (sizeof(int) == 4 && either.as_char[0] == 0x78) {
            printf ("Little endian\n");
        }
        else if (sizeof(int) == 4 && either.as_char[0] == 0x12) {
            printf ("Big endian\n");
        }
        else {
            printf ("Confused\n");
        }
    }

Strictly speaking, software endianness is an attribute of the compiler
toolchain, which could always—if it worked hard enough—produce the effect
of either endianness. But on a byte-addressable CPU like MIPS with native
32-bit arithmetic it would be unreasonably inefficient to buck the hardware;
thus we talk of the endianness of the CPU.

Of course, the question of byte layout within the address space applies to
other data types besides integers; it affects any item that occupies more than
a single byte, such as floating-point data types, text strings, and even the 32-
bit op-codes that represent machine instructions. For some of these noninteger
data types, the idea of arithmetic significance applies only in a limited way, and
for others it has no meaning at all.

When a language deals in software-constructed data types bigger than
the hardware can manage, then their endianness is purely an issue of software
convention—they can be constructed with either endianness. I hope
that modern compiler writers appreciate that it’s a good idea to be consistent
with the hardware’s own convention.

## Endianness and Program Portability

So long as binary data items are never imported into an application from elsewhere,
and so long as you avoid accessing the same piece of data under two
different integer types (as we deliberately did above), your CPU’s endianness
is invisible (and your code is portable). Modern C compilers will try to watch
out for you: If you do this by accident, you’ll probably get a compiler error or
warning.

You may not be able to live within those limitations, however; you may
have to deal with foreign data delivered into your system from elsewhere, or
with memory-mapped hardware registers. For either of these, you need to know
exactly how your compiler accesses memory.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_4.PNG">

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_5.PNG">

This all seems fairly harmless, but experience shows that of all data-mapping
problems, endianness is uniquely confusing. I think this is because it is difficult
even to describe the problem without taking a side. The origin of the two
alternatives lies in two different ways of drawing the pictures and describing the
data; both are natural in different contexts.

Aswe sawabove, big-endians typically drawtheir pictures organized around
words. So that gives us a big-endian picture of the data structure we used in
Figure 10.4. It would look a lot prettier with the IBM convention of labeling the
MS bit as bit 0, but that’s no longer done.

But little-endians are likely to emphasize a software-oriented, abstract
view of computer memory as an array of bytes. So the same data structure
looks like Figure 10.5. Little-endians don’t think of computer data as primarily
numeric, so they tend to put all the low numbers (bits, bytes, or whatever) on
the left.

It’s very difficult to achieve a real grasp of endianness without drawing
pictures, but many people find themselves struggling to set aside the conventions
they’re used to; for example, if you’re used to numbering the bits from
right to left, it can take a real effort of will to number them from left to right
(a picture of a little-endian structure as drawn by someone with big-endian
habits can look very illogical). This is the essence of the subject’s capacity to confuse:
It’s difficult even to think about an unfamiliar convention without getting
caught up in the ones you know.

#### 2.3 硬件和大小端

We saw previously that a CPU’s native endianness only shows up when it offers
direct support both for word-length numbers and a finer-resolution, byte-sized
memory system. Similarly, your hardware system acquires a recognizable
endianness when a byte-addressed system is wired up with buses that are multiple
bytes wide.

When you transfer multibyte data across the bus, each byte of that data has
its own individual address. If the lowest-address byte in the data travels on the
eight bus lines (“byte lane”) with the lowest bit numbers, the bus is little-endian.
But if the lowest-address byte in the data travels on the byte lanewith the highest
bit numbers, the bus is big-endian.

There’s no necessary connection between the “native” endianness of a CPU
and the endianness of its system interface considered as a bus. However, I don’t
know of any CPUs where the software and interface endianness are different, so
we can talk about “the endianness of a CPU” and mean both internal organization
and system interface.

Byte-addressable CPUs announce themselves as either big- or little-endian
every time they transfer data. Intel and DEC CPUs are little-endian; Motorola
680x0 and IBM CPUs are big-endian. MIPS CPUs can be either, as configured
from power-up; most other RISCs have followed the MIPS lead and chosen to
make endianness configurable—a boon when updating an existing system with
a new CPU.

Hardware engineers can hardly be blamed for connecting up different buses
by matching up the bit numbers. But trouble strikes when your system includes
buses, CPUs, or peripherals whose endianness doesn’t match. In this case the
choice is not a happy one; the system designer must choose the lesser of two
evils:

* Bit number consistent/byte sequence scrambled: Most obviously, the
designer can wire up the two buses according to their bit numbers, which
will have the effect of preserving bit numbering within aligned “words.”
But since the relationship between bit numbers and bytes-within-words
is different on the two buses, the two sides will see the sequence of bytes
in memory differently.
Any data that is not of bus-width size and bus-width aligned will get
mangled when transferred between the connected buses, with bytes
swapped within each bus-width-sized unit. This looks and feels worse
than the software problem. With wrong-endianness data in software,
you have no problem finding data type boundaries; it’s just that the data
doesn’t make sense. With this hardware problem the boundaries are
scrambled too (unless the data are, by chance, aligned on bus-width
“word” boundaries).
There’s a catch here. If the data being passed across the interface is always
aligned word-length integers, then bit-number-consistent wiring will
conceal the endianness difference, avoiding the need for software
conversion of integers. But hardware engineers very rarely know exactly
which data will be passed across an interface over the lifetime of a system,
so be cautious.

* Byte address consistent/integers scrambled: The designer can decide to preserve
byte addressing by connecting byte lanes that correspond to the
same byte-within-word address, even though the bit-numbering of the
data lines in the byte lane doesn’t match at all. Then at least the whole
system can agree on the data seen as an array of bytes.

However, there are presumably going to be componentswith mismatched
software endianness in the system. So your consistent byte addressing
is guaranteed to expose their disagreement about the representation of
multibyte integers. And—in particular—even a bus-width-aligned integer
(the “natural” unit of transfer)will appear byte-swapped whenmoved
to the other endianness.

For most purposes, byte address scrambling is much more harmful, and
we’d recommend “byte address consistent” wiring. When dealing with data
representation and transfer problems, programmers will usually fall back on
C’s basic model of memory as an array of bytes, with other data types built up
from that. When your assumptions about memory order don’t work out, it’s
very hard to see what’s going on.

Unfortunately, a bit number consistent/byte address scrambled connection
looks much more natural on a schematic; it can be very hard to persuade hardware
engineers to do the right thing.

Not every connection in a system matters. Suppose we have a 32-bit-wide
memory system bolted directly to a CPU. The CPU’s system interface may
not include a byte-within-word address—the address bus does not specify
address bits 1 and 0. Instead, many CPUs have four “byte enables,” which
show that data is being transferred on particular byte lanes. The memory
array is wired to the whole bus, and on a write the byte enables tell the
memory array which of four possible byte locations within the word will
actually get written. Internally, the CPU associates each of the byte lanes with
a byte-within-word address, but that has no effect on the operation of the
memory system. Effectively, the memory/CPU combination acts together and
inherits the endianness of the CPU; where byte-within-word 0 actually goes in
memory doesn’t matter, so long as the CPU can read it back again.

It’s very important not to be seduced by this helpful characteristic of a
RAM memory into believing that there’s no intrinsic endianness in a simple
CPU/RAM system. You can spot the endianness of any transfer on a wide bus.
Here’s a sample list of conditions in which you can’t just ignore the CPU’s
endianness when building a memory system:

* If your system uses firmware that’s preprogrammed into ROMmemory,
the hardware address and byte lane connection assignments within
the system need to match those assumed in the way the ROM was
programmed, and the data contained in the ROM needs to match the
CPU’s configured endianness. In effect, the contents of the ROM are
being delivered into your system from somewhere outside it. If the code
is to be executed directly from the ROM, it’s especially important to
get the endianness right, because it’s impossible for the CPU to apply
any corrective software byte-swapping to the op-codes as it fetches
them.

* When a DMA device gets to transfer data directly into memory, then its
notions of ordering will matter.

* When aCPUinterface does not in fact use byte enables, but instead issues
byte-within-word addresses with a byte-width code (quite common for
MIPS CPUs), then at least the hardware that decodes the CPU’s read
and write requests must know which endianness the CPU is using. This
can be particularly tricky if the CPU allows endianness to be softwareconfigured.

The next section is for you to tell your hardware engineer about how to
set up a byte address consistent system—and even how to make that system
configurable with the CPU, if some of your users might set up the MIPS CPU
both ways.


#### Wiring Endianness-Inconsistent Buses

Suppose we’ve got a 64-bit MIPS CPU configured big-endian, and we need to
connect it to a little-endian 32-bit bus such as PCI.

Figure 10.6 shows how we’d wire up the data buses to achieve the recommended
outcome of consistent byte addresses as seen by the big-endian CPU
and the little-endian bus.

The numbers called “byte lane” show the byte-within-bus-width part of the
address of the byte data traveling there.Writing in the byte lane numbers is the
key to getting one of these connections right.

Since the CPU bus is 64 bits wide and the PCI bus 32 bits, you need to be
able to connect each half of the wide bus to the narrow bus according to the
“word” address—that’s address bit 2, since address bits 1 and 0 are the bytewithin-
32-bit-word address. The CPU’s 64-bit bus is big-endian, so its highnumbered
bits carry the lower addresses, as you can see from the byte lane
numbers.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_6.PNG">

You may find yourself staring at the numbering of the connections around
the bus switch for quite some time before they really make sense. Such are the
joys of endianness.

And note that I only showed data. PCI is a multiplexed bus, and in some
clock cycles those “byte lanes” are carrying an address. In address cycles, PCI
bus wire 31 is carrying the most significant bit of the address. The address-time
connection from your MIPS-based system should not be swapped.

#### Wiring an Endianness-Configurable Connection

Suppose you want to build a board or bus switch device that allows you to
configure a MIPS CPU to run with either endianness. How can we generalize
the advice above?

We’d suggest that, if you can persuade your hardware designer, you should
put a programmable byte lane swapper between the CPU and the I/O system.
The way this works is shown diagrammatically in Figure 10.7; note that this is
only a 32-bit configurable interface and it’s an exercise for you to generalize it
to a 64-bit CPU connection.

We call this a byte lane swapper, not a byte swapper, to emphasize that
it does not alter its behavior on a per-transfer basis, and in particular to
indicate that it is not switched on and off for transfers of different sizes.
There are circumstances where it can be switched on and off for transfers to
different address regions—mapping some part of the system as bit number
consistent/byte address scrambled—but that’s for you to make work.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_7.PNG">

What a byte lane swapper does achieve is to ensure that whether your CPU
is set up to be big- or little-endian, the relationship between the CPU and the
now mismatched external bus or device can still be one where byte sequnce is
preserved.

You normally won’t put the byte-lane swapper between the CPU and its
local memory—this is just as well, because the CPU/local memory connection
is fast and wide, which would make the byte swapper expensive.

As mentioned above, so long as you can decode the CPU’s system interface
successfully, you can treat the CPU/local memory as a unit and install
the byte swapper between the CPU/memory unit and the rest of the system.
In this case, the relationship between bit number and byte order inside the
local memory changes with the CPU, but this fact is not visible from the
rest of the world.

#### False Cures and False Prophets for Endianness Problems

Every design team facing up to endianness for the first time goes through the
stage of thinking that the troubles reflect a hardware deficiency to be solved. It’s
never that simple. Here are a few examples.

* Configurable I/O controllers: Some newer I/O devices and system controllers
can themselves be configured into big-endian and little-endian
modes. You’re going to have to read the manual very carefully before
using such a feature, particularly if you mean to use it not as a static
(design time) option but rather as a jumper (reset time) option.

It is quite common for such a feature to affect only bulk data transfers,
leaving the programmer to handle other endianness issues, such as access
to bit-coded device registers or shared memory control fields. Also, the
controller designer probably didn’t have the benefit of this book—and
confusion about endianness is widespread.

* Hardware that byte-swaps according to transfer type: If you’re designing
in some byte-swap hardware, it seems appealing to try to solve
the whole problem. If we just swapped byte data to preserve its
addresses, but left words alone, couldn’t we prevent the whole software
problem? The answer is no, there aren’t any hardware fixes for
the software problem. For example, many of the transfers in a real
system are of data cache lines. They may contain an arbitrary mixture
of data sizes and alignments; if you think about it for a moment,
you’ll see that there simply isn’t any way to know where the boundaries
are, which means there’s no way to determine the required swap
configuration.

Conditional byte-swapping just adds confusion. Anything more than
unconditional byte lane swapping is snake oil.

# 2.4 Bi-endian Software for a MIPS CPU

You may want to create binary code that will run correctly on MIPS CPUs
with either endianness—probably for a particular board that may be run either
way or to create a portable device driver that may run on boards of either
configuration. It’s a bit tricky, and you will probably only do a tiny part of your
bootstrap code like this, but here are some guidelines.

The MIPS CPU doesn’t have to do toomuch to change endianness. The only
parts of the instruction set that recognize objects smaller than 32 bits are partialword
loads and stores. On a MIPS CPU with a 32-bit bus, the instruction:

    lbu t0, 1(zero)

takes the byte at byte program address 1, loads it into the least significant bits
(0 through 7) of register t0, and fills the rest of the register with zero bits. This
description is endianness independent. However, in big-endian mode the data
loaded into the register will be taken from bits 16–23 of the CPU data bus; in
little-endian mode, the byte is loaded from bits 8–15 of the CPU data bus.

Inside the MIPS CPU, there’s data-steering hardware that the CPU uses
to direct all the active bytes in a transfer from their respective byte lanes
at the interface to the correct positions within the internal registers. This
steering logic has to accommodate all permutations of load size, address,
and alignment (including the load/store left/right instructions described in
section 8.5.1).

It is the change in the relationship between the active byte lane and the
address on partial-word loads and stores that characterizes the MIPS CPU’s
endianness.When you reconfigure your MIPS CPU’s endianness, it’s that steering
logic between data and register whose behavior changes.

Complementing the chip’s configurability, most MIPS toolchains can produce
code of either endianness, based on a command-line option.

If you set a MIPS CPU to the wrong endianness for its system, then a couple
of things will happen.

First, if you change nothing else, the software will crash quickly, because on
any partial-word write the memory system will pick up garbage data from the
wrong part of the CPU bus. At the same time as reconfiguring the CPU, we’d
better reconfigure the logic that decodes CPU cycles.2

If you fix that, you’ll find that the CPU’s view of byte addressing becomes
scrambled with respect to the rest of the system; in terms of the description
above, we’ve implicitly opted for a connection that keeps the bit numbers consistent,
rather than the byte addresses.

Of course, data written by the CPU after a change of endianness will seem
fine to the CPU itself; if we allow changes of endianness only at reset time, then
volatile memory that is private to the CPU won’t give us any trouble.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_8.PNG">

Note also that the CPU’s view of bit numbering within aligned bus-width
words continues to match the rest of the system. This is the choice we described
earlier as bit number consistent and that we suggested you should generally
avoid. But in this particular case it has a useful side effect, because MIPS instructions
are encoded as bitfields in 32-bit words. An instruction ROM that makes
sense to a big-endian CPU will make sense to a little-endian CPU too, allowing
us to share a bootstrap. Nothing works perfectly—in this case, any data in
the ROM that doesn’t consist of aligned 32-bit words will be scrambled. Many
years ago, Algorithmics’ MIPS boards had just enough bi-endian code in their
boot ROM to detect that the main ROM program does not match the CPU’s
endianness and to print the helpful message:

    Emergency - wrong endianness configured.

The word Emergency is held as a C string, null-terminated. You should
now know enough to understand why the ROM start-up code contains the
enigmatic lines:

    .align 4
    .ascii "remEcneg\000\000\000y"

That’s what the string Emergency (with its standard C terminating null and
two bytes of essential padding) looks like when viewed with the wrong endianness.
It would be even worse if it didn’t start on a four-byte-aligned location.
Figure 10.8 (drawn from the point of view of a confirmed big-endian) shows
what is going on.

You’ve seen that writing bi-endian code is possible, but be aware that when
you’re ready to load it into ROM, you’ll be asking your tools to do something
they weren’t designed to handle. Typically, big-endian tools pack instruction
words into the bytes of a load file with the most significant bits first, and littleendian
tools work the other way around. You’ll need to think carefully about
the result you need to achieve, and examine the files you generate to make sure
everything went according to plan.

#### 2.5 Portability and Endianness-Independent Code

By a fairly well-respected convention, most MIPS toolchains define the symbol
BYTE ORDER as follows:

    #if BYTE_ORDER == BIG_ENDIAN
    /* big-endian version... */
    #else
    /* little-endian version... */
    #endif

So if you really need to, you can put in different code to handle each case. But
it’s better—wherever possible—to write endianness-independent code. Particularly
in a well-controlled situation (such as when writing code for a MIPS
system that may be initialized with the CPU in either mode), you can get rid
of a lot of dependencies by good thinking.

All data references that pick up data from an external source or device are
potentially endianness dependent. But according to how your system is wired,
youmay be able to produce code that works both ways. There are only two ways
of wiring the wrong endianness together: One preserves byte addresses and the
other bit numbers. For some particular peripheral register access in a particular
range of systems, there’s a good chance that the endianness change consistently
sticks to one of these.

If your device is typically mapped to be byte address compatible, then you
should program it strictly with byte operations. If ever, for reasons of efficiency
or necessity, you want to transfer more than one byte at a time, you need to
write endianness-conditional code that packs or unpacks that data.

If your device is compatible at the word (32-bit) level—for example, it consists
of registers wired (by however devious and indirect a route) to a fixed set
of MIPS data bus bits—then program it with bus-width read/write operations.
Thatwill be 32-bit or 64-bit loads and stores. If the device registers are notwired
to MIPS data bus bits starting at 0, you’ll probably want to shift the data after a
read and before a write. For example, 8-bit registers on a 32-bit bus in a system
originally conceived as big-endian are commonly wired via bits 31–24.

#### 2.6 Endianness and Foreign Data

This chapter is about programming, not a treatise on I/O and communications,
so we’ll keep this section brief. Any data that is not initialized in
your code, chosen libraries, and OS is foreign. It may be data you read
from some memory-mapped piece of hardware, data put into memory by
DMA, data in a preprogrammed ROM that isn’t part of your program, or
you may be trying to interpret a byte stream obtained from an “abstract”
I/O device under your OS.

The first stage is to figure out what this data looks like in memory; with
C, that can usually be accomplished by mapping out what its contents are as
an array of unsigned char. Even if you know your data and compiler well
enough to guess which C structure will successfully map to the data, fall back to
the array of bytes when something is not as you expect; it’s far too easy to miss
what is really going on if your data structure is incorrect.

Apart from endianness, the data may consist of data types that are not
supported by your compiler/CPU; it may have similar types but with completely
different encodings; it may have familiar data but be incorrectly
aligned; or, falling under this section’s domain, it may have the wrong
endianness.

If the chain along which the data has reached you has preserved byte order
at each stage, the worst that will happen is that integer data will be represented
with an opposite order, and it’s easy enough to build a “swap” macro to restore
the two, four, or eight bytes of an integer value.

But if the data has passed over a bit number consistent/byte address scrambled
interface, it can be more difficult. In these circumstances, you need to
locate the boundaries corresponding to the width of the bus where the data
got swapped; then, taking groups of bytes within those boundaries, swap them
without regard to the underlying data type. If you do it right, the result should
now make sense, with the correct byte sequence, although you may still need
to cope with the usual problems in the data—including, possibly, the need to
swap multibyte integer data again.

# 3 Trouble with Visible Caches

In section 4.6, you learned about the operations you can use to get your caches
initialized and operating correctly. This section alerts you to some of the problems
that can come up and explains what you can do to deal with them.

Most of the time, the caches are completely invisible to software, serving
only to accelerate the system as they should. But especially if you need to deal
with DMA controllers and the like, it can be helpful to think of the caches as
independent buffer memories, as shown in Figure 10.9.

It’s important to remember that transfers between cache and memory always
work with blocks of memory that fit the cache line structure—typically 16- or
32-byte-aligned blocks—so the cache may read or write a block because the

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_9.PNG">

CPU makes direct reference to just one of the byte addresses contained within
it; even if the CPU never touches any of the other bytes in the cache line, it’ll
still include all the bytes in the next transfer of that cache line.

In an ideal system, we could always be certain that the state of the memory
is up-to-date with all the operations requested by the CPU, and that
every valid cache line contains an exact copy of the appropriate memory
location. Unfortunately, practical systems can’t always live up to this ideal.
We’ll assume that you initialize your caches after any reset, and that you
avoid (rather than try to live with) the dreaded cache aliases described in
section 4.12. Starting from those assumptions, how does a real system’s
behavior fall short of the ideal?

* Stale data in the cache: When your CPU writes to memory in cached
space, it updates the cached copy (and may also write memory at the
same time). But if a a memory location is updated in any other way, any
cached copies of its contents continue to hold the old value, and are now
out-of-date. That can happen when a DMA controller writes data. Or,
when the CPU writes new instructions for itself, the I-cache may continue
to hold whatever was at the same location before. It’s important
for programmers to realize that the hardware generally doesn’t deal with
these conditions automatically.

* Stale data in memory: When the CPU writes some data into a (writeback)
cache line, that data is not immediately copied to memory. If the
data is read later by the CPU, it gets the cached copy and is fine; but if
something that isn’t the CPU reads memory, it may get the old value.
That can happen to an outbound DMA transfer.

The software weapons you have to fight problems caused by visible caches
are a couple of standard subroutines that allow you to clean up cache/memory
inconsistency, built on MIPS cache instructions. They operate on cache locations
corresponding to a specified area of memory and can write-back up-todate
cache data or invalidate cache locations (or both).

Well, of course, you can always map data uncached. In fact, there are some
circumstances when you will do just that: Some network device controllers,
for example, have a memory-resident control structure where they read and
write bytes and bit flags, and it’s a lot easier if you map that control structure
uncached. The same is true, of course, of memory-mapped I/O registers,
where you need total control over what gets read and written. You can do that
by accessing those registers through pointers in kseg1 or some other uncached
space; if you use cached space for I/O, bad things will happen!

If (unusually) you need to use the TLB to map hardware register accesses,
you can mark the page translation as uncached. That’s useful if someone has
built hardware whose I/O registers are not in the low 512 MB of the physical
memory space.

It’s possible that you might want to map a memory-like device (a graphics
frame buffer, perhaps) through cached space so as to benefit from the speed of
the block reads and writes that the CPU only uses to implement cache refills
and write-backs. But you’d have to explicitly manage the cache by invalidation
andwrite-back on every such access. Some embeddedCPUs provide strange and
wonderful cache options that can be useful for that kind of hardware—check
your manual.

#### 3.1 Cache Management and DMA Data

This is a common source of errors, and the most experienced programmers will
sometimes get caught out. Don’t let that worry you too much: Provided you
think clearly and carefully about what you’re trying to achieve, you’ll be able
to get your caches to behave as they should while your DMA transfers work
smoothly and efficiently.

When a DMA device puts data into memory, for example, on receipt of
network data, most MIPS systems don’t update the caches—even though some
cache lines may currently be holding addresses within the region just updated
by the DMA transfer. If the CPU subsequently reads the information in those
cache lines, it’ll pick up the old, stale version in the cache; as far as the CPU can
tell, that’s still marked valid, and there’s no indication that the memory has a
newer version.

To avoid this, your software must actively invalidate any caches’ lines that
fall within the address range covered by your DMA buffer, before there’s any
chance that the CPU will try to refer to them again. This is much easier to
manage if you round out all your DMA buffers so they start and end exactly
at cache line boundaries.

For outbound transfers, before you allowaDMAdevice to transfer data from
memory—such as a packet that you’re sending out via a network interface—you
must make absolutely sure that none of the data to be sent is still just sitting in
the cache. After your software has finished writing out the information to be
transferred by DMA, it must force the write-back of all cache lines currently
holding information within the address range that the DMA controller will use
for the transfer. Only then can you safely initiate the DMA transfer.

On some MIPS CPUs, you can avoid the need for explicit write-back operations
by configuring your caches to use write-through rather than write-back
behavior, but this cure is really worse than the disease — write-through tends
to bemuch slower overall and will also raise your system’s power consumption.

You really can get rid of the explicit invalidations and write-backs by accessing
all the memory used for all DMA transfers via an uncached address region.
This isn’t recommended either, because it’ll almost certainly degrade your system’s
overall performance farmore than you’d like.Even if your software’s access
to the buffers is purely sequential, caching the DMA buffer regions will mean
that information gets read and written in efficient cache-line-sized bursts rather
than single transfers. The best general advice is to cache everything, with only
the following exceptions:

* I/O device registers: Perhaps obvious, but worth pointing out. MIPS has
no dedicated input/output instructions, so all device registers must be
mapped somewhere in the address space, and very strange things will
happen if you accidentally let them be cached.

* DMA descriptor arrays: Sophisticated DMA controllers share control/
status information with the CPU using small descriptor data structures
held in memory. Typically, the CPU uses these to create a long list of
information to be transferred, and only then tells the DMA controller to
begin its work. If your system uses descriptors, you’ll want to access the
memory region that contains them through an uncached address region.

A portable OS like Linux must deal with a range of caches from the most
sophisticated and invisible to the crude and simple, so it has a fairlywell-defined
API (set of stable function calls) for driver writers to use and some terse documentation
on how to use them. See section 15.1.1.

#### 3.2 Cache Management and Writing Instructions: Self-Modifying Code

If your code ever tries to write instructions into memory, then execute them,
you’ll need to make sure you allow for cache behavior.

This can surprise you on two levels. First, if you have a write-back
D-cache, the instructions that your program writes out may not find their way
to main memory until something triggers a write-back of the relevant cache
lines. The instructions that your program wrote out could just be sitting in the
D-cache at the time you try to execute them, and the CPU’s fetches simply can’t
access them there. So the first step is to do write-back operations on the cache
lines at which you write the instructions; that at least ensures they reach main
memory.

The second surprise (regardless of which type of D-cache you have) is
that even after writing out the new instructions to some region of main
memory, your CPU’s I-cache may still hold copies of the information that
used to be held in those addresses. Before you tell the CPU to execute the
newly written instructions, it’s essential that your software first invalidates
all the lines in the I-cache that contain information at the affected address
range.

Of course, you could avoid the need for these explicit write-back and invalidate
operations by writing and then executing the new instructions within an
uncached address region; but that gives up the advantages of caching and is
almost always a mistake.

The general-purpose cache management instructions described in
section 4.9 are CP0 instructions, only usable by kernel-privilege software. That
doesn’t matter when cache operations are related to DMA operations, which
are also entirely kernel matters. But it does matter with applications like writing
instructions and executing them (think of a modern application using a
“just-in-time” interpreted/translated language).

So MIPS32/64 provides the synci instruction, which does a D-side writeback
and an I-side invalidate of one cache-line-sized piece of your new code.
Find out how in section 8.5.11.

#### 3.3 CacheManagement and Uncached orWrite-Through Data

If you mix cached and uncached references that map to the same physical range
of addresses, you need to think about what this means for the caches. Uncached
writes will update only the copy of a given address in main memory, possibly
leaving what’s nowa stale copy of that location’s contents in the D-cache—orthe
I-cache.Uncached loadswill pick up whatever they find in main memory—even
if that information is, in fact, stale with respect to an up-to-date copy present
only in cache.

Careful use of cached and uncached references to the same physical region
may be useful, or even necessary, in the low-level code that brings your system
into a known state following a reset. But for running code, you probably
don’t want to do that. For each region of physical memory, decide whether your
software should access it cached or uncached, then be absolutely consistent in
treating it that way.

#### 3.4 Cache Aliases and Page Coloring

There’s more about the hardware origin of cache aliases in section 4.12. The
problem occurs with L1 caches that are virtually indexed but physically tagged,
and where the index range is big enough to span two or more page sizes. The
index range is the size of one “set” of the cache, so with common 4-KB pages
you can get aliases in an 8-KB direct-mapped cache or a 32-KB four-way
set-associative cache.

The “page color” of a location is the value of those one or more virtual
address bits that choose a page-sized chunk within the appropriate cache set.
Two virtual pointers to the same physical data can produce an alias only if they
have a different page color. So long as all pointers to the same data have the same
color, all is well—all the data, even though at different virtual addresses, will be
stored in the same physical portion of the cache and will be correctly identified
by the (common) physical tag.

It’s quite common in Linux (for example) for a physical page to be accessible
at multiple virtual locations (shared libraries are routinely shared between
programs at different virtual addresses).

Most of the time, theOSis able to overalign virtual address choices for shared
data—the sharing processes may not use the same address, but we’ll make sure
their different virtual addresses are amultiple of, say, 64KBapart, so the different
virtual addresses have the same color. That takes up a bit more virtual memory,
but virtual memory is fairly cheap.

It’s easy to think that cache aliases are harmless so long as the data is “readonly”
(it must have been written once, but that was before there were aliases to
it):We don’t care if there aremultiple copies of a read-only page. But they’re only
mostly harmless. It is possible to tolerate aliases to read-only data, particularly in
the I-cache: But you need to make sure that cache management software is aware
that data that has been invalidated at one virtual address may still be cached at
another.

With the widespread use of virtual-memory OSs (particularly Linux) in the
embedded and consumer computing markets, MIPS CPUs are increasingly
being built so that cache aliases can’t happen. It’s about time this long-lasting
bug was fixed.

Whatever you need to do, the cache primitive operations required for a
MIPS32/64 CPU are described in section 4.9.1.

# 4 Memory Access Ordering and Reordering

Programmers tend to think of their code executing in a well-behaved sequence:
The CPU looks at an instruction, updates the state of the system in the appropriate
ways, then goes on to the next instruction. But our program can run
faster if we allow the CPU to break out of this purely sequential form of execution,
so that operations aren’t necessarily constrained to take place in strict
program order. This is particularly true of the read and write transactions
performed at the processor’s interface, triggered by its execution of load and
store instructions.

From the CPU’s point of view, a store requires only an outbound write
request: Present the memory address and data, and leave the memory controller
to get on with it. Practical memory and I/O devices are relatively slow, and in
the time the write is completed the CPU may be able to run tens or hundreds of
instructions.

Reads are different, of course: They require two-way communication
in the form of an outbound request and an inbound response. When the
CPU needs to know the contents of a memory location or a device register,
there’s probably not much it can do until the system responds with the
information.

In the quest for higher performance, that means we want to make reads
as fast as possible, even at the expense of making writes somewhat slower.
Taking this thinking a step further, we can even make write requests wait in a
queue, and pass any subsequent read requests to memory ahead of the buffered
writes. From the CPU’s point of view, this is a big advantage; by starting the
read transaction immediately, it gets the response back as soon as possible.
The writes will have to be done sometime, and the queue is of finite size: But
it’s likely that after this read is done there will be a period while the CPU is
running from cache. And if the queue fills up, we’ll just have to stop while
some writes happen: That’s certainly no worse than if we’d done the writes in
sequence.

You can probably see a problem here: Some programs may write a location
and then read it back again. If the read overtakes the write, we may get stale data
from memory and our programwill malfunction.Most of the time we can fix it
with extra hardware that checks an outgoing read request against the addresses
of entries in the write queue and doesn’t allow the read to overtake a matching
write.4

In systems where tasks that could be really concurrent (that is, they might be
running on different CPUs) share variables, the problem of ordering reads and
writes becomes more dangerous. It’s true that much of the time the tasks have
no expectation ofmutual ordering.Ordering matters when the tasks are deliberately
using shared memory for synchronization and communication, but in this
case the software will be using carefully crafted OS synchronization operations
(locks and semaphores, for example).

But there are some shared-memory communication tricks—often good,
cheap, efficient ones—that don’t need so many semaphores or locks but are
disrupted by arbitrary cycle reordering. Suppose, for example, we have two
tasks: one is writing a data structure, the other is reading it. They use the data
structure in turn, as shown in Figure 10.10.

For correct operation,we need to knowthatwhenthe reader sees the updated
value in the key field, we can guarantee that all the other updates will be visible
to the reader as well.

Unless we discard all the performance advantages of decoupling reads and
writes fromtheCPU, it’s not practical for hardware to conceal all ordering issues
fromthe programmer. The MIPS architecture provides the sync instruction for
this purpose: You’re assured that (for all participants in the shared
memory) all accesses made before the sync will precede those made afterward.
It’sworth dwelling on the limited nature of that promise: It only relates to ordering,
and only as seen by participants in uncached or cache-coherent memory
accesses.

To make the example above reliable on a suitable system, the writer should
include sync just before writing keyfield, and the reader should have a sync
just after reading keyfield. See section 8.5.9 for details. But there’s a lot more
to this subject; if you’re building such a system, you’re strongly recommended
to use an OS that provides suitable synchronization mechanisms, and read up
on this subject.

Different architectures make different promises about ordering. At one
extreme, you can require all CPU and system designers to contrive that all the
writes and reads made by one CPU appear to be in exactly the same order from
the viewpoint of another CPU: That’s called “strongly ordered.” There are
weaker promises too (such as “all writes remain in order”); but the MIPS architecture
takes the radical position that no guarantees are made at all.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_10_10.PNG">

#### 4.1 Ordering andWrite Buffers

Let’s escape fromthe lofty theory and describe something rather more practical.
The idea of holding outbound requests in a write buffer turns out to work especially
well in practice because of the way store instructions tend to be bunched
together. For a CPU running compiled MIPS code, it’s typical to find that only
about 10 percentage of the instructions executed are stores; but these accesses
tend to come in bursts—for example, when a function prologue saves a group
of register values.

Most of the time the operation of the write buffer is completely transparent
to software. But there are some special situations in which the programmer
needs to be aware of what’s happening:

1. Timing relations for I/O register accesses: This affects all MIPS CPUs. After
the CPU executes a store to update an I/O device register, the outbound
write request is liable to incur some delay in the write buffer, on its way
to the device. Other events, such as inbound interrupts, may take place
after the CPU executes the store instruction, but before the write request
takes effect within the I/O device. This can lead to surprising behavior:
For example, the CPU may receive an interrupt from a device “after” you
have told it not to generate interrupts. To give another example: If an I/O
device needs some software-implemented delay to recover after a write,
you must ensure that the write buffer is empty before you start counting
out that delay— ensuring also that the CPU waits while the write buffer
empties. It’s good practice to define a subroutine that does this job, and
it’s traditionally given the name wbflush(). See section 10.4.2 for hints
on implementing it.

2. Reads overtaking writes: The MIPS32/64 architecture permits this behavior,
discussed above. If your software is to be robust and portable, it should
not assume that read and write order is preserved. Where you need to
guarantee that two cycles happen in some particular order, you need the
sync instruction described in section 8.5.9.

3. Byte gathering: Some write buffers watch for partial-word writes within
the same memory word (or even writes within the same cache line) and
will combine those partial writes into a single operation.
To avoid unpleasant symptoms when uncachedwrites are combined into
a word-width, it’s a good idea to map your I/O registers such that each
register is in a separate word location (i.e., 8-bit registers should be at
least four bytes apart).

#### 4.2 Implementing wbflush

Most write queues can be emptied out by performing an uncached store to
any location and then performing an operation that reads the same data back.
A write queue certainly can’t permit the read to overtake the write—it would
return stale data. Put a sync instruction between the write and the read, and
that should be effective on any system compliant with MIPS32/64.

This is effective, but not necessarily efficient; you can minimize the overhead
by loading from the fastest memory available. Perhaps your system offers
something system-specific but faster. Use it after reading the following note!

> Write buffers are often implemented within the CPU, but may also be implemented
outside it; any system controller or memory interface that boasts of
a write-posting feature introduces another level of write buffering to your system.
Write buffers outside the CPU can give you just the same sort of trouble
as those inside it. Take care to find out where all the write buffers are located
in your system, and to allow for them in your programming.

# 5 Writing it in C

You probably alreadywrite almost everything inCor in C++. MIPS’s lack of special
I/O instructions means that I/O register accesses are just normal loads and
stores with appropriately chosen addresses; that’s convenient, but I/O
register accesses are usually somewhat constrained, so you need to make sure
the compiler doesn’t get too clever. MIPS’s use of large numbers of CP0 registers
also means that OS code can benefit from well-chosen use of C asm()
operations.

#### 5.1 Wrapping Assembly Code with the GNU C Compiler


TheGNUC Compiler (“GCC”) allows you to enclose snippets of assembly code
within C source files. GCC’s feature is particularly powerful, but other modern
compilers probably could support the example here. But their syntax is probably
quite different, so we’ll just discuss GCC here.
If you want low-level control over something that extends beyond a handful
of machine instructions, such as a library function that carries out some clever
computation, you’ll really need to get to gripswithwriting pure MIPS assembly;
but if you just want to insert a short sequence that consists of one or a few specific
MIPS instructions, the asm() directive can achieve the desired result quite
simply. Better still, you can leave it to the compiler to manage the selection of
registers according to its own conventions.

As an example, the following code makes GCC use the three-operand form
of multiply, available on more recent MIPS CPUs. If you just use the normal C
language * multiplication operator, the work could end up being done by the
original formof themultiply instruction that accepts only two source operands,
implicitly sending its double-length result to the hi/lo register pair.5

The C function mymul() is exactly like the three-operand mul and delivers
the less significant half of the double-length result; the more significant half is
simply discarded, and it’s up to you to ensure that overflows are either avoided
or irrelevant.

    static int __inline__ mymul(int a, int b)
    {
        int p;

        asm(
            "mul %0, %1, %2"
            : "=r" (p)
            : "r" (a), "r" (b)
        );

        return p;
    }

The function itself is declared inline, which instructs the compiler that
a use of this function should be replaced by a copy of its logic (which
permits local register optimization to apply). Adding static means that
the function need not be published for other modules to use, so no binary
of the function itself will be generated. It very often makes sense to wrap
an asm() like this: You’d probably usually then put the whole definition
in an include file. You could use a C preprocessor macro, but the inlined
function is a bit cleaner.

The declarations inside the asm() parentheses tell GCC to emit a MIPS mul
line to the assemblerwith three operands on the command line—onewill be the
output and two will be inputs.

Onthe line below,we tellGCCabout operand %0, the product: first, that this
value will be write-only (meaning that there’s no need to preserve its original
value) with the “=” modifier; the “r” tells GCC that it’s free to choose any of
the general-purpose registers to hold this value. Finally, we tell GCC that the
operand we wrote as %0 corresponds to the C variable p.

On the third line of the asm() construct, we tell GCC about operands %1
and %2.Again,we allowGCCto put these in any of the general-purpose registers,
and tell it that they correspond to the C variables a and b.

At the end of the example function, the resultwe obtained fromthemultiply
instruction is returned to the C caller.

GCC allows considerable control over the specification of the operands; you
can tell it that certain values are both read andwritten and that certain hardware
registers are left with meaningless values as a side effect of a particular assembly
sequence.You can dig out the details fromthe MIPS-specific sections of theGCC
manual.

#### 5.2 Memory-Mapped I/O Registers and “Volatile”

Most of you will be writing code that accesses I/O registers in C—you certainly
shouldn’t be using assembly code in the absence of any pressing need to do so,
and since all I/O registers in MIPSmust be memory-mapped, it is never difficult
to access them from C. Having said that, you should keep in mind that as compilers
advance, or if you make significant use of C++, it can become harder to
predict exactly the low-level instruction sequences that’ll end up in your code.
Here are some well-worn hints.

I might write a piece of code that is intended to poll the status register of a
serial port and to send a character when it’s ready:

    unsigned char *usart_sr = (unsigned char *) 0xBFF00000;
    unsigned char *usart_data = (unsigned char *) 0xBFF20000;
    #define TX_RDY 0x40
    void putc (ch)
    char ch;
    {
        while ((*usart_sr & TX_RDY) == 0);
        *usart_data = ch;
    }


I’d be upset if this sent two characters and then looped forever, but that
would be quite likely to happen. The compiler sees the memory-mapped I/O
reference implied by *usart sr as a loop-invariant fetch; there are no stores in
the while loop so it seems safe to pull the load out of the loop. Your compiler
has recognized that your C program is equivalent to:

    void putc(ch)
    char ch;
    {
        tmp = (*usart_sr & TX_RDY);
        while (tmp);
        *usart_data = ch;
    }

You could prevent this particular problem by defining your registers as follows:

    volatile unsigned char *usart_sr =
    (unsigned char *) 0xBFF00000;
    volatile unsigned char *usart_data =
    (unsigned char *) 0xBFF20000;

A similar situation can exist if you examine a variable that is modified by an
interrupt or other exception handler.Again, declaring the variable as volatile
should fix the problem.
I won’t guarantee that this will always work: The C bible describes the
operation of volatile as implementation dependent. I suspect, though, that
compilers that ignore the volatile keyword are implicitly not allowed to optimize
away loads.
Many programmers have trouble using volatile. The thing to remember
is that it behaves just like any other C type modifier—just like unsigned in the
example above. You need to avoid syndromes like this:

    typedef char * devptr;
    volatile devptr mypointer;

You’ve now told the compiler that it must keep loading the pointer value
from the variable devptr, but you’ve said nothing about the behavior of the
register you’re using it to point at. It would be more useful to write the code like
this:

    typedef volatile char * devptr;
    devptr mypointer;

Once you’ve dealt with this, the most common reason that optimized code
breaks will be that you have tried to drive the hardware too fast. There are often
timing constraints associated with reads and writes of hardware registers, and
you’ll often have to deliberately slow your code to fit in.

What’s the main lesson of this section? While it’s easier to write and
maintain hardware driver code in C than in assembly, it’s important to
use this option responsibly. In particular, you’ll need to understand enough
about the way the toolchain converts your high-level source code into lowlevel
machine instructions to make sure you get the system behavior that
you intended.

#### 5.3 Miscellaneous Issues WhenWriting C for MIPS Applications

* Negative pointers:When running simpleunmappedcode on aMIPSCPU,
all pointers are in the kseg0 or kseg1 areas, so any data pointer’s 32-bit
value has the top bit set and looks “negative.” Unmapped programs on
most other architectures are dealing with physical addresses, which are
usually a lot smaller than 2 GB!
Such pointer values could cause trouble when pointer values are being
compared, if the pointer were implicitly converted to a signed integer
type. Any implicit conversions between integer and pointer types (quite
common in C) should be made explicit and should specify an unsigned
integer type (you should use unsigned long for this).
Most compilers will warn about pointer-to-integer conversions, though
you may have to specify an option.

* Signed versus unsigned characters: In early C compilers, the char type
used for strings was usually equivalent to signed char; this is consistent
with the convention for larger integer values. However, as soon as you
have to deal with character encodings using more than 7-bit values, this
is dangerous when converting or comparing. Modern compilers usually
make char equivalent to unsigned char instead.
If you discover that your old program depends on the default signextension
of char types, good compilers offer an option that will restore
the traditional convention.

* Moving from 16-bit int: A significant number of programs are being
moved up from 16-bit x86 or other CPUs where the standard int is
a 16-bit value. Such programs may rely, much more subtly than you
think, on the limited size and overflow characteristics of 16-bit values.
Although you can get correct operation by translating such types
into short, that will be inefficient. In most cases you can let variables
quietly pick up the MIPS int size of 32 bits, but you should
be particularly aware of places where signed comparisons are used to
catch 16-bit overflow.

* Programming that depends on the stack: Some kind of function invocation
stack and data stack are implicit in C’s block structure.
Despite the MIPS hardware’s complete lack of stack support, MIPS
C compilers implement a fairly conventional stack structure. Even so,
if your program thinks it knows what the stack looks like, it won’t be
portable. If possible, don’t just replace the old assumptions with new
ones: Two of the most common motivations for stack abuse are now
satisfied with respectable and standards-conforming macro/library
operations, which may tackle what your software was trying to do
before:

    - stdargs: Use this include-file-based macro package to implement
routines with a variable number of parameters whose type need not
be predefined at compile time.

    - alloca(): To allocate memory at run time, use this library function,
which is “on the stack” in the sense that it will be automatically freed when the function allocating thememory returns. Some compilers
implement alloca() as a built-in function that actually extends
the stack; otherwise, there are pure-library implementations available.
But don’t assume that suchmemory is actually at an addresswith some
connection with the stack.

