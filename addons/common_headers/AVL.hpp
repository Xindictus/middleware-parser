//
// Created by konst on 11-Dec-19.
//

#ifndef ADDON_AVL_HPP
#define ADDON_AVL_HPP

#include "BST.hpp"

struct Call {
    std::string call_id;
    int line;
};

template<class BSTNodeType>
class AVL : BST<BSTNodeType> {
public:
    explicit AVL(BSTNodeType *type);

    void insert(BSTNodeType *);

    int search(std::string) override;

    void display() override;

    void clear() override;

    short getNumberOfNodes() const;

    void AVLtoArray(Call []);

    void AVLtoJSArray(napi_env, napi_value *);

private:
    short number_of_nodes;

    BSTNodeType *insert(BSTNodeType *, BSTNodeType *);

    int height(BSTNodeType *);

    int max(short, short);

    int get_node_balance(BSTNodeType *);

    BSTNodeType *right_rotate(BSTNodeType *);

    BSTNodeType *left_rotate(BSTNodeType *);

    void node_to_array(BSTNodeType *, Call [], short *);

    void node_to_js_array(BSTNodeType *, napi_env, napi_value *, short *);
};

template<class BSTNodeType>
BSTNodeType *AVL<BSTNodeType>::insert(BSTNodeType *node, BSTNodeType *new_node) {
    if (node == nullptr) {
        node = new_node;
        ++this->number_of_nodes;
        return node;
    }

    if (new_node->m_key < node->m_key) {
        node->m_left = this->insert(node->m_left, new_node);
    } else if (new_node->m_key > node->m_key) {
        node->m_right = this->insert(node->m_right, new_node);
    } else {
        delete new_node;
        return node;
    }

    /* 2. Update height of this ancestor node */
    node->m_height = 1 + this->max(this->height(node->m_left), this->height(node->m_right));

    /* 3. Get the balance factor of this ancestor
        node to check whether this node became
        unbalanced */
    int balance = this->get_node_balance(node);

    // If this node becomes unbalanced, then
    // there are 4 cases

    // Left Left Case
    if (balance > 1 && new_node->m_key < node->m_left->m_key) {
        return this->right_rotate(node);
    }

    // Right Right Case
    if (balance < -1 && new_node->m_key > node->m_right->m_key)
        return this->left_rotate(node);

    // Left Right Case
    if (balance > 1 && new_node->m_key > node->m_left->m_key) {
        node->m_left = this->left_rotate(node->m_left);
        return this->right_rotate(node);
    }

    // Right Left Case
    if (balance < -1 && new_node->m_key < node->m_right->m_key) {
        node->m_right = this->right_rotate(node->m_right);
        return this->left_rotate(node);
    }

    return node;
}

template<class BSTNodeType>
void AVL<BSTNodeType>::insert(BSTNodeType *new_node) {
    this->m_root = this->insert(this->m_root, new_node);
}

template<class BSTNodeType>
int AVL<BSTNodeType>::get_node_balance(BSTNodeType *node) {
    if (node == nullptr) {
        return 0;
    }

    return this->height(node->m_left) - this->height(node->m_right);
}

template<class BSTNodeType>
AVL<BSTNodeType>::AVL(BSTNodeType *type):BST<BSTNodeType>(type), number_of_nodes(0) {}

template<class BSTNodeType>
void AVL<BSTNodeType>::display() {
    BST<BSTNodeType>::display();
}

template<class BSTNodeType>
void AVL<BSTNodeType>::clear() {
    BST<BSTNodeType>::clear();
}

template<class BSTNodeType>
int AVL<BSTNodeType>::max(short a, short b) {
    return (a < b) ? b : a;
}

template<class BSTNodeType>
int AVL<BSTNodeType>::height(BSTNodeType *node) {
    if (node == nullptr) {
        return 0;
    }
    return node->m_height;
}

template<class BSTNodeType>
BSTNodeType *AVL<BSTNodeType>::right_rotate(BSTNodeType *y) {
    BSTNodeType *x = y->m_left;
    BSTNodeType *T2 = x->m_right;

    // Perform rotation
    x->m_right = y;
    y->m_left = T2;

    // Update heights
    y->m_height = this->max(this->height(y->m_left), this->height(y->m_right)) + 1;
    x->m_height = this->max(this->height(x->m_left), this->height(x->m_right)) + 1;

    // Return new root
    return x;
}

template<class BSTNodeType>
BSTNodeType *AVL<BSTNodeType>::left_rotate(BSTNodeType *x) {
    BSTNodeType *y = x->m_right;
    BSTNodeType *T2 = y->m_left;

    // Perform rotation
    y->m_left = x;
    x->m_right = T2;

    // Update heights
    x->m_height = this->max(this->height(x->m_left), this->height(x->m_right)) + 1;
    y->m_height = this->max(this->height(y->m_left), this->height(y->m_right)) + 1;

    // Return new root
    return y;
}

template<class BSTNodeType>
int AVL<BSTNodeType>::search(std::string str) {
    return BST<BSTNodeType>::search(str);
}

template<class BSTNodeType>
short AVL<BSTNodeType>::getNumberOfNodes() const {
    return this->number_of_nodes;
}

template<class BSTNodeType>
void AVL<BSTNodeType>::AVLtoArray(Call arr[]) {
    short index = 0;
    this->node_to_array(this->m_root, arr, &index);
}

template<class BSTNodeType>
void AVL<BSTNodeType>::AVLtoJSArray(napi_env env, napi_value *js_array) {
    short index = 0;
    this->node_to_js_array(this->m_root, env, js_array, &index);
}

template<class BSTNodeType>
void AVL<BSTNodeType>::node_to_array(BSTNodeType *node, Call arr[], short *index) {
    if (!node) return;
    this->node_to_array(node->m_left, arr, index);
    arr[(*index)].call_id = node->m_key;
    arr[(*index)++].line = node->m_value;
    this->node_to_array(node->m_right, arr, index);
}

template<class BSTNodeType>
void AVL<BSTNodeType>::node_to_js_array(BSTNodeType *node, napi_env env, napi_value *js_array, short *index) {
    if (!node) return;
    // traverse left
    this->node_to_js_array(node->m_left, env, js_array, index);

    // add to js array
    napi_value call_id, line_value, tellg_bytes, timestamp, obj;
    napi_status status = napi_generic_failure;

    status = napi_create_object(env, &obj);
    if (status != napi_ok) return;

    status = napi_create_string_utf8(env, node->m_key.c_str(), NAPI_AUTO_LENGTH, &call_id);
    if (status != napi_ok) return;

    status = napi_create_int32(env, node->m_value, &line_value);
    if (status != napi_ok) return;

    status = napi_create_int32(env, node->m_tellg_bytes, &tellg_bytes);
    if (status != napi_ok) return;

    status = napi_create_string_utf8(env, node->m_timestamp.c_str(), NAPI_AUTO_LENGTH, &timestamp);
    if (status != napi_ok) return;

    status = napi_set_named_property(env, obj, "call_id", call_id);
    if (status != napi_ok) return;

    status = napi_set_named_property(env, obj, "line", line_value);
    if (status != napi_ok) return;

    status = napi_set_named_property(env, obj, "tellg_bytes", tellg_bytes);
    if (status != napi_ok) return;

    status = napi_set_named_property(env, obj, "timestamp", timestamp);
    if (status != napi_ok) return;

    /****/
    napi_value action_btns;
    std::string actions;
    actions.reserve(100);

    actions.append("<button class='btn btn-info export-call' type='btn' value='");
    actions.append(std::to_string(node->m_tellg_bytes));
    actions.append("'>Export Call</button>");

    status = napi_create_string_utf8(env, actions.c_str(), NAPI_AUTO_LENGTH, &action_btns);
    if (status != napi_ok) return;

    status = napi_set_named_property(env, obj, "actions", action_btns);
    if (status != napi_ok) return;
    /****/

    status = napi_set_element(env, (*js_array), (*index)++, obj);
    if (status != napi_ok) return;

    // traverse right
    this->node_to_js_array(node->m_right, env, js_array, index);
}

#endif //ADDON_AVL_HPP
