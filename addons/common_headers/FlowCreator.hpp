//
// Created by konst on 17-Dec-19.
//

#ifndef PARSER_FLOW_CREATOR_HPP
#define PARSER_FLOW_CREATOR_HPP

#include <iostream>

class FlowCreator {
private:
    const std::string note_over;
    const std::string normal_line;
    const std::string dashed_line;
    const std::string open_arrow;
    const std::string dashed_open;
    const std::string semicolon;
    const std::string line_change;
    const std::string participant;

    std::string flow_sequence_str;

public:
    explicit FlowCreator(int reserve_size = 2000);

    void append(const std::string &);

    const std::string &getFlowSequenceStr() const;
};


#endif //PARSER_FLOW_CREATOR_HPP
