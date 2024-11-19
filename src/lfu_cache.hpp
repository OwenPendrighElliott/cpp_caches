#pragma once
#include "cache_interface.hpp"
#include <string>
#include <unordered_map>
#include <mutex>



template <typename K, typename V>
class LFUCache : public CacheInterface<K, V> {
private:
    typedef struct ListNode {
        K key;
        std::shared_ptr<V> value;
        int freq;
        ListNode* prev;
        ListNode* next;

        ListNode(K key, std::shared_ptr<V> value, int freq) 
            : key(key), value(value), freq(freq), prev(nullptr), next(nullptr) {}
    };
    
    int capacity;
    int minFreq;
    int hits;
    int misses;
    std::unordered_map<K, ListNode*> keyValueMap;
    std::unordered_map<int, ListNode*> freqListHeads;

    std::mutex cacheMutex;

    void removeMinFreqKey() {
        ListNode* listHead = freqListHeads[minFreq];

        ListNode* lruNode = listHead->prev;

        lruNode->prev->next = lruNode->next;
        lruNode->next->prev = lruNode->prev;

        keyValueMap.erase(lruNode->key);

        if (listHead->next == listHead) {
            delete listHead;
            freqListHeads.erase(minFreq);
            minFreq++;
        }
        delete lruNode;
    }


    void updateFrequency(ListNode* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;

        int oldFreq = node->freq;
        ListNode* oldListHead = freqListHeads[oldFreq];
        if (oldListHead->next == oldListHead) {
            delete oldListHead;
            freqListHeads.erase(oldFreq);
            if (oldFreq == minFreq) {
                minFreq++;
            }
        }

        node->freq++;

        int newFreq = node->freq;
        ListNode* newListHead = nullptr;
        if (freqListHeads.find(newFreq) == freqListHeads.end()) {
            newListHead = new ListNode(K{}, nullptr, 0);
            newListHead->next = newListHead;
            newListHead->prev = newListHead;
            freqListHeads[newFreq] = newListHead;
        } else {
            newListHead = freqListHeads[newFreq];
        }

        node->next = newListHead->next;
        node->prev = newListHead;
        newListHead->next->prev = node;
        newListHead->next = node;
    }


public:
    LFUCache(int capacity) : capacity(capacity), minFreq(0), hits(0), misses(0) {
    }

    ~LFUCache() {
        for (auto it = keyValueMap.begin(); it != keyValueMap.end(); it++) {
            delete it->second;
        }
        for (auto it = freqListHeads.begin(); it != freqListHeads.end(); it++) {
            delete it->second;
        }
    }

    std::shared_ptr<V> get(const K& key) {
        std::lock_guard<std::mutex> lock(cacheMutex);

        auto it = keyValueMap.find(key);
        
        if (it != keyValueMap.end()) {
            updateFrequency(it->second);
            hits++;
            return it->second->value;
        }
        misses++;
        return nullptr;
    }
   
    void put(const K& key, const V& value) {
        std::lock_guard<std::mutex> lock(cacheMutex);

        if (capacity <= 0) return;

        auto it = keyValueMap.find(key);

        if (it != keyValueMap.end()) {
            it->second->value = std::make_shared<V>(value);
            updateFrequency(it->second);
            return;
        }

        if (keyValueMap.size() >= capacity) {
            removeMinFreqKey();
        }

        ListNode* newNode = new ListNode(key, std::make_shared<V>(value), 1);
        keyValueMap[key] = newNode;

        if (freqListHeads.find(1) == freqListHeads.end()) {
            ListNode* listHead = new ListNode(K{}, nullptr, 0); // dummy node
            listHead->next = listHead;
            listHead->prev = listHead;
            freqListHeads[1] = listHead;
        }

        ListNode* listHead = freqListHeads[1];
        newNode->next = listHead->next;
        newNode->prev = listHead;
        listHead->next->prev = newNode;
        listHead->next = newNode;

        if (keyValueMap.size() == 1) {
            minFreq = 1;
        }
    }

    void clear() {
        minFreq = 0;
        hits = 0;
        misses = 0;
        keyValueMap.clear();
        freqListHeads.clear();
    }

    void changeCapacity(int newCapacity) {
        capacity = newCapacity;
        while (keyValueMap.size() > capacity) {
            removeMinFreqKey();
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
};