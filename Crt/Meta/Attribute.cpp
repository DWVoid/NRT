#include "Meta.h"

namespace {
    struct AttrEnum : NEWorld::IIterator<NEWorld::ClassAttribute> {
        explicit AttrEnum(NEWorld::ClassAttribute* base) noexcept:Cur(base) {}

        [[nodiscard]] bool Equals(const NEWorld::IEquitable<void>& rhs) const noexcept override {
            auto ptr = dynamic_cast<const AttrEnum*>(&rhs);
            return ptr ? Cur == ptr->Cur : false;
        }

        [[nodiscard]] bool Equals(const NEWorld::IIterator<NEWorld::ClassAttribute>& rhs) const noexcept override {
            return Cur == static_cast<const AttrEnum&>(rhs).Cur; //NOLINT
        }

        NEWorld::ClassAttribute& Current() noexcept override { return *Cur; }

        bool MoveNext() override { return Cur = Cur->Next; }

        NEWorld::ClassAttribute* Cur;
    };
}

namespace NEWorld {
    std::unique_ptr<IIterator<ClassAttribute>> EnumAttributes(const ClassReflect& cls) {
        return std::make_unique<AttrEnum>(reinterpret_cast<ClassAttribute*>(cls.TStart));
    }
}