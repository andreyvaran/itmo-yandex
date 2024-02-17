#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace cls_06 {

    class my_ostream {
    public:
        virtual my_ostream &operator<<(int value) = 0;

        virtual my_ostream &operator<<(double value) = 0;

        virtual my_ostream &operator<<(const std::string &value) = 0;
    };

    class my_ostream_con : public my_ostream {
    public:
        my_ostream_con() : output_stream(std::cout) {}

        my_ostream &operator<<(int value) override {
            output_stream << value;
            return *this;
        }

        my_ostream &operator<<(double value) override {
            output_stream << value;
            return *this;
        }

        my_ostream &operator<<(const std::string &value) override {
            output_stream << value;
            return *this;
        }

    private:
        std::ostream &output_stream;
    };

    class my_ostream_file : public my_ostream {
    public:
        explicit my_ostream_file(const std::string &filename) : output_file(filename) {
            if (!output_file.is_open()) {
                throw std::runtime_error("Failed to open file: " + filename);
            }
        }

        my_ostream &operator<<(int value) override {
            output_file << value;
            return *this;
        }

        my_ostream &operator<<(double value) override {
            output_file << value;
            return *this;
        }

        my_ostream &operator<<(const std::string &value) override {
            output_file << value;
            return *this;
        }

        ~my_ostream_file() {
            if (output_file.is_open()) {
                output_file.close();
            }
        }

    private:
        std::ofstream output_file;
    };

    class my_ostream_combo : public my_ostream {
    public:

        my_ostream_combo(my_ostream *stream1, my_ostream *stream2)
                : stream1(stream1), stream2(stream2) {}

        my_ostream &operator<<(int value) override {
            *stream1 << value;
            *stream2 << value;
            return *this;
        }

        my_ostream &operator<<(double value) override {
            *stream1 << value;
            *stream2 << value;
            return *this;
        }

        my_ostream &operator<<(const std::string &value) override {
            *stream1 << value;
            *stream2 << value;
            return *this;
        }

        my_ostream_combo(const my_ostream_combo &other)
                : stream1(other.stream1), stream2(other.stream2) {}

        ~my_ostream_combo() {
            delete stream1;
            delete stream2;
        }

    private:
        my_ostream *stream1;
        my_ostream *stream2;
    };
}
