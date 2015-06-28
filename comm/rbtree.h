/*
 * This file is modified from nginx rbtree.
 * Therefore it is under nginx BSD like license.
 */

/* Copyright (C) 2002-2015 Igor Sysoev
 * Copyright (C) 2011-2015 Nginx, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __JOR_RBTREE_H__
#define __JOR_RBTREE_H__


#include <stdint.h>

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) ({                    \
      const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})


typedef struct jor_rbtree_node_s  jor_rbtree_node_t;
struct jor_rbtree_node_s {
  uint32_t key;
  jor_rbtree_node_t *left;
  jor_rbtree_node_t *right;
  jor_rbtree_node_t *parent;
  char color;
};


typedef struct jor_rbtree_s  jor_rbtree_t;

typedef void (*jor_rbtree_insert_pt) (jor_rbtree_node_t *root,
    jor_rbtree_node_t *node, jor_rbtree_node_t *sentinel);

struct jor_rbtree_s {
  jor_rbtree_node_t *root;
  jor_rbtree_node_t *sentinel;
  jor_rbtree_insert_pt insert;
};

#define jor_rbtree_init(tree, s, i)                                         \
  jor_rbtree_sentinel_init(s);                                              \
  (tree)->root = s;                                                         \
  (tree)->sentinel = s;                                                     \
  (tree)->insert = i


void jor_rbtree_insert(jor_rbtree_t *tree, jor_rbtree_node_t *node);
void jor_rbtree_delete(jor_rbtree_t *tree, jor_rbtree_node_t *node);
void jor_rbtree_insert_value(jor_rbtree_node_t *root, jor_rbtree_node_t *node,
    jor_rbtree_node_t *sentinel);
void jor_rbtree_insert_timer_value(jor_rbtree_node_t *root,
    jor_rbtree_node_t *node, jor_rbtree_node_t *sentinel);


#define jor_rbt_red(node)               ((node)->color = 1)
#define jor_rbt_black(node)             ((node)->color = 0)
#define jor_rbt_is_red(node)            ((node)->color)
#define jor_rbt_is_black(node)          (!jor_rbt_is_red(node))
#define jor_rbt_copy_color(n1, n2)      (n1->color = n2->color)


#define jor_rbtree_sentinel_init(node)  jor_rbt_black(node)


static inline jor_rbtree_node_t*
jor_rbtree_min(jor_rbtree_node_t *node, jor_rbtree_node_t *sentinel)
{
  while (node->left != sentinel) {
    node = node->left;
  }

  return node;
}


#endif

