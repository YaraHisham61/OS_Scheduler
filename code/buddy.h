#ifndef buddy_cls
#define buddy_cls

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// #include<headers.h>

// BuddyNode structure
struct BuddyNode
{
    int size;
    int isFree;
    int start;
    int ending;
    int processNumber;
    struct BuddyNode *left;
    struct BuddyNode *right;
    struct BuddyNode *parent;
};

// Create a new BuddyNode
struct BuddyNode *newBuddyNode(int size, int isFree, int start, int ending){
    struct BuddyNode *BuddyNode = (struct BuddyNode *)malloc(sizeof(struct BuddyNode));
    BuddyNode->processNumber = -1;
    BuddyNode->size = size;
    BuddyNode->isFree = isFree;
    BuddyNode->start = start;
    BuddyNode->ending = ending;
    BuddyNode->left = NULL;
    BuddyNode->right = NULL;
    BuddyNode->parent = NULL;
    return BuddyNode;
}

// Split a BuddyNode into two buddies
void splitBuddyNode(struct BuddyNode *BuddyNode)
{
    // BuddyNode->isFree = 0;
    int halfSize = BuddyNode->size / 2;
    int mid = BuddyNode->start + halfSize;
    BuddyNode->left = newBuddyNode(halfSize, 1, BuddyNode->start, mid - 1);
    BuddyNode->left->parent = BuddyNode;
    BuddyNode->right = newBuddyNode(halfSize, 1, mid, BuddyNode->ending);
    BuddyNode->right->parent = BuddyNode;
}

// Find the smallest BuddyNode in the binary tree that can fit a block of the given size
struct BuddyNode *findBuddyNode(struct BuddyNode *BuddyNode, int size)
{
    int powerOfTwo = (int)pow(2, ceil(log2(size)));
    size = powerOfTwo;
    
    if (BuddyNode == NULL || BuddyNode->size < size || !BuddyNode->isFree)
    {
    //     printf("I'm here in the find 1 processNum = %d size = %d start = %d ending = %d isFree = %d\n",
    // BuddyNode->processNumber, BuddyNode->size, BuddyNode->start, BuddyNode->ending,BuddyNode->isFree);
        return NULL;
    }

    if (BuddyNode->size == size && BuddyNode->isFree == 1 && BuddyNode->left == NULL)
    {
    //     printf("I'm here in the find 2 processNum = %d size = %d start = %d ending = %d isFree = %d\n",
    // BuddyNode->processNumber, BuddyNode->size, BuddyNode->start, BuddyNode->ending,BuddyNode->isFree);
        BuddyNode->isFree = 0;
        return BuddyNode;
    }

    if (BuddyNode->left == NULL && BuddyNode->right == NULL && BuddyNode->size > size && BuddyNode->isFree == 1)
    {
        // printf("I'm here in the find 3 processNum = %d size = %d start = %d ending = %d isFree = %d\n",
    // BuddyNode->processNumber, BuddyNode->size, BuddyNode->start, BuddyNode->ending,BuddyNode->isFree);
        splitBuddyNode(BuddyNode);
    }

    if (BuddyNode->left == NULL && BuddyNode->right == NULL)
    {
        return NULL;
    }

    struct BuddyNode *leftBuddyNode = findBuddyNode(BuddyNode->left, size);
    if (leftBuddyNode != NULL)
    {
    //     printf("I'm here in the find 4 processNum = %d size = %d start = %d ending = %d isFree = %d\n",
    // BuddyNode->processNumber, BuddyNode->size, BuddyNode->start, BuddyNode->ending,BuddyNode->isFree);
        return leftBuddyNode;
    }
// printf("I'm here in the find 5 processNum = %d size = %d start = %d ending = %d isFree = %d\n",
    // BuddyNode->processNumber, BuddyNode->size, BuddyNode->start, BuddyNode->ending,BuddyNode->isFree);
    return findBuddyNode(BuddyNode->right, size);
}

// Merge two buddies into a parent BuddyNode
void mergeBuddyNodes(struct BuddyNode *BuddyNode)
{
    BuddyNode->left->parent = NULL;
    BuddyNode->right->parent = NULL;
    BuddyNode->left = NULL;
    BuddyNode->right = NULL;
    if (BuddyNode->parent == NULL)
    {
        return;
    }
    free(BuddyNode->left);
    free(BuddyNode->right);
    BuddyNode->isFree = 1;
    printf("I'm Here\n");
    if (BuddyNode->parent->left->isFree == 1 && BuddyNode->parent->right->isFree == 1)
        mergeBuddyNodes(BuddyNode->parent);
}

// Free a block of memory and merge its buddies if they are also free
void freeBuddyNode(struct BuddyNode *BuddyNode)
{
    BuddyNode->isFree = 1;
    if (BuddyNode->parent == NULL)
    {
        return;
    }
    if (BuddyNode->parent->left->isFree == 1 && BuddyNode->parent->right->isFree == 1)
    {
        mergeBuddyNodes(BuddyNode->parent);
    }
}

// int main()
// {
//     int memSize = 1024;
//     struct BuddyNode *root = newBuddyNode(memSize, 1, 0, memSize - 1);
//     struct BuddyNode *block1 = findBuddyNode(root, 200);

//     struct BuddyNode *block2 = findBuddyNode(root, 200);
    
//     struct BuddyNode *block3 = findBuddyNode(root, 255);
//     // struct BuddyNode *block5 = findBuddyNode(root, 10);
//     struct BuddyNode *block4 = findBuddyNode(root, 10);
    
    
    

//     if (block1 != NULL)
//         printf("Block 1: Start = %d, ending = %d, Size = %d, Free = %d\n", block1->start, block1->ending, block1->size, block1->isFree);
//     if (block2 != NULL)
//         printf("Block 2: Start = %d, ending = %d, Size = %d, Free = %d\n", block2->start, block2->ending, block2->size, block2->isFree);
//     if (block3 != NULL)
//         printf("Block 3: Start = %d, ending = %d, Size = %d, Free = %d\n", block3->start, block3->ending, block3->size, block3->isFree);
//     if (block4 != NULL)
//         printf("Block 4: Start = %d, ending = %d, Size = %d, Free = %d\n", block4->start, block4->ending, block4->size, block4->isFree);
//     freeBuddyNode(block1);
//     freeBuddyNode(block4);
//     freeBuddyNode(block3);
//     freeBuddyNode(block2);
//     if (block1 != NULL)
//         printf("Block 1: Start = %d, ending = %d, Size = %d, Free = %d\n", block1->start, block1->ending, block1->size, block1->isFree);
//     if (block2 != NULL)
//         printf("Block 2: Start = %d, ending = %d, Size = %d, Free = %d\n", block2->start, block2->ending, block2->size, block2->isFree);
//     if (block3 != NULL)
//         printf("Block 3: Start = %d, ending = %d, Size = %d, Free = %d\n", block3->start, block3->ending, block3->size, block3->isFree);
//     if (block4 != NULL)
//         printf("Block 4: Start = %d, ending = %d, Size = %d, Free = %d\n", block4->start, block4->ending, block4->size, block4->isFree);
//     block1 = findBuddyNode(root, 200);

//     block2 = findBuddyNode(root, 200);
    
//     block3 = findBuddyNode(root, 255);
//     // struct BuddyNode *block5 = findBuddyNode(root, 10);
//     block4 = findBuddyNode(root, 10);
    
    
    

//     if (block1 != NULL)
//         printf("Block 1: Start = %d, ending = %d, Size = %d, Free = %d\n", block1->start, block1->ending, block1->size, block1->isFree);
//     if (block2 != NULL)
//         printf("Block 2: Start = %d, ending = %d, Size = %d, Free = %d\n", block2->start, block2->ending, block2->size, block2->isFree);
//     if (block3 != NULL)
//         printf("Block 3: Start = %d, ending = %d, Size = %d, Free = %d\n", block3->start, block3->ending, block3->size, block3->isFree);
//     if (block4 != NULL)
//         printf("Block 4: Start = %d, ending = %d, Size = %d, Free = %d\n", block4->start, block4->ending, block4->size, block4->isFree);
    
//     // if (block5 != NULL)
//     //     printf("Block 5: Start = %d, ending = %d, Size = %d, Free = %d\n", block5->start, block5->ending, block5->size, block5->isFree);
//     return 0;
// }
#endif