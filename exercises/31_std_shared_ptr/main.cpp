#include "../exercise.h"
#include <memory>

// READ: `std::shared_ptr` <https://zh.cppreference.com/w/cpp/memory/shared_ptr>
// READ: `std::weak_ptr` <https://zh.cppreference.com/w/cpp/memory/weak_ptr>

// TODO: 将下列 `?` 替换为正确的值
int main(int argc, char **argv) {
    auto shared = std::make_shared<int>(10); // use_count = 1
    std::shared_ptr<int> ptrs[]{shared, shared, shared}; // use_count = 4

    std::weak_ptr<int> observer = shared;
    ASSERT(observer.use_count() == 4, "");

    ptrs[0].reset(); // use_count = 3
    ASSERT(observer.use_count() == 3, "");

    ptrs[1] = nullptr; // use_count = 2
    ASSERT(observer.use_count() == 2, "");

    ptrs[2] = std::make_shared<int>(*shared); // 创建新对象，不影响原 shared，原 use_count = 1
    ASSERT(observer.use_count() == 1, "");

    ptrs[0] = shared;  // use_count = 2
    ptrs[1] = shared;  // use_count = 3
    ptrs[2] = std::move(shared);  // shared 转交所有权给 ptrs[2]，use_count 仍为 3
    ASSERT(observer.use_count() == 3, "");

    std::ignore = std::move(ptrs[0]); // 无副作用
    ptrs[1] = std::move(ptrs[1]);     // 无副作用
    ptrs[1] = std::move(ptrs[2]);     // 仍持有原对象，use_count = 3
    ASSERT(observer.use_count() == 3, "");

    shared = observer.lock();  // 锁回 shared_ptr，use_count = 4
    ASSERT(observer.use_count() == 4, "");

    shared = nullptr;
    for (auto &ptr : ptrs) ptr = nullptr; // 全部释放，use_count = 0
    ASSERT(observer.use_count() == 0, "");

    shared = observer.lock();  // 对象已被释放，lock 返回 nullptr
    ASSERT(observer.use_count() == 0, "");

    return 0;
}
