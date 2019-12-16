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

namespace demo {

    int hash_call_id_MEB(char character) {
        return (character == '8') ? 1 : 0;
    }

    napi_value Method(napi_env env, napi_callback_info info) {
        napi_value calls, incoming_arr, outgoing_arr;
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
            napi_throw_error(env, NULL, "Failed to parse log path!");
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

        // Open file
        std::ifstream infile(path_buffer, std::ios::in);

        // Init AVL trees for incoming/outgoing calls
        AVL <BSTNode> incoming(nullptr);
        AVL <BSTNode> outgoing(nullptr);

        int line_count = 0;
        // TODO: make it long
        int cur_pos = infile.tellg();

        std::string line;
//        std::string date = file_name.substr(8, 6);
        std::string timestamp;
        std::string call_id;
        std::string hex_prefix = "0x";
        std::string call_str;
        call_str.reserve(10);

        std::regex r(R"(([0,8]+[a-f0-9]{7}))", std::regex::optimize);
        std::smatch m;

        while (std::getline(infile, line)) {
            ++line_count;

            // MEB call-id is always at the same pos
            if (line.length() >= 48) {
                call_id = line.substr(39, 8);

                // Add regex match to AVL tree
                if (std::regex_match(call_id, m, r)) {
                    call_str.append(hex_prefix);
                    // TODO: ⸮⸮⸮ lol ??? wtf, just get line
                    call_str.append(m.str());

//                    if (call_str.compare("0x80000065") == 0) {
//                        napi_value res;
//                        napi_create_uint32(env, cur_pos, &res);
//                        return res;
//                    }
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

            call_str.clear();
            call_id.clear();
            timestamp.clear();
            cur_pos = infile.tellg();
        }

        free(path_buffer);

        // Return Date
//        status = napi_set_named_property(env, calls, "date", res);
//        if (status != napi_ok) return nullptr;

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

    napi_value Method2(napi_env env, napi_callback_info info) {
        napi_status status;
        size_t argc = 2;
        int number = 0;
        size_t size_copied = 0;
        const int buffer_size = 9;
        char call_id_buffer[buffer_size + 1];
        napi_value argv[2];

        status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

        if (status != napi_ok) {
            napi_throw_error(env, NULL, "Failed to parse arguments");
        }


        status = napi_get_value_string_utf8(env, argv[0], call_id_buffer, buffer_size + 1, &size_copied);

        if (status != napi_ok) {
            napi_throw_error(env, NULL, "Invalid call-id was passed as an argument!");
        }

        call_id_buffer[buffer_size] = '\0';

        status = napi_get_value_int32(env, argv[1], &number);

        if (status != napi_ok) {
            napi_throw_error(env, NULL, "Invalid cursor position was passed as an argument!");
        }

        int byte_pos = 0;

        std::string line, temp = "80000065", call_id(call_id_buffer);
        std::ifstream infile("13.log", std::ifstream::in);
        std::ofstream outfile("test.log", std::ofstream::out);

        std::string call_regex = R"((#))";
        std::string end_of_call_regex = R"((\s#\s\sCCall::~CCall.\sCalls:))";

//        call_regex.replace(call_regex.begin(), call_regex.end(), '#', temp);
//        end_of_call_regex.replace(end_of_call_regex.begin(), end_of_call_regex.end(), '#', temp);
        // call id = 80000065
        // tellg = 30168561

        infile.seekg(30168561);

        // TODO: MATCH INPUT call id
        std::regex r(temp, std::regex::optimize);
        std::regex r1(temp, std::regex::optimize);

        std::smatch m;

        while (std::getline(infile, line)) {
            if (line.length() >= 48) {
                temp = line.substr(39, 8);

                // Add regex match to AVL tree
                if (std::regex_match(temp, m, r)) {
                    outfile << line << '\n';
                }
            }

            if (std::regex_match(line, m, r1)) {
                break;
            }
        }

        napi_value res, arr;
//        napi_create_string_utf8(env, line.c_str(), line.length(), &res);
        napi_create_object(env, &arr);
        napi_create_string_utf8(env, call_id.c_str(), call_id.length(), &res);
        napi_set_named_property(env, arr, "buffer", res);
        napi_create_int32(env, number, &res);
        napi_set_named_property(env, arr, "pos", res);

        return arr;
    }

    napi_value init(napi_env env, napi_value exports) {
        napi_status status;
        napi_value fn;

        status = napi_create_function(env, nullptr, 0, Method, nullptr, &fn);
        if (status != napi_ok) return nullptr;

        status = napi_set_named_property(env, exports, "parseLog", fn);
        if (status != napi_ok) return nullptr;

        status = napi_create_function(env, nullptr, 0, Method2, nullptr, &fn);
        if (status != napi_ok) return nullptr;

        status = napi_set_named_property(env, exports, "exportCall", fn);
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
