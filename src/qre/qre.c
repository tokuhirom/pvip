#include "qre.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>


typedef struct {
  qre_type_t type;
  qre_node* children;
  size_t size;
} qre_parent_node;

struct qre_node {
  qre_type_t type;
  union {
    struct {
        const char* ptr;
        size_t len;
    } str;
    struct {
        qre_node** nodes;
        size_t size;
    } children;
  };
};

#define ARENA_SIZE 256

typedef struct arena_t {
  struct arena_t* next;
  struct qre_node nodes[ARENA_SIZE];
  int idx;
} arena_t;

struct qre_t {
  struct arena_t* arena;
};

qre_t* qre_new() {
  qre_t* qre = malloc(sizeof(qre_t));
  if (!qre) {
    fprintf(stderr, "[qre] Cannot allocate memory\n");
    abort();
  }
  memset(qre, 0, sizeof(qre_t));
  qre->arena = malloc(sizeof(arena_t));
  if (!qre->arena) {
    fprintf(stderr, "[qre] Cannot allocate memory\n");
    abort();
  }
  memset(qre->arena, 0, sizeof(arena_t));
  return qre;
}

static void dump(qre_node* node, int depth) {
    switch (node->type) {
        case QRE_NODE_ELEMS: {
            printf("(elems ");
            int i;
            for (i=0; i<node->children.size; ++i) {
                dump(node->children.nodes[i], depth+1);
            }
            break;
        }
        case QRE_NODE_STRING:
            printf("(string \"");
            fwrite(node->str.ptr, 1, node->str.len, stdout);
            printf("\")");
            break;
        case QRE_NODE_NOP:
            printf("(nop)");
            break;
    }
    printf(")");
}

void qre_dump(qre_node* node) {
    dump(node, 0);
}

void qre_free(qre_t* qre) {
    arena_t *arena = qre->arena;
    while (arena) {
        int i;
        for (i=0; i<arena->idx; ++i) {
            qre_node* node = &(arena->nodes[i]);
            switch (node->type) {
            case QRE_NODE_STRING:
                free(node->str.ptr);
                break;
            case QRE_NODE_ELEMS:
                free(node->children.nodes);
                break;
            }
        }
        arena_t* next = arena->next;
        free(arena);
        arena = next;
    }
    free(qre);
}

static qre_node* qre_node_alloc(qre_t* qre) {
    if (qre->arena->idx == ARENA_SIZE) {
        arena_t* arena = malloc(sizeof(arena_t));
        if (!arena) {
            fprintf(stderr, "[qre] Cannot allocate memory\n");
            abort();
        }
        memset(arena, 0, sizeof(arena_t));
        arena->next = qre->arena;
        qre->arena = arena;
    }
    qre_node* node = &(qre->arena->nodes[qre->arena->idx++]);
    memset(node, 0, sizeof(qre_node));
    return node;
}

qre_node* qre_node_new_children(qre_parser_ctx* ctx, qre_type_t type) {
    assert(ctx);
    assert(ctx->qre);
    qre_node* node = qre_node_alloc(ctx->qre);
    node->type = type;
    return node;
}

qre_node* qre_node_push_child(qre_node* parent, qre_node* child) {
    parent->children.size++;
    parent->children.nodes = realloc(parent->children.nodes, sizeof(qre_node*)*parent->children.size);
    parent->children.nodes[parent->children.size-1] = child;
    return parent;
}

qre_node* qre_node_new_str(qre_parser_ctx* ctx, qre_type_t type, const char* s, size_t l) {
    qre_node* node = qre_node_alloc(ctx->qre);
    node->type = type;
    node->str.ptr = malloc(l);
    if (!node->str.ptr) {
        fprintf(stderr, "[qre] Cannot allocate memory\n");
        abort();
    }
    memcpy(node->str.ptr, s, l);
    node->str.len = l;
    return node;
}

