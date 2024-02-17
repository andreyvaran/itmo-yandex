//
// Created by andreyvaran on 30.10.23.
//
#include <regex>
#include <vector>
#include <map>

#include "get_size.hpp"

namespace {
    using namespace std;

    vector<pair<size_t, size_t>> ParseTypes(const string &object) {
        regex reg("alignas\\([0-9]*\\)|char|short|int|long");
        smatch matches;
        vector<pair<size_t, size_t>> type_position;

        for (auto it = sregex_iterator(object.begin(), object.end(), reg);
             it != sregex_iterator(); ++it) {
            if (!it->str().empty()) {
                type_position.emplace_back(it->position(), it->length());
            }
        }
        return type_position;
    }

    size_t LongestType(const vector<pair<size_t, size_t>> &type_position,
                       const map<string_view, size_t> &type_size, const string_view &sv) {
        size_t max = 0;
        for (const auto &[position, length]: type_position) {
            string_view type = sv.substr(position, length);
            if (length >= 6) { //checking word is 'alignas'
                string alignas_value{type.substr(8, type.length() - 9)};
                size_t tmp = stoi(alignas_value);
                max = std::max(max, tmp);
                continue;
            }
            size_t tmp = type_size.at(type);
            max = std::max(max, tmp);
        }
        return max;
    }
}

namespace cls_06 {
    size_t GetSize(const string &object) {
        string_view sv_object{object};
        auto type_position = ParseTypes(object);
        const map<string_view, size_t> types_sizes{{"char",  1},
                                                   {"short", 2},
                                                   {"int",   4},
                                                   {"long",  8},};
        size_t alignment = LongestType(type_position, types_sizes, sv_object);
        size_t struct_length = alignment;
        size_t remaining_part = alignment;
        size_t offset = 0;

        for (const auto &[position, length]: type_position) {
            string_view value_type = sv_object.substr(position, length);

            if (value_type.length() >= 6) {  //checking word is 'alignas'
                string alignas_value{value_type.substr(8, value_type.length() - 9)};
                offset = stoi(alignas_value);
                continue;
            }

            size_t type_size = types_sizes.at(value_type);
            if (offset != 0) {
                type_size = offset;
                if ((struct_length - remaining_part) % alignment != 0)
                    type_size += (struct_length - remaining_part) % alignment;
                offset = 0;
            }
            if (remaining_part < type_size) {
                struct_length = (struct_length / alignment + 1) * alignment;
                remaining_part = alignment;
            }
            remaining_part -= type_size;
        }
        return struct_length == 0 ? 1 : struct_length;
    }
}