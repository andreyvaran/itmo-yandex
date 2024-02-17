#include "network.hpp"
#include <cstring> // Для работы со строками
#include <cstdint>

static uint64_t get_current_time() {
    static std::uint64_t current_time = 0;
    return current_time++;
}
//
Post *create_post(char const *user, char const *text, Post *head) {
    Post *new_post = new Post;
    new_post->user = new char[strlen(user) + 1];
    strcpy(const_cast<char *>(new_post->user), user);

    new_post->text = new char[strlen(text) + 1];
    strcpy(const_cast<char *>(new_post->text), text);
    new_post->timestamp = get_current_time();
    new_post->previous = head;


    return new_post;
}

void destroy_posts(Post const *head) {
    while (head) {
        Post const *next = head->previous;
        delete[] head->user;
        delete[] head->text;

        delete head;
        head = next;
    }
}

Post *previous(Post const *post) {
    return post ? post->previous : nullptr;
}

char const *get_user(Post const *post) {
    return post ? post->user : nullptr;
}

char const *get_text(Post const *post) {
    return post ? post->text : nullptr;
}

Post *remove_by_content(Post *head, char const *content) {
    Post *current = head;
    Post *previous = nullptr;
    Post *new_head = nullptr;

    while (current != nullptr) {
        if (std::strstr(current->text, content) == nullptr) {
            if (previous == nullptr) {
                new_head = current;
            } else {
                previous->previous = current;
            }
            previous = current;
            Post *next = current->previous;
            current = next;
        } else {
            Post *next = current->previous;
            delete[] current->user;
            delete[] current->text;
            delete current;
            current = next;
        }
    }

    if (previous != nullptr) {
        previous->previous = nullptr;
    }

    return new_head;
}


std::uint64_t timestamp(Post const *post) {
    if (post != nullptr) {
        return post->timestamp;
    } else {
        return 0;
    }
}

Post *merge(Post *head_1, Post *head_2) {
    Post *merged_head = nullptr;
    Post *current_1 = head_1;
    Post *current_2 = head_2;
    Post *prev_merged = nullptr;

    while (current_1 != nullptr || current_2 != nullptr) {
        if (current_1 == nullptr) {
            if (prev_merged == nullptr) {
                merged_head = current_2;
            } else {
                prev_merged->previous = current_2;
            }
            prev_merged = current_2;
            current_2 = current_2->previous;
        } else if (current_2 == nullptr) {
            if (prev_merged == nullptr) {
                merged_head = current_1;
            } else {
                prev_merged->previous = current_1;
            }
            prev_merged = current_1;
            current_1 = current_1->previous;
        } else {
            if (current_1->timestamp > current_2->timestamp) {
                if (prev_merged == nullptr) {
                    merged_head = current_1;
                } else {
                    prev_merged->previous = current_1;
                }
                prev_merged = current_1;
                current_1 = current_1->previous;
            } else {
                if (prev_merged == nullptr) {
                    merged_head = current_2;
                } else {
                    prev_merged->previous = current_2;
                }
                prev_merged = current_2;
                current_2 = current_2->previous;
            }
        }
    }

    if (prev_merged != nullptr) {
        prev_merged->previous = nullptr;
    }

    return merged_head;
}


void set_text(Post *post, char const *new_text) {
    if (post != nullptr && new_text != nullptr) {
        if (post->text != nullptr) {
            delete[] post->text;
        }

        post->text = new char[strlen(new_text) + 1];
        std::strcpy(const_cast<char *>(post->text), new_text);
    }
}

void replace_with_stars(Post *post, char const *pattern) {
    size_t pattern_length = strlen(pattern);
    while (post) {
        char *found = strstr(const_cast<char *>(post->text), pattern);
        while (found) {
            for (size_t i = 0; i < pattern_length; ++i) {
                found[i] = '*';
            }
            found = strstr(found + pattern_length, pattern);
        }
        post = post->previous;
    }
}
//char const *get_user(Post const *post) {
//    return post ? post->user : nullptr;
//}
//
//char const *get_text(Post const *post) {
//    return post ? post->text : nullptr;
//}

//Post *remove_by_content(Post *head, char const *content) {
//    Post *current = head;
//    Post *previous = nullptr;
//    Post *new_head = nullptr;
//
//    while (current != nullptr) {
//        if (std::strstr(current->text, content) == nullptr) {
//            if (previous == nullptr) {
//                new_head = current;
//            } else {
//                previous->previous = current;
//            }
//            previous = current;
//            Post *next = current->previous;
//            current = next;
//        } else {
//            Post *next = current->previous;
//            delete[] current->user;
//            delete[] current->text;
//            delete current;
//            current = next;
//        }
//    }
//
//    if (previous != nullptr) {
//        previous->previous = nullptr;
//    }
//
//    return new_head;
//}