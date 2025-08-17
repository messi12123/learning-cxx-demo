#include "../exercise.h"
#include <memory>

// READ: `std::shared_ptr` <https://zh.cppreference.com/w/cpp/memory/shared_ptr>
// READ: `std::weak_ptr` <https://zh.cppreference.com/w/cpp/memory/weak_ptr>

// TODO: 将下列 `?` 替换为正确的值
int main(int argc, char **argv) {
    auto shared = std::make_shared<int>(10);
    std::shared_ptr<int> ptrs[]{shared, shared, shared};

    std::weak_ptr<int> observer = shared;
    ASSERT(observer.use_count() == 4, "");  // shared + ptrs[0] + ptrs[1] + ptrs[2]

    ptrs[0].reset();
    ASSERT(observer.use_count() == 3, "");  // 释放一份

    ptrs[1] = nullptr;
    ASSERT(observer.use_count() == 2, "");  // 再释放一份

    ptrs[2] = std::make_shared<int>(*shared);
    ASSERT(observer.use_count() == 1, "");  // ptrs[2] 指向新对象，不再共享旧控制块

    ptrs[0] = shared;            // +1
    ptrs[1] = shared;            // +1
    ptrs[2] = std::move(shared); // 转移 shared 到 ptrs[2]，总拥有者数不变
    ASSERT(observer.use_count() == 3, "");

    std::ignore = std::move(ptrs[0]); // 移走并销毁临时，等价于清空 ptrs[0]，-1
    ptrs[1] = std::move(ptrs[1]);     // 自我移动，无变化
    ptrs[1] = std::move(ptrs[2]);     // 先释放自身再接管 ptrs[2]，两者合并为一个拥有者
    ASSERT(observer.use_count() == 1, "");

    shared = observer.lock();         // 再获得一份强引用
    ASSERT(observer.use_count() == 2, "");

    shared = nullptr;
    for (auto &ptr : ptrs) ptr = nullptr; // 清空数组里剩余的那一份
    ASSERT(observer.use_count() == 0, "");

    shared = observer.lock();         // 对象已销毁，lock 得到空
    ASSERT(observer.use_count() == 0, "");

    return 0;
}
