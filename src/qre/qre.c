#include "qre.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct {
  const char* str;
} qre_str_node;

typedef struct {
  qre_type_t type;
  qre_node* children;
  size_t size;
} qre_parent_node;

struct qre_node {
  qre_type_t type;
  union {
    qre_str_node str;
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

void qre_dump(qre_node* node) {
}

void qre_free(qre_t* qre) {
  free(qre);
}

qre_node* qre_node_new_children(qre_parser_ctx* ctx, qre_type_t type) {
    assert(ctx);
    assert(ctx->qre);
    if (ctx->qre->arena->idx == ARENA_SIZE) {
        arena_t* arena = malloc(sizeof(arena_t));
        if (!arena) {
            fprintf(stderr, "[qre] Cannot allocate memory\n");
            abort();
        }
        memset(arena, 0, sizeof(arena_t));
        arena->next = ctx->qre->arena;
        ctx->qre->arena = arena;
    }
    qre_node* node = &(ctx->qre->arena->nodes[ctx->qre->arena->idx++]);
    memset(node, 0, sizeof(qre_node));
    node->type = type;
    return node;
}

qre_node* qre_node_push_child(qre_node* parent, qre_node* child) {
    parent->children.size++;
    parent->children.nodes = realloc(parent->children.nodes, sizeof(qre_node*)*parent->children.size);
    parent->children.nodes[parent->children.size-1] = child;
    return parent;
}

