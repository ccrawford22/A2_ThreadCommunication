# Thread Communication

This README file provides an overview of the programming assignment focused on thread communication. The assignment aims to practice the use of POSIX 
threads for concurrent execution of multiple related tasks while sharing process-level data and resources.

## Functionality

The program utilizes separate POSIX threads to achieve a similar flow of operations as the previous assignment. Additional features include parsing 
optional command line arguments and displaying task progress. The program follows the steps outlined below:

1. **populatetree thread**: This thread reads words from a vocabulary file and populates a dictionary tree.

2. **readprefix thread**: This thread reads prefixes from a test file and inserts them into a prefix queue. The queue can be implemented using the
   `std::queue` data structure.

5. **countprefix thread**: This thread removes prefixes from the prefix queue and performs the following actions for each retrieved prefix:
   - Searches and counts the number of words in the dictionary tree that start with the prefix.
   - If the count is equal to or greater than the number specified by an optional command line argument (default is 1 if not specified), it prints the
     prefix and the count to a file. Each prefix and count are written on a separate line in the format: `<prefix> <count>`. For example, `'hour 10'`
     indicates that there are 10 words in the dictionary tree that start with the prefix 'hour'.

While the worker threads execute their respective tasks, the main thread monitors and displays the progress of the `populatetree` and `countprefix` threads. 
The progress is displayed as a progress bar (specified in `a2-specs.pdf`) on the standard output. Once the progress bar is fully printed for the `populatetree` 
execution, a new line is printed, followed by the word count of the vocabulary file. Similarly, once the progress bar is fully printed for the `countprefix` 
execution, a new line is printed, followed by the word count of the prefix (or test) file.

Note: Detailed implementation instructions and specifications can be found in the `a2-specs.pdf` document.

## Conclusion

This programming assignment focuses on implementing thread communication using POSIX threads. By utilizing separate threads for different tasks, sharing
process-level data, and coordinating their actions, the program aims to accomplish the desired logic flow. The main thread is responsible for managing the 
execution and displaying progress, while the worker threads handle specific operations related to reading, processing, and counting words and prefixes.
