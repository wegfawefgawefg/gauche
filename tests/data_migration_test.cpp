#include "../src/app/data_migration.hpp"
#include <chrono>
#include <cstdio>
#include <fstream>

int main() {
    namespace fs=std::filesystem;
    const auto root=fs::temp_directory_path()/("teeming-data-test-"+
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    const auto old=root/"previous", current=root/"current";
    fs::create_directories(old/"gubsy");fs::create_directories(current/"gubsy");
    std::ofstream(old/"gubsy/settings")<<"old";
    std::ofstream(current/"gubsy/settings")<<"new";
    std::ofstream(old/"loadout")<<"saved kit";
    std::string error, value;
    bool okay=migrate_user_data(old,current,error);
    std::ifstream(current/"gubsy/settings")>>value;
    okay &= value=="new" && fs::exists(current/"loadout") && fs::exists(old/"loadout");
    fs::remove(current/"loadout");
    okay &= migrate_user_data(old,current,error) && !fs::exists(current/"loadout");
    std::ofstream(root/"blocked")<<"not a directory";
    okay &= !migrate_user_data(old,root/"blocked",error) && fs::exists(old/"loadout");
    okay &= migrate_user_data(root/"absent",root/"fresh",error);
    fs::remove_all(root);
    if (!okay) {std::fprintf(stderr,"Data migration failed: %s\n",error.c_str());return 1;}
    std::puts("Data migration passed: preserved profiles, existing settings, retry safety and first launch");
}
