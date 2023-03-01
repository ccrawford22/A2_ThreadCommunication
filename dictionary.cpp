#include "dictionary.h"
#include <iostream>

using namespace std;

int stringLength(const char *text)
{
    int i = 0;
    while (text[i] != '\0')
    {
        i++;
    }
    return i;
}

// New node creation helper
dictNode *dictEntry::createNode()
{
    dictNode *temp = new dictNode();
    for (int i = 0; i < NCHILD; ++i)
    {
        temp->next[i] = nullptr;
    }
    // temp->next[TERMINAL_INDEX] = new dictNode();
    return temp;
}

int dictEntry::charToDictIndex(char character)
{
    int index = -1;

    // Check for punctuation first
    if (character == '\0')
        index = 29;
    else if (character == '_')
        index = 28;
    else if (character == '-')
        index = 27;
    else if (character == '\'')
        index = 26;
    // treat as alphanumeric
    else
    {
        // Make uppercase if necessary
        if (character - 'A' >= 26)
            character -= ('a' - 'A');
        // convert to 0 - 26
        index = character - 'A';
    }
    // Check index range
    if (index < 0 || index > 29)
        return -1;
    else
        return index;
}

dictEntry::dictEntry()
{
    this->root = createNode();
}

dictEntry::~dictEntry() // '~' means destructure
{
    remove(this->root);
    this->root = nullptr;
}

void dictEntry::remove(struct dictNode *node)
{
    if (node)
    {
        for (int i = 0; i < NCHILD; i++)
        {
            remove(node->next[i]);
            node->next[i] = nullptr;
        }
        free(node);
        return;
    }
}

bool dictEntry::add(const char *wordBeingInserted)
{
    int len = stringLength(wordBeingInserted);
    if (len < 1)
    {
        cout << "dictEntry::add('" << wordBeingInserted << "') Attempting to add an empty string to the dictionary." << endl;
        return false;
    }

    // if (!this->root) this->root = createNode();
    dictNode *head = this->root;
    int i = 0;

    while (i <= len)
    {
        int index = charToDictIndex(wordBeingInserted[i]);
        if (index > -1)
        {
            // create new dictNode at index if necessary
            if (head->next[index] == nullptr)
            {
                head->next[index] = createNode();
            }
            // rinse and repeat
            head = head->next[index];
        }
        else
        {
            cout << "dictEntry:add('"
                 << wordBeingInserted << "') Unknown character: '"
                 << wordBeingInserted[i] << "'." << endl;
            return false;
        }
        i++;
    }
    return true;
}

dictNode *dictEntry::findEndingNodeOfAStr(const char *strBeingSearched)
{
    // returns root if string being searched is empty
    // change line below to nullptr if we should we return nullptr if searching for an empty string
    dictNode *result = this->root;
    if (stringLength(strBeingSearched) > 0 && this->root)
    {
        result = this->root; // start search at the root

        int i = 0;
        int charIndex = -1;
        while (strBeingSearched[i] != '\0')
        {
            charIndex = charToDictIndex(strBeingSearched[i]);
            if (result->next[charIndex])
            {
                result = result->next[charIndex];
                i++;
            }
            else
            {
                // prefix is not yet in dictionary
                return nullptr;
            }
        }
    }
    return result;
}

void dictEntry::countWordsStartingFromANode(struct dictNode *temp, int &count)
{
    if (temp)
    {
        if (temp->next[TERMINAL_INDEX])
        {
            count += 1;
        }
        for (int i = 0; i < TERMINAL_INDEX; i++)
        {
            countWordsStartingFromANode(temp->next[i], count);
        }
    }
}

void dictEntry::countWordsStartingFromAString(const char *strBeingSearched, int &count)
{
    countWordsStartingFromANode(findEndingNodeOfAStr(strBeingSearched), count);
}

void dictEntry::printRoot()
{
    dictEntry::printNode(this->root);
}

void dictEntry::printNode(struct dictNode *node)
{
    if (node)
    {
        bool printedOne = false;
        cout << "dictNode = {";
        for (int i = 0; i < NCHILD; i++)
        {
            if (node->next[i])
            {
                if (printedOne)
                {
                    cout << ",";
                }
                else
                {
                    printedOne = true;
                }
                cout << "'" << MAP[i] << "'";
            }
        }
        cout << "}" << endl;
    }
    else
    {
        cout << "dictNode = nullPtr" << endl;
    }
}
