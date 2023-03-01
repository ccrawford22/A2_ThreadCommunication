// CS480 - Spring 2023
// Cody Crawford
// RedID:824167663
// Program 1
#include "dictionary.h"
#include "countprefix.h"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>

using namespace std;

// int main(int argc, char *argv[])
// {
//     if (argc != 3)
//     {
//         cerr << "Usage: " << argv[0] << " vocabulary_file text_file" << endl;
//         return 1;
//     }

//     dictEntry tree;

//     ifstream vocabFile(argv[1], ifstream::in); // open file for parsing
//     if (!vocabFile.is_open())
//     {
//         cerr << "Error: Failed to open vocabulary file '" << argv[1] << "'" << endl;
//         return 2;
//     }

//     // parse each word
//     string line;
//     // delimiter string to seperate words
//     const char *delimiters = "\n\r !\"#$%&()*+,./0123456789:;<=>?@[\\]^`{|}~";
//     while (getline(vocabFile, line))
//     {
//         char *word = strtok((char *)line.c_str(), delimiters);
//         while (word != nullptr)
//         {
//             // call add method to insert word to build dictionary tree
//             if (!tree.add(word))
//             {
//                 // handle error from insertion result
//                 cout << "\tUnable to add word '" << word << "' to dictionary tree." << endl;
//             }
//             // read next word
//             word = strtok(NULL, delimiters);
//         }
//     }

//     vocabFile.close();

//     // read in test text file
//     ifstream textFile(argv[2]);
//     if (!textFile.is_open())
//     {
//         cerr << "Error: Failed to open text file" << argv[2] << endl;
//         return 3;
//     }

//     while (getline(textFile, line))
//     {
//         char *word = strtok((char *)line.c_str(), delimiters);
//         while (word != nullptr)
//         {
//             int count = 0;
//             // count # of words that start with a substring in dictionary tree
//             tree.countWordsStartingFromAString(word, count);
//             // print count
//             cout << word << " " << count << endl;
//             // read next word
//             word = strtok(NULL, delimiters);
//         }
//     }
//     textFile.close();

//     return 0;
// }