COMP2002 Assignment 2 – Parallel Merge Sort


Project Number/Title
    - Project: COMP2002 Assignment 2 – Parallel Merge Sort
    - Group name: Group 101
    - Authors:
        Quang Thang Nguyen (a1942274)
        Quan Hoang (a1965988)


Overview
    This program sorts an array of randomly generated integers using merge sort implemented in C. It supports sequential sorting and a parallel version using POSIX threads. A command-line cutoff level controls how many levels of worker threads are created before the algorithm switches to sequential merge sort.


Manifest
    - mergesort.c – Implements merging, sequential merge sort, parallel merge sort, and the allocation of thread arguments.
    - mergesort.h – Declares the sorting functions, shared arrays, cutoff value, and thread argument structure.
    - test-mergesort.c – Generates test data, measures execution time, and checks whether the output is sorted.
    - Makefile – Builds the test-mergesort executable using GCC and pthreads.
    - README – Documents the project, usage, test results, limitations, and references.
    

Building the Project
    From the project directory, run:
    make clean
    make

    The Makefile compiles mergesort.c and test-mergesort.c and links them into the test-mergesort executable. The compiler and POSIX threads support must be available. Run make clean to remove generated object files, dependency files, and the executable.


Features and Usage
    Run the program with three arguments:
    ./test-mergesort <input_size> <cutoff_level> <seed>

    - input_size is the number of integers to generate (the supplied test program requires at least 2).
    - cutoff_level controls the depth of thread creation. Level 0 uses sequential merge sort; at a level equal to or greater than the cutoff, sorting proceeds sequentially.
    - seed initializes the random number generator, allowing the same data to be reproduced between runs.

    For example:
    ./test-mergesort 100 2 1234
    ./test-mergesort 100000000 0 1234
    ./test-mergesort 100000000 4 1234
    
    my_mergesort() recursively divides an inclusive subarray into two halves and merges the sorted halves. parallel_mergesort() creates two POSIX threads for separate, non-overlapping halves when the current level is below the cutoff. Each parent waits for both children using pthread_join() before merging their results. merge() uses the shared auxiliary array B as temporary storage, but each simultaneously executing merge operates on a different index range. buildArgs() allocates a structure containing the range and recursion level for each task. Parent calls release their child argument structures after joining; the provided test program releases the root argument.


Testing


    The supplied test-mergesort program was run on a MacBook Pro using the same random seed (1234) across cutoff levels to make comparisons consistent. Successful runs reported Sorting ... elements took ... seconds. rather than sorting failed!!!!. The terminal output shows successful runs for 100 elements at cutoff 2, 1,000,000 elements at cutoff 0, and 100,000,000 elements at cutoff levels 0 through 6.


    Large-array performance: 100,000,000 elements (seed 1234)
Cutoff level	Execution time (seconds)	Speedup relative to cutoff 0
0	            16.55                   	1.00x
1	            8.91	                    1.86x
2	            4.89	                    3.38x
3	            3.15	                    5.25x
4	            2.68	                    6.18x
5	            2.74	                    6.04x
6	            2.85	                    5.81x


    Speedup is calculated as the sequential time divided by the parallel time, for example 16.55 / 2.68 = 6.18 at cutoff 4. The best observed execution time was 2.68 seconds (cutoff 4), which exceeded the assignment's minimum 2x speedup requirement on this machine. Increasing the cutoff beyond 4 did not improve the recorded time; the overhead of additional threads may outweigh the extra parallelism. These timings are single observed runs and may vary depending on hardware and system load. Gradescope results may differ.

    The supplied test program checks whether the final array is sorted in nondecreasing order. Its check does not independently verify preservation of all original element counts, and no separate memory-sanitizer or forced thread-failure results are claimed here.


Known Bugs


    No sorting failures were observed in the runs described above. However, the current implementation has these known robustness limitations:
    - buildArgs() does not check whether malloc() returns NULL before dereferencing the pointer. Memory allocation failure could cause a crash.
    - parallel_mergesort() does not check return values from pthread_create() or pthread_join(). Thread-creation failure could lead to invalid joins, missing work, or incorrect cleanup.
    - Very high cutoff levels may create excessive numbers of threads and exhaust system resources.
    - The midpoint uses (left + right) / 2, which can overflow for sufficiently large valid integer indices, although this was not an issue in the 100,000,000-element test.
    These limitations were not exercised by the reported successful tests and should not be interpreted as evidence that all failure cases are handled safely.


Reflection and Self Assessment


    The implementation illustrates how a divide-and-conquer algorithm can be parallelised without changing its merge-sort logic. The key design requirement is synchronisation: both child ranges must be completely sorted before their parent merges them. Creating threads for non-overlapping subarrays avoids simultaneous writes to the same elements, while pthread_join() establishes the ordering needed before each merge.

    The performance results also demonstrate a practical trade-off. More worker threads reduced execution time initially, but deeper thread creation did not continuously improve performance. On the tested MacBook, cutoff 4 performed best among the reported runs. The program's remaining weaknesses are primarily error handling and resource exhaustion rather than the normal sorting path. Future improvements would check allocation and pthread return values, use a fallback when a worker cannot be created, and test with additional seeds and concurrency/memory-checking tools.


Sources Used


    - COMP2002 Assignment 2 specification, lecture materials, and supplied starter code (mergesort.c, mergesort.h, test-mergesort.c, Makefile, and README.template).
    - Arpaci-Dusseau, R. H., & Arpaci-Dusseau, A. C. Operating Systems: Three Easy Pieces, chapters “Introduction to Threads” and “Threads API.” https://pages.cs.wisc.edu/~remzi/OSTEP/
    - OpenAI ChatGPT (2026), assistance with explaining pthread synchronisation, reviewing implementation limitations, interpreting benchmark results, and drafting this README. https://chatgpt.com/