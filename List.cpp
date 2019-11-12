#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <algorithm>

typedef int Elem_t;

const Elem_t POISON         = -1073676287;
const int    INITSIZE       = 20,
             INCREASECOEFF  = 2,
             N_ERRORS       = 5,
             SIZE_ERROR     = 1,
             FREE_ERROR     = 2,
             TAIL_ERROR     = 3,
             HEAD_ERROR     = 4,
             DATA_ERROR     = 5,
             NULL_POINT     = 6;

#define CHECKLIST(list) if(ListOk(list)) ListDump(list, __FILE__, __LINE__, __PRETTY_FUNCTION__);

struct Element {

    Elem_t   data;
    int      next;
    int      prev;
};

struct List {

    Element* Elem;
    int      head;
    int      tail;
    int      free;
    int      size;
    int      errcode;
};

int ListOk(List* list) {

    if (!list)
        return NULL_POINT;

    if (list->size < 0)
        list->errcode |= (1 << (SIZE_ERROR - 1));

    if (list->free >= list->size || list->free < 0)
        list->errcode |= (1 << (FREE_ERROR - 1));

    if (list->tail >= list->size || list->tail < 0)
        list->errcode |= (1 << (TAIL_ERROR - 1));

    if (list->head >= list->size || list->head < 0)
        list->errcode |= (1 << (HEAD_ERROR - 1));

    if (!list->Elem)
        list->errcode |= (1 << (DATA_ERROR - 1));

    else if ( !((list->errcode >> (SIZE_ERROR - 1)) & 1) & !((list->errcode >> (FREE_ERROR - 1)) & 1) ) {
        for(int i = list->free; i; i = list->Elem[i].next) {
            if (list->Elem[i].data != POISON || list->Elem[i].prev != -1) {
                list->errcode |= (1 << (DATA_ERROR - 1));
                break;
            }
        }
    }
    else if ( !((list->errcode >> (SIZE_ERROR - 1)) & 1) & !((list->errcode >> (TAIL_ERROR - 1)) & 1) & !((list->errcode >> (HEAD_ERROR - 1)) & 1) ) {
        for(int i = list->head; i; i = list->Elem[i].next) {
            if (list->Elem[i].next >= list->size || list->Elem[i].prev >= list->size || list->Elem[i].next < 0 || list->Elem[i].prev < 0) {
                list->errcode |= (1 << (DATA_ERROR - 1));
                break;
            }
        }
    }

    return list->errcode;
}

bool ErrorPrint(int errcode) {

    if (errcode < 0 || errcode > (1 << N_ERRORS) - 1)//one bit for each error
        printf("(ERRCODE_ERROR)");
    else {

        if ((errcode >> (SIZE_ERROR  - 1)) & 1)
            printf("(SIZE_ERROR) ");

        if ((errcode >> (FREE_ERROR - 1)) & 1)
            printf("(FREE_ERROR) ");

        if ((errcode >> (TAIL_ERROR - 1)) & 1)
            printf("(TAIL_ERROR) ");

        if ((errcode >> (HEAD_ERROR - 1)) & 1)
            printf("(HEAD_ERROR) ");

        if ((errcode >> (DATA_ERROR - 1)) & 1)
            printf("(DATA_ERROR) ");
        }

    return 0;
}

bool DotWriter(const List* list) {

    FILE* DotListDump = fopen("DotListDump.txt", "wb");
    if (!DotListDump) {
        printf("\n COULD NOT OPEN DUMP FILE\n");
        return 1;
    }

    fprintf(DotListDump, "digraph ListDump {\n"
                         "\"Elem[0] \\n data: %d \\n next: %d\\n prev: %d\"", list->Elem[0].data, list->Elem[0].next, list->Elem[0].prev);

    for(int ph = list->head; ph; ph = list->Elem[ph].next) {
        fprintf(DotListDump, "->\"Elem[%d] \\n data: %d \\n next: %d\\n prev: %d\"", ph, list->Elem[ph].data, list->Elem[ph].next, list->Elem[ph].prev);
        }
    fprintf(DotListDump, "\n");

    fprintf(DotListDump, "FREE");

    for(int ph = list->free; ph; ph = list->Elem[ph].next) {
        fprintf(DotListDump, "->\"Elem[%d] \\n data: %d \\n next: %d\\n prev: %d\"", ph, list->Elem[ph].data, list->Elem[ph].next, list->Elem[ph].prev);
        }

    fprintf(DotListDump, "\n}");

    fclose(DotListDump);

    system("dot -Tjpg -o F:\\ListDump.jpg F:\\proga\\List\\DotListDump.txt");
    system("F:\\ListDump.jpg");
    return 0;
}

bool ListDump(const List* list, const char* file, int line, const char* function) {

    if (!list) {
        printf("\n ERROR: NULL POINTER TO LIST! \n");
        return 1;
    }

    printf("\n LIST DUMP: File: %s, line: %d, function: %s \n", file, line, function);

    printf("\n List [%p] ", list);
    (list->errcode == 0) ? printf("(ok)\n\n") : printf("(ERROR)\n\n");

    for(int i = 0; i < list->size; i++)
        printf("[%2d] : %4d %3d %3d\n", i, list->Elem[i].data, list->Elem[i].next, list->Elem[i].prev);

    printf("\nhead = %d\n"
           "tail = %d\n"
           "free = %d\n"
           "size = %d\n"
           "errcode = %d " , list->head, list->tail, list->free, list->size, list->errcode);

    ErrorPrint(list->errcode);
    printf("\n");

    DotWriter(list);

    return 0;
}

int ListIncrease(List* list) {

    CHECKLIST(list)

    Element* reserve = (Element*)realloc(list->Elem, sizeof(Element) * (list->size *= INCREASECOEFF));
    list->Elem = (reserve != NULL) ? reserve : list->Elem;

    if (!reserve) {
        list->size /= INCREASECOEFF;
        printf("Not able to increase list");
        return 1;
    }

    int ph = 0;
    if (list->free) {
        for(ph = list->free; list->Elem[ph].next; ph = list->Elem[ph].next);
        list->Elem[ph].next = list->size / INCREASECOEFF;
    }
    else
        list->free = list->size / INCREASECOEFF;

    for(ph = list->size / INCREASECOEFF ; ph < list->size; ph++) {
         list->Elem[ph].data = POISON;
         list->Elem[ph].next = ((ph + 1) + list->size) % list->size;
         list->Elem[ph].prev = -1;
    }

    CHECKLIST(list);
    return 0;
}

int ListDecrease(List* list) {

    CHECKLIST(list)

    Element* reserve = (Element*)realloc(list->Elem, list->size /= INCREASECOEFF);
    list->Elem = (reserve != NULL) ? reserve : list->Elem;

    if (!reserve) {
        list->size *= INCREASECOEFF;
        printf("Not able to decrease list");
        return 1;
    }

    CHECKLIST(list);
    return 0;
}

bool ListSort(List* list) {

    CHECKLIST(list)

    if (!list->head)
        return 1;

    Element* reserve = (Element*)calloc(list->size, sizeof(Element));
    if (!reserve)
        return 1;

    reserve[0].data = POISON;

    int ph = 1;

    for( int i = list->head; i; ph++, i = list->Elem[i].next) {
        reserve[ph].data = list->Elem[i].data;
        reserve[ph].next = ((ph + 1) + list->size) % list->size;
        reserve[ph].prev = ((ph - 1) + list->size) % list->size;
    }

    reserve[ph - 1].next = 0;
    list->head = 1;
    list->tail = ph - 1;
    list->free = ph;

    for( ; ph < list->size; ph++) {
         reserve[ph].data = POISON;
         reserve[ph].next = ((ph + 1) + list->size) % list->size;
         reserve[ph].prev = -1;
    }
    free(list->Elem);
    list->Elem = reserve;

    CHECKLIST(list)
    return 0;
}

bool ListInit(List* list) {

    assert(list != NULL);

    list->size    = INITSIZE;
    list->head    = 0;
    list->free    = 1;
    list->tail    = 0;
    list->errcode = 0;

    list->Elem = (Element*)calloc(list->size, sizeof(Element));

    if (!list->Elem) {
        printf("COULD NOT INIT LIST");
        return 1;
    }

    for(int i = 1; i < list->size; i++) {      //func
         list->Elem[i].data = POISON;
         list->Elem[i].next = ((i + 1) + list->size) % list->size;
         list->Elem[i].prev = -1;
    }

    list->Elem[0].data = POISON;
    list->Elem[0].next = 0;
    list->Elem[0].prev = 0;

    CHECKLIST(list);
    return 0;
}

int ValueSearch(List* list, Elem_t value) {//returns physical number

    CHECKLIST(list);

    int pn = 1;

    for( ; pn < list->size; pn++)
        if (list->Elem[pn].data == value)
            break;

    CHECKLIST(list);
    return (pn >= list->size) ? -1 : pn;
    }

int NumberSearch(List* list, int number) {//logical number, returns physical number

    CHECKLIST(list);

    int pn = list->head;

    for(int counter = 1; counter != number && pn != 0; pn = list->Elem[pn].next, counter++);

    CHECKLIST(list);
    return pn;
}

int PushPrev(List* list, Elem_t value, int number) {//physical number

    CHECKLIST(list);

    if (!list->free)
        ListIncrease(list);
                                                                   //PushNext
    int exfree = list->free;
    list->free = list->Elem[list->free].next;

    list->head = (number == list->head) ? exfree : list->head;

    list->Elem[list->Elem[number].prev].next = (list->Elem[number].prev) ? exfree : 0;

    list->Elem[exfree].data = value;
    list->Elem[exfree].prev = list->Elem[number].prev;
    list->Elem[exfree].next = number;
    list->Elem[number].prev = exfree;

    CHECKLIST(list);
    return exfree;
}

int PushNext(List* list, Elem_t value, int number) {//physical number

    CHECKLIST(list);

    if (!list->free)
        ListIncrease(list);

    int exfree = list->free;
    list->free = list->Elem[list->free].next;

    list->tail = (list->tail == number) ? exfree : list->tail;

    list->Elem[list->Elem[number].next].prev = (list->Elem[number].next) ? exfree : 0;

    list->Elem[exfree].data = value;
    list->Elem[exfree].next = list->Elem[number].next;
    list->Elem[exfree].prev = number;
    list->Elem[number].next = exfree;

    CHECKLIST(list);
    return exfree;
}

int PushLast(List* list, Elem_t value) {//physical number

    CHECKLIST(list);

    if (!list->free)
        ListIncrease(list);

    int exfree = list->free;
    list->free = list->Elem[list->free].next;

    list->head = (list->tail == 0) ? exfree : list->head;

    list->Elem[exfree].data     = value;
    list->Elem[list->tail].next = (list->tail == 0) ? 0 : exfree;
    list->Elem[exfree].prev     = list->tail;
    list->tail                  = exfree;
    list->Elem[exfree].next     = 0;

    CHECKLIST(list);
    return exfree;
}

int PushFirst(List* list, Elem_t value) {//physical number

    CHECKLIST(list);

    if (!list->free)
        ListIncrease(list);

    int exfree = list->free;
    list->free = list->Elem[list->free].next;

    list->tail = (list->head == 0) ? exfree : list->tail;

    list->Elem[exfree].data     = value;
    list->Elem[list->head].prev = (list->head == 0) ? 0 : exfree;
    list->Elem[exfree].next     = list->head;
    list->head                  = exfree;
    list->Elem[exfree].prev     = 0;

    CHECKLIST(list);
    return exfree;
}

/*int ListPush(List* list, Elem_t value, const char mode, Elem_t num = 0) {//mode = (f - first, l - last, p - prev, n - next)

    CHECKLIST(list);

    if (!list->free)
        ListIncrease(list);


    if (mode == 'f')
        return PushFirst(list, value);

    if (mode == 'l')
        return PushLast(list, value);

    int ph = ValueSearch(list, num);

    if (ph == -1) {
        printf("\nELEMENT TO PUSH IS NOT FOUND\n");
        return 0;
    }

    if (mode == 'p')
        return (ph == list->head) ? PushFirst(list, value) : PushPrev(list, value, ph);

    if (mode == 'n')
        return (ph == list->tail) ? PushLast(list, value) : PushNext(list, value, ph);


    printf("UNKNOWN MODE");
    CHECKLIST(list);
    return 0;
} */

int ListDeletePH(List* list, int number) {//physical number

    CHECKLIST(list);

    if (list->Elem[number].prev == -1 || !number)
        return 0;

    if (number == list->tail) {
        list->Elem[list->Elem[number].prev].next = list->Elem[number].next;
        list->tail = list->Elem[number].prev;
    }
    else if (number == list->head) {
        list->Elem[list->Elem[number].next].prev = list->Elem[number].prev;
        list->head = list->Elem[number].next;
    }
    else {
        list->Elem[list->Elem[number].prev].next = list->Elem[number].next;
        list->Elem[list->Elem[number].next].prev = list->Elem[number].prev;
    }

    list->Elem[number].data = POISON;
    list->Elem[number].next = list->free;
    list->Elem[number].prev = -1;

    list->free = number;

    CHECKLIST(list);
    return number;
}

bool ListKill(List* list) {

    CHECKLIST(list);

    free(list->Elem);
    list->size    = 0;
    list->head    = 0;
    list->free    = 0;
    list->tail    = 0;
    list->errcode = 0;
    list->Elem    = NULL;

    return 0;
}

int main() {

    List list = {};

    ListInit(&list);

    PushLast(&list, 1);
    for(int i = 1, sign = 1; i < 20; i++, sign *= -1)
        PushLast(&list, i * 10  * sign);

    ListDump(&list, __FILE__, __LINE__, __PRETTY_FUNCTION__);

    ListKill(list);

    return 0;
}

