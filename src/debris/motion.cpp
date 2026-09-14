#include "system.hpp"

#include <algorithm>
#include <cmath>

namespace {

constexpr std::size_t limit = 4096;

std::size_t bucket(Cell cell) {
    return (static_cast<std::uint32_t>(cell.x) * 73856093U ^
            static_cast<std::uint32_t>(cell.y) * 19349663U) % 1024U;
}

Cell cell_at(float x, float y) {
    return {static_cast<int>(std::floor(x)), static_cast<int>(std::floor(y))};
}

bool free_cell(const Stage& stage, float x, float y) {
    const Tile* tile = stage.at(cell_at(x, y));
    return tile != nullptr && walkable(*tile);
}

bool light_material(DebrisKind kind) {
    return kind == DebrisKind::OakLeaf || kind == DebrisKind::BirchLeaf ||
           kind == DebrisKind::FernLeaf || kind == DebrisKind::GrassBlade ||
           kind == DebrisKind::Feather || kind == DebrisKind::Spore;
}

} // namespace

void rebuild_debris_index(LooseDebris& debris) {
    debris.buckets.fill(-1);
    for (std::size_t i = 0; i < debris.pieces.size(); ++i) {
        LoosePiece& piece = debris.pieces[i];
        const std::size_t bin = bucket(cell_at(piece.x, piece.y));
        piece.next = debris.buckets[bin];
        debris.buckets[bin] = static_cast<int>(i);
    }
}

void add_loose_piece(LooseDebris& debris, LoosePiece piece) {
    const Cell cell = cell_at(piece.x, piece.y);
    const std::size_t bin = bucket(cell);
    int occupants = 0;
    for (int i = debris.buckets[bin]; i >= 0; i = debris.pieces[static_cast<std::size_t>(i)].next) {
        LoosePiece& other = debris.pieces[static_cast<std::size_t>(i)];
        if (cell_at(other.x, other.y) != cell) continue;
        ++occupants;
        if (other.kind == piece.kind && other.settled > 20 && other.count < 12) {
            ++other.count;
            return;
        }
    }
    if (occupants >= 12) return;
    if (debris.pieces.size() >= limit) {
        // BUDGET: Recycle settled scraps; a busy explosion never grows the collection.
        for (std::size_t n = 0; n < limit; ++n) {
            const std::size_t i = debris.recycle++ % limit;
            if (debris.pieces[i].settled <= 20) continue;
            debris.pieces[i] = piece;
            rebuild_debris_index(debris);
            return;
        }
        return;
    }
    piece.next = debris.buckets[bin];
    debris.buckets[bin] = static_cast<int>(debris.pieces.size());
    debris.pieces.push_back(piece);
}

void push_debris(LooseDebris& debris, Cell cell, float radius, float force, Cell direction) {
    const int extent = static_cast<int>(std::ceil(radius));
    std::vector<LoosePiece> scattered;
    for (int y = cell.y - extent; y <= cell.y + extent; ++y)
        for (int x = cell.x - extent; x <= cell.x + extent; ++x) {
            const Cell query{x, y};
            for (int i = debris.buckets[bucket(query)]; i >= 0;
                 i = debris.pieces[static_cast<std::size_t>(i)].next) {
                LoosePiece& p = debris.pieces[static_cast<std::size_t>(i)];
                if (cell_at(p.x, p.y) != query) continue;
                float dx = p.x - (static_cast<float>(cell.x) + .5F);
                float dy = p.y - (static_cast<float>(cell.y) + .5F);
                const float length = std::sqrt(dx * dx + dy * dy);
                if (length > radius) continue;
                if (length < .03F) { dx = .4F; dy = -.3F; }
                const float falloff = force * (1 - length / radius);
                p.vx += falloff * (dx / std::max(.1F, length) + static_cast<float>(direction.x));
                p.vy += falloff * (dy / std::max(.1F, length) + static_cast<float>(direction.y));
                p.spin = p.vx * 50;
                p.settled = 0;
                if (p.count > 1) {
                    LoosePiece split = p;
                    split.count = 1;
                    split.vx = -p.vy; split.vy = p.vx;
                    split.x += .05F;
                    --p.count;
                    scattered.push_back(split);
                }
            }
        }
    for (LoosePiece piece : scattered) add_loose_piece(debris, piece);
}

void step_debris(LooseDebris& debris, const Stage& stage, std::uint64_t tick, bool wind) {
    for (std::size_t i = 0; i < debris.pieces.size(); ++i) {
        LoosePiece& p = debris.pieces[i];
        if (wind && light_material(p.kind) && (tick + i * 17) % 360 == 0 &&
            stage.at_or_border(cell_at(p.x, p.y)).kind == TileKind::Grass) {
            p.vx += .012F; p.vy -= .004F; p.settled = 0;
        }
        if (p.settled > 0) { p.settled = static_cast<std::uint8_t>(std::min(255, p.settled + 1)); continue; }
        p.vx = std::clamp(p.vx, -.35F, .35F);
        p.vy = std::clamp(p.vy, -.35F, .35F);
        // WALLS: Resolve each axis separately so a scrap slides along a wall, not through it.
        if (free_cell(stage, p.x + p.vx, p.y)) p.x += p.vx;
        else p.vx *= -.2F;
        if (free_cell(stage, p.x, p.y + p.vy)) p.y += p.vy;
        else p.vy *= -.2F;
        const float friction = light_material(p.kind) ? .92F : .83F;
        p.vx *= friction; p.vy *= friction;
        p.angle += p.spin;
        p.spin *= .88F;
        if (std::abs(p.vx) + std::abs(p.vy) < .001F) {
            p.vx = p.vy = p.spin = 0;
            p.settled = 1;
        }
    }
    rebuild_debris_index(debris);
}
