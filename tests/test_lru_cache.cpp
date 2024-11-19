#include <gtest/gtest.h>
#include <memory>
#include <unordered_map>
#include "../src/cache_interface.hpp"
#include "../src/lru_cache.hpp"

TEST(LRUCacheTest, IntegerKeysAndValues) {
    LRUCache<int, int> cache(2);
    cache.put(1, 10);
    cache.put(2, 20);
    EXPECT_EQ(*cache.get(1), 10);
    cache.put(3, 30); // 2 should be evicted (LRU policy)
    EXPECT_EQ(cache.get(2), nullptr);
    EXPECT_EQ(*cache.get(3), 30);
}

TEST(LRUCacheTest, StringKeysAndValues) {
    LRUCache<std::string, std::string> cache(2);
    cache.put("X", "Xylophone");
    cache.put("Y", "Yellow");
    EXPECT_EQ(*cache.get("X"), "Xylophone");
    cache.put("Z", "Zebra"); // "Y" should be evicted
    EXPECT_EQ(cache.get("Y"), nullptr);
    EXPECT_EQ(*cache.get("Z"), "Zebra");
}

TEST(LRUCacheTest, CapacityChange) {
    LRUCache<int, int> cache(2);
    cache.put(1, 10);
    cache.put(2, 20);
    cache.changeCapacity(1); // Capacity reduced to 1
    EXPECT_EQ(cache.get(1), nullptr); // 1 should have been evicted
    EXPECT_EQ(*cache.get(2), 20);
}
