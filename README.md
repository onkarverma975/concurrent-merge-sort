# concurrent-merge-sort(parallel processing)

Given n numbers, sort the numbers using Merge Sort.

Recursively make two child processes, one for the left half, one of the right
half. 

If the number of elements in the array for a process is less than 5,
perform a selection sort.

The parent of the two children then merges the result and returns back to the
parent and so on.
