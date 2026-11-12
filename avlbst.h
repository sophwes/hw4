#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    AVLNode(const Key& key, const Value& value,
            AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    int8_t getBalance() const;
    void setBalance(int8_t balance);
    void updateBalance(int8_t diff);

    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;
};

template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(
    const Key& key,
    const Value& value,
    AVLNode<Key, Value>* parent)
    : Node<Key, Value>(key, value, parent), balance_(0)
{
}

template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{
}

template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}

template<class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert(const std::pair<const Key, Value>& new_item);
    virtual void remove(const Key& key);

protected:
    virtual void nodeSwap(
        AVLNode<Key, Value>* n1,
        AVLNode<Key, Value>* n2);

    AVLNode<Key, Value>* rotateLeft(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* rotateRight(AVLNode<Key, Value>* node);
    void insertFix(AVLNode<Key, Value>* node);
    void removeFix(AVLNode<Key, Value>* node, int diff);
};

// Insert a new key, or update the value of an existing key.
template<class Key, class Value>
void AVLTree<Key, Value>::insert(
    const std::pair<const Key, Value>& new_item)
{
    if (this->root_ == NULL) {
        this->root_ = new AVLNode<Key, Value>(
            new_item.first, new_item.second, NULL);
        return;
    }

    AVLNode<Key, Value>* current =
        static_cast<AVLNode<Key, Value>*>(this->root_);

    while (true) {
        if (new_item.first < current->getKey()) {
            if (current->getLeft() == NULL) {
                AVLNode<Key, Value>* added =
                    new AVLNode<Key, Value>(
                        new_item.first, new_item.second, current);

                current->setLeft(added);
                insertFix(added);
                return;
            }

            current = current->getLeft();
        }
        else if (current->getKey() < new_item.first) {
            if (current->getRight() == NULL) {
                AVLNode<Key, Value>* added =
                    new AVLNode<Key, Value>(
                        new_item.first, new_item.second, current);

                current->setRight(added);
                insertFix(added);
                return;
            }

            current = current->getRight();
        }
        else {
            current->setValue(new_item.second);
            return;
        }
    }
}

// Remove a key, swapping with its predecessor when it has two children.
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    AVLNode<Key, Value>* target =
        static_cast<AVLNode<Key, Value>*>(this->internalFind(key));

    if (target == NULL) {
        return;
    }

    if (target->getLeft() != NULL && target->getRight() != NULL) {
        AVLNode<Key, Value>* pred =
            static_cast<AVLNode<Key, Value>*>(
                this->predecessor(target));

        nodeSwap(target, pred);
    }

    AVLNode<Key, Value>* parent = target->getParent();
    AVLNode<Key, Value>* child = target->getLeft();

    if (child == NULL) {
        child = target->getRight();
    }

    int diff = 0;

    if (parent == NULL) {
        this->root_ = child;
    }
    else if (parent->getLeft() == target) {
        parent->setLeft(child);
        diff = 1;
    }
    else {
        parent->setRight(child);
        diff = -1;
    }

    if (child != NULL) {
        child->setParent(parent);
    }

    delete target;
    removeFix(parent, diff);
}

// Provided helper: swap node positions and their balance factors.
template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(
    AVLNode<Key, Value>* n1,
    AVLNode<Key, Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);

    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

// Balance is right subtree height minus left subtree height.
// Rotate left and return the new root of this subtree.
template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateLeft(
    AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* pivot = node->getRight();
    AVLNode<Key, Value>* parent = node->getParent();
    AVLNode<Key, Value>* middle = pivot->getLeft();

    node->setRight(middle);

    if (middle != NULL) {
        middle->setParent(node);
    }

    pivot->setParent(parent);

    if (parent == NULL) {
        this->root_ = pivot;
    }
    else if (parent->getLeft() == node) {
        parent->setLeft(pivot);
    }
    else {
        parent->setRight(pivot);
    }

    pivot->setLeft(node);
    node->setParent(pivot);

    int nodeBalance = node->getBalance();
    int pivotBalance = pivot->getBalance();

    nodeBalance = nodeBalance - 1 - std::max(0, pivotBalance);
    pivotBalance = pivotBalance - 1 + std::min(0, nodeBalance);

    node->setBalance(static_cast<int8_t>(nodeBalance));
    pivot->setBalance(static_cast<int8_t>(pivotBalance));

    return pivot;
}

// Rotate right and return the new root of this subtree.
template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateRight(
    AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* pivot = node->getLeft();
    AVLNode<Key, Value>* parent = node->getParent();
    AVLNode<Key, Value>* middle = pivot->getRight();

    node->setLeft(middle);

    if (middle != NULL) {
        middle->setParent(node);
    }

    pivot->setParent(parent);

    if (parent == NULL) {
        this->root_ = pivot;
    }
    else if (parent->getLeft() == node) {
        parent->setLeft(pivot);
    }
    else {
        parent->setRight(pivot);
    }

    pivot->setRight(node);
    node->setParent(pivot);

    int nodeBalance = node->getBalance();
    int pivotBalance = pivot->getBalance();

    nodeBalance = nodeBalance + 1 - std::min(0, pivotBalance);
    pivotBalance = pivotBalance + 1 + std::max(0, nodeBalance);

    node->setBalance(static_cast<int8_t>(nodeBalance));
    pivot->setBalance(static_cast<int8_t>(pivotBalance));

    return pivot;
}

// Walk upward while insertion increases subtree height.
template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* parent = node->getParent();

    while (parent != NULL) {
        if (parent->getLeft() == node) {
            parent->updateBalance(-1);
        }
        else {
            parent->updateBalance(1);
        }

        int balance = parent->getBalance();

        if (balance == 0) {
            return;
        }

        if (balance == -2) {
            if (parent->getLeft()->getBalance() > 0) {
                rotateLeft(parent->getLeft());
            }

            rotateRight(parent);
            return;
        }

        if (balance == 2) {
            if (parent->getRight()->getBalance() < 0) {
                rotateRight(parent->getRight());
            }

            rotateLeft(parent);
            return;
        }

        node = parent;
        parent = parent->getParent();
    }
}

// diff is +1 when the left subtree shrank,
// and -1 when the right subtree shrank.
template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(
    AVLNode<Key, Value>* node, int diff)
{
    while (node != NULL) {
        node->updateBalance(static_cast<int8_t>(diff));

        int balance = node->getBalance();
        AVLNode<Key, Value>* subtreeRoot = node;

        // The subtree's height did not change.
        if (balance == -1 || balance == 1) {
            return;
        }

        if (balance == -2) {
            if (node->getLeft()->getBalance() > 0) {
                rotateLeft(node->getLeft());
            }

            subtreeRoot = rotateRight(node);

            // A nonzero balance means this rotation preserved height.
            if (subtreeRoot->getBalance() != 0) {
                return;
            }
        }
        else if (balance == 2) {
            if (node->getRight()->getBalance() < 0) {
                rotateRight(node->getRight());
            }

            subtreeRoot = rotateLeft(node);

            if (subtreeRoot->getBalance() != 0) {
                return;
            }
        }

        AVLNode<Key, Value>* parent = subtreeRoot->getParent();

        if (parent == NULL) {
            return;
        }

        diff = (parent->getLeft() == subtreeRoot) ? 1 : -1;
        node = parent;
    }
}

#endif