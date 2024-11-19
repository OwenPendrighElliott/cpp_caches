#pragma once
#include "cache_interface.hpp"
#include <string>
#include <unordered_map>
#include <mutex>

template <typename K, typename V>
class LRUCache : public CacheInterface<K, V> {
private:
    struct ListNode {
        K key;
        std::shared_ptr<V> value;
        ListNode* prev;
        ListNode* next;

        ListNode(K key, std::shared_ptr<V> value) 
            : key(key), value(value), prev(nullptr), next(nullptr) {}
    };

    int capacity;
    std::unordered_map<K, ListNode*> keyValueMap;
    ListNode* head;
    ListNode* tail;
    int hits;
    int misses;

    std::mutex cacheMutex;

    void removeNode(ListNode* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void addNode(ListNode* node) {
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
    }

    void moveToHead(ListNode* node) {
        removeNode(node);
        addNode(node);
    }

    void removeLRUItem() {
        ListNode* lruNode = tail->prev;
        removeNode(lruNode);
        keyValueMap.erase(lruNode->key);
        delete lruNode;
    }

public:
    LRUCache(int capacity) : capacity(capacity), hits(0), misses(0) {
        head = new ListNode(K{}, nullptr);
        tail = new ListNode(K{}, nullptr);
        head->next = tail;
        tail->prev = head;
    }

    std::shared_ptr<V> get(const K& key) {
        std::lock_guard<std::mutex> lock(cacheMutex);

        auto it = keyValueMap.find(key);

        if (it != keyValueMap.end()) {
            moveToHead(it->second);
            hits++;
            return it->second->value;
        }
        misses++;
        return nullptr;
    }

    void put(const K& key, const V& value) {
        std::lock_guard<std::mutex> lock(cacheMutex);

        auto it = keyValueMap.find(key);

        if (it != keyValueMap.end()) {
            it->second->value = std::make_shared<V>(value);
            moveToHead(it->second);
        } else {
            if (keyValueMap.size() >= capacity) {
                removeLRUItem();
            }
            ListNode* newNode = new ListNode(key, std::make_shared<V>(value));
            keyValueMap[key] = newNode;
            addNode(newNode);
        }
    }

    void clear() {
        std::lock_guard<std::mutex> lock(cacheMutex);
        for (auto& pair : keyValueMap) {
            delete pair.second;
        }
        keyValueMap.clear();
        head->next = tail;
        tail->prev = head;
        hits = 0;
        misses = 0;
    }

    void changeCapacity(int newCapacity) {
        std::lock_guard<std::mutex> lock(cacheMutex);
        capacity = newCapacity;
        while (keyValueMap.size() > capacity) {
            removeLRUItem();
        }
    }

    std::unordered_map<std::string, int> getStats() {
        std::unordered_map<std::string, int> stats;
        stats["hits"] = hits;
        stats["misses"] = misses;
        stats["capacity"] = capacity;
        stats["size"] = keyValueMap.size();
        return stats;
    }

    ~LRUCache() {
        clear();
        delete head;
        delete tail;
    }
};