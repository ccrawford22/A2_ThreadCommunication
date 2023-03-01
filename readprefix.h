#ifndef READ_PREFIX_H
#define READ_PREFIX_H

using namespace std::this_thread; // sleep_for, sleep_until

void *readPrefixToQueue(void *shared)
{
    // local properties for quick reference
    SHARED_DATA *data = (SHARED_DATA *)shared;
    string testFilename = data->filePath[TESTFILEINDEX];

    // debugging
    if (data->verbose)
    {
        pthread_mutex_lock(&(data->queue_mutex));
        cout << "Attempting to load file: " << testFilename << "'" << endl;
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
            data->taskResult[TESTFILEINDEX] = -1;

            return nullptr;
        }
    }

    // debugging
    if (data->verbose)
    {
        pthread_mutex_lock(&(data->queue_mutex));
        cout << "Done waiting for dictionary tree. Starting to load file: '" << testFilename << "'" << endl;
        pthread_mutex_unlock(&(data->queue_mutex));
    }

    // read in the vocabulary file
    ifstream testFile(testFilename, ifstream::in); // open file for parsing
    if (!testFile.is_open())
    {
        pthread_mutex_lock(&(data->queue_mutex));
        cout << endl
             << "Unable to open <<" << testFilename << ">>" << endl;
        pthread_mutex_unlock(&(data->queue_mutex));

        // let other threads know there was an error reading the vocab file
        data->taskResult[TESTFILEINDEX] = -1;
    }
    else
    {
        // count total number of characters in file and store in shared data
        testFile.seekg(0, std::ios_base::end);
        data->totalNumOfCharsInFile[TESTFILEINDEX] = testFile.tellg();

        // rewind read head to begining of document
        testFile.seekg(0);

        // parse each prefixWord
        string prefixLine;

        const char *delimiters = "\n\r !\"#$%&()*+,./0123456789:;<=>?@[\\]^`{|}~";
        while (getline(testFile, prefixLine))
        {
            char *prefixWord = strtok((char *)prefixLine.c_str(), delimiters);
            while (prefixWord != nullptr)
            {
                // debugging
                if (data->verbose)
                {
                    pthread_mutex_lock(&(data->queue_mutex));
                    cout << "\tAttempting to add prefixWord '" << prefixWord << "' to Dictionary Tree." << endl;
                    pthread_mutex_unlock(&(data->queue_mutex));
                }

                // insert prefixWord in prefix queue
                pthread_mutex_lock(&(data->queue_mutex));
                data->prefixQueue.push(prefixWord);
                pthread_mutex_unlock(&(data->queue_mutex));

                // update number of words read from file
                data->wordCountInFile[TESTFILEINDEX] += 1;

                // read next prefix word
                prefixWord = strtok(NULL, delimiters);
            }

            // update number of characters read in shared data
            data->numOfCharsReadFromFile[TESTFILEINDEX] += prefixLine.length() + 1; // + 1 for endline character
        }

        // close stream
        testFile.close();

        // let other threads know this one is complete
        data->taskResult[TESTFILEINDEX] = 1;

        // debugging
        if (data->verbose)
        {
            pthread_mutex_lock(&(data->queue_mutex));
            cout << "Finished loading file: '" << testFilename << "'" << endl;
            cout << "\tTotal characters in file: " << data->totalNumOfCharsInFile[TESTFILEINDEX] << endl;
            cout << "\tTotal characters read: " << data->numOfCharsReadFromFile[TESTFILEINDEX] << endl;
            cout << "\tTotal words in file: " << data->wordCountInFile[TESTFILEINDEX] << endl;
            pthread_mutex_unlock(&(data->queue_mutex));
        }
    }

    return nullptr;
}

#endif