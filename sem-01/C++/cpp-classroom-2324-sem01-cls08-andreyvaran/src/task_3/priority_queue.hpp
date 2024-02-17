#include <vector>
#include <algorithm>
#pragma once

namespace cls08 {

    template <typename T, typename Comparator = std::less<T>>
    class priority_queue {
    private:
        std::vector<T> data;
        Comparator comp;

    public:
        priority_queue() = default;

        priority_queue(const priority_queue& other) : data(other.data), comp(other.comp) {
            std::make_heap(data.begin(), data.end(), comp);
        }

        priority_queue& operator=(const priority_queue& other) {
            if (this != &other) {
                data = other.data;
                comp = other.comp;
                std::make_heap(data.begin(), data.end(), comp);
            }
            return *this;
        }

        ~priority_queue() = default;

        void push(const T& value) {
            data.push_back(value);
            std::push_heap(data.begin(), data.end(), comp);
        }

        void pop() {
            std::pop_heap(data.begin(), data.end(), comp);
            data.pop_back();
        }

        const T& front() const {
            return data.front();
        }

        size_t size() const {
            return data.size();
        }

        bool empty() const {
            return data.empty();
        }
    };

}
