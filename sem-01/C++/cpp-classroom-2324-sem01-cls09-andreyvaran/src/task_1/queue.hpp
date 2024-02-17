#pragma once

#include <utility>
#include <cstddef>

namespace cls09 {

    template<typename T>
    class ListNode {
    public:
        T value;
        ListNode *next;

        ListNode(const T &value) : value(value), next(nullptr) {}

        ListNode(T &&value) : value(std::move(value)), next(nullptr) {}
    };

    template<typename T>
    class LinkedList {
    public:
        ListNode<T> *head;

        LinkedList() : head(nullptr) {}

        LinkedList(const LinkedList &other) : head(nullptr) {
            ListNode<T> **ptr = &head;
            for (ListNode<T> *curr = other.head; curr != nullptr; curr = curr->next) {
                *ptr = new ListNode<T>(curr->value);
                ptr = &((*ptr)->next);
            }
        }

        LinkedList(LinkedList &&other) noexcept: head(other.head) {
            other.head = nullptr;
        }

        LinkedList &operator=(const LinkedList &other) {
            if (this != &other) {
                LinkedList tmp(other);
                std::swap(head, tmp.head);
            }
            return *this;
        }

        LinkedList &operator=(LinkedList &&other) noexcept {
            if (this != &other) {
                clear();
                std::swap(head, other.head);
            }
            return *this;
        }

        ~LinkedList() {
            clear();
        }

        void push_back(const T &value) {
            ListNode<T> **ptr = &head;
            while (*ptr) {
                ptr = &((*ptr)->next);
            }
            *ptr = new ListNode<T>(value);
        }

        void push_back(T &&value) {
            ListNode<T> **ptr = &head;
            while (*ptr) {
                ptr = &((*ptr)->next);
            }
            *ptr = new ListNode<T>(std::move(value));
        }

        void pop_front() {
            if (head) {
                ListNode<T> *tmp = head;
                head = head->next;
                delete tmp;
            }
        }

        size_t size() const {
            size_t count = 0;
            for (ListNode<T> *curr = head; curr != nullptr; curr = curr->next) {
                ++count;
            }
            return count;
        }

    private:
        void clear() {
            while (head) {
                pop_front();
            }
        }
    };


    template<typename T>
    class Queue {
    private:
        LinkedList<T> list;

    public:
        Queue() = default;

        Queue(const Queue &other) {
            LinkedList<T> temp_list;
            try {
                for (ListNode<T> *curr = other.list.head; curr != nullptr; curr = curr->next) {
                    temp_list.push_back(curr->value);
                }
            } catch (...) {
                throw;
            }
            list = std::move(temp_list);
        }

        Queue(Queue &&other) noexcept: list(std::move(other.list)) {}

        Queue &operator=(const Queue &other) {
            if (this != &other) {
                Queue temp(other);
                std::swap(list, temp.list);
            }
            return *this;
        }

        Queue &operator=(Queue &&other) noexcept {
            if (this != &other) {
                list = std::move(other.list);
            }
            return *this;
        }

        ~Queue() = default;

        void push(const T &value) {
            list.push_back(value);
        }

        void push(T &&value) {
            list.push_back(std::move(value));
        }

        void pop() {
            list.pop_front();
        }

        T &front() {
            return list.head->value;
        }

        const T &front() const {
            return list.head->value;
        }

        size_t size() const {
            return list.size();
        }


    };

}




