#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int leafDepth(Node* node);


bool equalPaths(Node * root)
{
    // Add your code below
    return leafDepth(root) != -1;
}

int leafDepth(Node* node)
{
    if (node == nullptr) {
        return 0;
    }

    // Leaf node
    if (node->left == nullptr && node->right == nullptr) {
        return 1;
    }

    if (node->left == nullptr) {
        int d = leafDepth(node->right);
        return (d == -1) ? -1 : d + 1;
    }
    if (node->right == nullptr) {
        int d = leafDepth(node->left);
        return (d == -1) ? -1 : d + 1;
    }

    int l = leafDepth(node->left);
    int r = leafDepth(node->right);
    if (l == -1 || r == -1 || l != r) {
        return -1;
    }
    return l + 1;
}