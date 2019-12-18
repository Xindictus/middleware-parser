//
// Created by konst on 17-Dec-19.
//

#include <sstream>
#include <iostream>
#include "FlowCreator.hpp"

// TODO: CHANGE DEFINITIONS OF ARROWS ACCORDING TO MERMAID
FlowCreator::FlowCreator(int reserve_size) : note_over("Note over "), normal_line("->"), dashed_line("-->"),
                                             open_arrow("->>"), dashed_open("-->>"), semicolon(": "),
                                             line_change("\n"), participant("participant ") {

    this->flow_sequence_str.reserve(reserve_size);
//    this->flow_sequence_str.append("Title: MEB trace diagram");
    this->flow_sequence_str.append("sequenceDiagram");
    this->flow_sequence_str.append(this->line_change);

    // Add participants in optimal order for viewing the diagram
    this->flow_sequence_str.append(this->participant);
    this->flow_sequence_str.append("NQU");
    this->flow_sequence_str.append(this->line_change);

    this->flow_sequence_str.append(this->participant);
    this->flow_sequence_str.append("H323");
    this->flow_sequence_str.append(this->line_change);

    this->flow_sequence_str.append(this->participant);
    this->flow_sequence_str.append("CPU");
    this->flow_sequence_str.append(this->line_change);

    this->flow_sequence_str.append(this->participant);
    this->flow_sequence_str.append("MEB");
    this->flow_sequence_str.append(this->line_change);

    this->flow_sequence_str.append(this->participant);
    this->flow_sequence_str.append("APP");
    this->flow_sequence_str.append(this->line_change);
}

void FlowCreator::append(const std::string &str) {
    std::istringstream f(str);
    std::string actor_1;
    std::string actor_2;
    std::string message;

    std::getline(f, actor_1, '_');
    std::getline(f, actor_2, '_');
    std::getline(f, message);

    if (actor_1 != actor_2) {
        this->flow_sequence_str.append(actor_1);
        this->flow_sequence_str.append(
                (actor_1 == "NQU" || actor_2 == "NQU") ? this->dashed_open : this->open_arrow
        );
        this->flow_sequence_str.append(actor_2);
        this->flow_sequence_str.append(this->semicolon);
        this->flow_sequence_str.append(message);
    } else {
        this->flow_sequence_str.append(this->note_over);
        this->flow_sequence_str.append(actor_1);
        this->flow_sequence_str.append(this->semicolon);
        this->flow_sequence_str.append(message);
    }

    this->flow_sequence_str.append(this->line_change);
}

const std::string &FlowCreator::getFlowSequenceStr() const {
    return flow_sequence_str;
}