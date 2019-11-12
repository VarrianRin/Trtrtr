#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define CHECKNULLPTR if(!elem || !data) { printf("NULL POINTER ERROR!"); return NULL; }
#define CHECKTREE(tree)    //if(!TreeOk(tree)) TreeDump(tree);
// функция определения объекта, TreeOk, сравнение двух объектов, список с элементами

const int ELEMSIZE = 50,
          ANSSIZE  = 20,
          CODESIZE = 100;

struct Element {

    char info[ELEMSIZE];
    Element* right;
    Element* left;
};

struct Tree {

    Element* head;
};

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

    tree->head = (Element*)calloc(1, sizeof(Element));

    tree->head->right = NULL;
    tree->head->left  = NULL;

    CHECKTREE(tree)
    return 0;
}

Element* AddRightElem(Element* elem,const char* data) {

    CHECKNULLPTR
    CHECKTREE(tree)

    Element* son = (Element*)calloc(1, sizeof(Element));

    for(int i = 0; i < ELEMSIZE && data[i]; i++)
            son->info[i] = data[i];

    elem->right = son;

    CHECKTREE(tree)
    return son;
}

Element* AddLeftElem(Element* elem,const char* data) {

    CHECKNULLPTR
    CHECKTREE(tree)

    Element* son = (Element*)calloc(1, sizeof(Element));

    for(int i = 0; i < ELEMSIZE && data[i]; i++)
            son->info[i] = data[i];

    elem->left = son;

    CHECKTREE(tree)
    return son;
}

int ChangeData(Element* elem,const char* data) {

    CHECKNULLPTR
    CHECKTREE(tree)

    for(int i = 0; i < ELEMSIZE && data[i]; i++)
            elem->info[i] = data[i];

    CHECKTREE(tree)
    return 0;
}

bool Coder(const Element* elem, FILE* TreeCode) {

    fprintf(TreeCode, "{ \"%s\" ", elem->info);

    if (elem->left)
        Coder(elem->left, TreeCode);

    if (elem->right)
        Coder(elem->right, TreeCode);

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

    fscanf(TreeCode, " %c", &symbol);
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

        fscanf(TreeCode, " \"%[^\"]\" ", data);
        if (data[0])
            for(int i = 0; i < ELEMSIZE && data[i]; i++)
                tree->head->info[i] = data[i];
    }

    Decoder(tree->head, TreeCode);

    fclose(TreeCode);
    CHECKTREE(tree)
    return 1;
}

bool AddCharacter(Element* elem) {

    if(!elem) {
        printf("\nCOULD NOT ADD CHARACTER");
        return 0;
    }
    CHECKTREE(tree)

    char answer[ELEMSIZE] = {};
    printf("Hmmm, interesting... Anyway next time I will know\n");
    printf("WHO IS YOU CHARACTER?!?\n");

    scanf(" %[^\n]", answer);
    AddLeftElem(elem, answer);

    printf("HOW %s DIFFERS FROM %s???\n", answer, elem->info);
    scanf(" %[^\n]", answer);

    AddRightElem(elem, elem->info);
    ChangeData(elem, answer);

    CHECKTREE(tree)
    return 1;
}

bool Interrogator(Element* elem) {

    printf("%s\n", elem->info);

    char answer[ANSSIZE] = {};
    scanf(" %s", answer);

    if ( !strcmp(answer, "yes") ) {
        if (elem->left)
            Interrogator(elem->left);
        else {
            printf("HAHA, OUTPLAYED! As always, you meatbag\n");
            return 1;
        }
    }

    if ( !strcmp(answer, "no") ) {
        if (elem->right)
            Interrogator(elem->right);
        else {
            AddCharacter(elem);
            return 0;
        }
    }

    else if( strcmp(answer, "no") && strcmp(answer, "yes") ) {
        printf("I don't understand you meatbag! ANSWER PROPERLY!!\n");
        Interrogator(elem);
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

        char elem1[ELEMSIZE] = {};
        char elem2[ELEMSIZE] = {};
        printf("it doesn't work yet");
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
        Interrogator(tree->head);
    }

    if ( !strcmp(answer, "no") )
        NoCase(tree);

    return 0;
}

bool Kill(Element* elem) {

    if (elem->left) {

        Kill(elem->left);

        free(elem->left);
        elem->left = NULL;
    }

    if (elem->right) {

        Kill(elem->right);

        free(elem->right);
        elem->right = NULL;
    }

    for(int i = 0; i < ELEMSIZE; i++)
        elem->info[i] = '\0';

    return 0;
}

int TreeKill(Tree* tree) {

    if(!tree) {
        printf("\nNULL POINT TO TREE!\n");
        return 0;
    }
    CHECKTREE(tree)

    Kill(tree->head);

    free(tree->head);
    tree->head = NULL;

    return 1;
}

int main() {

    Tree tree = {};
    TreeInit(&tree);
    TreeDecoder(&tree, "TreeCode.txt");

    //NodeDump(tree.head);

    Akinator(&tree);

    TreeCoder(&tree, "TreeCode.txt");

    TreeDump(&tree);

    TreeKill(&tree);

    return 0;
}

