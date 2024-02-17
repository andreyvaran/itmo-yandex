#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <type_traits>

namespace exam {

    template<typename T>
    class set {
    public:
        using key_type = T;
        using value_type = T;
        using size_type = std::size_t;

        class element {
        public:
            using value_type = T;
            using reference = const T &;
            using const_reference = const T &;
            using pointer = const T *;
            using const_pointer = const T *;

            element() : node_(nullptr) {}

            element(const element &other) : node_(other.node_) {}

            element &operator=(const element &other) {
                if (this != &other) {
                    node_ = other.node_;
                }
                return *this;
            }

            const_reference operator*() const { return node_->value; }

            const_pointer operator->() const { return &(node_->value); }

            bool operator==(const element &other) const { return node_ == other.node_; }

            bool operator!=(const element &other) const { return node_ != other.node_; }

        private:
            struct Node {
                T value;
                std::unique_ptr<Node> left;
                std::unique_ptr<Node> right;
                Node *parent;

                Node(T val) : value(std::move(val)), left(nullptr), right(nullptr), parent(nullptr) {}
            };

            Node *node_;

            explicit element(Node *node) : node_(node) {}

            friend class set<T>;
        };

        set() : root_(nullptr), size_(0) {}

        bool empty() const { return size_ == 0; }

        size_type size() const { return size_; }

        void clear() {
            root_.reset();
            size_ = 0;
        }

        element end() const { return element(nullptr); }

        element find(const key_type &key) const {
            Node *current = root_.get();
            while (current) {
                if (key < current->value)
                    current = current->left.get();
                else if (current->value < key)
                    current = current->right.get();
                else
                    return element(current);
            }
            return end();
        }

        std::pair<element, bool> insert(const value_type &value) {
            if (!root_) {
                //error: use of deleted function ‘constexpr _test__basic__moveable_key__func()::Key::Key(const _test__basic__moveable_key__func()::Key&)’
                //  962 |     { return unique_ptr<_Tp>(new _Tp(std::forward<_Args>(__args)...)); }
                root_ = std::make_unique<Node>(T(value));
                size_++;
                return {element(root_.get()), true};
            }

            Node *current = root_.get();
            Node *parent = nullptr;

            while (current) {
                if (!(value < current->value) && !(current->value < value)) {
                    return {element(current), false};
                }
                parent = current;
                current = (value < current->value) ? current->left.get() : current->right.get();
            }

            auto newNode = std::make_unique<Node>(value);
            Node *newNodePtr = newNode.get();
            if (value < parent->value) {
                parent->left = std::move(newNode);
            } else {
                parent->right = std::move(newNode);
            }

            newNodePtr->parent = parent;
            size_++;
            return {element(newNodePtr), true};
        }

        std::pair<element, bool> insert(value_type &&value) {
            if (!root_) {
                root_ = std::make_unique<Node>(std::move(value));
                size_++;
                return {element(root_.get()), true};
            }

            Node *current = root_.get();
            Node *parent = nullptr;

            while (current) {
                //  ==
                if (!(value < current->value) && !(current->value < value)) {
                    return {element(current), false};
                }
                parent = current;
                current = (value < current->value) ? current->left.get() : current->right.get();
            }

            auto newNode = std::make_unique<Node>(std::move(value));
            Node *newNodePtr = newNode.get();
            if (value < parent->value) {
                parent->left = std::move(newNode);
            } else {
                parent->right = std::move(newNode);
            }

            newNodePtr->parent = parent;
            size_++;
            return {element(newNodePtr), true};
        }

        friend void swap(set &a, set &b) {
            using std::swap;
            swap(a.root_, b.root_);
            swap(a.size_, b.size_);
        }

        set(const set &other) : root_(nullptr), size_(0) {
            copyTree(other.root_);
        }

        set &operator=(const set &other) {
            if (this != &other) {
                clear();
                copyTree(other.root_);
            }
            return *this;
        }

        set(set &&other) noexcept: root_(std::move(other.root_)), size_(other.size_) {
            other.size_ = 0;
        }

        set &operator=(set &&other) noexcept {
            if (this != &other) {
                clear();
                root_ = std::move(other.root_);
                size_ = other.size_;
                other.size_ = 0;
            }
            return *this;
        }

    private:
        using Node = typename element::Node;

        std::unique_ptr<Node> root_;
        size_type size_;

        void copyTree(const std::unique_ptr<Node> &node) {
            if (node) {
                insert(node->value);
                copyTree(node->left);
                copyTree(node->right);
            }
        }


    };

//    template<typename T>
//    void swap(set<T>& a, set<T>& b) {
//        using std::swap;
//        swap(a.root_, b.root_);
//        swap(a.size_, b.size_);
//    }

}

