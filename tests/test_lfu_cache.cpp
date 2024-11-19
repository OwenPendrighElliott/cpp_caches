#include <gtest/gtest.h>
#include <memory>
#include <unordered_map>
#include "../src/cache_interface.hpp"
#include "../src/lfu_cache.hpp"

TEST(LFUCacheTest, IntegerKeysAndValues) {
    LFUCache<int, int> cache(2);
    cache.put(1, 10);
    cache.put(2, 20);
    EXPECT_EQ(*cache.get(1), 10);
    cache.put(3, 30); // 2 should be evicted (LFU policy)
    EXPECT_EQ(cache.get(2), nullptr);
    EXPECT_EQ(*cache.get(3), 30);
}

TEST(LFUCacheTest, StringKeysAndValues) {
    LFUCache<std::string, std::string> cache(2);
    cache.put("A", "Apple");
    cache.put("B", "Banana");
    EXPECT_EQ(*cache.get("A"), "Apple");
    cache.put("C", "Cherry"); // "B" should be evicted
    EXPECT_EQ(cache.get("B"), nullptr);
    EXPECT_EQ(*cache.get("C"), "Cherry");
}

TEST(LFUCacheTest, StatsTracking) {
    LFUCache<int, int> cache(2);
    cache.put(1, 10);
    cache.get(1);
    cache.put(2, 20);
    auto stats = cache.getStats();
    EXPECT_EQ(stats["hits"], 1);
    EXPECT_EQ(stats["misses"], 0);
}
