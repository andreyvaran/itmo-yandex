#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <cstdint>
//t1 -
struct Post {
    char const *user;
    char const *text;
    std::uint64_t timestamp;
    Post *previous;
};

Post *create_post(char const *user, char const *text, Post *head = nullptr);

void destroy_posts(Post const *head);

Post *previous(Post const *post);

char const *get_user(Post const *post);

char const *get_text(Post const *post);
//t2 -
Post *remove_by_content(Post *head, char const *content);
//t3 -
std::uint64_t timestamp(Post const *post);

Post *merge(Post *head_1, Post *head_2);
//t4 -
void set_text(Post *post, char const *new_text);
//t5 -
void replace_with_stars(Post *post, char const *pattern);

#endif // NETWORK_HPP
