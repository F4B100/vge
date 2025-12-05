//
// Created by Fabio on 16/11/2025.
//

#ifndef TRIMUN_TREE_H
#define TRIMUN_TREE_H

#include "typeDefs.h"

typedef struct TreeNode {
    void *data;
    struct TreeNode **children;
} treeNode, *pTreeNode;

typedef struct BinaryTreeNode {
    struct BinaryTreeNode *children[2];
    void *data;
} binaryTreeNode, *pBinaryTreeNode;

pTreeNode treeAlloc();
pTreeNode binartTreeAlloc();

void treeInsertChild(pTreeNode, void *value, int8 (*comparator)(void *, void *));
void binaryTreeInsert(pBinaryTreeNode, void *value, int8 (*comparator)(void *, void *));

#endif //TRIMUN_TREE_H