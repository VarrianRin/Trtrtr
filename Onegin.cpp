#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys\stat.h>
#include <ctype.h>

//! struct _stat buff\n
//! needs to be for _stat

struct _stat buff;

//! struct data\n
//! @param  char* index - pointers to strings
//! @param  int size - size of strings

struct data {
    char* index;
    int size;
} *Data;

//! int writer\n
//! writes without empty strings
//! @param [in]  file - pointer to file
//! @param [in]  Data - struct data with pointers to strings from file
//! @param [in]  NLines - number of lines in text
//! @param [out] line - number of lines written

int writer (const char* file, const struct data Data[], int NLines) {

    assert (file != NULL);

    FILE* EmptyFile = fopen(file, "w");
    assert (EmptyFile != NULL);

    int line = 0;

    for (; line < NLines; line++) {

        if (Data[line].size > 1) {
            printf("line = %d\n", line);
            fprintf(EmptyFile, (const char*) Data[line].index);
            fprintf(EmptyFile, "\n");
            printf("'%s'\n length = %d\n", Data[line].index, Data[line].size);
        }
    }

    fclose(EmptyFile);
    return line;
    }

//! WinReader\n
//! only for Windows files
//! @param [in]  file - pointer to file
//! @param [out] text (calloc) - pointer to text read

char* WinReader (const char* file) {

    assert(file != NULL);

    FILE* TextFile = fopen(file, "rb");
    assert(TextFile != NULL);

    int StatReturn = _stat(file, &buff);
    assert(StatReturn == 0);

    char* text  = (char*) calloc(buff.st_size + 1, sizeof(text[0]));

    int SizeFile = fread(text, sizeof(text[0]), buff.st_size, TextFile);
    assert (SizeFile > 0);
    fclose(TextFile);

    text[SizeFile] = '\0';

    return text;
    }

//! WinTextSorter1\n
//! Doesn't delete strings\n
//! Only for Windows files\n
//! @param [in]  Data - struct data with pointers to string
//! @param [in]  text - pointer to text
//! @param [out] line - number of strings sorted

int WinTextSorter1 (char* text) {

    int NLines = 1;

    for(int i = 0; i < buff.st_size; i++)
        if (text[i] == '\n')
            NLines++;

    Data = (struct data*) calloc(NLines, sizeof(struct data));

    int line = 1;

    Data[0].index = text;

    for ( ; (text = strchr(text + 1, '\n')) != '\0'; line++) {

        *text = '\0';
        *(text - 1) = '\0';
        Data[line].index = text + 1;
        Data[line - 1].size = Data[line].index - Data[line - 1].index - 2;
    }

    Data[line - 1].size = Data[0].index + buff.st_size - Data[line - 1].index;

    return line;
}

//! WinTextSorter2\n
//! Deletes strings without letters\n
//! Only for Windows files\n
//! @param [in]  Data - struct data with pointers to string
//! @param [in]  text - pointer to text
//! @param [out] line - number of strings sorted

int WinTextSorter2 (struct data Data[], char* text) {


    int line = 1;

    Data[0].index = text;

    for ( ; (text = strchr(text + 1, '\n')) != '\0'; line++) {

        *text = '\0';

        if (*(text + 2) == '\n') {
            text += 2;
            line--;
            continue;
        }
        *(text - 1) = '\n';

        Data[line].index = (text + 1);
    }

    return line;
}

//! strcmp_edited\n
//! strcmp that considers only letters\n
//! used for qsort\n
//! @param [in]  elem1 - pointer to first string
//! @param [in]  elem2 - pointer to second string
//! @param [out] -1 if elem1 < elem2; 0 if elem1 = elem2; 1 if elem1 > elem2; syntacic comparison

int strcmp_edited (const void* elem1, const void* elem2) {

    //assert (elem1 != NULL);
    //assert (elem2 != NULL);

    const char* s1 = (*(struct data *) elem1).index;
    const char* s2 = (*(struct data *) elem2).index;

    for ( ; ; ) {

        if (!isalpha(*s1)) {

            while(!isalpha(*s1) && *s1 != '\0')
                s1++;
        }

        if (!isalpha(*s2)) {

            while(!isalpha(*s2) && *s2 != '\0')
                s2++;
        }

        if (*s1 != *s2) {

            return(*s1 > *s2) ? 1: -1;
        }

        for( ; *s1 == *s2; s1++, s2++) {

            if(*s1 == '\0')
                return 0;
        }
    }

    assert(0);//exit(EXIT_FAILURE);
    return 0;
}

int strcmp_reverse (const void* elem1, const void* elem2) {

    char* str1 = (*(struct data *) elem1).index;
    char* str2 = (*(struct data *) elem2).index;

    int size1 = (*(struct data *) elem1).size;
    int size2 = (*(struct data *) elem2).size;

    char* s1 = (str1 + size1 - 1);
    char* s2 = (str2 + size2 - 1);

    for( ; ; ) {

        if (!isalpha(*s1)) {

            while(!isalpha(*s1) && *s1 != '\0')
                s1--;
        }

        if (!isalpha(*s2)) {

            while(!isalpha(*s2) && *s2 != '\0')
                s2--;
        }

        if (*s1 != *s2) {
            return(*s1 > *s2) ? 1: -1;
        }

    for ( ; *s1 == *s2; s1--, s2--) {

        if (*s1 == '/0')
            return 0;

        }
    }

    assert(0);
    return 0;
}

//! main\n
//! @param NLines - number of strings in file
//! @param text - pointer to text from file

int main() {

    char* text = WinReader ("onegin.txt");
    assert (text != NULL);

    int NLines = WinTextSorter1(text);

    for (int i = 0; i < NLines; i++)
        assert(Data[i].index != NULL);

    qsort(Data, NLines, sizeof(struct data), strcmp_reverse);

    int WrReturn = writer ("write in me.txt", Data, NLines);
    assert (WrReturn != 0);

    free(text);
    return 0;
}
