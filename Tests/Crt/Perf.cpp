#include <chrono>
#include <iostream>
#include <gtest/gtest.h>
#include "Meta.h"
#include "Service.h"

class A : public NEWorld::Object {

};

struct AA : NEWorld::ClassAttribute {
    int x, y;
    char z;
    AA(int a, int b, char c) noexcept: x(a), y(b), z(c) {}

    void OnRuntimeAttach() noexcept override {
        std::cout << x << y << z << "Attached to:" << Class->Class->name() << " with " << AttributeClass->name() << std::endl;
    }
};

NW_META_CLASS_ATTRIBUTE(A, AA, 1, 2, 'v')

TEST(NRTMETA, Class) {
    auto c = NEWorld::Meta::Find(typeid(A));
    if (c) {
        auto a = NEWorld::Activator::New(*c);
        auto cc = a->GetClass();
        std::cout << cc->Class->name() << ' ' << cc->Class->hash_code() << std::endl;
        auto& act = cc->ActivatorProperty;
    }
}

struct B : public NEWorld::Object {
    B() { std::puts("construct"); }
    ~B() { std::puts("destructs"); }
};

NW_MAKE_SERVICE(B, "AAAA", 0.0, C)

TEST(NRTMETA, Service) {
    NEWorld::ServiceHandle x { "AAAA" };
    auto& y = x.Get<B>();
}
