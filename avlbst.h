#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft(AVLNode<Key, Value>* n);
    void rotateRight(AVLNode<Key, Value>* n);
    void insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n);
    void removeFix(AVLNode<Key, Value>* n, int8_t diff);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    const Key& key = new_item.first;

    // Empty tree: new node becomes the root
    if (this->root_ == NULL) {
        this->root_ = new AVLNode<Key, Value>(key, new_item.second, NULL);
        return;
    }

    // Walk down like a normal BST insert
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* n = NULL;
    while (true) {
        if (key < curr->getKey()) {
            if (curr->getLeft() == NULL) {
                n = new AVLNode<Key, Value>(key, new_item.second, curr);
                curr->setLeft(n);
                break;
            }
            curr = curr->getLeft();
        }
        else if (curr->getKey() < key) {
            if (curr->getRight() == NULL) {
                n = new AVLNode<Key, Value>(key, new_item.second, curr);
                curr->setRight(n);
                break;
            }
            curr = curr->getRight();
        }
        else {
            // Key already exists: overwrite value, tree shape unchanged
            curr->setValue(new_item.second);
            return;
        }
    }

    // n is the new leaf (balance 0), curr is its parent p
    AVLNode<Key, Value>* p = curr;
    if (p->getBalance() == -1 || p->getBalance() == 1) {
        // p was leaning; new node fills the shorter side
        p->setBalance(0);
    }
    else {
        // p was balanced; it now leans toward n and its height grew
        if (n == p->getLeft()) p->setBalance(-1);
        else p->setBalance(1);
        insertFix(p, n);
    }
}

/*
 * Balance convention: balance = height(right) - height(left)
 * p is the parent of n, and the subtree rooted at p just grew taller by 1.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n)
{
    if (p == NULL) return;
    AVLNode<Key, Value>* g = p->getParent();
    if (g == NULL) return;

    if (p == g->getLeft()) {
        g->updateBalance(-1);
        if (g->getBalance() == 0) {
            return;
        }
        else if (g->getBalance() == -1) {
            insertFix(g, p);
        }
        else { // -2: rotate
            if (n == p->getLeft()) {
                // zig-zig (left-left)
                rotateRight(g);
                p->setBalance(0);
                g->setBalance(0);
            }
            else {
                // zig-zag (left-right)
                rotateLeft(p);
                rotateRight(g);
                if (n->getBalance() == -1) {
                    p->setBalance(0); g->setBalance(1);
                }
                else if (n->getBalance() == 0) {
                    p->setBalance(0); g->setBalance(0);
                }
                else {
                    p->setBalance(-1); g->setBalance(0);
                }
                n->setBalance(0);
            }
        }
    }
    else {
        g->updateBalance(1);
        if (g->getBalance() == 0) {
            return;
        }
        else if (g->getBalance() == 1) {
            insertFix(g, p);
        }
        else { // +2: rotate
            if (n == p->getRight()) {
                // zig-zig (right-right)
                rotateLeft(g);
                p->setBalance(0);
                g->setBalance(0);
            }
            else {
                // zig-zag (right-left)
                rotateRight(p);
                rotateLeft(g);
                if (n->getBalance() == 1) {
                    p->setBalance(0); g->setBalance(-1);
                }
                else if (n->getBalance() == 0) {
                    p->setBalance(0); g->setBalance(0);
                }
                else {
                    p->setBalance(1); g->setBalance(0);
                }
                n->setBalance(0);
            }
        }
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    AVLNode<Key, Value>* n = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (n == NULL) return;

    // Two children: swap with predecessor (AVL nodeSwap also swaps balances)
    if (n->getLeft() != NULL && n->getRight() != NULL) {
        AVLNode<Key, Value>* pred =
            static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(n));
        nodeSwap(n, pred);
    }

    // n now has 0 or 1 child
    AVLNode<Key, Value>* p = n->getParent();
    AVLNode<Key, Value>* child = (n->getLeft() != NULL) ? n->getLeft() : n->getRight();

    // Removing from p's left makes p lean right (+1), and vice versa
    int8_t diff = 0;
    if (p != NULL) {
        if (n == p->getLeft()) {
            diff = 1;
            p->setLeft(child);
        }
        else {
            diff = -1;
            p->setRight(child);
        }
    }
    else {
        this->root_ = child;
    }
    if (child != NULL) {
        child->setParent(p);
    }

    delete n;
    removeFix(p, diff);
}

/*
 * The subtree on one side of n just got shorter by 1.
 * diff = +1 if the left side shrank, -1 if the right side shrank.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* n, int8_t diff)
{
    if (n == NULL) return;

    // Compute next call's arguments before any rotation changes n's position
    AVLNode<Key, Value>* p = n->getParent();
    int8_t ndiff = 0;
    if (p != NULL) {
        ndiff = (n == p->getLeft()) ? 1 : -1;
    }

    int newBal = n->getBalance() + diff;

    if (newBal == -2) {
        // Left-heavy: c is the taller (left) child
        AVLNode<Key, Value>* c = n->getLeft();
        if (c->getBalance() == -1) {
            // zig-zig
            rotateRight(n);
            n->setBalance(0);
            c->setBalance(0);
            removeFix(p, ndiff);
        }
        else if (c->getBalance() == 0) {
            // zig-zig, height unchanged afterwards
            rotateRight(n);
            n->setBalance(-1);
            c->setBalance(1);
        }
        else {
            // zig-zag
            AVLNode<Key, Value>* g = c->getRight();
            rotateLeft(c);
            rotateRight(n);
            if (g->getBalance() == 1) {
                n->setBalance(0); c->setBalance(-1);
            }
            else if (g->getBalance() == 0) {
                n->setBalance(0); c->setBalance(0);
            }
            else {
                n->setBalance(1); c->setBalance(0);
            }
            g->setBalance(0);
            removeFix(p, ndiff);
        }
    }
    else if (newBal == 2) {
        // Right-heavy: c is the taller (right) child
        AVLNode<Key, Value>* c = n->getRight();
        if (c->getBalance() == 1) {
            // zig-zig
            rotateLeft(n);
            n->setBalance(0);
            c->setBalance(0);
            removeFix(p, ndiff);
        }
        else if (c->getBalance() == 0) {
            // zig-zig, height unchanged afterwards
            rotateLeft(n);
            n->setBalance(1);
            c->setBalance(-1);
        }
        else {
            // zig-zag
            AVLNode<Key, Value>* g = c->getLeft();
            rotateRight(c);
            rotateLeft(n);
            if (g->getBalance() == -1) {
                n->setBalance(0); c->setBalance(1);
            }
            else if (g->getBalance() == 0) {
                n->setBalance(0); c->setBalance(0);
            }
            else {
                n->setBalance(-1); c->setBalance(0);
            }
            g->setBalance(0);
            removeFix(p, ndiff);
        }
    }
    else if (newBal == -1 || newBal == 1) {
        // Was balanced, now leans: height of n's subtree unchanged, stop
        n->setBalance(newBal);
    }
    else {
        // newBal == 0: n's subtree got shorter, keep going up
        n->setBalance(0);
        removeFix(p, ndiff);
    }
}

/*
 * Left rotation around n: n's right child c takes n's place,
 * n becomes c's left child, c's old left subtree becomes n's right.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* n)
{
    AVLNode<Key, Value>* c = n->getRight();
    AVLNode<Key, Value>* p = n->getParent();

    n->setRight(c->getLeft());
    if (c->getLeft() != NULL) c->getLeft()->setParent(n);

    c->setParent(p);
    if (p == NULL) this->root_ = c;
    else if (n == p->getLeft()) p->setLeft(c);
    else p->setRight(c);

    c->setLeft(n);
    n->setParent(c);
}

/*
 * Right rotation around n: mirror of rotateLeft.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* n)
{
    AVLNode<Key, Value>* c = n->getLeft();
    AVLNode<Key, Value>* p = n->getParent();

    n->setLeft(c->getRight());
    if (c->getRight() != NULL) c->getRight()->setParent(n);

    c->setParent(p);
    if (p == NULL) this->root_ = c;
    else if (n == p->getLeft()) p->setLeft(c);
    else p->setRight(c);

    c->setRight(n);
    n->setParent(c);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif