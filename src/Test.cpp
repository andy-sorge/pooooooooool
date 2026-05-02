#include "Test.hpp"

void test_dot() {
    Vector a{3, 4}, b{1, 2};
    assert(a.dot(b) == 11.0);
}

void test_magnitude() {
    Vector a{3, 4};
    assert(a.magnitude() == 5.0);
}

void test_normalized() {
    Vector a{3, 4};
    assert(std::abs(a.normalized().magnitude() - 1.0) < 1e-9);
}

void test_add() {
    Vector a{3, 4}, b{1, 2};
    Vector sum = a + b;
    assert(sum.x == 4.0 && sum.y == 6.0);
}

void test_subtract() {
    Vector a{3, 4}, b{1, 2};
    Vector diff = a - b;
    assert(diff.x == 2.0 && diff.y == 2.0);
}

void run_vector_tests() {
    test_dot();
    test_magnitude();
    test_normalized();
    test_add();
    test_subtract();
}