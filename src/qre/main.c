#include "qre.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>

static void usage() {
    printf("Usage: qre regex\n");
    exit(0);
}

static void run_eval(const char* src, size_t len) {
    qre_t* qre = qre_new();
    PVIPString* error;
    qre_node* node = qre_parse(qre, src, len, true, &error);
    if (!node) {
        PVIP_string_say(error);
        exit(1);
    }
    qre_dump(node);
    printf("\n");
    qre_free(qre);
}

int main(int argc, char **argv) {
    const char* eval = NULL;

    int optind=1;
    for (;optind< argc; ++optind) {
        if (*(argv[optind]) == '-') {
            switch (*(argv[optind] + 1)) {
            case 'e':
                optind++;
                if (optind == argc) {
                    abort();
                }
                eval = argv[optind];
                break;
            }
        } else {
            break;
        }
    }

    if (eval) {
        run_eval(eval, strlen(eval));
    } else if (optind == argc) {
        usage();
    } else if (optind < argc) {
        const char* filename = argv[optind];
        struct stat s;
        if (stat(filename, &s) != 0) {
            fprintf(stderr, "There is no %s", filename);
            return 1;
        }

        FILE* fp = fopen(filename, "r");
        if (!fp) {
            fprintf(stderr, "Cannot open file %s", filename);
            return 1;
        }

        char* buf = malloc(s.st_size+1);
        if (!buf) { abort(); }

        if (fread(buf, 1, s.st_size, fp) != s.st_size) {
            abort();
        }

        fclose(fp);

        buf[s.st_size] = 0; // 為念

        run_eval(buf, s.st_size);

        free(buf);
    }
    return 0;
}

