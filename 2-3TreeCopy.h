#pragma once
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <vector>

template<class Value>
class Set {
private:
    struct Node {
        Node* parent = nullptr;
        std::vector<Node*> sons;
        std::vector<Value> keys;

        Node() = default;

        Node(Value x) {
            keys.push_back(x);
        }
    };

public:
    Set() = default;

    template <typename IteratorOfInitializerList>
    Set(IteratorOfInitializerList it_begin, IteratorOfInitializerList it_end) {
        root = nullptr;
        size_ = 0;
        for (auto it = it_begin; it != it_end; ++it) {
            insert(*it);
        }
    }

    Set(std::initializer_list<Value> elements) {
        root = nullptr;
        size_ = 0;
        for (const auto &element : elements) {
            insert(element);
        }
    }

    Set(const Set<Value>& other) {
        root = nullptr;
        size_ = 0;
        for (const auto &element : other) {
            insert(element);
        }
    }

    Set& operator=(const Set<Value> &other) {
        if (this == &other) {
            return *this;
        }
        destroy_dfs(root);
        root = nullptr;
        size_ = 0;
        for (const auto &element : other) {
            insert(element);
        }
        return *this;
    }

    ~Set() {
        destroy_dfs(root);
    }

    size_t size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    void insert(const Value& x) {
        Insert(x);
    }

    void erase(const Value& x) {
        Erase(x);
    }

    class iterator {
    public:
        iterator() {
            node = nullptr;
            is_end = true;
        }

        iterator(Node* node_) {
            node = node_;
            if (node == nullptr) {
                is_end = true;
            } else {
                is_end = false;
            }
        }

        iterator(Node* node_, bool flag) {
            node = node_;
            is_end = flag;
        }

        iterator& operator++() {
            Node* node_ = get_next(node, true);
            if (node_ == nullptr) {
                is_end = true;
            } else {
                node = node_;
            }
            return *this;
        }

        iterator operator++(int) {
            Node* node_ = get_next(node, true);
            iterator temp = *this;
            if (node_ == nullptr) {
                is_end = true;
            } else {
                node = node_;
            }
            return temp;
        }

        iterator& operator--() {
            if (is_end) {
                is_end = false;
                return *this;
            }
            Node* node_ = get_prev(node, true);
            if (node_ == nullptr) {
                //is_end = true;
            } else {
                node = node_;
            }
            return *this;
        }

        iterator operator--(int) {
            if (is_end) {
                is_end = false;
                return *this;
            }
            Node* node_ = get_prev(node, true);
            iterator temp = *this;
            if (node_ == nullptr) {
                //is_end = true;
            } else {
                node = node_;
            }
            return temp;
        }

        const Value& operator*() {
            return node->keys[0];
        }

        const Value* operator->() {
            return &(node->keys[0]);
        }

        bool operator==(const iterator &other) const {
            return node == other.node && is_end == other.is_end;
        }

        bool operator!=(const iterator &other) const {
            return node != other.node || is_end != other.is_end;
        }

    private:
        Node* node = nullptr;
        bool is_end = true;

        Node* get_next(Node *t, bool up) {
            if (t->sons.size()==0 && !up) {
                return t;
            }
            if (!up) {
                return get_next(t->sons[0], up);
            } else {
                Node* parent = t->parent;
                if (parent == nullptr) {
                    return parent;
                } else if (parent->sons[0] == t) {
                    return get_next(parent->sons[1], false);
                } else if (parent->sons[1] == t && parent->sons.size() > 2) {
                    return get_next(parent->sons[2], false);
                } else {
                    return get_next(parent, true);
                }
            }
        }

        Node* get_prev(Node *t, bool up) {
            if (t->sons.size()==0 && !up) {
                return t;
            }
            if (!up) {
                return get_prev(t->sons.back(), up);
            } else {
                Node* parent = t->parent;
                if (parent == nullptr) {
                    return parent;
                } else if (parent->sons[1] == t) {
                    return get_prev(parent->sons[0], false);
                } else if (parent->sons.size() > 2 && parent->sons[2] == t) {
                    return get_prev(parent->sons[1], false);
                } else {
                    return get_prev(parent, true);
                }
            }
        }
    };

    iterator begin() const {
        return iterator(get_first());
    }

    iterator end() const {
        return iterator(get_last(), true);
    }

    iterator find(const Value& x) const {
        Node* it = search(x);
        if (it == nullptr ||  ( (it->keys[0] < x) || (x < it->keys[0])  ) /*it->keys[0] != x*/) {
            return end();
        } else {
            return iterator(it);
        }
    }

    iterator lower_bound(const Value& x) const {
        Node* it = search(x);
        if (it == nullptr || it->keys[0] < x) {
            return end();
        } else {
            return iterator(it);
        }
    }


private:
    //23Tree
    Node* root = nullptr;
    size_t size_ = 0;

    bool is_leaf(Node *t) const {
        if (t == nullptr) {
            return true;
        }
        return t->sons.size() == 0;
    }

    void update_keys(Node* t) {
        if (t == nullptr) {
            return;
        }
        if (is_leaf(t)) {
            t = t->parent;
        }
        while (t != nullptr) {
            for (size_t i = 0; i < t->sons.size(); i++) {
                t->keys[i] = t->sons[i]->keys.back();
            }
            if (t->sons.size() == 2) {
                if (t->keys[1] < t->keys[0]) {
                    std::swap(t->keys[0], t->keys[1]);
                    std::swap(t->sons[0], t->sons[1]);
                }
            } else if (t->sons.size() == 3) {
                if (t->keys[1] < t->keys[0]) {
                    std::swap(t->keys[0], t->keys[1]);
                    std::swap(t->sons[0], t->sons[1]);
                }
                if (t->keys[2] < t->keys[0]) {
                    std::swap(t->keys[0], t->keys[2]);
                    std::swap(t->sons[0], t->sons[2]);
                }
                if (t->keys[2] < t->keys[1]) {
                    std::swap(t->keys[2], t->keys[1]);
                    std::swap(t->sons[2], t->sons[1]);
                }
            } else {
                for (size_t i = 0; i < t->sons.size(); i++) {
                    for (size_t j = i+1; j < t->sons.size(); j++) {
                        if (t->keys[j] < t->keys[i]) {
                            std::swap(t->keys[j], t->keys[i]);
                            std::swap(t->sons[j], t->sons[i]);
                        }
                    }
                }
            }
            t = t->parent;
        }
    }

    void sort_children(Node* t) {
        if (t == nullptr) {
            return;
        }
        for (size_t i = 0; i < t->sons.size(); i++) {
            for (size_t j = i+1; j < t->sons.size(); j++) {
                if (t->keys[j] < t->keys[i]) {
                    std::swap(t->keys[j], t->keys[i]);
                    std::swap(t->sons[j], t->sons[i]);
                }
            }
        }
    }

    Node* get_brother(Node* t) {
        if (t == nullptr) {
            return nullptr;
        } else if (t->parent == nullptr) {
            return nullptr;
        } else {
            if (t->parent->sons[1] == t) {
                return t->parent->sons[0];
            } else {
                return t->parent->sons[1];
            }
        }
    }

    void add_to_children(Node* t, Node* to_add) {
        auto it1 = t->sons.begin();
        auto it2 = t->keys.begin();
        while (it1 != t->sons.end() && (*it2) < to_add->keys.back()) {
            it1++;
            it2++;
        }
        t->sons.insert(it1, to_add);
        t->keys.insert(it2, to_add->keys.back());
        to_add->parent = t;
    }

    void delete_children(Node* t, Node* to_delete) {
        if (to_delete == nullptr) {
            return;
        }
        auto it1 = t->sons.begin();
        auto it2 = t->keys.begin();
        while (it1 != t->sons.end() && (*it1) != to_delete) {
            it1++;
            it2++;
        }
        t->sons.erase(it1);
        t->keys.erase(it2);
        to_delete->parent = nullptr;
    }

    Node* search(Value x) const {
        Node* t = root;
        if (t == nullptr) {
            return t;
        }
        while (!t->sons.empty()) {
            if (t->sons.size() == 2) {
                if (t->keys[0] < x) {
                    t = t->sons[1];
                } else {
                    t = t->sons[0];
                }
            } else if (t->keys[1] < x) {
                t = t->sons[2];
            } else if (t->keys[0] < x) {
                t = t->sons[1];
            } else {
                t = t->sons[0];
            }
        }
        return t;
    }

    void split(Node* t) {
        if (t->sons.size() <= 3) {
            return;
        }
        Node* right = new Node();
        right->keys = {t->keys[2], t->keys[3]};
        right->sons = {t->sons[2], t->sons[3]};
        right->sons[0]->parent = right;
        right->sons[1]->parent = right;
        right->parent = t->parent;
        Node* left = new Node();
        left->keys = {t->keys[0], t->keys[1]};
        left->sons = {t->sons[0], t->sons[1]};
        left->sons[0]->parent = left;
        left->sons[1]->parent = left;
        left->parent = t->parent;
        if (left->parent == nullptr) {
            delete t;
            root = new Node();
            left->parent = root;
            right->parent = root;
            root->sons = {left, right};
            root->keys = {left->keys.back(), right->keys.back()};
        } else {
            delete_children(t->parent, t);
            delete t;
            Node* parent = left->parent;
            add_to_children(parent, left);
            add_to_children(parent, right);
            split(parent);
            update_keys(left);
        }
    }

    void Insert(Value x) {
        Node* to_add = new Node(x);
        size_++;
        if (root == nullptr) {
            root = to_add;
            return;
        }
        Node* t = search(x);
        if ( !(t->keys[0] < x) && !(x < t->keys[0])) {
            size_--;
            delete to_add;
            return;
        }
        if (t->parent == nullptr) {
            Node* temp = root;
            root = new Node();
            add_to_children(root, to_add);
            add_to_children(root, temp);
            //update_keys(root);
            return;
        }
        Node* parent = t->parent;
        add_to_children(parent, to_add);
        //update_keys(to_add);
        //update_keys(parent);
        split(parent);
        //update_keys(parent);
        update_keys(to_add);
    }

    void erase(Node *t) {
        if (t == nullptr) {
            return;
        }
        if (t == root) {
            delete t;
            root = nullptr;
            return;
        }
        Node* parent = t->parent;
        if (parent->sons.size() == 3) {
            delete_children(parent, t);
            update_keys(parent);
            delete t;
            return;
        }
        Node* brother_of_parent = get_brother(parent);
        if (brother_of_parent == nullptr) {
            Node* temp = root;
            if (root->sons[0] == t) {
                root = root->sons[1];
                delete temp;
                delete t;
            } else {
                root = root->sons[0];
                delete temp;
                delete t;
            }
            root->parent = nullptr;
            return;
        }
        Node* brother = get_brother(t);
        delete_children(parent, t);
        delete_children(parent, brother);
        add_to_children(brother_of_parent, brother);
        delete t;
        split(brother_of_parent);
        update_keys(brother);
        erase(parent);
    }

    void Erase(Value x) {
        Node* t = search(x);
        if (t == nullptr || t->keys[0] < x || x < t->keys[0]) {
            return;
        }
        size_--;
        erase(t);
        if (size_ == 0) {
            root = nullptr;
        }
    }

    Node* get_first() const {
        Node* t = root;
        if (t == nullptr) {
            return t;
        }
        while (!is_leaf(t)) {
            t = t->sons[0];
        }
        return t;
    }

    Node* get_last() const {
        Node* t = root;
        if (t == nullptr) {
            return t;
        }
        while (!is_leaf(t)) {
            t = t->sons.back();
        }
        return t;
    }

    void destroy_dfs(Node* t) {
        if (t == nullptr) {
            return;
        }
        while(!t->sons.empty()) {
            destroy_dfs(t->sons.back());
            t->sons.pop_back();
        }
        delete t;
    }
};
