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


/*
 * The red-black tree code is based on the algorithm described in
 * the "Introduction to Algorithms" by Cormen, Leiserson and Rivest.
 */


#include <stddef.h>
#include "rbtree.h"


static inline void jor_rbtree_left_rotate(jor_rbtree_node_t **root,
    jor_rbtree_node_t *sentinel, jor_rbtree_node_t *node);
static inline void jor_rbtree_right_rotate(jor_rbtree_node_t **root,
    jor_rbtree_node_t *sentinel, jor_rbtree_node_t *node);


void
jor_rbtree_insert(jor_rbtree_t *tree, jor_rbtree_node_t *node)
{
  jor_rbtree_node_t  **root, *temp, *sentinel;

  /* a binary tree insert */

  root = (jor_rbtree_node_t **) &tree->root;
  sentinel = tree->sentinel;

  if (*root == sentinel) {
    node->parent = NULL;
    node->left = sentinel;
    node->right = sentinel;
    jor_rbt_black(node);
    *root = node;

    return;
  }

  tree->insert(*root, node, sentinel);

  /* re-balance tree */

  while (node != *root && jor_rbt_is_red(node->parent)) {

    if (node->parent == node->parent->parent->left) {
      temp = node->parent->parent->right;

      if (jor_rbt_is_red(temp)) {
        jor_rbt_black(node->parent);
        jor_rbt_black(temp);
        jor_rbt_red(node->parent->parent);
        node = node->parent->parent;

      } else {
        if (node == node->parent->right) {
          node = node->parent;
          jor_rbtree_left_rotate(root, sentinel, node);
        }

        jor_rbt_black(node->parent);
        jor_rbt_red(node->parent->parent);
        jor_rbtree_right_rotate(root, sentinel, node->parent->parent);
      }

    } else {
      temp = node->parent->parent->left;

      if (jor_rbt_is_red(temp)) {
        jor_rbt_black(node->parent);
        jor_rbt_black(temp);
        jor_rbt_red(node->parent->parent);
        node = node->parent->parent;

      } else {
        if (node == node->parent->left) {
          node = node->parent;
          jor_rbtree_right_rotate(root, sentinel, node);
        }

        jor_rbt_black(node->parent);
        jor_rbt_red(node->parent->parent);
        jor_rbtree_left_rotate(root, sentinel, node->parent->parent);
      }
    }
  }

  jor_rbt_black(*root);
}


void
jor_rbtree_insert_value(jor_rbtree_node_t *temp, jor_rbtree_node_t *node,
    jor_rbtree_node_t *sentinel)
{
  jor_rbtree_node_t  **p;

  for ( ;; ) {

    p = (node->key < temp->key) ? &temp->left : &temp->right;

    if (*p == sentinel) {
      break;
    }

    temp = *p;
  }

  *p = node;
  node->parent = temp;
  node->left = sentinel;
  node->right = sentinel;
  jor_rbt_red(node);
}


void
jor_rbtree_insert_timer_value(jor_rbtree_node_t *temp, jor_rbtree_node_t *node,
    jor_rbtree_node_t *sentinel)
{
  jor_rbtree_node_t  **p;

  for ( ;; ) {

    /*
     * Timer values
     * 1) are spread in small range, usually several minutes,
     * 2) and overflow each 49 days, if milliseconds are stored in 32 bits.
     * The comparison takes into account that overflow.
     */

    /*  node->key < temp->key */

    p = ((int32_t) (node->key - temp->key) < 0)
      ? &temp->left : &temp->right;

    if (*p == sentinel) {
      break;
    }

    temp = *p;
  }

  *p = node;
  node->parent = temp;
  node->left = sentinel;
  node->right = sentinel;
  jor_rbt_red(node);
}


void
jor_rbtree_delete(jor_rbtree_t *tree, jor_rbtree_node_t *node)
{
  uint32_t             red;
  jor_rbtree_node_t  **root, *sentinel, *subst, *temp, *w;

  /* a binary tree delete */

  root = (jor_rbtree_node_t **) &tree->root;
  sentinel = tree->sentinel;

  if (node->left == sentinel) {
    temp = node->right;
    subst = node;

  } else if (node->right == sentinel) {
    temp = node->left;
    subst = node;

  } else {
    subst = jor_rbtree_min(node->right, sentinel);

    if (subst->left != sentinel) {
      temp = subst->left;
    } else {
      temp = subst->right;
    }
  }

  if (subst == *root) {
    *root = temp;
    jor_rbt_black(temp);

    /* DEBUG stuff */
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->key = 0;

    return;
  }

  red = jor_rbt_is_red(subst);

  if (subst == subst->parent->left) {
    subst->parent->left = temp;

  } else {
    subst->parent->right = temp;
  }

  if (subst == node) {

    temp->parent = subst->parent;

  } else {

    if (subst->parent == node) {
      temp->parent = subst;

    } else {
      temp->parent = subst->parent;
    }

    subst->left = node->left;
    subst->right = node->right;
    subst->parent = node->parent;
    jor_rbt_copy_color(subst, node);

    if (node == *root) {
      *root = subst;

    } else {
      if (node == node->parent->left) {
        node->parent->left = subst;
      } else {
        node->parent->right = subst;
      }
    }

    if (subst->left != sentinel) {
      subst->left->parent = subst;
    }

    if (subst->right != sentinel) {
      subst->right->parent = subst;
    }
  }

  /* DEBUG stuff */
  node->left = NULL;
  node->right = NULL;
  node->parent = NULL;
  node->key = 0;

  if (red) {
    return;
  }

  /* a delete fixup */

  while (temp != *root && jor_rbt_is_black(temp)) {

    if (temp == temp->parent->left) {
      w = temp->parent->right;

      if (jor_rbt_is_red(w)) {
        jor_rbt_black(w);
        jor_rbt_red(temp->parent);
        jor_rbtree_left_rotate(root, sentinel, temp->parent);
        w = temp->parent->right;
      }

      if (jor_rbt_is_black(w->left) && jor_rbt_is_black(w->right)) {
        jor_rbt_red(w);
        temp = temp->parent;

      } else {
        if (jor_rbt_is_black(w->right)) {
          jor_rbt_black(w->left);
          jor_rbt_red(w);
          jor_rbtree_right_rotate(root, sentinel, w);
          w = temp->parent->right;
        }

        jor_rbt_copy_color(w, temp->parent);
        jor_rbt_black(temp->parent);
        jor_rbt_black(w->right);
        jor_rbtree_left_rotate(root, sentinel, temp->parent);
        temp = *root;
      }

    } else {
      w = temp->parent->left;

      if (jor_rbt_is_red(w)) {
        jor_rbt_black(w);
        jor_rbt_red(temp->parent);
        jor_rbtree_right_rotate(root, sentinel, temp->parent);
        w = temp->parent->left;
      }

      if (jor_rbt_is_black(w->left) && jor_rbt_is_black(w->right)) {
        jor_rbt_red(w);
        temp = temp->parent;

      } else {
        if (jor_rbt_is_black(w->left)) {
          jor_rbt_black(w->right);
          jor_rbt_red(w);
          jor_rbtree_left_rotate(root, sentinel, w);
          w = temp->parent->left;
        }

        jor_rbt_copy_color(w, temp->parent);
        jor_rbt_black(temp->parent);
        jor_rbt_black(w->left);
        jor_rbtree_right_rotate(root, sentinel, temp->parent);
        temp = *root;
      }
    }
  }

  jor_rbt_black(temp);
}


static inline void
jor_rbtree_left_rotate(jor_rbtree_node_t **root, jor_rbtree_node_t *sentinel,
    jor_rbtree_node_t *node)
{
  jor_rbtree_node_t  *temp;

  temp = node->right;
  node->right = temp->left;

  if (temp->left != sentinel) {
    temp->left->parent = node;
  }

  temp->parent = node->parent;

  if (node == *root) {
    *root = temp;

  } else if (node == node->parent->left) {
    node->parent->left = temp;

  } else {
    node->parent->right = temp;
  }

  temp->left = node;
  node->parent = temp;
}


static inline void
jor_rbtree_right_rotate(jor_rbtree_node_t **root, jor_rbtree_node_t *sentinel,
    jor_rbtree_node_t *node)
{
  jor_rbtree_node_t  *temp;

  temp = node->left;
  node->left = temp->right;

  if (temp->right != sentinel) {
    temp->right->parent = node;
  }

  temp->parent = node->parent;

  if (node == *root) {
    *root = temp;

  } else if (node == node->parent->right) {
    node->parent->right = temp;

  } else {
    node->parent->left = temp;
  }

  temp->right = node;
  node->parent = temp;
}

