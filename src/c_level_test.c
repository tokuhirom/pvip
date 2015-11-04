#include "pvip.h"
#include "string.h"

int C;

int main() {
    C=0;
    printf("1..2\n");

    pvip_t* pvip = pvip_new();

    {
        PVIPString * x = PVIP_string_new();
        PVIP_string_printf(x, "%s%d", "hoge", 2);

        if (x->len==5 && !memcmp(x->buf, "hoge2", 5)) {
            printf("ok 1\n");
        } else {
            printf("not ok 1\n");
        }

        PVIP_string_destroy(x);
    }

    {
        PVIPNode *node = PVIP_parse_string(pvip, "\n", 1, 0, NULL);
        if (node) {
            printf("ok 2\n");
        } else {
            printf("not ok 2\n");
        }
    }

    pvip_free(pvip);

    return 0;
}
