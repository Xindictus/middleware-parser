//
// Created by konst on 17-Dec-19.
//

#include <iostream>
#include "RepetitionNinja.hpp"

RepetitionNinja::RepetitionNinja() {
    this->repetition_string_last.reserve(50);
    this->repetition_string_second_to_last.reserve(50);
}

short RepetitionNinja::isRepeated(const std::string &str) {
    if (repetition_string_last == str || repetition_string_second_to_last == str) {
        return 1;
    } else {
        repetition_string_second_to_last.clear();
        repetition_string_second_to_last.append(repetition_string_last);

        repetition_string_last.clear();
        repetition_string_last.append(str);
    }

    return 0;
}