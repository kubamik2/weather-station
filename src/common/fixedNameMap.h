#pragma once
#include "fixedVector.h"
#include <array>
#include <cstring>
#include <optional>

template<typename Item, const size_t MAX_NAME_SIZE, const size_t SIZE>
class FixedNameMap {
public:
    typedef struct {
        std::array<char, MAX_NAME_SIZE> name;
        Item item;
    } KeyValuePair;

    FixedNameMap() {}

    bool isFull() const noexcept {
        mItems.size() == SIZE;
    }

    size_t size() const noexcept {
        return mItems.size();
    }

    bool set(const char *name, Item item) noexcept {
        if (auto index = findIndex(name)) {
            mItems[index.value()].item = item;
            return true;
        } else {
            KeyValuePair pair;
            strncpy(pair.name.data(), name, MAX_NAME_SIZE);
            pair.item = item;
            return mItems.pushBack(std::move(pair));
        }
    }

    std::optional<Item*> get(const char *name) noexcept {
        if (auto index = findIndex(name)) {
            return &mItems[index.value()].item;
        } else {
            return {};
        }
    }

    std::optional<const Item*> get(const char *name) const noexcept {
        if (auto index = findIndex(name)) {
            return &mItems[index.value()].item;
        } else {
            return {};
        }
    }

    KeyValuePair* begin() noexcept {
        return mItems.begin();
    }

    KeyValuePair* end() noexcept {
        return mItems.end();
    }

    const KeyValuePair* cbegin() const noexcept {
        return mItems.cbegin();
    }

    const KeyValuePair* cend() const noexcept {
        return mItems.cend();
    }

private:
    FixedVector<KeyValuePair, SIZE> mItems;

    std::optional<size_t> findIndex(const char *name) const {
        for (size_t i = 0; i < mItems.size(); i++) {
            auto &[currentName, item] = mItems[i];
            if (strncmp(currentName.data(), name, MAX_NAME_SIZE) == 0) {
                return i;
            }
        }
        return {};
    }
};
