extern "C" {
#include "counter.h"
}

#include <gtest/gtest.h>

TEST(CounterTest, Init) {
    Counter c;
    counter_init(&c);
    EXPECT_EQ(counter_get(&c), 0);
}

TEST(CounterTest, Increment) {
    Counter c;
    counter_init(&c);
    counter_increment(&c);
    EXPECT_EQ(counter_get(&c), 1);
}
