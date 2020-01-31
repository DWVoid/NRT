#pragma once

#include "Compile/Config.h"
#include "Compile/DyldExec.h"
#include "Object/Object.h"
#include <atomic>
#include <memory>
#include <vector>
#include <utility>
#include <typeinfo>
#include <exception>
#include <type_traits>

namespace NEWorld {
    class Object;
    struct ClassReflect;
    struct Activator;

    template <class T>
    struct Class { };

    struct ClassActivatorProperty {
        using SoloOperator = void (*)(void*);
        using DualOperator = void (*)(void*, void*);
        const SoloOperator DefaultConstructor{nullptr}, Destructor{nullptr};
        const DualOperator CopyConstructor{nullptr}, CopyAssignmentOperator{nullptr},
                MoveConstructor{nullptr}, MoveAssignmentOperator{nullptr};
        template <class T>
        explicit ClassActivatorProperty(Class<T>) noexcept
                :DefaultConstructor(GetDefaultConstructor<T>()),
                 CopyConstructor(GetCopyConstructor<T>()),
                 MoveConstructor(GetMoveConstructor<T>()),
                 CopyAssignmentOperator(GetCopyAssignmentOperator<T>()),
                 MoveAssignmentOperator(GetMoveAssignmentOperator<T>()),
                 Destructor(GetDestructor<T>()) { }
    private:
        template <class T>
        static constexpr SoloOperator GetDefaultConstructor() noexcept {
            if constexpr(std::is_default_constructible_v<T>) {
                return [](void* mem) { new(mem)T(); };
            }
            return nullptr;
        }

        template <class T>
        static constexpr DualOperator GetCopyConstructor() noexcept {
            if constexpr(std::is_copy_constructible_v<T>) {
                return [](void* mem, void* from) { new(mem)T(*reinterpret_cast<const T*>(from)); };
            }
            return nullptr;
        }

        template <class T>
        static constexpr DualOperator GetMoveConstructor() noexcept {
            if constexpr(std::is_move_constructible_v<T>) {
                return [](void* mem, void* from) { new(mem)T(std::move(*reinterpret_cast<T*>(from))); };
            }
            return nullptr;
        }

        template <class T>
        static constexpr DualOperator GetCopyAssignmentOperator() noexcept {
            if constexpr(std::is_copy_assignable_v<T>) {
                return [](void* lhs, void* rhs) {
                    *reinterpret_cast<T*>(lhs) = *reinterpret_cast<const T*>(rhs);
                };
            }
            return nullptr;
        }
        template <class T>
        static constexpr DualOperator GetMoveAssignmentOperator() noexcept {
            if constexpr(std::is_move_assignable_v<T>) {
                return [](void* lhs, void* rhs) {
                    *reinterpret_cast<T*>(lhs) = std::move(*reinterpret_cast<T*>(rhs));
                };
            }
            return nullptr;
        }
        template <class T>
        static constexpr SoloOperator GetDestructor() noexcept {
            if constexpr(std::is_destructible_v<T>) {
                return [](void* mem) { reinterpret_cast<T*>(mem)->~T(); };
            }
            return nullptr;
        }
    };

    struct ClassSpecialProperty {
        const bool IsTriviallyDefaultConstructable{}, IsTriviallyDestructible{},
                IsTriviallyCopyableConstructable{}, IsTriviallyCopyAssignable{},
                IsTriviallyMoveConstructable{}, IsTriviallyMoveAssignable{},
                IsNoexceptDefaultConstructable{}, IsNoexceptDestructible{},
                IsNoexceptCopyConstructable{}, IsNoexceptCopyAssignable{},
                IsNoexceptMoveConstructable{}, IsNoexceptMoveAssignable{};
        template <class T>
        constexpr explicit ClassSpecialProperty(Class<T>) noexcept
                :IsTriviallyDefaultConstructable(std::is_trivially_default_constructible_v<T>),
                 IsTriviallyDestructible(std::is_trivially_destructible_v<T>),
                 IsTriviallyCopyableConstructable(std::is_trivially_copy_constructible_v<T>),
                 IsTriviallyCopyAssignable(std::is_trivially_copy_assignable_v<T>),
                 IsTriviallyMoveConstructable(std::is_trivially_move_constructible_v<T>),
                 IsTriviallyMoveAssignable(std::is_trivially_move_assignable_v<T>),
                 IsNoexceptDefaultConstructable(std::is_nothrow_default_constructible_v<T>),
                 IsNoexceptDestructible(std::is_nothrow_destructible_v<T>),
                 IsNoexceptCopyConstructable(std::is_nothrow_copy_constructible_v<T>),
                 IsNoexceptCopyAssignable(std::is_nothrow_copy_assignable_v<T>),
                 IsNoexceptMoveConstructable(std::is_nothrow_move_constructible_v<T>),
                 IsNoexceptMoveAssignable(std::is_nothrow_move_assignable_v<T>) { }
    };

    struct ClassLayoutProperty {
        const int Size{}, Alignment{};
        const bool IsStandardLayout{};
        const long long int PmrBaseOffset{};
        template <class T>
        explicit ClassLayoutProperty(Class<T>) noexcept
                : Size(sizeof(T)), Alignment(alignof(T)), IsStandardLayout(std::is_standard_layout_v<T>),
                  PmrBaseOffset(GetPmrBaseOffset<T>()) { }
    private:
        template <class T>
        long long int GetPmrBaseOffset() noexcept {
            if constexpr (std::is_convertible_v<T*, Object*>) {
                const auto mockMem = uintptr_t(0b1'0000000000'0000000000'0000000000);
                const auto mockBase = reinterpret_cast<T*>(mockMem);
                const auto pmr = reinterpret_cast<uintptr_t>(static_cast<Object*>(mockBase));
                return pmr-mockMem;
            }
            else {
                return -1;
            }
        }
    };

    class ClassNotConvertible : public std::exception {
    public:
        explicit ClassNotConvertible(Object* object) noexcept
                :mObject(object) { }
        [[nodiscard]] NRTCRT_API const char* what() const noexcept override;
    private:
        Object* mObject;
    };

    class Object {
    public:
        NRTCRT_API Object() noexcept;
        NRTCRT_API Object(Object&&) noexcept;
        NRTCRT_API Object(const Object&) noexcept;
        NRTCRT_API Object& operator=(Object&&) noexcept;
        NRTCRT_API Object& operator=(const Object&) noexcept;
        NRTCRT_API virtual ~Object() noexcept;
        const ClassReflect* GetClass() const noexcept;
        template <class T>
        T& As() {
            if (const auto obj = dynamic_cast<T*>(this); obj) {
                return *obj;
            }
            else {
                throw ClassNotConvertible(this);
            }
        }
    private:
        friend struct ::NEWorld::Activator;
        mutable std::atomic<const ClassReflect*> mClassInfo{nullptr};
    };

    struct ClassReflect {
        const std::type_info* Class{};
        const ClassLayoutProperty LayoutProperty;
        const ClassActivatorProperty ActivatorProperty;
        const ClassSpecialProperty SpecialProperty;
        void* Next{nullptr}, * TStart{nullptr}, * TEnd{nullptr};
        template <class T>
        explicit ClassReflect(NEWorld::Class<T>) noexcept
                : Class(std::addressof(typeid(T))), LayoutProperty(NEWorld::Class<T>()),
                  ActivatorProperty(NEWorld::Class<T>()), SpecialProperty(NEWorld::Class<T>()) { }
    };

    class MemoryMissAlign : public std::exception {
    public:
        [[nodiscard]] NRTCRT_API const char* what() const noexcept override;
    };

    class ActivatorError : public std::exception {
    public:
        enum class Type : int {
            NotDefaultConstructable = 0,
            NotDestructible, NotCopyable, NotMovable, NotAnObject
        };
        explicit ActivatorError(Type type) noexcept;
        [[nodiscard]] NRTCRT_API const char* what() const noexcept override;
    private:
        Type mType;
    };

    struct Activator {
        // Value-style interface
        NRTCRT_API static void Construct(const ClassReflect& ref, void* mem);
        NRTCRT_API static void Destruct(const ClassReflect& ref, void* mem);
        NRTCRT_API static void Copy(const ClassReflect& ref, const void* from, void* to);
        NRTCRT_API static void Move(const ClassReflect& ref, void* from, void* to);
        NRTCRT_API static void CopyAssign(const ClassReflect& ref, void* lhs, const void* rhs);
        NRTCRT_API static void MoveAssign(const ClassReflect& ref, void* lhs, void* rhs);
        // Object-style interface
        NRTCRT_API static Object* New(const ClassReflect& ref);
        NRTCRT_API static void Delete(Object* object);
        NRTCRT_API static Object* Clone(const Object* object);
        NRTCRT_API static void CloneInto(const Object* object, Object* target);
    };

    struct ClassAttribute {
        ClassAttribute* Previous{nullptr}, * Next{nullptr};
        const ClassReflect* Class{nullptr};
        const std::type_info* AttributeClass{};
        virtual void OnRuntimeAttach() noexcept { }
    };

    template <class U, class... Args>
    U MakeAttribute(Args&& ... args) noexcept {
        U ret{std::forward<Args>(args)...};
        ret.Previous = ret.Next = nullptr;
        ret.AttributeClass = std::addressof(typeid(U));
        return ret;
    }

    NRTCRT_API std::unique_ptr<IIterator<ClassAttribute>> EnumAttributes(const ClassReflect& cls);

    namespace Meta {
        using ClassHook = void (*)(const ClassReflect& c, bool installed) noexcept;
        NRTCRT_API int Inject(ClassHook clh) noexcept;
        NRTCRT_API void Inject(ClassReflect& ref) noexcept;
        NRTCRT_API void Inject(ClassReflect& c, ClassAttribute& attr) noexcept;
        NRTCRT_API void Remove(ClassReflect& ref) noexcept;
        NRTCRT_API void RemoveClh(int index) noexcept;
        NRTCRT_API const ClassReflect* Find(const std::type_info& info) noexcept;
    }
}

namespace NEWorld::Meta::InterOp {
    template <class T>
    ClassReflect& Class() {
        struct Helper {
            Helper() noexcept { Inject(Reflect); }
            ~Helper() noexcept { Remove(Reflect); }
            ClassReflect Reflect = ::NEWorld::ClassReflect(::NEWorld::Class<T>());
        };
        static auto r = Helper();
        return r.Reflect;
    }

    template <class T, class A, class ...Ts>
    A& Attribute(Ts&& ... args) {
        struct Helper {
            explicit Helper(Ts&& ... args) noexcept
                    :Attribute(MakeAttribute<A>(std::forward<Ts>(args)...)) {
                Inject(Class<T>(), Attribute);
            }
            A Attribute;
        };
        static auto r = Helper(std::forward<Ts>(args)...);
        return r.Attribute;
    }
}

#define META_CLASS_HOOK(x) namespace {\
    int NW_CONCAT_2(NW_UNIQUE_NAME, IDX);\
    NW_DY_LD_EXEC([]() noexcept { NW_CONCAT_2(NW_UNIQUE_NAME, IDX)=::NEWorld::Inject(x); })\
    NW_DY_UD_EXEC([]() noexcept { ::NEWorld::RemoveClh(NW_CONCAT_2(NW_UNIQUE_NAME, IDX)); })\
}

#define NW_META_CLASS(x) \
    NW_DY_LD_EXEC([]() noexcept { ::NEWorld::Meta::InterOp::Class<x>(); })

#define NW_MACRO_INTEROP_META_CLASS_ATTR_1(x, y, ...) ::NEWorld::Meta::InterOp::Attribute<x, y>(__VA_ARGS__);
#define NW_META_CLASS_ATTRIBUTE(x, y, ...) \
    NW_DY_LD_EXEC([]() noexcept { NW_MACRO_INTEROP_META_CLASS_ATTR_1(x, y, __VA_ARGS__) })
