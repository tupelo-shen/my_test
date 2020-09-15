[TOC]

For most of this book we’ve described the MIPS architecture from the perspective of how the hardware looks to a programmer. In this chapter, we’re going to describe some standards about how MIPS binary programs should be created to make them compatible with each other.

These standards are designed around characteristics of the hardware, but are often just arbitrary—it has to be done some way, and there’s an advantage if every toolchain does it the same. We’ve met one of those standards already: the register usage conventions described way back in section 2.2.

In this chapter,we’re going to look at howcompilers represent data for MIPS programs, at argument passing for functions, and at the use of the stack. In all cases, we’ll draw all our examples from the C language, though essentially the same conventions apply to other languages. Data representation and function linkage are aspects of a formal standard called an ABI (Application Binary Interface), and we’re describing the conventions used in the ABI, sometimes called o32. But the ABI document also specifies the encoding of object files (formats like ELF used to hold binary programs and libraries), and you won’t find that in this book.

Linux requires more conventions to allow applications to be dynamically linked together out of incomplete programs and shared libraries.We’ll describe that in Chapter 16.

The organization of the data is profoundly affected by the CPU’s endianness, which was described at length in section 10.2.

The most important ABIs in MIPS history are:

* o32: Grew from traditional MIPS conventions (“o” for old), and described in detail here. o32 is still pretty much universally used by embedded toolchains and for 32-bit Linux.

* n64: Newformal ABI for 64-bit programs on 64-bit CPUs running under Silicon Graphics’ Irix operating system. SGI’s 64-bit model makes both pointers and C long integer types into 64-bit data items. However, n64 also changes the conventions for using registers and the rules for passing parameters; because it puts more arguments in registers, it improves performance slightly.

* n32: A partner ABI to n64, this is really for “32-bit” programs on 64-bit CPUs. It is mostly the same as n64, except for having pointers and the C long data type implemented as 32 bits. That can be useful—for applications where a 32-bit memory space is already spacious, 64-bit pointers represent nothing but extra overhead.

# 11.1 Data Representations and Alignment

When you define data in C, the data you get in memory is compilation-target dependent.[^1] Moreover,while it’s  “nonportable” to assume you have a particular layout, nonportable C is often a more maintainable way of defining a fixed data layout than anything else at your disposal.

The data layout chosen is constrained by what the hardware will do. MIPS CPUs can only load multibyte data that is “naturally” aligned—a four-byte quantity only from a four-byte boundary, and so on—but many CISC architectures don’t have this restriction. The MIPS compiler attempts to ensure that data lands in the right place; this requires  far-reaching (and not always obvious) behaviors.

For the purposes of this section, memory is taken as an array of unsigned 8-bit quantities, whose index is the virtual address. For all known MIPS architecture CPUs, this corresponds to a C definition `unsigned char []`.

Like all the modern computers I know of, MIPS uses twos-complement representation for signed integers—so in any data size “-1” is represented by binary all-ones. The overwhelming advantage of twos-complement numbers is that the basic arithmetic operations (add, subtract, multiply, divide) have the same implementation for signed and unsigned data types.[^2]

C integer data types come in `signed` and `unsigned` versions, which are always the same size and alignment. When you don’t specify which, you typically get a `signed int`, `long`, or `long long` but often an `unsigned char`.[^3]

[^1]: Strictly speaking, it’s also compiler dependent, but in practice MIPS compilers all comply with the conventions described in this section.
[^2]: At least, until the result has greater precision than the operands.
[^3]: This is an ANSI C feature. In early C char was also signed by default. Most compilers allow you to change the default for char with a command-line flag—useful when recompiling old software.

## 11.1.1 Sizes of Basic Types

Table 11.1 lists fundamental C data types and how they’re implemented for MIPS architecture CPUs. We’ll come back to the long and pointer types a bit later—their size changes according to which ABI you use.

TABLE 11.1 Data Types and Memory Representations

| C type | Assm Name | Size (bytes) |
| ------ | --------- | ------------ |
| char   | byte      | 1 | 
| short  | half      | 2 |
| int    | word      | 4 |
| long long | dword  | 8 |
| float | word       | 4 |
| double | dword     | 8 |

The assembler does not distinguish storage definitions for integer and floating-point data types.

## 11.1.2 Size of “long” and Pointer Types

We left those out of the table, because they come out differently in the different ABIs. But they’re also always the same as something else.

So for o32, n32 (and any plausible ABI to be used on 32-bit CPUs), `long` is implemented just like an `int`; for the 64-bit n64 ABI `long` is implemented just like a `long long`.

And in all cases a pointer is always stored just like an `unsigned long`; the MIPS architecture always boasts a simple “flat” address space.

## 11.1.3 Alignment Requirements

All these primitive data types can only be directly handled by standard MIPS instructions if they are naturally aligned: that is, a two-byte datum starts at an address that is even (zero modulo 2), a four-byte datum starts at an address that is zero modulo 4, and an eight-byte datum starts at an address that is zero modulo 8.[^4]

[^4]: For MIPS32 CPUs using only 32-bit registers and data paths, eight-byte data types are not handled by any machine instruction, and the eight-byte alignment restriction is not strictly necessary. However, it is still imposed in all known ABIs.

11.1.4 Memory Layout of Basic Types and How It Changes with Endianness

Figure 11.1 shows howeach basic type is laid out in our byte-addressed memory; the arrangement is different for big-endian and little-endian software.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_11_1.PNG">

I’ve given in to the temptation to reverse the bits between the two endianness layouts. For memory addressing purposes, this is meaningless; bytes are indivisible 8-bit objects. However, reversing the bit numbers makes the bitwise depiction of the fields of floating-point numbers easier to absorb (and prettier).

Each of these data types is naturally aligned, as described previously.

“Endianness” can be a troubling subject and is discussed at length in section 10.2.

## 11.1.5 Memory Layout of Structure and Array Types and Alignment

Complex types are built by concatenating simple types but inserting unused (“padding”) bytes between items, so as to respect the alignment rules.[^5]

It’s worth giving a couple of examples. Here’s the byte offsets of data items in a `struct mixed`:

    struct mixed {
        char c;         /* byte 0 */
        /* bytes 1-7 are ‘‘padding’’ */
        double d;       /* bytes 8-15 */
        short s;        /* bytes 16-17 */
    };

It’s worth stressing that the byte offsets of the fields of constructed data types (other than those using C bitfields, see section 11.1.6) are unaffected by endianness.

A data structure or array is aligned in memory to the largest alignment boundary required by any data type inside it. So a `struct mixed` will start on an eight-byte boundary; and that means that if you build an array of these structures you will need padding between each array element. C compilers provide for this by “tail padding” the structure to make it usable for an array, so `sizeof(struct mixed) == 24` and the structure should really be annotated:

    struct mixed {
        char c; /* byte 0 */
        /* bytes 1-7 are ‘‘padding’’ */
        double d; /* bytes 8-15 */
        short s; /* bytes 16-17 */
        /* bytes 18-23 are ‘‘tail padding’’ */
    };

Just to remind you: The size and alignment requirement of pointer and long data types can be four or eight, depending on whether you’re exploiting 64-bit operations.

## 11.1.6 Bitfields in Structures

C allows you to define structures that pack several short “bitfield” members into one or more locations of a standard integer type. This is a useful feature for emulation, for hardware interfacing, and perhaps for defining dense data structures, but it is fairly incomplete. Bitfield definitions are nominally CPU dependent (but so is everything) but also genuinely endianness dependent.

[^5]: 5. Some compiler systems provide mechanisms to alter the alignment rules for particular data definitions. This allows you to model more possible data patterns with C data declarations, and the compiler will generate appropriate code (with some loss of efficiency) to handle the resulting unaligned basic data types. There are some hints in section 11.1.7.

<img src="https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/mips-architecture/others/images/see_mips_run_11_2.PNG">

You may recall that in section 7.9.3 we used a bitfield structure to map the fields of a single-precision IEEE floating-point value (a C float) stored in memory. An FP single value is multibyte, so you should probably expect this definition to be endianness dependent. The big-endian version looked like this:

    struct ieee754sp_konst {
        unsigned sign:1;
        unsigned bexp:8;
        unsigned mant:23;
    };

C bitfields are always packed—that is, the fields are not padded out to yield any particular alignment. But compilers reject bitfields that span the boundaries of the C type used to hold them (in the example, that’s an `unsigned`, which is short for unsigned int).

The structure and mapping for a big-endian CPU is shown in Figure 11.2 (using a typical big-endian’s picture); a little-endian version is shown in Figure 11.3.

The C compiler insists that, even for bitfields, items declared first in the structure occupy lower addresses: When you’re little-endian, you need to turn the declaration backward:

    struct ieee754sp_konst {
        unsigned mant:23;
        unsigned bexp:8;
        unsigned sign:1;
    };

To see why that works, you can see from Figure 11.3 that in little-endian mode the compiler packs bits into structures starting from low-numbered bits.

Does this make sense? Certainly some; if you tried to implement bitfields in a less endianness-dependent way, then in the following example `struct fourbytes` would have a memory layout different from `struct fouroctets`- and that doesn’t seem reasonable:

    struct fourbytes {
        signed char a; signed char b; signed char c;
        signed char d;
    }
    struct fouroctets {
        int a:8; int b:8; int c:8; int d:8;
    }

It’s probably not surprising that the CPU’s endianness shows up when looking inside a floating-point number; we said earlier that accessing the same data with different C types often showed up the CPU’s nature. But it reminds us that endianness remains a real and pervasive issue, even when there’s no foreign data or hardware to manage.

A field can only be packed inside one storage unit of its defined type; if we try to define a structure for a MIPS double-precision floating-point number, the mantissa field contains part of two 32-bit int storage units and can’t be defined in one go. The best we can do is something like this:

    struct ieee754dp_konst {
        unsigned sign:1;
        unsigned bexp:11;
        unsigned manthi:20; /* cannot get 52 bits into... */
        unsigned mantlo:32; /* .. a regular C bitfield */
    };

You’re permitted to leave out the name of the field definition, so you don’t have to invent names for fields that are just there for padding.

WithGNUC or other modern compilers, we could have used an `unsigned long long` bitfield and defined the double-precision floating-point register in one go. But the ANSI C specification does not require compilers to support `long long`.

The full alignment rules for bitfields are a little complicated:

1. As we said above, a bitfield must reside entirely in a storage unit that is appropriate for its declared type. Thus, a bitfield never crosses its unit boundary.
2. Bitfields can share a storage unit with other struct/union members, including members that are not bitfields (to pack together, the adjacent structure member must be of a smaller integer type).
3. Structures generally inherit their own alignment requirement from the alignment requirement of their most demanding type. Named bitfields will cause the structure to be aligned (at least) as well as the type requires. Unnamed fields—regardless of their defined type—only force the storage unit or overall structure alignment to that of the smallest integer type that can accommodate that many bits.
4. You might want to be able to force subsequent structure members to occupy a new storage unit. In some compilers you can do that with an `unnamed zero`-width field. Zero-width fields are otherwise illegal (or at least pointless).

You now know everything you need to map C data declarations to memory in a manner compatible with the various ABIs.

## 11.1.7 Unaligned Data from C

Sometimes the alignment rules that help make MIPS CPUs efficient are a nuisance, because you’d like to force your C structure to represent an exact byte-for-byte memory layout, perhaps to match some data from another application.

The GNU C compiler (and most other good ones) allows you to control the alignment used to find a location for any data, whether simple variable or complex structure. In the GNU C dialect, `__attribute__ (( __packed__ ))` in a declaration will cause all normal alignment requirements to be ignored and the data packed, while `__attribute__ (( __aligned(16)__ ))` insists on a 16-byte alignment greater than that used for any standard data type by a MIPS compiler.[^6]

By a judicious mixture of both attributes, you can set out a data structure anyway you like (individual `__aligned__` () controls inside a packed structure alignment as specified).
