#include "supply_audit.hpp"
#include "../items/supply.hpp"
#include <array>
#include <cstdio>

// Read the actual registry and item capacities; no game, SDL or simulation needed.
int run_supply_audit() {
    constexpr const char* origins[]{"shared","forest","ice","industrial"};
    std::array<std::array<int,static_cast<std::size_t>(ItemRole::Count)>,4> counts{};
    std::puts("origin,item,role,native_weight,first_stage,bundle,source_mask");
    for (unsigned i=1;i<static_cast<unsigned>(ItemKind::Count);++i) {
        const auto kind=static_cast<ItemKind>(i);const auto& supply=item_supply(kind);
        if (supply.role==ItemRole::None) continue;
        const auto origin=static_cast<std::size_t>(supply.origin);
        ++counts[origin][static_cast<std::size_t>(supply.role)];
        std::printf("%s,%s,%s,%d,%d,%d,%u\n",origins[origin],item_name(kind),
            item_role_name(supply.role),supply.weight,supply.stage,supply_count(kind),supply.sources);
    }
    for (std::size_t b=0;b<counts.size();++b) {
        std::fprintf(stderr,"%s:",origins[b]);
        for (std::size_t r=1;r<counts[b].size();++r)
            std::fprintf(stderr," %s=%d",item_role_name(static_cast<ItemRole>(r)),counts[b][r]);
        std::fputc('\n',stderr);
    }
    return 0;
}
