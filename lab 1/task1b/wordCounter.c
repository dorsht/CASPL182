#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    // 1b
    int i, wasSpaceOrEnter, wordCounter, charNum, longestWord, legalInput = 1,
    pressW=0, pressC = 0, pressL = 0;
    for (i=1;i<argc;i++)
    {
        if (strcmp(argv[i],"-w") ==0)
            pressW = 1;
        else if (strcmp(argv[i],"-c")==0)
            pressC = 1;
        else if (strcmp(argv[i],"-l")==0)
            pressL = 1;
        else legalInput = 0;
    }
    if (!legalInput) return 1; // illegal input
    if (pressC == 0 && pressL == 0 && pressW == 0)
        pressW = 1; // default setting, if input like -l,-w or -c didn't found
    // set the start values for the variables here
    wasSpaceOrEnter = 0; 
    wordCounter = 0; 
    charNum = 0;
    longestWord = 0;
    printf("#> wordCounter\n");
    int tempLongestWord = 0;
    int ch = getchar();
    while (ch!=EOF)
    {
        char c = (char)ch;
        if (c==' '||c=='\n')
        {
            if (!wasSpaceOrEnter)
            {
                wordCounter++;
                wasSpaceOrEnter = 1;
                if (tempLongestWord>longestWord)
                    longestWord = tempLongestWord;
                tempLongestWord = 0;
            }
        }
        else if (c>32 && c<127){
            tempLongestWord++;
            charNum++;
            wasSpaceOrEnter = 0;
        }
        ch = getchar();
    }
    if (pressW) printf("%d\n",wordCounter);
    if (pressC) printf("%d\n", charNum);
    if (pressL) printf("%d\n", longestWord);
    return 0;
}