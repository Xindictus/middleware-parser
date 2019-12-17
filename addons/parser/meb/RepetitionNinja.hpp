//
// Created by konst on 17-Dec-19.
//

#ifndef MEB_PARSER_REPETITION_NINJA_HPP
#define MEB_PARSER_REPETITIONNINJA_HPP

#include <iostream>

class RepetitionNinja {
private:
    std::string repetition_string_last;
    std::string repetition_string_second_to_last;

public:
    explicit RepetitionNinja();

    short isRepeated(const std::string &);
};

#endif //MEB_PARSER_REPETITION_NINJA_HPP
