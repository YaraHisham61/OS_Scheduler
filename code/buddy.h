#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// BuddyNode structure
struct BuddyNode
{
    int size;
    int isFree;
    int start;
    int end;
    int processNumber;
    struct BuddyNode *left;
    struct BuddyNode *right;
    struct BuddyNode *parent;
};

// Create a new BuddyNode
struct BuddyNode *newBuddyNode(int size, int isFree, int start, int end)
{
    struct BuddyNode *BuddyNode = (struct BuddyNode *)malloc(sizeof(struct BuddyNode));
    BuddyNode->size = size;
    BuddyNode->isFree = isFree;
    BuddyNode->start = start;
    BuddyNode->end = end;
    BuddyNode->left = NULL;
    BuddyNode->right = NULL;
    BuddyNode->parent = NULL;
    return BuddyNode;
}

// Split a BuddyNode into two buddies
void splitBuddyNode(struct BuddyNode *BuddyNode)
{
    BuddyNode->isFree = 0;
    int halfSize = BuddyNode->size / 2;
    int mid = BuddyNode->start + halfSize;
    BuddyNode->left = newBuddyNode(halfSize, 1, BuddyNode->start, mid - 1);
    BuddyNode->left->parent = BuddyNode;
    BuddyNode->right = newBuddyNode(halfSize, 1, mid, BuddyNode->end);
    BuddyNode->right->parent = BuddyNode;
}

// Find the smallest BuddyNode in the binary tree that can fit a block of the given size
struct BuddyNode *findBuddyNode(struct BuddyNode *BuddyNode, int size)
{
    int powerOfTwo = (int)pow(2, ceil(log2(size)));
    size = powerOfTwo;
    if (BuddyNode->size < size)
    {
        return NULL;
    }
    if (BuddyNode->size == size && BuddyNode->isFree == 1 && BuddyNode->left == NULL)
    {
        BuddyNode->isFree = 0;
        return BuddyNode;
    }
    if (BuddyNode->left == NULL && BuddyNode->right == NULL && BuddyNode->size > size)
    {
        splitBuddyNode(BuddyNode);
    }
    struct BuddyNode *leftBuddyNode = findBuddyNode(BuddyNode->left, size);
    if (leftBuddyNode != NULL)
    {
        return leftBuddyNode;
    }
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
//     int memSize = 64;
//     struct BuddyNode *root = newBuddyNode(memSize, 1, 0, memSize - 1);
//     struct BuddyNode *block1 = findBuddyNode(root, 2);
//     struct BuddyNode *block2 = findBuddyNode(root, 5);
//     struct BuddyNode *block3 = findBuddyNode(root, 10);
//     struct BuddyNode *block4 = findBuddyNode(root, 30);
//     freeBuddyNode(block2);
//     // freeBuddyNode(block1);
//     // freeBuddyNode(block3);
//     struct BuddyNode *block5 = findBuddyNode(root, 64);
//     printf("Block 1: Start = %d, End = %d, Size = %d, Free = %d\n", block1->start, block1->end, block1->size, block1->isFree);
//     printf("Block 2: Start = %d, End = %d, Size = %d, Free = %d\n", block2->start, block2->end, block2->size, block2->isFree);
//     printf("Block 3: Start = %d, End = %d, Size = %d, Free = %d\n", block3->start, block3->end, block3->size, block3->isFree);
//     printf("Block 4: Start = %d, End = %d, Size = %d, Free = %d\n", block4->start, block4->end, block4->size, block4->isFree);
//     if (block5 != NULL)
//         printf("Block 5: Start = %d, End = %d, Size = %d, Free = %d\n", block5->start, block5->end, block5->size, block5->isFree);
//     return 0;
// }