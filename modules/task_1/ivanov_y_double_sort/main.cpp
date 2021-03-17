// Copyright 2021 Ivanov Yaroslav
#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include "./sort_double.h"

TEST(Local_compare, Test_works_byte_sort) {
    int size = 100;
    double* vec = generateDouble(size);
    ASSERT_NO_THROW(loc_sort(vec, size));
}

TEST(Local_compare, Test_works_bubble_sort) {
    int size = 100;
    double* vec = generateDouble(size);
    ASSERT_NO_THROW(BubbleForCompare(vec, size));
}

TEST(Local_compare, Test_size_one_no_exceptions) {
    int size = 1;
    double* vec = generateDouble(size);
    ASSERT_NO_THROW(loc_sort(vec, size));
}

TEST(Local_compare, Test_size_four_eq_real_sort_mass) {
    int size = 4;
    double* vec = generateDouble(size);
    std::vector<double> loc{ 10.0, 11.0, 14.0, 16.0 };
    std::vector<double> loc1{ 14.0, 16.0, 11.0, 10.0 };
    for (int i = 0; i < 4; i++) {
        vec[i] = loc1[i];
    }
    loc_sort(vec, size);
    for (int i = 0; i < 4; i++) {
        loc1[i] = vec[i];
    }
    ASSERT_EQ(loc1, loc);
}

TEST(Local_compare, Test_seq_compare_100) {
    int size = 100;
    double* vec = generateDouble(size);
    double* vec1 = vec;
    vec1 = BubbleForCompare(vec, size);
    loc_sort(vec, size);
    ASSERT_EQ(vec, vec1);
}

TEST(Local_compare, Test_seq_compare_1000) {
    int size = 1000;
    double* vec = generateDouble(size);
    double* vec1 = vec;
    vec1 = BubbleForCompare(vec, size);
    loc_sort(vec, size);
    ASSERT_EQ(vec, vec1);
}

TEST(Local_compare, Test_negativ_size) {
    int size = -10;
    ASSERT_ANY_THROW(generateDouble(size));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
