//
// Created by konst on 11-Dec-19.
//

#ifndef ADDON_BST_HPP
#define ADDON_BST_HPP

template<class BSTNodeType>
class BST {
public:
    BSTNodeType *m_root;

    explicit BST(BSTNodeType *);

    virtual void insert(BSTNodeType *);

    virtual void display();

    virtual int search(std::string);

    virtual void clear();

protected:
    void traverse(BSTNodeType *);

    void delete_tree(BSTNodeType *);
};

template<class BSTNodeType>
BST<BSTNodeType>::BST(BSTNodeType *t_root) : m_root(t_root) {}

template<class BSTNodeType>
void BST<BSTNodeType>::insert(BSTNodeType *node) {
    BSTNodeType *cur = this->m_root, *parent = cur;

    if (!this->m_root) {
        this->m_root = node;
        return;
    }

    while (cur) {
        parent = cur;

        if (node->m_key < cur->m_key) {
            cur = cur->m_left;
        } else if (node->m_key > cur->m_key) {
            cur = cur->m_right;
        } else {
            delete node;
            return;
        }
    }

    if (node->m_key < parent->m_key) {
        parent->m_left = node;
    } else {
        parent->m_right = node;
    }
}

template<class BSTNodeType>
void BST<BSTNodeType>::display() {
    this->traverse(this->m_root);
}

template<class BSTNodeType>
void BST<BSTNodeType>::traverse(BSTNodeType *node) {
    if (!node) return;
    this->traverse(node->m_left);
    std::cout << node->m_key << std::endl;
    this->traverse(node->m_right);
}

template<class BSTNodeType>
void BST<BSTNodeType>::clear() {
    this->delete_tree(this->m_root);
}

template<class BSTNodeType>
void BST<BSTNodeType>::delete_tree(BSTNodeType *node) {
    if (node == nullptr) return;
    this->delete_tree(node->m_left);
    this->delete_tree(node->m_right);
    delete node;
}

template<class BSTNodeType>
int BST<BSTNodeType>::search(std::string key) {
    BSTNodeType *t_node = this->m_root;

    short found = 0;
    short line = 0;
    while (t_node != nullptr) {

        if (key.compare(t_node->m_key) == 0) {
            line = t_node->m_value;
            ++found;
        } else if (key.compare(t_node->m_key) < 0) {
            t_node = t_node->m_left;
        } else {
            t_node = t_node->m_right;
        }

        if (found) break;
    }

    return line;
}

#endif //ADDON_BST_HPP
