extern "C" {
#include "counter.h"
#include "status.h"
}
#include <gtest/gtest.h>

class CounterTest : public ::testing::Test {
protected:
    Counter c;
    void SetUp() override { ASSERT_EQ(counter_init(&c, 0), STATUS_OK); }
};

TEST_F(CounterTest, MultipleIncrements) {
    for (int i = 0; i < 100; ++i) ASSERT_EQ(counter_increment(&c), STATUS_OK);
    EXPECT_EQ(counter_get(&c), 100);
}

TEST_F(CounterTest, MultipleDecrementsIntoNegative) {
    for (int i = 0; i < 5; ++i) ASSERT_EQ(counter_decrement(&c), STATUS_OK);
    EXPECT_EQ(counter_get(&c), -5); // aktuální chování dovoluje záporné hodnoty
}

TEST_F(CounterTest, ReinitResetsState) {
    ASSERT_EQ(counter_increment(&c), STATUS_OK);
    ASSERT_EQ(counter_init(&c, 42), STATUS_OK);
    EXPECT_EQ(counter_get(&c), 42);
}

TEST_F(CounterTest, InterleavedOps) {
    // 0 -> +1 -> +1 -> -1 -> +3 -> -2  => výsledně 2
    ASSERT_EQ(counter_increment(&c), STATUS_OK); // 1
    ASSERT_EQ(counter_increment(&c), STATUS_OK); // 2
    ASSERT_EQ(counter_decrement(&c), STATUS_OK); // 1
    ASSERT_EQ(counter_increment(&c), STATUS_OK); // 2
    ASSERT_EQ(counter_increment(&c), STATUS_OK); // 3
    ASSERT_EQ(counter_increment(&c), STATUS_OK); // 4
    ASSERT_EQ(counter_decrement(&c), STATUS_OK); // 3
    ASSERT_EQ(counter_decrement(&c), STATUS_OK); // 2
    EXPECT_EQ(counter_get(&c), 2);
}

TEST_F(CounterTest, NegativeInit) {
    ASSERT_EQ(counter_init(&c, -10), STATUS_OK);
    EXPECT_EQ(counter_get(&c), -10);
    ASSERT_EQ(counter_increment(&c), STATUS_OK);
    EXPECT_EQ(counter_get(&c), -9);
}

TEST(CounterProperty, IncrementThenDecrementIsNoOp) {
    Counter c;
    ASSERT_EQ(counter_init(&c, 123), STATUS_OK);
    ASSERT_EQ(counter_increment(&c), STATUS_OK);
    ASSERT_EQ(counter_decrement(&c), STATUS_OK);
    EXPECT_EQ(counter_get(&c), 123);
}

TEST(CounterDefensive, NullArgs) {
    EXPECT_EQ(counter_init(nullptr, 0), STATUS_INVALID_ARG);
    EXPECT_EQ(counter_increment(nullptr), STATUS_INVALID_ARG);
    EXPECT_EQ(counter_decrement(nullptr), STATUS_INVALID_ARG);
    EXPECT_EQ(counter_get(nullptr), 0); // getter vrací 0 pro NULL dle implementace
}
