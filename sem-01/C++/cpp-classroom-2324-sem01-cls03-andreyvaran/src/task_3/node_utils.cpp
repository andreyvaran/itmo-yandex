#include <iostream>
#include "node_utils.hpp"

void revert(Node** head) {
    Node* prev = nullptr;
    Node* current = *head;
    Node* next;

    while (current != nullptr) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    *head = prev;
}

void cleanup(Node* head) {
    while (head != nullptr) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
}
