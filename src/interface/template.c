#include <stdio.h>
#include <stdint.h>

/**
 * @brief Internal function interface struct
 */
struct InternalOps {
    void (*drawPoint)(int x, int y, uint32_t color, void *object);
};

static struct InternalOps internalOps;
static void *parent;

void Interface_Init(struct InternalOps ops, void *object) {
    internalOps = ops;
    parent = object;
}

/* BEGIN */



/* END */

struct InterfaceParam *Interface_Params() {
    return params;
}

int Interface_ParamsNum() {
    return sizeof(params) / sizeof(params[0]);
}
