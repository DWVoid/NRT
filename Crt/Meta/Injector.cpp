#include "Meta.h"
#include <vector>
#include <mutex>
#include <unordered_map>

namespace {
    std::recursive_mutex gLock {}; // NOLINT
    std::vector<NEWorld::Meta::ClassHook> gHooks;
    std::unordered_map<size_t, NEWorld::ClassReflect*> gClasses;
}

namespace NEWorld::Meta {
    int Inject(ClassHook clh) noexcept {
        std::lock_guard lk {gLock};
        const int result = gHooks.size();
        gHooks.push_back(clh);
        return result;
    }

    void Inject(ClassReflect& ref) noexcept {
        std::lock_guard lk {gLock};
        for (auto x : gHooks) { x(ref, false);}
        gClasses.insert_or_assign(ref.Class->hash_code(), std::addressof(ref));
    }

    void Inject(ClassReflect& c, ClassAttribute& attr) noexcept {
        if (!c.TStart) {
            c.TStart = c.TEnd = std::addressof(attr);
        }
        else {
            auto end = reinterpret_cast<ClassAttribute*>(c.TEnd);
            end->Next = std::addressof(attr);
            attr.Previous = end;
            c.TEnd = std::addressof(attr);
        }
        attr.Class = std::addressof(c);
        attr.OnRuntimeAttach();
    }

    void Remove(ClassReflect& ref) noexcept {
        std::lock_guard lk {gLock};
        const auto search = gClasses.find(ref.Class->hash_code());
        if (search != gClasses.end()) {
            gClasses.erase(search);
        }
    }

    void RemoveClh(int index) noexcept {
        std::lock_guard lk {gLock};
        gHooks[index] = nullptr;
    }

    const ClassReflect* Find(const std::type_info& info) noexcept {
        std::lock_guard lk {gLock};
        const auto search = gClasses.find(info.hash_code());
        return search != gClasses.end() ? search->second : nullptr;
    }
}