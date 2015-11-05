#include "qre.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc==1) {
        printf("Usage: qre regex\n");
        exit(0);
    }
    qre_t* qre = qre_new();
    PVIPString* error;
    qre_node* node = qre_parse(qre, argv[1], strlen(argv[1]), true, &error);
    qre_dump(node);
    printf("\n");
    qre_free(qre);
    return 0;
}

