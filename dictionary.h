// CS480 - Spring 2023
// Cody Crawford
// RedID:824167663
// Program 2

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <iostream>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <math.h>

#define NCHILD 30 /* a-z, ', -, _, terminator of word */
#define MAP "ABCDEFGHIJKLMNOPQRSTUVWXYZ'-_"
#define TERMINAL_INDEX 29

struct dictNode
{
    // Children nodes represent mapping to possible characters
    // of a word and the terminator character of a word.
    // Note the C string ends with a null \0 character.
    // Essentially, the index of each node in the next[] dictNode*
    // array is mapped to a particular valid character
    // or the terminator character.
    // For example, say index 0 is mapped
    // to character ‘a’, index 29 is mapped to the terminator
    // character. If the next character of the word is a, a new node
    // would be created and assigned to next[0]. Setting next[0]
    // from a null pointer to a new node means setting the next
    // character of the word to ‘a’.
    // Note all nodes in the next[] are initialized with a null
    // pointer.
    // After setting the last node corresponding to the last
    // character of the word, to terminate a word, you would
    // set the child node of the next [] of last character node
    // at the terminator index position to a new node, to indicate
    // the end of the word.
    struct dictNode *next[NCHILD];
};

// String length counter
int stringLength(const char *text);

// Interface for Dictionary Tree class
class dictEntry
{
private:
    dictNode *createNode();
    int charToDictIndex(char character);
    void remove(struct dictNode *node);

public:
    struct dictNode *root;
    dictEntry();
    ~dictEntry();

    // changed void to bool based on program rec
    bool add(const char *wordBeingInserted = nullptr);
    dictNode *findEndingNodeOfAStr(const char *strBeingSearched);
    void countWordsStartingFromANode(struct dictNode *temp, int &count);
    void countWordsStartingFromAString(const char *strBeingSearched, int &count);
    void printRoot();
    void printNode(struct dictNode *node);
};

#endif