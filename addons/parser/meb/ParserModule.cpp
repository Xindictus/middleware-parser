#include <node_api.h>
#include <fstream>
//#include <memory>
//#include <forward_list>
#include <regex>
//#include <chrono>
#include <iostream>
//#include <utility>

#include "BST.hpp"
#include "AVL.hpp"
#include "BSTNode.hpp"
#include "RepetitionNinja.hpp"
#include "FlowCreator.hpp"

namespace demo {

    int hash_call_id_MEB(char character) {
        return (character == '8') ? 1 : 0;
    }

    napi_value CallParser(napi_env env, napi_callback_info info) {
        napi_value calls, incoming_arr, outgoing_arr, date_str;
        napi_status status = napi_generic_failure;

        // Expected arguments
        size_t argc = 1;

        // Arguments' array
        napi_value argv[1];

        // String buffer for log path
        size_t path_buffer_size = 0;
        char *path_buffer;

        status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

        if (status != napi_ok) {
            napi_throw_error(env, NULL, "Failed to parse arguments");
        }

        status = napi_get_value_string_utf8(env, argv[0], NULL, 0, &path_buffer_size);

        if (status != napi_ok) {
            napi_throw_error(env, NULL, "Failed to parse log path size!");
        }

        path_buffer = (char *) malloc(path_buffer_size + 1);

        status = napi_get_value_string_utf8(env, argv[0], path_buffer, path_buffer_size + 1, &path_buffer_size);

        if (status != napi_ok) {
            napi_throw_error(env, NULL, "Failed to parse log path!");
        }

        path_buffer[path_buffer_size] = '\0';

        // Creating response object
        status = napi_create_object(env, &calls);
        if (status != napi_ok) return nullptr;

        // Create arrays for incoming/outgoing calls
        status = napi_create_array(env, &incoming_arr);
        if (status != napi_ok) return nullptr;

        status = napi_create_array(env, &outgoing_arr);
        if (status != napi_ok) return nullptr;

        // Will be used to get the date
        std::string path_buffer_str(path_buffer);
        free(path_buffer);

        // Open file
        std::ifstream infile(path_buffer_str, std::ifstream::binary | std::ifstream::in);

        // Init AVL trees for incoming/outgoing calls
        AVL <BSTNode> incoming(nullptr);
        AVL <BSTNode> outgoing(nullptr);

        int line_count = 0;
        // TODO: make it long for safety
        int cur_pos = infile.tellg();

        std::string line;
        // TODO: Get date dynamically
        std::string date = path_buffer_str.substr(8, 6);
        std::string timestamp;
        std::string call_id;
        std::string hex_prefix = "0x";
        std::string call_str;
        call_str.reserve(10);

        // TODO: impove regex performance with ^ and $
        std::regex r(R"(([0,8]+[a-f0-9]{7}))", std::regex::optimize);
        std::smatch m;

        while (std::getline(infile, line)) {
            // Line count for call-id
            ++line_count;

            // MEB call-id is always at the same pos
            if (line.length() >= 48) {
                call_id = line.substr(39, 8);

                // Add regex match to AVL tree
                if (std::regex_match(call_id, m, r)) {
                    // TODO: improve regex performance with regex_search
                    call_str.append(hex_prefix);
                    // TODO: ⸮⸮⸮ lol ??? wtf, just get line
                    call_str.append(m.str());

                    timestamp = line.substr(11, 12);

                    if (hash_call_id_MEB(call_str.c_str()[2])) {
                        // incoming call
                        incoming.insert(new BSTNode(call_str, line_count, cur_pos, timestamp));
                    } else {
                        // outgoing call
                        outgoing.insert(new BSTNode(call_str, line_count, cur_pos, timestamp));
                    }
                }
            }

            // Clear strings for next loop.
            call_str.clear();
            call_id.clear();
            timestamp.clear();

            // Keep current cursor position for faster parsing on exporting.
            cur_pos = infile.tellg();
        }

        // Add Date to return object
        status = napi_create_string_utf8(env, path_buffer_str.c_str(), path_buffer_str.length(), &date_str);
        if (status != napi_ok) return nullptr;

        status = napi_set_named_property(env, calls, "date", date_str);
        if (status != napi_ok) return nullptr;

        // Convert AVL trees to JS arrays
        incoming.AVLtoJSArray(env, &incoming_arr);
        outgoing.AVLtoJSArray(env, &outgoing_arr);

        // Add arrays to return object
        status = napi_set_named_property(env, calls, "incoming_calls", incoming_arr);
        if (status != napi_ok) return nullptr;

        status = napi_set_named_property(env, calls, "outgoing_calls", outgoing_arr);
        if (status != napi_ok) return nullptr;

        // Free AVL trees
        incoming.clear();
        outgoing.clear();

        return calls;
    }

    napi_value ExportCall(napi_env env, napi_callback_info info) {
        // # of expected parameters
        size_t argc = 4;
        size_t size_copied = 0;
        size_t path_buffer_size = 0;
        int cursor_pos = 0;
        const int buffer_size = 9;
        char *path_buffer_input;
        char *path_buffer_output;
        char call_id_buffer[buffer_size + 1];
        napi_value argv[4];
        napi_status status;

        // Parsing arguments coming from JS
        status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

        if (status != napi_ok) {
            napi_throw_error(env, NULL, "Failed to parse arguments");
        }

        // Get CALL-ID from JS
        status = napi_get_value_string_utf8(env, argv[0], call_id_buffer, buffer_size + 1, &size_copied);

        if (status != napi_ok) {
            napi_throw_error(env, NULL, "Invalid call-id was passed as an argument!");
        }

        call_id_buffer[buffer_size] = '\0';

        // Get cursor position from JS
        status = napi_get_value_int32(env, argv[1], &cursor_pos);

        if (status != napi_ok) {
            napi_throw_error(env, NULL, "Invalid cursor position was passed as an argument!");
        }

        // Get input log path from JS
        status = napi_get_value_string_utf8(env, argv[2], NULL, 0, &path_buffer_size);

        if (status != napi_ok) {
            napi_throw_error(env, NULL, "Failed to parse input log path size!");
        }

        path_buffer_input = (char *) malloc(path_buffer_size + 1);

        status = napi_get_value_string_utf8(env, argv[2], path_buffer_input, path_buffer_size + 1, &path_buffer_size);

        if (status != napi_ok) {
            napi_throw_error(env, NULL, "Failed to parse input log path!");
        }

        path_buffer_input[path_buffer_size] = '\0';

        // Get output log path from JS
        status = napi_get_value_string_utf8(env, argv[3], NULL, 0, &path_buffer_size);

        if (status != napi_ok) {
            napi_throw_error(env, NULL, "Failed to parse output log path size!");
        }

        path_buffer_output = (char *) malloc(path_buffer_size + 1);

        status = napi_get_value_string_utf8(env, argv[3], path_buffer_output, path_buffer_size + 1, &path_buffer_size);

        if (status != napi_ok) {
            napi_throw_error(env, NULL, "Failed to parse output log path!");
        }

        path_buffer_output[path_buffer_size] = '\0';

        // Setup streams
        std::string line, line_call, call_id(call_id_buffer);
        std::ifstream infile(path_buffer_input, std::ifstream::binary | std::ifstream::in);
        std::ofstream outfile(path_buffer_output, std::ofstream::out);

        // Creating dynamic regex based on call-id
        std::string regex_call_tmpl = R"(#)";
        std::string regex_end_of_call_tmpl = R"(\s#\s\sCCall::~CCall.\sCalls:)";

        std::regex regex_replace_hashtag("#");
        std::regex regex_call(std::regex_replace(regex_call_tmpl, regex_replace_hashtag, call_id),
                              std::regex::optimize);
        std::regex regex_end_of_call(std::regex_replace(regex_end_of_call_tmpl, regex_replace_hashtag, call_id),
                                     std::regex::optimize);

        // Set cursor to start of call to earn time from parsing the whole file
        infile.seekg(cursor_pos);

        while (std::getline(infile, line)) {
            if (line.length() >= 48) {
                line_call = line.substr(39, 8);

                if (std::regex_search(line_call, regex_call)) {
                    outfile << line << '\n';
                }

                // Stop parsing if the ending of the call has been reached
                if (std::regex_search(line, regex_end_of_call)) {
                    break;
                }
            }
        }

        free(path_buffer_input);
        free(path_buffer_output);

        return nullptr;
    }

    napi_value AnalyzeTraceCall(napi_env env, napi_callback_info info) {
        size_t argc = 1;
        napi_value flow_sequence, argv[1];
        napi_status status;

        size_t path_buffer_size = 0;
        char *path_buffer_input;
        std::string line;

        // Parsing arguments coming from JS
        status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

        if (status != napi_ok) {
            napi_throw_error(env, NULL, "Failed to parse arguments");
        }

        // Parsing input trace file
        status = napi_get_value_string_utf8(env, argv[0], NULL, 0, &path_buffer_size);

        if (status != napi_ok) {
            napi_throw_error(env, NULL, "Failed to parse input log path size!");
        }

        path_buffer_input = (char *) malloc(path_buffer_size + 1);

        status = napi_get_value_string_utf8(env, argv[0], path_buffer_input, path_buffer_size + 1, &path_buffer_size);

        if (status != napi_ok) {
            napi_throw_error(env, NULL, "Failed to parse input log path!");
        }

        path_buffer_input[path_buffer_size] = '\0';

        // Open file
        std::ifstream infile(path_buffer_input, std::ifstream::binary | std::ifstream::in);

        std::regex messages_meb(R"(e[A-Z0-9]{3,4}_[A-Z0-9]{3,4}_[a-zA-Z0-9_]*)");

        std::smatch m;

        RepetitionNinja checker;
        FlowCreator flow(5000);

        while (std::getline(infile, line)) {
            // Check for MEB messages
            if (std::regex_search(line, m, messages_meb)) {
                // For each message, filter duplicates and append them to flow string
                for (const auto &i : m) {
                    if (!checker.isRepeated(i.str())) {
                        flow.append(i.str().substr(1));
                    }
                }
            }
        }

        free(path_buffer_input);

        // TODO: return flowchart and fix free
//        status = napi_create_string_utf8(env, path_buffer_input, NAPI_AUTO_LENGTH, &flow_sequence);
        status = napi_create_string_utf8(env, flow.getFlowSequenceStr().c_str(), NAPI_AUTO_LENGTH, &flow_sequence);
        if (status != napi_ok) return nullptr;

        return flow_sequence;
    }

    napi_value init(napi_env env, napi_value exports) {
        napi_status status;
        napi_value fn;

        status = napi_create_function(env, nullptr, 0, CallParser, nullptr, &fn);
        if (status != napi_ok) return nullptr;

        status = napi_set_named_property(env, exports, "parseLog", fn);
        if (status != napi_ok) return nullptr;

        status = napi_create_function(env, nullptr, 0, ExportCall, nullptr, &fn);
        if (status != napi_ok) return nullptr;

        status = napi_set_named_property(env, exports, "exportCall", fn);
        if (status != napi_ok) return nullptr;

        status = napi_create_function(env, nullptr, 0, AnalyzeTraceCall, nullptr, &fn);
        if (status != napi_ok) return nullptr;

        status = napi_set_named_property(env, exports, "analyzeCall", fn);
        if (status != napi_ok) return nullptr;

        return exports;
    }

    /**
     * Regex
     * ------------------------------------------------------------------------------------
     * Get timestamp: \d{4}-\d{2}-\d{2}\s\d{2}:\d{2}:\d{2}.\d{3}
     * Get   call-id: \s([0-9]+[a-f0-9]{7})\s\s                     (needs rtrim)
     * Get  messages: e[A-Z0-9]*_[A-Z0-9]*_[a-zA-Z0-9_]*            (substring 1-end, split _, arr[0] - arr[1] for flow, rest for )
     * Find call-id: distinct search
     *
     * auto start = std::chrono::system_clock::now();
     * auto end = std::chrono::system_clock::now();
     * std::chrono::duration<double> diff = end - start;
     */
    NAPI_MODULE(NODE_GYP_MODULE_NAME, init
    )

}  // namespace demo
