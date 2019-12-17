//
// Created by konst on 17-Dec-19.
//

#ifndef ADDON_QUEUE_HPP
#define ADDON_QUEUE_HPP

struct Position {
    int x;
    int y;
};

template<class QueueNode>
class Queue {
public:
    QueueNode *m_head;
    QueueNode *m_tail;

    explicit Queue();

    void enqueue(QueueNode *);

    void print_queue();

    Position dequeue();

    void delete_queue();

    ~Queue();
};

template<class QueueNode>
Queue<QueueNode>::Queue() : m_head(nullptr), m_tail(nullptr) {}

template<class QueueNode>
void Queue<QueueNode>::enqueue(QueueNode *t_node) {
    if (!m_head) {
        this->m_head = t_node;
        this->m_tail = t_node;
    } else {
        this->m_tail->m_next = t_node;
        this->m_tail = this->m_tail->m_next;
    }
};

template<class QueueNode>
void Queue<QueueNode>::print_queue() {
    QueueNode *temp_node = this->m_head;

    while (temp_node) {
        temp_node->toString();
        temp_node = temp_node->m_next;
    }
}

template<class QueueNode>
Position Queue<QueueNode>::dequeue() {
    QueueNode *temp_node = this->m_head->m_next;
    Position pos = this->m_head->m_pos;

    delete this->m_head;

    this->m_head = temp_node;

    return pos;
};

template<class QueueNode>
void Queue<QueueNode>::delete_queue() {
    while (this->m_head) {
        this->dequeue();
    }
}

template<class QueueNode>
Queue<QueueNode>::~Queue() {
    this->delete_queue();
}

#endif //ADDON_QUEUE_HPP
