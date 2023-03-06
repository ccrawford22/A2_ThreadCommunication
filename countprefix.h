// CS480 - Spring 2023
// Cody Crawford
// RedID:824167663
// Program 2

#ifndef COUNT_PREFIX_H
#define COUNT_PREFIX_H

#include <fstream>

void *countPrefixesInQueue(void *shared)
{
    // local properties for quick reference
    SHARED_DATA *data = (SHARED_DATA *)shared;
    string outFilename = data->filePath[OUTFILEINDEX];

    // debugging
    if (data->verbose)
    {
        pthread_mutex_lock(&(data->queue_mutex));
        cout << "Attempting to save file: " << outFilename << "'" << endl;
        cout << "\tWaiting until dictionary tree has been built." << endl;
        pthread_mutex_unlock(&(data->queue_mutex));
    }

    // wait until vocabulary tree is built
    while (data->taskResult[DICTSRCFILEINDEX] != 1)
    {
        // take a brief pause between checks
        sleep_for(data->defaultWait);

        // check for tree error
        if (data->taskResult[DICTSRCFILEINDEX] == -1)
        {
            data->taskResult[OUTFILEINDEX] = -1;

            pthread_exit(0);
        }
    }

    // debugging
    if (data->verbose)
    {
        pthread_mutex_lock(&(data->queue_mutex));
        cout << "Done waiting for dictionary tree. Starting to save file: '" << outFilename << "'" << endl;
        pthread_mutex_unlock(&(data->queue_mutex));
    }

    // build the out file string
    string outString = "";
    // count each prefixWord
    string prefixWord;
    int prefixCount;
    // delimiter string to seperate words:
    bool process = true;
    while (process)
    {
        if (data->prefixQueue.size() <= 0 && data->taskResult[TESTFILEINDEX] == 1)
            process = false;
        else if (data->prefixQueue.size())
        {
            // get oldest prefix word
            prefixCount = 0;
            pthread_mutex_lock(&(data->queue_mutex));
            prefixWord = data->prefixQueue.front();
            data->prefixQueue.pop();
            pthread_mutex_unlock(&(data->queue_mutex));

            // debugging
            if (data->verbose)
            {
                pthread_mutex_lock(&(data->queue_mutex));
                cout << "\tAttempting to count prefixWord '" << prefixWord << "'." << endl;
                pthread_mutex_unlock(&(data->queue_mutex));
            }

            // count prefix word
            data->dictionary.countWordsStartingFromAString(prefixWord.c_str(), prefixCount);
            // insert prefixWord and count into output string
            if (prefixCount >= data->minNumOfWordsWithAPrefixForPrinting)
            {
                outString += prefixWord + " " + to_string(prefixCount) + "\n";
            }

            // update number of words read from file
            data->numOfProcessedPrefixes += 1;
        }
    }

    ofstream outFile(outFilename, ofstream::out); // open file for writing
    if (!outFile.is_open())
    {
        pthread_mutex_lock(&(data->queue_mutex));
        cerr << endl
             << "Unable to write <<" << outFilename << ">>" << endl;
        pthread_mutex_unlock(&(data->queue_mutex));

        // let other threads know there was an error writing the output file
        data->taskResult[OUTFILEINDEX] = -1;
    }
    else
    {
        // write contents of file
        outFile << outString;

        // close stream
        outFile.close();

        // let other threads know this one is complete
        data->taskResult[OUTFILEINDEX] = 1;

        // debugging
        if (data->verbose)
        {
            pthread_mutex_lock(&(data->queue_mutex));
            cout << "Finished saving file: '" << outFilename << "'" << endl;
            cout << "\tTotal prefixes processed: " << data->numOfProcessedPrefixes << endl;
            pthread_mutex_unlock(&(data->queue_mutex));
        }
    }

    pthread_exit(0);
}

#endif