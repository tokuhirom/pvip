%{

#include "qre.h"
#include <assert.h>

#define YYSTYPE qre_node*
#define YY_NAME(n) qre_parser_##n
#define YY_XTYPE qre_parser_ctx
#define PARSER (&(G->data))

#define NOP() qre_node_new_children(&(G->data), QRE_NODE_NOP)

static inline char qre_input(char *buf, YY_XTYPE D) {
    if (D.str->len == D.str->pos) {
        return 0;
    } else {
        *buf = D.str->buf[D.str->pos];
        D.str->pos = D.str->pos+1;
        return 1;
    }
}

#define YY_INPUT(buf, result, max_size, D)		\
    result = qre_input(buf, D);

/* Append string to current string literal */
#define APPEND_S(s, l) \
    G->data.literal_str=PVIP_node_append_string(PARSER, G->data.literal_str, (s), (l))

/* Append node to current string literal */
#define APPEND_N(e) \
    G->data.literal_str=PVIP_node_append_string_node(PARSER, G->data.literal_str, e)


%}

comp_init = - e:elems - end-of-file {
    $$ = (G->data.root = e);
}

elems = (
        e1:expr {
            $$ = qre_node_new_children(&(G->data), QRE_NODE_ELEMS);
            qre_node_push_child($$, e1);
            e1 = $$;
        }
        (
            - e2:expr {
                qre_node_push_child(e1, e2);
                $$=e1;
            }
        )* {
            if (e1->children.size == 1) {
                $$ = e1->children.nodes[0];
            }
        }
    )
    | ws* { $$=NOP(); }

expr = e1:elem {
        $$ = qre_node_new_children(&(G->data), QRE_NODE_OR);
        qre_node_push_child($$, e1);
        e1 = $$;
    } (
        - '||' - e2:elem {
            qre_node_push_child(e1, e2);
            $$ = e1;
        }
    )* {
        if (e1->children.size == 1) {
            $$ = e1->children.nodes[0];
        }
    }

elem =
    '[' - e:elems - ']' { $$ = e; }
    | "(" - e:elems - ")" {
        $$ = qre_node_new_children(&(G->data), QRE_NODE_CAPTURE);
        qre_node_push_child($$, e);
    }
    | <[A-Za-z0-9]+> {
        $$ = qre_node_new_str(&(G->data), QRE_NODE_STRING, yytext, yyleng);
    }

- = ws*

esc = '\\'

ws = 
    '\n'
    | ' ' | '\f' | '\v' | '\t' | '\205' | '\240'

end-of-file = !'\0'

%%

qre_node* qre_parse(qre_t* qre, const char *string, int len, int debug, PVIPString **error) {
    qre_node *root = NULL;

    GREG g;
    YY_NAME(init)(&g);

#ifdef YY_DEBUG
    g.debug=debug;
#endif

    g.data.str = malloc(sizeof(PVIPParserStringState));
    g.data.str->buf     = string;
    g.data.str->len     = len;
    g.data.str->pos     = 0;
    g.data.qre = qre;

    if (!YY_NAME(parse)(&g)) {
      if (error) {
        *error = PVIP_string_new();
        PVIP_string_concat(*error, "** Syntax error in regexp**\n", strlen("** Syntax error in regexp**\n"));
        free(g.data.str);
      }
      goto finished;
    }
    if (g.limit!=g.pos) {
        if (error) {
            *error = PVIP_string_new();
            PVIP_string_concat(*error, "Syntax error! Around:\n", strlen("Syntax error! Around:\n"));
            int i;
            for (i=0; g.limit!=g.pos && i<24; i++) {
                char ch = g.data.str->buf[g.pos++];
                if (ch) {
                    PVIP_string_concat_char(*error, ch);
                }
            }
            PVIP_string_concat_char(*error, '\n');
        }
        goto finished;
    }
    root = g.data.root;

finished:

    free(g.data.str);
    assert(g.data.root);
    YY_NAME(deinit)(&g);
    return root;
}


/*
 TODO: [ paren ]
 TODO: ( paren )
 TODO: ( pa || ren )
 TODO: <[ a b c ]>
 TODO: # comment
*/

