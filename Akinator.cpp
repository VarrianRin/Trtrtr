#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/*
проверка на циклы
цвета
ненулевую инфу
nill
  */

#define CHECKNULLPTR     if(!elem || !data) { printf("NULL POINTER ERROR!"); return NULL; }
#define CHECKTREE(tree)  if(TreeOk(tree)) TreeDump(tree);

const int ELEMSIZE   = 50,
          ANSSIZE    = 20,
          CODESIZE   = 100,
          NULL_POINT = 1,
          HEAD_ERROR = 2;

struct Element {

    char*    info;
    Element* right;
    Element* left;
};

struct Tree {

    Element* head;
};

int TreeOk(const Tree* tree) {

    if (!tree)
        return NULL_POINT;

    if (!tree->head)
        return HEAD_ERROR;

    return 0;
}

bool ElemDump(const Element* elem, FILE* TreeDump) {

    fprintf(TreeDump, "\"data: %s\\nleft: %p\\nright: %p\"", elem->info, elem->left, elem->right);

    if (elem->right) {
        fprintf(TreeDump, "->");
        ElemDump(elem->right, TreeDump);
    }

    fprintf(TreeDump, "\n\"data: %s\\nleft: %p\\nright: %p\"", elem->info, elem->left, elem->right);

    if (elem->left) {
        fprintf(TreeDump, "->");
        ElemDump(elem->left, TreeDump);
    }

    return 0;
}

int TreeDump(const Tree* tree) {

    if (!tree) {
        printf("NULL POINTER TO TREE");
        return 0;
    }

    FILE* TreeDump = fopen("TreeDump.txt", "w");
    if (!TreeDump) {
        printf("\n COULD NOT OPEN DUMP FILE\n");
        return 0;
    }

    fprintf(TreeDump, "digraph TreeDump {\n");

    ElemDump(tree->head, TreeDump);

    fprintf(TreeDump, "\n}");

    fclose(TreeDump);

    system("dot -Tjpg -o F:\\proga\\Akinator\\TreeDump.jpg F:\\proga\\Akinator\\TreeDump.txt");
    system("F:\\proga\\Akinator\\TreeDump.jpg");
    return 1;
}

int NodeDump(const Element* elem) {

    if (!elem) {
        printf("NULL POINTER TO elem");
        return 1;
    }

    printf("\n%s\n%p\n%p", elem->info, elem->left, elem->right);

    if (elem->left)
        NodeDump(elem->left);

    if (elem->right)
        NodeDump(elem->right);

    return 0;
}

int TreeInit(Tree* tree) {

    if (!tree) {
        printf("COULD NOT INIT TREE");
        return 1;
    }

    Element* reserve = (Element*)calloc(1, sizeof(Element));
    if (!reserve) {
        printf("COULD NOT CALLOC HEAD");
        return 1;
    }

    tree->head = reserve;

    tree->head->right = NULL;
    tree->head->left  = NULL;

    tree->head->info  = (char*)calloc(1, sizeof(char));

    CHECKTREE(tree)
    return 0;
}

Element* AddRightElem(Element* elem, const char* data) {

    CHECKNULLPTR

    Element* son = (Element*)calloc(1, sizeof(Element));

    son->info = (char*)calloc(strlen(data) + 1, sizeof(char));
    assert(son->info);

    strcpy(son->info, data);

    elem->right = son;

    return son;
}

Element* AddLeftElem(Element* elem, const char* data) {

    CHECKNULLPTR

    Element* son = (Element*)calloc(1, sizeof(Element));

    son->info = (char*)calloc(strlen(data) + 1, sizeof(char));
    assert(son->info);

    strcpy(son->info, data);

    elem->left = son;

    return son;
}

int ChangeData(Element* elem, const char* data) {

    CHECKNULLPTR

    elem->info = (char*)realloc(elem->info, (strlen(data) + 1) * sizeof(char));
    assert(elem->info);

    strcpy(elem->info, data);

    return 0;
}

bool Coder(const Element* elem, FILE* TreeCode) {

    fprintf(TreeCode, "{ \"%s\" ", elem->info);

    if (elem->left)
        Coder(elem->left, TreeCode);
    else if (!elem->left && elem->right)
        fprintf(TreeCode, "nill ");

    if (elem->right)
        Coder(elem->right, TreeCode);
    else if (!elem->right && elem->left)
        fprintf(TreeCode, "nill ");

    fprintf(TreeCode, "}");

    return 0;
}

int TreeCoder(const Tree* tree, const char* FileName) {

    if(!tree || !FileName) {
        printf("NULL POINTER ERROR!");
        return 0;
    }
    CHECKTREE(tree)

    FILE* TreeCode = fopen("TreeCode.txt", "w");
    if (!TreeCode) {
        printf("COULD NOT OPEN FILE");
        return 0;
    }

    Coder(tree->head, TreeCode);

    fclose(TreeCode);
    CHECKTREE(tree)
    return 1;
}

bool Decoder(Element* elem, FILE* TreeCode) {

    char symbol = 0;
    char data[ELEMSIZE] = {};
    Element* root = NULL;
    fscanf(TreeCode, " %c", &symbol);

    if (symbol == '{') {

        fscanf(TreeCode, " \"%[^\"]\"", data);

        if (data[0])
            root = AddLeftElem(elem, data);

        Decoder(root, TreeCode);
    }
    else if (symbol == '}')
        return 0;

    fscanf(TreeCode, " %c", &symbol);
    if (symbol == '{') {

        fscanf(TreeCode, " \"%[^\"]\" ", data);
        if (data[0])
            root = AddRightElem(elem, data);

        Decoder(root, TreeCode);
    }

    fscanf(TreeCode, " }");
    return 0;
}

int TreeDecoder(Tree* tree, const char* FileName) {

    if (!FileName) {
        printf("INVALID NAME OF FILE");
        return 0;
    }
    CHECKTREE(tree)

    FILE* TreeCode = fopen(FileName, "r");
    if (!TreeCode) {
        printf("COULD NOT OPEN FILE");
        return 0;
    }

    char symbol = 0;
    char data[ELEMSIZE] = {};

    fscanf(TreeCode, " %c", &symbol);
    if (symbol == '{') {

        fscanf(TreeCode, " \"%[^\"]\"", data);

        if (data[0])
            ChangeData(tree->head, data);
    }

    Decoder(tree->head, TreeCode);

    fclose(TreeCode);
    CHECKTREE(tree)
    return 1;
}

int Search(const Element* elem, const char* data) {

    CHECKNULLPTR

    if ( !strcmp(elem->info, data) )
        return 2;

    int ret = 0;

    if (elem->left && (ret = Search(elem->left, data)) != 0)
        return 10 * ret + 1;

    if (elem->right && (ret = Search(elem->right, data)) != 0)
        return 10 * ret;

    return 0;
}

Element* PathDecoder(Element* node, int* path1, int* path2 = nullptr) {

    if (!path2) {

        for( ; *path1 > 10; *path1 /= 10) {

            if (*path1 % 10) {
                printf(" is %s", node->info);
                if (*path1 >= 100) printf(", ");
                node = node->left;
            }
            else {
                printf(" is not %s", node->info);
                if (*path1 >= 100) printf(", ");
                node = node->right;
            }
        }
    }

    else {

        for( ; *path1 > 10, *path2 > 10, *path1 % 10 == *path2 % 10; *path1 /= 10, *path2 /= 10) {

            if (*path1 % 10) {
                printf("%s", node->info);
                if (*path1 % 1000 == *path2 % 1000) printf(", ");
                else if (*path1 % 100 == *path2 % 100) printf(" and ");
                node = node->left;
            }
            else {
                printf("not %s", node->info);
                if (*path1 % 1000 == *path2 % 1000) printf(", ");
                else if (*path1 % 100 == *path2 % 100) printf(" and ");
                node = node->right;
            }
        }
    }

    return node;
}

bool MistakeCase(const char* data, Tree* tree) {

    int path = Search(tree->head, data);

    if (!path)
        return 0;

    printf("NO! I KNOW %s!"
           " Object %s", data, data);

    Element* node = tree->head;

    PathDecoder(node, &path);

    return 1;
}

bool AddCharacter(Element* elem, Tree* tree) {

    char answer[ELEMSIZE] = {};

    printf("Hmmm, interesting... Anyway next time I will know\n");
    printf("WHO IS YOU CHARACTER?!?\n");

    scanf(" %30[^\n]", answer);
    fflush(stdin);

    if (MistakeCase(answer, tree))
        return 0;

    AddLeftElem(elem, answer);

    printf("HOW %s DIFFERS FROM %s???\n", answer, elem->info);
    scanf(" %30[^\n]", answer);

    fflush(stdin);

    AddRightElem(elem, elem->info);
    ChangeData(elem, answer);

    return 1;
}

bool Interrogator(Element* elem, Tree* tree) {

    printf("%s?\n", elem->info);

    char answer[ANSSIZE] = {};
    scanf(" %5s", answer);

    if ( !strcmp(answer, "yes") ) {
        if (elem->left)
            Interrogator(elem->left, tree);
        else {
            printf("HAHA, OUTPLAYED! As always, you meatbag\n");
            return 1;
        }
    }

    if ( !strcmp(answer, "no") ) {
        if (elem->right)
            Interrogator(elem->right, tree);
        else {
            AddCharacter(elem, tree);
            return 0;
        }
    }

    else if( strcmp(answer, "no") && strcmp(answer, "yes") ) {
        fflush(stdin);
        printf("I don't understand you meatbag! ANSWER PROPERLY!!\n");
        Interrogator(elem, tree);
    }
    return 1;
}

int Compare(const char* obj1, const char* obj2, const Tree* tree) {

    if (!obj1 || !obj2) {
        printf("\nNULL POINTERS TO OBJECT\n");
        return -1;
    }

    int path1     = Search(tree->head, obj1);
    int path2     = Search(tree->head, obj2);
    Element* node = tree->head;

    if (!path1 || !path2)
        return 0;

    if (!((path1 % 10) ^ (path2 % 10))) {

        printf("%s and %s are both ", obj1, obj2);

        node = PathDecoder(node, &path1, &path2);

        printf("\nBut %s", obj1);
        PathDecoder(node, &path1);

        printf("\nAnd %s", obj2);
        PathDecoder(node, &path2);
    }
    else {

        printf("%s", obj1);
        PathDecoder(node, &path1);

        printf("\nAnd %s", obj2);
        PathDecoder(node, &path2);
    }

    return 1;
}

bool NoCase(const Tree* tree) {

    char answer[ANSSIZE];

    printf("So, what do you want?\n"
           "1 TreeDump\n"
           "2 Compare objects\n"
           "3 Leave\n");

    scanf(" %s", answer);

    if ( !strcmp(answer, "TreeDump") || !strcmp(answer, "1") )
        TreeDump(tree);

    else if ( !strcmp(answer, "Compare") || !strcmp(answer, "2") ) {

        char obj1[ELEMSIZE] = {};
        char obj2[ELEMSIZE] = {};

        scanf(" %s %s", obj1, obj2);
        Compare(obj1, obj2, tree);
    }

    else if ( !strcmp(answer, "Leave") || !strcmp(answer, "3") )
        return 1;

    return 1;
}

int Akinator(Tree* tree) {

    if (!tree) {
        printf("\nI DON'T WANT TO PLAY WITH YOU!\n");
        return 1;
    }
    CHECKTREE(tree)

    printf("You want to play with me?\n");

    char answer[ANSSIZE] = {};

    scanf(" %s", answer);

    if ( !strcmp(answer, "yes") ) {
        printf("Okay, meatbag, let's begin\n\n");
        Interrogator(tree->head, tree);
    }

    if ( !strcmp(answer, "no") )
        NoCase(tree);

    return 0;
}

bool KillElem(Element* elem) {

    if (elem->left) {

        KillElem(elem->left);

        free(elem->left);
        elem->left = NULL;
    }

    if (elem->right) {

        KillElem(elem->right);

        free(elem->right);
        elem->right = NULL;
    }

    if (elem->info) {

        free(elem->info);
        elem->info = NULL;
    }

    return 0;
}

int TreeKill(Tree* tree) {

    if(!tree) {
        printf("\nNULL POINT TO TREE!\n");
        return 0;
    }
    CHECKTREE(tree)

    KillElem(tree->head);

    free(tree->head);
    tree->head = NULL;

    return 1;
}

int main() {

    Tree tree = {};
    TreeInit(&tree);
    TreeDecoder(&tree, "TreeCode.txt");

    Akinator(&tree);

    TreeCoder(&tree, "TreeCode.txt");

    TreeKill(&tree);

    return 0;
}

