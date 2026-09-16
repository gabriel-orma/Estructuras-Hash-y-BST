#include <iostream>
#include <string>
#include <chrono>
#include <cstdint>
using namespace std;

const uint64_t SEED = chrono::steady_clock::now().time_since_epoch().count();

uint64_t splitmix64(uint64_t x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

uint64_t hashKey(long long x) { return splitmix64(x + SEED); }

uint64_t hashKey(const string& s) {
    uint64_t h = 0;
    for (char c : s) h = h * 131 + c;
    return splitmix64(h + SEED);
}

template <class K, class V>
struct HashMap {
    struct Node {
        K key;
        V value;
        Node* next;
        Node(const K& k, const V& v, Node* nx) : key(k), value(v), next(nx) {}
    };

    Node** A;
    int m;
    int n;

    HashMap(int capacity = 8) {
        m = 8;
        while (m * 3 / 4 < capacity) m *= 2;
        n = 0;
        A = new Node*[m]();
    }

    ~HashMap() {
        for (int i = 0; i < m; i++) {
            Node* t = A[i];
            while (t != nullptr) { Node* s = t->next; delete t; t = s; }
        }
        delete[] A;
    }

    HashMap(const HashMap&) = delete;
    HashMap& operator=(const HashMap&) = delete;

    int index(const K& k) { return hashKey(k) % m; }

    Node* findNode(const K& k) {
        for (Node* t = A[index(k)]; t != nullptr; t = t->next)
            if (t->key == k) return t;
        return nullptr;
    }

    void rehash() {
        int oldM = m;
        m *= 2;
        Node** B = new Node*[m]();
        for (int i = 0; i < oldM; i++) {
            Node* t = A[i];
            while (t != nullptr) {
                Node* next = t->next;
                int j = index(t->key);
                t->next = B[j];
                B[j] = t;
                t = next;
            }
        }
        delete[] A;
        A = B;
    }

    V& operator[](const K& k) {
        if (Node* p = findNode(k)) return p->value;
        if (n + 1 > m * 3 / 4) rehash();
        int i = index(k);
        A[i] = new Node(k, V(), A[i]);
        n++;
        return A[i]->value;
    }

    bool contains(const K& k) { return findNode(k) != nullptr; }

    V get(const K& k, V defaultValue = V()) {
        Node* p = findNode(k);
        return p ? p->value : defaultValue;
    }

    void erase(const K& k) {
        int i = index(k);
        Node* t = A[i], *prev = nullptr;
        while (t != nullptr) {
            if (t->key == k) {
                if (prev == nullptr) A[i] = t->next;
                else prev->next = t->next;
                delete t;
                n--;
                return;
            }
            prev = t;
            t = t->next;
        }
    }

    int size() { return n; }

    template <class F>
    void forEach(F f) {
        for (int i = 0; i < m; i++)
            for (Node* t = A[i]; t != nullptr; t = t->next)
                f(t->key, t->value);
    }
};