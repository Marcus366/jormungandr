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


static inline void jr_rbtree_left_rotate(jr_rbtree_node_t **root,
    jr_rbtree_node_t *sentinel, jr_rbtree_node_t *node);
static inline void jr_rbtree_right_rotate(jr_rbtree_node_t **root,
    jr_rbtree_node_t *sentinel, jr_rbtree_node_t *node);


void
jr_rbtree_insert(jr_rbtree_t *tree, jr_rbtree_node_t *node)
{
  jr_rbtree_node_t  **root, *temp, *sentinel;

  /* a binary tree insert */

  root = (jr_rbtree_node_t **) &tree->root;
  sentinel = tree->sentinel;

  if (*root == sentinel) {
    node->parent = NULL;
    node->left = sentinel;
    node->right = sentinel;
    jr_rbt_black(node);
    *root = node;

    return;
  }

  tree->insert(*root, node, sentinel);

  /* re-balance tree */

  while (node != *root && jr_rbt_is_red(node->parent)) {

    if (node->parent == node->parent->parent->left) {
      temp = node->parent->parent->right;

      if (jr_rbt_is_red(temp)) {
        jr_rbt_black(node->parent);
        jr_rbt_black(temp);
        jr_rbt_red(node->parent->parent);
        node = node->parent->parent;

      } else {
        if (node == node->parent->right) {
          node = node->parent;
          jr_rbtree_left_rotate(root, sentinel, node);
        }

        jr_rbt_black(node->parent);
        jr_rbt_red(node->parent->parent);
        jr_rbtree_right_rotate(root, sentinel, node->parent->parent);
      }

    } else {
      temp = node->parent->parent->left;

      if (jr_rbt_is_red(temp)) {
        jr_rbt_black(node->parent);
        jr_rbt_black(temp);
        jr_rbt_red(node->parent->parent);
        node = node->parent->parent;

      } else {
        if (node == node->parent->left) {
          node = node->parent;
          jr_rbtree_right_rotate(root, sentinel, node);
        }

        jr_rbt_black(node->parent);
        jr_rbt_red(node->parent->parent);
        jr_rbtree_left_rotate(root, sentinel, node->parent->parent);
      }
    }
  }

  jr_rbt_black(*root);
}


void
jr_rbtree_insert_value(jr_rbtree_node_t *temp, jr_rbtree_node_t *node,
    jr_rbtree_node_t *sentinel)
{
  jr_rbtree_node_t  **p;

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
  jr_rbt_red(node);
}


void
jr_rbtree_insert_timer_value(jr_rbtree_node_t *temp, jr_rbtree_node_t *node,
    jr_rbtree_node_t *sentinel)
{
  jr_rbtree_node_t  **p;

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
  jr_rbt_red(node);
}


void
jr_rbtree_delete(jr_rbtree_t *tree, jr_rbtree_node_t *node)
{
  uint32_t             red;
  jr_rbtree_node_t  **root, *sentinel, *subst, *temp, *w;

  /* a binary tree delete */

  root = (jr_rbtree_node_t **) &tree->root;
  sentinel = tree->sentinel;

  if (node->left == sentinel) {
    temp = node->right;
    subst = node;

  } else if (node->right == sentinel) {
    temp = node->left;
    subst = node;

  } else {
    subst = jr_rbtree_min(node->right, sentinel);

    if (subst->left != sentinel) {
      temp = subst->left;
    } else {
      temp = subst->right;
    }
  }

  if (subst == *root) {
    *root = temp;
    jr_rbt_black(temp);

    /* DEBUG stuff */
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->key = 0;

    return;
  }

  red = jr_rbt_is_red(subst);

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
    jr_rbt_copy_color(subst, node);

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

  while (temp != *root && jr_rbt_is_black(temp)) {

    if (temp == temp->parent->left) {
      w = temp->parent->right;

      if (jr_rbt_is_red(w)) {
        jr_rbt_black(w);
        jr_rbt_red(temp->parent);
        jr_rbtree_left_rotate(root, sentinel, temp->parent);
        w = temp->parent->right;
      }

      if (jr_rbt_is_black(w->left) && jr_rbt_is_black(w->right)) {
        jr_rbt_red(w);
        temp = temp->parent;

      } else {
        if (jr_rbt_is_black(w->right)) {
          jr_rbt_black(w->left);
          jr_rbt_red(w);
          jr_rbtree_right_rotate(root, sentinel, w);
          w = temp->parent->right;
        }

        jr_rbt_copy_color(w, temp->parent);
        jr_rbt_black(temp->parent);
        jr_rbt_black(w->right);
        jr_rbtree_left_rotate(root, sentinel, temp->parent);
        temp = *root;
      }

    } else {
      w = temp->parent->left;

      if (jr_rbt_is_red(w)) {
        jr_rbt_black(w);
        jr_rbt_red(temp->parent);
        jr_rbtree_right_rotate(root, sentinel, temp->parent);
        w = temp->parent->left;
      }

      if (jr_rbt_is_black(w->left) && jr_rbt_is_black(w->right)) {
        jr_rbt_red(w);
        temp = temp->parent;

      } else {
        if (jr_rbt_is_black(w->left)) {
          jr_rbt_black(w->right);
          jr_rbt_red(w);
          jr_rbtree_left_rotate(root, sentinel, w);
          w = temp->parent->left;
        }

        jr_rbt_copy_color(w, temp->parent);
        jr_rbt_black(temp->parent);
        jr_rbt_black(w->left);
        jr_rbtree_right_rotate(root, sentinel, temp->parent);
        temp = *root;
      }
    }
  }

  jr_rbt_black(temp);
}


static inline void
jr_rbtree_left_rotate(jr_rbtree_node_t **root, jr_rbtree_node_t *sentinel,
    jr_rbtree_node_t *node)
{
  jr_rbtree_node_t  *temp;

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
jr_rbtree_right_rotate(jr_rbtree_node_t **root, jr_rbtree_node_t *sentinel,
    jr_rbtree_node_t *node)
{
  jr_rbtree_node_t  *temp;

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

