#ifndef NETWORKING_TRIE_H
#define NETWORKING_TRIE_H

#include <string>
using namespace std;

template<typename TYPE,
        size_t RANGE, // max number of children
        size_t (*ASSIGN_INDEX)(TYPE val), // function that assigns indexes in the range [0, RANGE) to values
        typename KEY // key stored at each node
>
struct trie_node { // a trie node, representing an entire tree
    bool marked = false;

    trie_node *children[RANGE] = {nullptr};
    size_t child_count = 0;

    KEY key;

    virtual ~trie_node() {
        remove_all_children();
    }

    trie_node *get_child(TYPE val) const {
        return children[ASSIGN_INDEX(val)];
    }

    trie_node *create_child(TYPE val) {
        size_t in = ASSIGN_INDEX(val);

        if (children[in] != nullptr) return children[in]; // child already exists

        children[in] = new trie_node;
        child_count++;

        return children[in];
    }

    void remove_child(TYPE val) {
        size_t in = ASSIGN_INDEX(val);

        if (children[in] == nullptr) return; // no such child

        delete children[in]; // deconstructor will call child->remove_all_children()
        children[in] = nullptr;
        child_count--;
    }

    void remove_all_children() {
        for (size_t i = 0; i < RANGE; i++) {
            trie_node *child = children[i];
            if (child == nullptr) return; // child doesn't exists

            delete child; // deconstructor will call child->remove_all_children()
        }
    }

    trie_node *search(const basic_string<TYPE> &str) {
        trie_node *curr_node = this;
        for (char c: str) {
            trie_node *child = curr_node->get_child(c);
            if (child == nullptr) return nullptr; // word doesn't exists

            curr_node = child;
        }

        return curr_node;
    }

    trie_node * add_word(const basic_string<TYPE> &str) {
        if (str.empty()) return nullptr;

        trie_node *curr_node = this;
        for (char c: str) {
            trie_node *child = curr_node->get_child(c);
            if (child == nullptr) child = curr_node->create_child(c); // node should be added

            curr_node = child;
        }

        curr_node->marked = true;
        return curr_node;
    }

    void remove_word(const basic_string<TYPE> &str) {
        if (str.empty()) return;

        // we would want to remove the final node representing the entire word,
        // and any other node on the path contributes only to the removed word
        trie_node *lowest_ans_par = this; // represents the parent of the lowest ancestor which contributes only to the removed word
        char lowest_ans_val = str[0]; // represents the value that points from lowest_ans_par to our ancestor

        trie_node *curr_node = this;
        for (char c: str) {
            trie_node *child = curr_node->get_child(c);
            if (child == nullptr) return; // word doesn't exists

            if (curr_node->marked || curr_node->count_children() > 1) { // this node contributes to other words besides ours
                lowest_ans_par = curr_node;
                lowest_ans_val = c;
            }

            curr_node = child;
        }

        if (!curr_node->marked) return; // word doesn't exists

        lowest_ans_par->remove_child(lowest_ans_val); // remove our lowest valid ancestor
    }

    // does the trie contain this word
    bool contains_word(const basic_string<TYPE> &str) {
        trie_node *node = search(str);
        return node != nullptr && node->marked;
    }

    // does the trie contain some word which has this prefix
    bool is_prefix(const basic_string<TYPE> &str) {
        return search(str) != nullptr;
    }

    // does the trie contain some word which is a prefix of str
    bool contains_prefix(const basic_string<TYPE> &str) {
        trie_node *curr_node = this;
        for (char c: str) {
            trie_node *child = curr_node->get_child(c);
            if (child == nullptr) return false;

            curr_node = child;
            if (curr_node->marked) return true; // prefix exists
        }

        return false;
    }

};



#endif //NETWORKING_TRIE_H
