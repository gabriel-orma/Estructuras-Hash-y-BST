#include <iostream>
#include <vector>
using namespace std;

template <class T>
struct BST {
    struct Node {
        T key;
        int sz = 1;
        Node* left = nullptr;
        Node* right = nullptr;
        Node* parent = nullptr;
        Node(const T& k) : key(k) {}
    };

    Node* root = nullptr;

    BST() {}

    ~BST() {
        vector<Node*> stack;
        if (root) stack.push_back(root);
        while (!stack.empty()) {
            Node* x = stack.back(); stack.pop_back();
            if (x->left) stack.push_back(x->left);
            if (x->right) stack.push_back(x->right);
            delete x;
        }
    }

    BST(const BST&) = delete;
    BST& operator=(const BST&) = delete;

    int sz(Node* x) { return x ? x->sz : 0; }

    void update(Node* x) {
        while (x != nullptr) {
            x->sz = 1 + sz(x->left) + sz(x->right);
            x = x->parent;
        }
    }

    Node* minimum(Node* x) {
        if (x == nullptr) return nullptr;
        while (x->left != nullptr) x = x->left;
        return x;
    }

    Node* maximum(Node* x) {
        if (x == nullptr) return nullptr;
        while (x->right != nullptr) x = x->right;
        return x;
    }

    Node* successor(Node* x) {
        if (x->right != nullptr) return minimum(x->right);
        Node* y = x->parent;
        while (y != nullptr && x == y->right) { x = y; y = y->parent; }
        return y;
    }

    Node* predecessor(Node* x) {
        if (x->left != nullptr) return maximum(x->left);
        Node* y = x->parent;
        while (y != nullptr && x == y->left) { x = y; y = y->parent; }
        return y;
    }

    void transplant(Node* u, Node* v) {
        if (u->parent == nullptr) root = v;
        else if (u == u->parent->left) u->parent->left = v;
        else u->parent->right = v;
        if (v != nullptr) v->parent = u->parent;
    }

    int size() { return sz(root); }

    Node* find(const T& k) {
        Node* x = root;
        while (x != nullptr && k != x->key)
            x = (k < x->key) ? x->left : x->right;
        return x;
    }

    bool contains(const T& k) { return find(k) != nullptr; }

    void insert(const T& k) {
        Node* y = nullptr;
        Node* x = root;
        while (x != nullptr) {
            y = x;
            x = (k < x->key) ? x->left : x->right;
        }
        Node* z = new Node(k);
        z->parent = y;
        if (y == nullptr) root = z;
        else if (k < y->key) y->left = z;
        else y->right = z;
        update(y);
    }

    void eraseNode(Node* z) {
        Node* from;
        if (z->left == nullptr) {
            from = z->parent;
            transplant(z, z->right);
        } else if (z->right == nullptr) {
            from = z->parent;
            transplant(z, z->left);
        } else {
            Node* y = minimum(z->right);
            if (y->parent != z) {
                from = y->parent;
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            } else {
                from = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
        }
        delete z;
        update(from);
    }

    bool erase(const T& k) {
        Node* z = find(k);
        if (z == nullptr) return false;
        eraseNode(z);
        return true;
    }

    Node* lowerBound(const T& k) {
        Node* x = root, *res = nullptr;
        while (x != nullptr) {
            if (!(x->key < k)) { res = x; x = x->left; }
            else x = x->right;
        }
        return res;
    }

    Node* upperBound(const T& k) {
        Node* x = root, *res = nullptr;
        while (x != nullptr) {
            if (k < x->key) { res = x; x = x->left; }
            else x = x->right;
        }
        return res;
    }

    Node* floor(const T& k) {
        Node* x = root, *res = nullptr;
        while (x != nullptr) {
            if (!(k < x->key)) { res = x; x = x->right; }
            else x = x->left;
        }
        return res;
    }

    Node* lower(const T& k) {
        Node* x = root, *res = nullptr;
        while (x != nullptr) {
            if (x->key < k) { res = x; x = x->right; }
            else x = x->left;
        }
        return res;
    }

    int countLess(const T& k) {
        Node* x = root;
        int r = 0;
        while (x != nullptr) {
            if (x->key < k) { r += sz(x->left) + 1; x = x->right; }
            else x = x->left;
        }
        return r;
    }

    Node* kth(int k) {
        Node* x = root;
        while (x != nullptr) {
            int t = sz(x->left);
            if (k == t + 1) return x;
            if (k <= t) x = x->left;
            else { k -= t + 1; x = x->right; }
        }
        return nullptr;
    }

    template <class F>
    void inorder(F f) {
        for (Node* x = minimum(root); x != nullptr; x = successor(x))
            f(x->key);
    }
};