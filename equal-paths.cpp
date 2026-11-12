#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here


// Record the first leaf's depth, then compare every other leaf.
bool checkLeafDepths(Node* node, int depth, int& leafDepth)
{
    if (node == nullptr) {
        return true;
    }

    if (node->left == nullptr && node->right == nullptr) {
        if (leafDepth == -1) {
            leafDepth = depth;
        }
        return depth == leafDepth;
    }

    return checkLeafDepths(node->left, depth + 1, leafDepth)
        && checkLeafDepths(node->right, depth + 1, leafDepth);
}

bool equalPaths(Node* root)
{
    int leafDepth = -1;
    return checkLeafDepths(root, 0, leafDepth);
}
