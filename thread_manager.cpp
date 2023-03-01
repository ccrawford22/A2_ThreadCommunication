#define BADFLAG 1
#define NORMALEXIT 0

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>

#include <string>
#include <fstream>
#include <iostream>
#include <chrono>

#include "shared.h"

// NOTE this source code file is for simple demonstration
// of how command line parsing and pthreads work,
// this file MAY NOT compile

// Thread for populating a dictionary tree
#include "populatetree.h"
// Thread for reading in prefixes and queuing them
#include "readprefix.h"
// Thread for searching and counting prefixes in a dictionary tree
#include "countprefix.h"

using namespace std;              // Make sure std components accessible
using namespace std::this_thread; // sleep_for, sleep_until

// TODO: Remove this function
void *print_message_function(void *ptr)
{
  char *message;
  message = (char *)ptr;
  cout << endl;
  printf("%s", message);
  cout << endl;
  pthread_exit(0);
}

void print_progress(SHARED_DATA &sharedData)
{
  float pctDecimal = (float)sharedData.numOfCharsReadFromFile[DICTSRCFILEINDEX] /
                     sharedData.totalNumOfCharsInFile[DICTSRCFILEINDEX];
  float progress = min(1.0f, max(0.0f, pctDecimal));
  int pos = sharedData.numOfProgressMarks * progress;
  cout << "[";
  for (int i = 0; i < sharedData.numOfProgressMarks; ++i)
  {
    if (i < pos && i % sharedData.hashmarkInterval == sharedData.hashmarkInterval - 1)
      cout << "#";
    else if (i < pos)
      cout << "-";
    else if (i == pos)
      cout << ">";
    else
      cout << " ";
  }
  cout << "]" << int(progress * 100.0f) << " %\r";
  cout.flush();
}

/**
 * @brief
 *
 * @param argc
 * @param argv
 * @return int
 */

int main(int argc, char **argv)
{

  // CHECK USAGE
  if (argc < 3)
  {
    cerr << "Usage: " << argv[0] << " vocabulary_file text_file ["
                                    "-p number_of_progress_marks "
                                    "-h number_of_hash_marks "
                                    "-n min_word_count_to_output "
                                    "]"
         << endl;
    exit(BADFLAG);
  }

  // DEFINE AND INITIALIZE MAIN THREAD VARIABLES
  // bool verbose = false; // chatty or quiet program
  int option; /* command line switch */
  int idx;    // general purpose index variable
  string filename;
  string ext = DEFAULT_OUTPUT_EXT;

  // shared data to be used for communication between threads
  // main thread, populatetree, readprefix, countprefix
  SHARED_DATA sharedData;

  // initialize the sharedData, you can have a helper do it
  sharedData.numOfProgressMarks = DEFAULT_NUMOF_MARKS;
  sharedData.hashmarkInterval = DEFAULT_HASHMARKINTERVAL;
  sharedData.minNumOfWordsWithAPrefixForPrinting = DEFAULT_MINNUM_OFWORDS_WITHAPREFIX;
  sharedData.filePath[OUTFILEINDEX] = DEFAULT_OUTPUT_FILENAME;
  sharedData.defaultWait = DEFAULT_WAIT;
  pthread_mutex_init(&sharedData.queue_mutex, NULL); // TODO: Is this necessary?
  // // other stuff (e.g. declarations)
  // int marksProgress = 50;
  // int marksHash = 10;
  // int count = 1; // some counter set by -n, set default

  // string filenameVocab = "";
  // string filenameTest = "";

  // HANDLE OPTIONAL ARGUMENTS
  /*
   * This example uses the standard C library getopt (man -s3 getopt)
   * It relies on several external variables that are defined when
   * the getopt.h routine is included. On POSIX2 compliant machines
   * <getopt.h> is included in <unistd.h> and only the unix standard
   * header need be included.
   *
   * getopt takes:
   *    argc - Number of strings in argv
   *    argv - Strings for each token (contiguous characters in the
   *           command line. Example:
   *           ./program -n 56 -v boo bear
   *           argv[0] = "a.out", argv[1] = "-n", argv[2] = "56"
   *           argv[3] = "-v", argv[4] = "boo", argv[5] = "bear"
   *    optionstr - String indicating optional arguments to process
   *           Options with a : after them expect some type of value
   *           after them.  Example:  "n:o:v" n and o expect arguments,
   *           v does not
   */

  while ((option = getopt(argc, argv, "vp:h:n:o:")) != -1)
  {
    /* If the option has an argument, optarg is set to point to the
     * argument associated with the option.  For example, if
     * -n is processed, optarg points to "56" in the example above.
     */
    switch (option)
    {
    case 'v': /* optarg is undefined */
      cout << "-v argument is present, turning on verbose mode" << endl;
      sharedData.verbose = true;
      break;

    case 'p': /* Assume this takes a number */
      /*
        N number of progress marks (either hyphen or #) for displaying 100% progress of a thread execution,
        default is 50 if not specified. Minimal number is 10. If a number less than 10 is specified,
        program should print to the standard output: Number of progress marks must be a number and at least 10
        then exit.
      */
      /* optarg will contain the string following -n
        -n is expected to be an integer in this case, so convert the
        string to an integer.
       */
      sharedData.numOfProgressMarks = atoi(optarg);
      if (sharedData.verbose)
        cout << "-p argument = " << sharedData.numOfProgressMarks << endl;
      if (sharedData.numOfProgressMarks < 10)
      {
        cout << endl
             << "Number of progress marks must be a number and at least 10." << endl;
        cout << "-p argument = " << sharedData.numOfProgressMarks << endl;
        exit(BADFLAG);
      }
      break;
    case 'h': /* Assume this takes a number */
              /*
              place a hash mark “#” in the progress bar every N characters, default is 10 if not specified.
              If an out-of-range number (<=0 or > 10) is specified, program should print to the standard output:
              Hash mark interval for progress must be a number, greater than 0, and less than or equal to 10
              then exit.
              */
      sharedData.hashmarkInterval = atoi(optarg);
      if (sharedData.verbose)
        cout << "-h argument = " << sharedData.hashmarkInterval << endl;

      if (sharedData.hashmarkInterval > 10 || sharedData.hashmarkInterval <= 0)
      {
        cout << endl
             << "Hash mark interval for progress must be a number, greater than 0, and less than or equal to 10." << endl;
        cout << "-h argument = " << sharedData.hashmarkInterval << endl;
        exit(BADFLAG);
      }
      break;
    case 'n': /* Assume this takes a number */
              /*
              print prefix and its count (refer to 3. b in Functionality) to an output file
              (countprefix_output.txt) only if the number of dictionary words starting with
              the prefix is equal to or greater than N, default is 1 if not specified.
              */
      sharedData.minNumOfWordsWithAPrefixForPrinting = atoi(optarg);
      if (sharedData.verbose)
        cout << "-n argument = " << sharedData.minNumOfWordsWithAPrefixForPrinting << endl;
      if (sharedData.minNumOfWordsWithAPrefixForPrinting <= 0)
      {
        cout << endl
             << "Count (arg -n) must be greater than or equal to 0." << endl;
        cout << "-n argument = " << sharedData.minNumOfWordsWithAPrefixForPrinting << endl;
        exit(BADFLAG);
      }
      break;

    case 'o': /* optarg points to whatever follows -o */
      // optarg contains the output, do something appropriate
      filename = optarg;
      if (sharedData.verbose)
        cout << "-o argument (optional output file name) = " << filename << endl;
      if (filename.length() <= ext.length() ||
          0 != filename.compare(filename.length() - ext.length(), ext.length(), ext))
      {
        cout << endl
             << "Output file (arg -o) must be a properly named '.txt' file" << endl;
        cout << "-o argument (optional output file name) = " << filename << endl;
        exit(BADFLAG);
      }
      sharedData.filePath[OUTFILEINDEX] = optarg;
      break;

    default:
      // print something about the usage and exit
      exit(BADFLAG); // BADFLAG is an error # defined in a header
    }
  }

  /*
   * Once the getop loop is done external variable optind contains
   * a number.  This is the first argument of argv to process
   * after all options have been processed.
   * argv[optind] is the next mandatory argument.
   */

  // HANDLE MANDATORY ARGUMENTS
  idx = optind;

  /* If idx < argc, there are mandatory arguments to process */
  if (idx < argc)
  {
    /* Process positional arguments:
     *argv[idx] argv[idx+1], ..., argv[argc-1]
     */

    // ...
    sharedData.filePath[DICTSRCFILEINDEX] = argv[idx];
    if (sharedData.verbose)
      cout << "Vocab file path = " << sharedData.filePath[DICTSRCFILEINDEX] << endl;
    idx++;

    sharedData.filePath[TESTFILEINDEX] = argv[idx];
    if (sharedData.verbose)
      cout << "Test file path = " << sharedData.filePath[TESTFILEINDEX] << endl;
    idx++;

    // continue to process more mandatory arguments
    // if there are any left

    if (sharedData.verbose && idx < argc)
    {
      cout << "Recieved " << argc - idx << "additional arguments that will be ignored:" << endl;
      for (int i = idx; i < argc; i++)
      {
        cout << "\tArg " << i << " = " << argv[i] << endl;
      }
    }
  }

  // sample pthread code
  pthread_attr_t pthread_attr_default;

  pthread_t thread1;
  char message1[] = "Hello World";

  pthread_attr_init(&pthread_attr_default);
  // TODO: Bring the next two lines back into the code
  // pthread_create(&thread1, &pthread_attr_default,
  //                &print_message_function, (void *)message1);

  // populatetree thread, more comments here of what this thread does
  pthread_t populateTreeThread;
  // readprefix thread, more comments here of what this thread does
  pthread_t readPrefixThread;
  // countprefix thread, more comments here of what this thread does
  pthread_t countPrefixThread;

  if (pthread_create(&populateTreeThread, NULL,
                     &populateTree, &sharedData))
  {
    // error handling
    pthread_mutex_lock(&(sharedData.debug_mutex));
    cout << endl
         << "Unable to create populateTreeThread." << endl;
    pthread_mutex_unlock(&(sharedData.debug_mutex));
    exit(BADFLAG); // BADFLAG is an error # defined in a header
  }

  if (pthread_create(&readPrefixThread, NULL,
                     &readPrefixToQueue, &sharedData))
  {
    // error handling
    pthread_mutex_lock(&(sharedData.debug_mutex));
    cout << endl
         << "Unable to create readPrefixThread." << endl;
    pthread_mutex_unlock(&(sharedData.debug_mutex));
    exit(BADFLAG); // BADFLAG is an error # defined in a header
  }

  if (pthread_create(&countPrefixThread, NULL,
                     &countPrefixesInQueue, &sharedData))
  {
    pthread_mutex_lock(&(sharedData.debug_mutex));
    cout << endl
         << "Unable to create countPrefix thread." << endl;
    pthread_mutex_unlock(&(sharedData.debug_mutex));
    exit(BADFLAG); // BADFLAG is an error # defined in a header
  }

  // HANDLE PROGRESS BARS
  // start on a new line
  cout << endl;
  while (sharedData.taskCompleted[DICTSRCFILEINDEX] != 1)
  {
    // take a brief pause between checks
    sleep_for(sharedData.DEFAULT_WAIT);

    // wait until thread has finished counting characters in the file
    // to avoid division by 0

    if (sharedData.totalNumOfCharsInFile[DICTSRCFILEINDEX] == 0)
      continue;

    // handle populatetree states
    switch (sharedData.taskCompleted[DICTSRCFILEINDEX])
    {
    case -1: // an error has occured
      exit(BADFLAG);
      break;
    case 0: // still working
      // print progress bar for populatetree thread
      // use the SHARED_DATA
      pthread_mutex_lock(&(sharedData.debug_mutex));
      print_progress(sharedData);
      pthread_mutex_unlock(&(sharedData.debug_mutex));

      break;

    default: // done
      break;
    }
  }

  // print progress one more time to make sure it is fully displayed
  pthread_mutex_lock(&(sharedData.debug_mutex));
  print_progress(sharedData);
  pthread_mutex_unlock(&(sharedData.debug_mutex));
  // create a new line so that other progress bars appear below this one
  cout << endl;

  // print progress bar for populatetree thread
  // use the SHARED_DATA

  // print progress bar for countprefix thread
  // note this needs to wait for the completion of readprefix thread
  // use the SHARED_DATA

  exit(NORMALEXIT); // Normal Exit!
}
