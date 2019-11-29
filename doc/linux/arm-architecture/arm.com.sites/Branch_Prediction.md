# 什么是分支预测?

The pipeline in a modern microprocessor contains many stages, including instruction fetch, decoding, register allocation and renaming, µop reordering, execution, and retirement. Handling instructions in a pipelined manner allows the microprocessor to do many things at the same time. While one instruction is being executed, the next instructions are being fetched and decoded. The biggest problem with pipelining is branches in the code. For example, a conditional jump allows the instruction flow to go in any of two directions. If there is only one pipeline, then the microprocessor doesn’t know which of the two branches to feed into the pipeline until the branch instruction has been executed. The longer the pipeline, the more time does the microprocessor waste if it has fed the wrong branch into the pipeline. Branches are predicted before they are actually executed. It is used to improve the performance. For every pipeline flush, a penalty of 13 cycles is incurred otherwise in Cortex-A8. Beneficial in cores with long pipelines, as in case of Cortex-A8.

![branch_predict_1](https://raw.githubusercontent.com/tupelo-shen/my_test/master/doc/linux/arm-architecture/arm.com.sites/images/branch_predict_1.jpg)

# Types of Branch Prediction

Static Branch predictor – Static prediction predict that backward-pointing branches will be taken (assuming that the backwards branch is the bottom of a program loop), and forward-pointing branches will not be taken (assuming they are early exits from the loop or other processing code). This only mispredicts the very last branch of a loop. Eg. “for” loop in “C”.

Dynamic Branch predictor – They decide whether a particular branch would be taken or not depending on the previous history. Dynamic branch prediction schemes offer significantly better accuracy than static schemes.

Local Branch Predictors – It looks at the history of just that particular branch to which it is related to.

Global Branch Predictors – It looks at the history of all the branches. Global branch predictors make use of the fact that the behavior of many branches is strongly correlated with the history of other recently taken branches.

# Branch Prediction in Intel Sandy Bridge

The Sandy Bridge reverses the trend of ever more complicated branch prediction algorithms by not having a separate predictor for loops. The redesign of the branch prediction mechanism has probably been necessary in order to handle the new µop cache. A further reason for the simplification may be a desire to reduce the pipeline length and thereby the misprediction penalty.

## Misprediction penalty

The misprediction penalty is often shorter than on the Nehalem thanks to the µop cache. The misprediction penalty was measured to 15 clock cycles or more for branches inside the µop cache and slightly more for branches in the level-1 code cache.

## Pattern recognition for conditional jumps

There appears to be a two-level predictor with a 32-bit global history buffer and a history pattern table of unknown size. There is no specialized loop predictor. Nested loops and loops with branches inside are not predicted particularly well.

## Pattern recognition for indirect jumps and calls

Indirect jumps and indirect calls (but not returns) are predicted using the same two-level predictor as branch instructions.

## BTB organization
The branch target buffer in Sandy Bridge is bigger than in Nehalem according to unofficial rumors. It is unknown whether it has one level, as in Core 2 and earlier processors, or two levels as in Nehalem. It can handle a maximum of four call instructions per 16 bytes of code. The return stack buffer has 16 entries.

# Branch Prediction in AMD Bobcat

## BTB organization
The position of jumps and branches is stored in two arrays, a "sparse branch marker array" which can hold 2 branches for each 64-bytes cache line, and a "dense branch marker array" which can hold 2 branches for every 8 bytes of code, according to the article cited below. In my tests, the Bobcat could predict 2 branches per 64 bytes of code in the level-2 cache, which indicates that the sparse branch array, but not the dense branch array, is coupled to both levels of cache. If both arrays are used, we would expect a maximum of 18 branches per 64 bytes of level-1 cache. In my tests, the Bobcat was able to predict 16 or 17 branches per line of level-1 cache, depending on the position of the branches, but not 18.

There are many mispredictions when these number of branches are exceeded, as the branches keep evicting each other from the arrays.

Branches that are never taken are included in the branch marker arrays.

## Misprediction penalty

The article cited below indicates a misprediction penalty of 13 clock cycles. In my tests, however, the misprediction penalty ranged from 8 to 17 clock cycles depending on the subsequent instructions. There was no consistent difference between integer and floating point code in this respect.

## Pattern recognition for conditional jumps

The branch predictor behaves approximately as a two-level adaptive branch predictor with a global 26-bit history, or slightly better. There is no dedicated loop predictor. This means that loops with many branches or other loops inside are poorly predicted. I have no information on the size of the pattern history table, but we can surely assume that it is less than 226.

Branches that are always taken are included in the history buffer, while unconditional jumps and branches never taken are not.

## Prediction of indirect branches

The Bobcat can predict indirect jumps with more than two different targets. The array used for this prediction is apparently quite small, as mispredictions are frequent.

## Return stack buffer

The Bobcat has a return stack buffer with 12 entries, according to my measurements.

# Overview of the Branch Predictor  in Cortex-A8


    void fname1(void)
    {
       //...
    }
    void fnameN(void)
    {
        //...
    }
    /*  The above functions are written so that later in our code we can have pointers to these functions.   */
    main ()
    {
        int array[]= { 0,1,…,Y }; /* Switch cases will be called by the parameters/values present in this array */
        void (*farray[N]) (void) = { fname1……..,fnameN };   /* Array of pointers to a function */
        int *parray = array;                                /* pointer to array */
        int i, count = N;                                   /* variables required for looping */
        start_perf_counters();
        for (i=0;i<(N*2*Y);i++)     /* The value of ‘i’ is so chosen to cover each function call atleast once*/
        {
            switch (*parray++)          /* switch to a case specified by parray and increment the  pointer also */
            {
            case 0 :
                if (count>=N)
                {
                    count = 0;
                    (*arr[count])();
                }
            }
        }
    }