#include "../exercise.h"
#include <memory>

// READ: `std::shared_ptr` <https://zh.cppreference.com/w/cpp/memory/shared_ptr>
// READ: `std::weak_ptr` <https://zh.cppreference.com/w/cpp/memory/weak_ptr>

// TODO: 将下列 `?` 替换为正确的值
int main(int argc, char **argv) {
    auto shared = std::make_shared<int>(10);        // use_count = 1
    std::shared_ptr<int> ptrs[]{shared, shared, shared}; // use_count = 4 (shared + 3)

    std::weak_ptr<int> observer = shared;
    ASSERT(observer.use_count() == 4, "");          // 1(shared)+3(ptrs)

    ptrs[0].reset();                                // -1
    ASSERT(observer.use_count() == 3, "");

    ptrs[1] = nullptr;                              // -1
    ASSERT(observer.use_count() == 2, "");

    ptrs[2] = std::make_shared<int>(*shared);       // ptrs[2] 指向新对象，不再共享旧对象 -> -1
    ASSERT(observer.use_count() == 1, "");

    ptrs[0] = shared;                               // +1
    ptrs[1] = shared;                               // +1
    ptrs[2] = std::move(shared);                    // 原 shared 空，ptrs[2] 接管 -> +1
    ASSERT(observer.use_count() == 4, "");          // use_count 回到 4

    std::ignore = std::move(ptrs[0]);               // 移走 ptrs[0]，旧的 -1
    ptrs[1] = std::move(ptrs[1]);                   // 自己 move 给自己，不变
    ptrs[1] = std::move(ptrs[2]);                   // ptrs[1] 原来的释放(-1)，再接管 ptrs[2](+1)，总数不变
    ASSERT(observer.use_count() == 3, "");          // 现在只有 ptrs[1]、以及之前的 observer/weak，不算，实际有 3 个 shared_ptr

    shared = observer.lock();                       // 再拿一份强引用 -> +1
    ASSERT(observer.use_count() == 4, "");

    shared = nullptr;                               // -1
    for (auto &ptr : ptrs) ptr = nullptr;           // 把剩余的 ptrs 清掉 -> -2
    ASSERT(observer.use_count() == 0, "");

    shared = observer.lock();                       // 对象已析构，lock 得到 nullptr
    ASSERT(observer.use_count() == 0, "");

    return 0;
}
