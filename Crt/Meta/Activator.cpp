#include "Meta.h"

namespace NEWorld {
    static void CheckAlign(void* mem, int align) {
        if (!reinterpret_cast<uintptr_t>(mem)%align) {
            throw MemoryMissAlign();
        }
    }

    void Activator::Construct(const ClassReflect& ref, void* mem) {
        CheckAlign(mem, ref.LayoutProperty.Alignment);
        if (ref.ActivatorProperty.DefaultConstructor) {
            ref.ActivatorProperty.DefaultConstructor(mem);
            if (ref.LayoutProperty.PmrBaseOffset>=0) {
                const auto pmrMem = reinterpret_cast<uintptr_t>(mem) +ref.LayoutProperty.PmrBaseOffset;
                const auto pmr = reinterpret_cast<Object*>(pmrMem);
                pmr->mClassInfo = std::addressof(ref);
            }
        }
        else {
            throw ActivatorError(ActivatorError::Type::NotDefaultConstructable);
        }
    }

    void Activator::Destruct(const ClassReflect& ref, void* mem) {
        if (ref.ActivatorProperty.Destructor) {
            ref.ActivatorProperty.Destructor(mem);
        }
        else {
            throw ActivatorError(ActivatorError::Type::NotDestructible);
        }
    }

    void Activator::Copy(const ClassReflect& ref, const void* from, void* to) {
        CheckAlign(to, ref.LayoutProperty.Alignment);
        if (ref.ActivatorProperty.CopyConstructor) {
            ref.ActivatorProperty.CopyConstructor(to, const_cast<void*>(from));
        }
        else {
            throw ActivatorError(ActivatorError::Type::NotCopyable);
        }
    }

    void Activator::Move(const ClassReflect& ref, void* from, void* to) {
        CheckAlign(to, ref.LayoutProperty.Alignment);
        if (ref.ActivatorProperty.MoveConstructor) {
            ref.ActivatorProperty.MoveConstructor(to, from);
        }
        else {
            throw ActivatorError(ActivatorError::Type::NotMovable);
        }
    }

    void Activator::CopyAssign(const ClassReflect& ref, void* lhs, const void* rhs) {
        if (ref.ActivatorProperty.CopyAssignmentOperator) {
            ref.ActivatorProperty.CopyAssignmentOperator(lhs, const_cast<void*>(rhs));
        }
        else {
            throw ActivatorError(ActivatorError::Type::NotCopyable);
        }
    }

    void Activator::MoveAssign(const ClassReflect& ref, void* lhs, void* rhs) {
        if (ref.ActivatorProperty.MoveAssignmentOperator) {
            ref.ActivatorProperty.MoveAssignmentOperator(lhs, rhs);
        }
        else {
            throw ActivatorError(ActivatorError::Type::NotMovable);
        }
    }

    const char* MemoryMissAlign::what() const noexcept { return "Memory Alignment Check Failure"; }

    ActivatorError::ActivatorError(ActivatorError::Type type) noexcept
            :mType(type) { }

    const char* ActivatorError::what() const noexcept {
        switch (mType) {
        case Type::NotDefaultConstructable: return "Activator Error: Class Not Default Constructable";
        case Type::NotDestructible: return "Activator Error: Class Not Destructible";
        case Type::NotCopyable: return "Activator Error: Class Not Copyable";
        case Type::NotMovable: return "Activator Error: Class Not Movable";
        case Type::NotAnObject: return "Activator Error: Class Info Not Refer To An NEWorld Object";
        }
    }
    
    Object* Activator::New(const ClassReflect& ref) {
        if (ref.LayoutProperty.PmrBaseOffset==-1) {
            throw ActivatorError(ActivatorError::Type::NotAnObject);
        }
        const auto mem = ::new(std::align_val_t(ref.LayoutProperty.Alignment), std::nothrow)
                std::byte[ref.LayoutProperty.Size];
        auto handler = std::unique_ptr<std::byte[]>(mem);
        if (mem) {
            Construct(ref, mem);
            handler.release();
            return reinterpret_cast<Object*>(mem+ref.LayoutProperty.PmrBaseOffset);
        }
        return nullptr;
    }

    void Activator::Delete(Object* object) {
        if (object) {
            const auto ref = object->GetClass();
            const auto mem = reinterpret_cast<std::byte*>(object) - ref->LayoutProperty.PmrBaseOffset;
            Destruct(*ref, mem);
            delete[] mem;
        }
    }

    Object* Activator::Clone(const Object* object) {
        if (object) {
            const auto ref = object->GetClass();
            const auto memFrom = reinterpret_cast<const std::byte*>(object) - ref->LayoutProperty.PmrBaseOffset;
            const auto memTo = ::new(std::align_val_t(ref->LayoutProperty.Alignment), std::nothrow)
                    std::byte[ref->LayoutProperty.Size];
            auto handler = std::unique_ptr<std::byte[]>(memTo);
            if (memTo) {
                Copy(*ref, memFrom, memTo);
                handler.release();
                return reinterpret_cast<Object*>(memTo+ref->LayoutProperty.PmrBaseOffset);
            }
        }
        return nullptr;
    }

    void Activator::CloneInto(const Object* object, Object* target) {
        if (object && target) {
            if (object->GetClass() != target->GetClass()) {
                throw ActivatorError(ActivatorError::Type::NotCopyable);
            }
            const auto ref = object->GetClass();
            const auto memFrom = reinterpret_cast<const std::byte*>(object) - ref->LayoutProperty.PmrBaseOffset;
            const auto memTo = reinterpret_cast<std::byte*>(target) - ref->LayoutProperty.PmrBaseOffset;
            CopyAssign(*ref, memTo, memFrom);
        }
    }
    
    static_assert(std::is_standard_layout_v<ClassReflect>);
}