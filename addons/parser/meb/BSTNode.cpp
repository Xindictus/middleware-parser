//
// Created by konst on 13-Dec-19.
//

#include "BSTNode.hpp"

BSTNode::BSTNode(std::string t_key, int t_value, int t_tellg_bytes, std::string t_timestamp) :
        m_left(nullptr), m_right(nullptr), m_height(1), m_key(std::move(t_key)),
        m_value(t_value), m_tellg_bytes(t_tellg_bytes), m_timestamp(std::move(t_timestamp)) {}