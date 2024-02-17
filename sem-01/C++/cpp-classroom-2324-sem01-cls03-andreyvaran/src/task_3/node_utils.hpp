#pragma once
#include <iostream>


struct Node {
    int value;
    Node* next;
};

// revert
void revert(Node** head);
// cleanup
void cleanup(Node* head);
