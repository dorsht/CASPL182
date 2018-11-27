#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    // 2
    int i, legalInput = 1, pressW=0, pressC = 0, pressL = 0, pressN = 0,
    wasSpaceOrEnter, wordCounter, charNum, longestWord, enterNum, fromFile = 0;
    FILE *f = stdin; // default value
    for (i=1;i<argc;i++)
    {
        if (strcmp(argv[i],"-w") ==0)
            pressW = 1;
        else if (strcmp(argv[i],"-c")==0)
            pressC = 1;
        else if (strcmp(argv[i],"-l")==0)
            pressL = 1;
        else if (strcmp(argv[i],"-n")==0)
            pressN = 1;
        else if (strcmp(argv[i],"-i")==0){
            char* fileName = argv[i+1];
            f = fopen(fileName,"r"); // I used this to read the file only
            if (f == NULL) legalInput = 0;
            else fromFile = 1;
            i = argc+1;
        }
      else legalInput = 0;
    }
    if (!legalInput) return 1; // illegal input
    if (pressC == 0 && pressL == 0 && pressW == 0 && pressN == 0)
        pressW = 1; // default setting, if input like -l,-w, -n or -c didn't found
    // set the start values for the variables here
    wasSpaceOrEnter = 0; 
    wordCounter = 0; 
    charNum = 0;
    longestWord = 0;
    enterNum = 0;
    printf("#> wordCounter\n");
    int tempLongestWord = 0;
    int ch = fgetc(f);
    while (ch!=EOF)
    {
        char c = (char)ch;
        if (c==' '||c=='\n')
        {
            if (c=='\n') enterNum++;
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
        ch = fgetc(f);
    }
    if (fromFile) fclose(f);
    if (pressW) printf("%d\n",wordCounter);
    if (pressC) printf("%d\n", charNum);
    if (pressL) printf("%d\n", longestWord);
    if (pressN) printf("%d\n", enterNum);
    return 0;
}