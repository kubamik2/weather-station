#pragma once
#include <array>
#include <cstddef>

template <typename Item, const size_t SIZE> class FixedVector {
public:
    FixedVector()
        : mSize(0) {
    }

    bool pushBack(Item item) {
        if (mSize >= SIZE) {
            return false;
        }
        mItems[mSize++] = item;
        return true;
    }

    void pop() noexcept {
        if (mSize > 0) {
            mSize--;
        }
    }

    size_t size() const noexcept {
        return mSize;
    }

    Item &operator[](size_t index) {
        return mItems[index];
    }

    const Item &operator[](size_t index) const {
        return mItems[index];
    }

    Item *begin() noexcept {
        return mItems.begin();
    }

    Item *end() noexcept {
        return &mItems[mSize];
    }

    const Item *cbegin() const noexcept {
        return mItems.cbegin();
    }

    const Item *cend() const noexcept {
        return &mItems[mSize];
    }

private:
    size_t mSize;
    std::array<Item, SIZE> mItems;
};
