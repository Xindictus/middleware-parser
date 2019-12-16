//
// Created by konst on 13-Dec-19.
//

#ifndef TEST_BSTNODE_HPP
#define TEST_BSTNODE_HPP

#include <iostream>

class BSTNode {
public:
    BSTNode *m_left;
    BSTNode *m_right;
    short m_height;
    std::string m_key;
    int m_value;
    int m_tellg_bytes;
    std::string m_timestamp;

    explicit BSTNode(std::string t_key = std::string(), int t_value = 0, int t_tellg_bytes = 0,
                     std::string t_timestamp = std::string());
};


#endif //TEST_BSTNODE_HPP
