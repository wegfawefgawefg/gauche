#include "data_migration.hpp"
#include <fstream>

bool migrate_user_data(const std::filesystem::path& previous,
                       const std::filesystem::path& current, std::string& error) {
    const auto marker=current/".name-migration-complete";
    std::error_code failure;
    if (std::filesystem::exists(marker,failure)) return true;
    if (!failure) std::filesystem::create_directories(current,failure);
    if (!failure && std::filesystem::exists(previous,failure)) {
        // Existing Teeming settings win. Keep the old tree intact, and retry
        // incomplete copies next launch instead of silently losing profiles.
        if (!failure) std::filesystem::copy(previous,current,
            std::filesystem::copy_options::recursive |
            std::filesystem::copy_options::skip_existing |
            std::filesystem::copy_options::copy_symlinks,failure);
    }
    if (failure) {error=failure.message();return false;}
    std::ofstream done(marker);
    done<<"Teeming data migration complete\n";
    done.close();
    if (!done) {error="Could not mark data migration complete";return false;}
    return true;
}
