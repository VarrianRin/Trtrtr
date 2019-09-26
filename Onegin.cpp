#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys\stat.h>
#include <ctype.h>

//! struct data\n
//! @param  char* index - pointers to lines
//! @param  int size - size of lines

struct data {
    char* index;
    int size;
};

//! StringWriter\n
//! writes text in file\n
//! @param [in]  FileName - name of file
//! @param [in]  text - pointer to text
//! @param [in]  count - number of symbols to be written
//! @param [in]  whattodo - what to do with file ("w" - write in empty file or "a" - append)
//! @param [out] n_symbol - number of symbols written

int StringWriter (const char* FileName, const char* text, int count, const char* whattodo) {

    assert (FileName != NULL);
    assert (text != NULL);

    FILE* File = fopen(FileName, whattodo);
    assert (File != NULL);

    int n_symbol = 0;

    for(; n_symbol < count; n_symbol++) {
        fprintf(File, "%c", text[n_symbol]);
        if (text[n_symbol] == '\0') {
            fprintf(File, "\n");
            n_symbol++;
        }
    }
    assert(n_symbol == count);
    return n_symbol;
}

//! StructWriter\n
//! writes text in struct without empty lines in file\n
//! @param [in]  FileName - name of file
//! @param [in]  Lines - struct data with pointers to lines from file
//! @param [in]  NLines - number of lines in text
//! @param [in]  whattodo - what to do with file ("w" - write in empty file or "a" - append)
//! @param [out] line_count - number of lines written

int StructWriter (const char* FileName, const struct data* Lines, int NLines, const char* whattodo) {

    assert (FileName != NULL);

    FILE* File = fopen(FileName, whattodo);
    assert (File != NULL);

    int line_count = 0;

    for (; line_count < NLines; line_count++)
        if (Lines[line_count].size > 1)
            fprintf(File, "%s\n", Lines[line_count].index);

    assert(line_count == NLines);

    fclose(File);
    return line_count;
    }

//! WinReader\n
//! only for Windows files\n
//! makes first and last symbol '\0'\n
//! @param [in]  FileName - pointer to file
//! @param [in]  SizeFile - size of file
//! @param [out] text (calloc) - pointer to text read
//! @warning initializing text (calloc)

char* WinReader (const char* FileName, int SizeFile) {//sizefile

    assert(FileName != NULL);

    FILE* TextFile = fopen(FileName, "rb");
    assert(TextFile != NULL);

    char* text  = (char*) calloc(SizeFile + 1, sizeof(text[0]));

    assert(text != NULL);

    int ReadReturn= fread(text, sizeof(text[0]), SizeFile, TextFile);
    assert (ReadReturn == SizeFile);
    fclose(TextFile);

    text[SizeFile] = '\0';
    text[-1] = '\0';

    return text;
}

//! WinTextParser\n
//! Doesn't delete lines\n
//! Only for Windows files\n
//! @param [in]  text - pointer to text
//! @param [in]  SizeFile - size of the file
//! @param [in]  Lines - strust data with pointers to lines an their sizes
//! @param [out] line - number of lines sorted

int WinTextParser (char* text, int SizeFile, struct data* Lines) {

    int line = 1;

    Lines[0].index = text;

    for ( ; (text = strchr(text + 1, '\n')) != NULL; line++) {

        *text = '\0';
        *(text - 1) = '\0';
        Lines[line].index = text + 1;
        Lines[line - 1].size = Lines[line].index - Lines[line - 1].index - 2;
    }

    Lines[line - 1].size = Lines[0].index + SizeFile - Lines[line - 1].index;

    return line;
}

//! strcmp_edited\n
//! strcmp that ignores non-letters\n
//! used for qsort\n
//! @param [in]  elem1 - pointer to first string
//! @param [in]  elem2 - pointer to second string
//! @param [out] -1 if elem1 < elem2; 0 if elem1 = elem2; 1 if elem1 > elem2; syntacic comparison

int strcmp_edited (const void* elem1, const void* elem2) {

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

    assert(0);
    return 0;
}

//! strcmp_reverse\n
//! strcmp that compares from ends of lines and ignores non-letters\n
//! used for qsort\n
//! @param [in]  elem1 - pointer to first string
//! @param [in]  elem2 - pointer to second string
//! @param [out] -1 if elem1 < elem2; 0 if elem1 = elem2; 1 if elem1 > elem2; syntacic comparison

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

            if (s1 == '\0')
                return 0;
        }
    }

    assert(0);
    return 0;
}

//! GetFileSize\n
//! returns size of file\n
//! @param [in]  FileName - path to file
//! @param [out] buff.st_size - size of file

int GetFileSize(const char* FileName) {

    assert(FileName != 0);

    struct _stat buff {};
    int StatReturn = _stat(FileName, &buff);
    assert(StatReturn == 0);

    return buff.st_size;
}

//! LineCounter\n
//! counts lines\n
//! @param [in]  text - text with lines
//! @param [out] NLines - number of lines in text

int LineCounter(const char* text) {

    assert(text != 0);
    int NLines = 1;

    for(int i = 0; text[i] != '\0'; i++)
        if (text[i] == '\n')
            NLines++;

    return NLines;
}

//! main\n
//! @param Onegin - Onegin
//! @param InputFile - file to write in
//! @param SizeFile - size of onegin
//! @param Lines - struct data with pointers to lines and sizes of them
//! @param NLines - number of lines in file
//! @param text - pointer to text from file

int main() {

    const char Onegin[] = {"onegin.txt"};
    const char InputFile[] = {"write in me.txt"};

    int SizeFile = GetFileSize(Onegin);

    char* text = WinReader(Onegin, SizeFile);
    assert (text != NULL);

    int NLines = LineCounter(text);
    struct data* Lines;
    Lines = (struct data*) calloc(NLines, sizeof(struct data));

    int ParsReturn = WinTextParser(text, SizeFile, Lines);
    assert(ParsReturn == NLines);

    for (int i = 0; i < NLines; i++)
        assert(Lines[i].index != NULL);

    qsort(Lines, NLines, sizeof(struct data), strcmp_edited);

    int WrReturn1 = StructWriter(InputFile, Lines, NLines, "w");
    assert (WrReturn1 != 0);

    qsort(Lines, NLines, sizeof(struct data), strcmp_reverse);

    int WrReturn2 = StructWriter(InputFile, Lines, NLines, "a");
    assert (WrReturn2 != 0);

    int WrReturn3 = StringWriter(InputFile, text, SizeFile, "a");
    assert (WrReturn3 != 0);

    free(text);
    free(Lines);
    return 0;
}
