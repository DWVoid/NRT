#include "Meta.h"

namespace {
    const std::type_info& Id(const NEWorld::Object* object) noexcept {
        return typeid(*object);
    }
}

namespace NEWorld {
    const char* ClassNotConvertible::what() const noexcept {
        return "Object downcast failure";
    }

    Object::Object() noexcept = default;

    Object::Object(Object&& rhs) noexcept
            :mClassInfo(rhs.mClassInfo.load()) { }

    Object::Object(const Object& rhs) noexcept
            :mClassInfo(rhs.mClassInfo.load()) { }

    Object& Object::operator=(Object&& rhs) noexcept { return *this; }

    Object& Object::operator=(const Object& rhs) noexcept { return *this; } // NOLINT

    Object::~Object() noexcept = default;

    const ClassReflect* Object::GetClass() const noexcept {
        if (mClassInfo) {
            return mClassInfo;
        }
        else {
            mClassInfo = Meta::Find(Id(this));
            return mClassInfo;
        }
    }

    NW_META_CLASS(Object)
}