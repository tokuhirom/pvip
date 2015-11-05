#ifndef QRE_H_
#define QRE_H_

#include <stdio.h>
#include "../pvip.h"

struct qre_t;
struct qre_node;
typedef struct qre_t qre_t;
typedef struct qre_node qre_node;

typedef enum qre_type_t {
  QRE_NODE_ELEMS,
  QRE_NODE_STRING,
  QRE_NODE_NOP
} qre_type_t;

typedef struct {
    const char  *buf;
    size_t len;
    size_t pos;
} PVIPParserStringState;

typedef struct {
  PVIPParserStringState* str;
  qre_node *root;
  qre_t* qre;
} qre_parser_ctx;

qre_t* qre_new();
qre_node* qre_parse(qre_t* qre, const char *string, int len, int debug, PVIPString **error);
void qre_dump(qre_node*);
void qre_free(qre_t*);
qre_node* qre_node_push_child(qre_node* parent, qre_node* child);
qre_node* qre_node_new_children(qre_parser_ctx* ctx, qre_type_t type);

#endif // QRE_H_
