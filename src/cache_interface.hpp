#pragma once

template <typename K, typename V>
class CacheInterface {
public:
    ~CacheInterface() {}

    std::shared_ptr<V> get(const K& key) {
        throw std::runtime_error("Not implemented");
    }
   
    void put(const K& key, const V& value) {
        throw std::runtime_error("Not implemented");
    }

    void clear() {
        throw std::runtime_error("Not implemented");
    }

    void changeCapacity(int newCapacity) {
        throw std::runtime_error("Not implemented");
    }

    std::unordered_map<std::string, int> getStats() {
        throw std::runtime_error("Not implemented");
    }
};