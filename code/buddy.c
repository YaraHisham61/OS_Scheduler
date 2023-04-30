#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Node structure
struct Node
{
    int size;
    int isFree;
    int start;
    int end;
    struct Node *left;
    struct Node *right;
    struct Node *parent;
};

// Create a new node
struct Node *newNode(int size, int isFree, int start, int end)
{
    struct Node *node = (struct Node *)malloc(sizeof(struct Node));
    node->size = size;
    node->isFree = isFree;
    node->start = start;
    node->end = end;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    return node;
}

// Split a node into two buddies
void splitNode(struct Node *node)
{
    node->isFree = 0;
    int halfSize = node->size / 2;
    int mid = node->start + halfSize;
    node->left = newNode(halfSize, 1, node->start, mid - 1);
    node->left->parent = node;
    node->right = newNode(halfSize, 1, mid, node->end);
    node->right->parent = node;
}

// Find the smallest node in the binary tree that can fit a block of the given size
struct Node *findNode(struct Node *node, int size)
{
    int powerOfTwo = (int)pow(2, ceil(log2(size)));
    size = powerOfTwo;
    if (node->size < size)
    {
        return NULL;
    }
    if (node->size == size && node->isFree == 1 && node->left == NULL)
    {
        node->isFree = 0;
        return node;
    }
    if (node->left == NULL && node->right == NULL && node->size > size)
    {
        splitNode(node);
    }
    struct Node *leftNode = findNode(node->left, size);
    if (leftNode != NULL)
    {
        return leftNode;
    }
    return findNode(node->right, size);
}

// Merge two buddies into a parent node
void mergeNodes(struct Node *node)
{
    node->left->parent = NULL;
    node->right->parent = NULL;
    node->left = NULL;
    node->right = NULL;
    if (node->parent == NULL)
    {
        return;
    }
    free(node->left);
    free(node->right);
    node->isFree = 1;
    printf("I'm Here\n");
    if (node->parent->left->isFree == 1 && node->parent->right->isFree == 1)
        mergeNodes(node->parent);
}

// Free a block of memory and merge its buddies if they are also free
void freeNode(struct Node *node)
{
    node->isFree = 1;
    if (node->parent == NULL)
    {
        return;
    }
    if (node->parent->left->isFree == 1 && node->parent->right->isFree == 1)
    {
        mergeNodes(node->parent);
    }
}

// int main()
// {
//     int memSize = 64;
//     struct Node *root = newNode(memSize, 1, 0, memSize - 1);
//     struct Node *block1 = findNode(root, 2);
//     struct Node *block2 = findNode(root, 5);
//     struct Node *block3 = findNode(root, 10);
//     struct Node *block4 = findNode(root, 30);
//     freeNode(block2);
//     // freeNode(block1);
//     // freeNode(block3);
//     struct Node *block5 = findNode(root, 64);
//     printf("Block 1: Start = %d, End = %d, Size = %d, Free = %d\n", block1->start, block1->end, block1->size, block1->isFree);
//     printf("Block 2: Start = %d, End = %d, Size = %d, Free = %d\n", block2->start, block2->end, block2->size, block2->isFree);
//     printf("Block 3: Start = %d, End = %d, Size = %d, Free = %d\n", block3->start, block3->end, block3->size, block3->isFree);
//     printf("Block 4: Start = %d, End = %d, Size = %d, Free = %d\n", block4->start, block4->end, block4->size, block4->isFree);
//     if (block5 != NULL)
//         printf("Block 5: Start = %d, End = %d, Size = %d, Free = %d\n", block5->start, block5->end, block5->size, block5->isFree);
//     return 0;
// }