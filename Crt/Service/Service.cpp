#include "Service.h"
#include <unordered_map>
#include <string_view>
#include <algorithm>

namespace {
    struct ServiceRecord {
        const NEWorld::ServiceInfo* Info;
        NEWorld::Object* Object;
        std::atomic_int Counter;
        bool operator<(const ServiceRecord& rhs) {
            return Info->Priority<rhs.Info->Priority;
        }
    };

    class ServiceHost {
    public:
        static ServiceHost& Get() noexcept {
            static ServiceHost host{};
            return host;
        }

        void Add(const NEWorld::ServiceInfo* info) {
            auto& vec = GetVector(info);
            vec.emplace_back(std::make_unique<ServiceRecord>());
            vec.back()->Info = info;
            std::stable_sort(vec.begin(), vec.begin());
        }

        std::pair<NEWorld::Object*, void*> GetService(const char* name) {
            const auto iter = mRegistered.find(name);
            if (iter!=mRegistered.end()) {
                auto& x = iter->second[0];
                if (!x->Object) {
                    auto& cls = *(x->Info->Class);
                    NEWorld::Activator::Construct(cls, x->Info->Mem);
                    x->Object = reinterpret_cast<NEWorld::Object*>(reinterpret_cast<uintptr_t>(x->Info->Mem)
                            +cls.LayoutProperty.PmrBaseOffset);
                }
                x->Counter.fetch_add(1);
                return {x->Object, x.get()};
            }
            return {nullptr, nullptr};
        }

        void Remove(const NEWorld::ServiceInfo* info) {
            auto& vec = GetVector(info);
            for (auto x = vec.begin(); x!=vec.end(); ++x) {
                if (x->get()->Info==info) {
                    if (x->get()->Counter != 0) { std::terminate(); }
                    vec.erase(x);
                    return;
                }
            }
        }
    private:
        std::vector<std::unique_ptr<ServiceRecord>>& GetVector(const NEWorld::ServiceInfo* info) {
            const auto iter = mRegistered.find(info->Name);
            if (iter!=mRegistered.end()) return iter->second;
            return mRegistered.insert_or_assign(info->Name, std::vector<std::unique_ptr<ServiceRecord>>{}).first->second;
        }
        std::unordered_map<std::string_view, std::vector<std::unique_ptr<ServiceRecord>>> mRegistered;
    };
}

namespace NEWorld {
    void ServiceInfo::OnRuntimeAttach() noexcept {
        ServiceHost::Get().Add(this);
    }

    ServiceInfo::~ServiceInfo() noexcept {
        ServiceHost::Get().Remove(this);
    }

    ServiceHandle::ServiceHandle(const char* name)
            :mObj(ServiceHost::Get().GetService(name)) { }

    ServiceHandle::~ServiceHandle() {
        auto& x = *reinterpret_cast<ServiceRecord*>(mObj.second);
        if (x.Counter.fetch_sub(1)==1) {
            auto obj = x.Object;
            Activator::Destruct(*(obj->GetClass()),
                    reinterpret_cast<std::byte*>(obj) - obj->GetClass()->LayoutProperty.PmrBaseOffset);
        }
    }
}