#include <gtest/gtest.h>
#include <algorithm>
#include <vector>

// А вот тут collvalue.h не надо, берите просто инты
#include "sorting.h"

// Но проверьте, что сортировки таки работают...



TEST(BubbleSort, SameVector)
{
    std::vector<int> data = {1, 2, 3, 4, 5};
    bubble_sort(data.begin(), data.end());
    EXPECT_TRUE(std::is_sorted(data.begin(), data.end()));
    EXPECT_EQ((std::vector<int>{1, 2, 3, 4, 5}), data);
}

TEST(BubbleSort, HandlesDuplicates)
{
    std::vector<int> data = {3, 1, 2, 3, 1, 2};
    bubble_sort(data.begin(), data.end());
    EXPECT_TRUE(std::is_sorted(data.begin(), data.end()));
    EXPECT_EQ((std::vector<int>{1, 1, 2, 2, 3, 3}), data);
}

TEST(BubbleSort, EmptyAndSingleElement)
{
    std::vector<int> empty;
    bubble_sort(empty.begin(), empty.end());
    EXPECT_TRUE(std::is_sorted(empty.begin(), empty.end()));

    std::vector<int> single = {42};
    bubble_sort(single.begin(), single.end());
    EXPECT_TRUE(std::is_sorted(single.begin(), single.end()));
    EXPECT_EQ((std::vector<int>{42}), single);
}





TEST(QuickSort, SameVector)
{
    std::vector<int> data = {1, 2, 3, 4, 5};
    quick_sort(data.begin(), data.end());
    EXPECT_TRUE(std::is_sorted(data.begin(), data.end()));
    EXPECT_EQ((std::vector<int>{1, 2, 3, 4, 5}), data);
}

TEST(QuickSort, HandlesDuplicates)
{
    std::vector<int> data = {3, 1, 2, 3, 1, 2};
    quick_sort(data.begin(), data.end());
    EXPECT_TRUE(std::is_sorted(data.begin(), data.end()));
    EXPECT_EQ((std::vector<int>{1, 1, 2, 2, 3, 3}), data);
}

TEST(QuickSort, EmptyAndSingleElement)
{
    std::vector<int> empty;
    quick_sort(empty.begin(), empty.end());
    EXPECT_TRUE(std::is_sorted(empty.begin(), empty.end()));

    std::vector<int> single = {42};
    quick_sort(single.begin(), single.end());
    EXPECT_TRUE(std::is_sorted(single.begin(), single.end()));
    EXPECT_EQ((std::vector<int>{42}), single);
}
