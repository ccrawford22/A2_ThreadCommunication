#ifndef POPULATE_TREE_H
#define POPULATE_TREE_H

#include <cstring>
#include <iostream>
#include <fstream>
#include <chrono>

#include "shared.h"

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until

void *populateTree(void *shared)
{
    SHARED_DATA *data = (SHARED_DATA *)shared;
    string dictFilename = data->filePath[DICTSRCFILEINDEX];

    // debugging
    if (data->verbose)
    {
        // TODO: might need to change to queue_mutex
        pthread_mutex_lock(&(data->queue_mutex));
        cout << endl
             << "Attemting to load file: " << dictFilename << "'" << endl;
        pthread_mutex_unlock(&(data->queue_mutex));
    }

    // read in the vocabulary file
    ifstream vocabFile(dictFilename, ifstream::in);
    if (!vocabFile.is_open())
    {
        pthread_mutex_lock(&(data->queue_mutex));
        cout << endl
             << "Unable to open <<" << dictFilename << ">>" << endl;
        pthread_mutex_unlock(&(data->queue_mutex));

        // let other threads know there was an error reading the vocab file
        data->taskResult[DICTSRCFILEINDEX] = -1;
    }
    else
    {
        // count total number of characters in file and store in shared data
        vocabFile.seekg(0, std::ios_base::end);
        data->totalNumOfCharsInFile[DICTSRCFILEINDEX] = vocabFile.tellg();

        // rewind read head to begining of document
        vocabFile.seekg(0);

        // parse each vocab word
        string vocabLine;
        // delimiter string to seperate words
        const char *delimiters = "\n\r !\"#$%&()*+,./0123456789:;<=>?@[\\]^`{|}~";
        while (getline(vocabFile, vocabLine))
        {
            char *vocabWord = strtok((char *)vocabLine.c_str(), delimiters);
            while (vocabWord != nullptr)
            {
                if (data->verbose)
                {
                    pthread_mutex_lock(&(data->queue_mutex));
                    cout << "\tAttempting to add vocabWord '" << vocabWord << "' to Dictionary Tree." << endl;
                    pthread_mutex_unlock(&(data->queue_mutex));
                }

                // call add method to insert vocabWord to build the dictionary tree
                if (!data->dictionary.add(vocabWord))
                {
                    pthread_mutex_lock(&(data->queue_mutex));
                    cout << "\tUnable to add vocabWord '" << vocabWord << "' to Dictionary Tree." << endl;
                    pthread_mutex_unlock(&(data->queue_mutex));
                }

                // update number of words read from file
                data->wordCountInFile[DICTSRCFILEINDEX] += 1;

                // read next vocabWord
                vocabWord = strtok(NULL, delimiters);
            }

            // update number of characters read in shared data
            data->numOfCharsReadFromFile[DICTSRCFILEINDEX] += vocabLine.length() + 1; // + 1 is for ending character
        }

        // close the stream
        vocabFile.close();

        // let other threads know this is complete
        data->taskResult[DICTSRCFILEINDEX] = 1;
    }

    return nullptr;
}

#endif