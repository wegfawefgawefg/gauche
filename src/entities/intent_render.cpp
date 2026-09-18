#include "intent_render.hpp"
#include "attacks.hpp"
#include "glass_eel.hpp"
#include "lens_warden.hpp"
#include "../surfaces/conduction.hpp"

#include <algorithm>

void draw_enemy_intents(tr::Renderer* renderer, const Game& game,
                        ViewCamera camera, float zoom, const LightingCache& lighting) {
    tr::set_blend(renderer, SDL_BLENDMODE_BLEND);
    for (const Entity& enemy : game.entities) {
        if (enemy.health <= 0 || enemy.sleep_ticks > 0 || enemy.stun_ticks > 0) continue;
        EnemyAttack attack = enemy_attack(enemy);
        const bool optical = enemy.kind == EntityKind::LensWarden && enemy.label_a == WardenCharge;
        if (optical) {
            const BeamTrace beam = warden_beam(game, enemy);
            for (int i = 0; i < beam.count && attack.count < static_cast<int>(attack.cells.size()); ++i)
                attack.cells[static_cast<std::size_t>(attack.count++)] = beam.cells[static_cast<std::size_t>(i)].cell;
        }
        const bool water = enemy.kind == EntityKind::GlassEel && enemy.label_a == EelCharge;
        if (water) {
            const WetWave wave = wet_wave(game, enemy.cell, eel_shock_reach);
            if (wave.ground_node>=0) continue;
            for (int i = 0; i < wave.count && attack.count < static_cast<int>(attack.cells.size()); ++i)
                attack.cells[static_cast<std::size_t>(attack.count++)] = wave.nodes[static_cast<std::size_t>(i)].cell;
        }
        if (attack.count == 0) continue;
        const LightColor seen = lit_sprite_color(lighting, enemy.cell);
        if (std::max({seen.red, seen.green, seen.blue}) < .10F) continue;
        for (int i = 0; i < attack.count; ++i) {
            const Cell cell = attack.cells[static_cast<std::size_t>(i)];
            // Smoke can prevent acquisition, but cannot cancel a committed burst.
            if (!optical && !water && !clear_sight(game, enemy.cell, cell,
                enemy.kind!=EntityKind::RivetGunner)) continue;
            SDL_FRect rect = tile_rect(cell, camera, zoom);
            if (rect.x < -rect.w || rect.y < -rect.h || rect.x > 640 || rect.y > 360) continue;
            const LightColor brightness = lit_sprite_color(lighting, cell);
            const float level = std::clamp(std::max({brightness.red, brightness.green, brightness.blue}), 0.0F, 1.0F);
            const auto alpha = static_cast<Uint8>(level * 170);
            // INSET: Deliberate committed attacks stay legible beside tile seams and props.
            rect.x += 1; rect.y += 1; rect.w -= 2; rect.h -= 2;
            tr::set_color_bytes(renderer, attack.sleep ? 158 : 218, attack.sleep ? 152 : 100,
                                   attack.sleep ? 209 : 78, static_cast<Uint8>(alpha / 4));
            tr::fill_rect(renderer, &rect);
            tr::set_color_bytes(renderer, attack.sleep ? 158 : 218, attack.sleep ? 152 : 100,
                                   attack.sleep ? 209 : 78, alpha);
            tr::rect(renderer, &rect);
        }
    }
    tr::set_blend(renderer, SDL_BLENDMODE_NONE);
}
