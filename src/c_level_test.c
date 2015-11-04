#include "pvip.h"
#include "string.h"

int C;

int main() {
    C=0;
    printf("1..3\n");

    pvip_t* pvip = pvip_new();

    {
        PVIPString * x = PVIP_string_new();
        PVIP_string_printf(x, "%d", 1);
        if (x->len==1 && memcmp(x->buf, "1", 1)==0) {
            printf("ok 1\n");
        } else {
            printf("not ok 1\n");
            printf("# len: %ld\n", x->len);
            printf("# char[0]: %x\n", x->buf[0]);
        }

        PVIP_string_printf(x, "%s%d", "hoge", 2);
        if (x->len==6 && !memcmp(x->buf, "1hoge2", 6)) {
            printf("ok 2\n");
        } else {
            printf("not ok 2\n");
        }

        PVIP_string_destroy(x);
    }

    {
        PVIPNode *node = PVIP_parse_string(pvip, "\n", 1, 0, NULL);
        if (node) {
            printf("ok 3\n");
        } else {
            printf("not ok 3\n");
        }
    }

    pvip_free(pvip);

    return 0;
}
