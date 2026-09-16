#include "sleep.hpp"
#include <algorithm>

void observe_sleep(Cosmetics& cosmetics,const Game& game,Cell focus) {
    // Owner handles stop stale Zs on waking, death, despawn or slot reuse.
    std::erase_if(cosmetics.sprites,[&](const SpriteParticle& mote) {
        if (mote.sprite!=Sprite::SleepZ) return false;
        const auto* actor=get_entity(game,mote.owner);
        return !actor || actor->health<=0 || actor->sleep_ticks<=0;
    });
    if (cosmetics.sleep_tick==game.tick) return;
    cosmetics.sleep_tick=game.tick;
    int count=static_cast<int>(std::count_if(cosmetics.sprites.begin(),cosmetics.sprites.end(),
        [](const SpriteParticle& p){return p.sprite==Sprite::SleepZ;}));
    for (int slot=0;slot<max_entities && count<96;++slot) {
        const auto& actor=game.entities[static_cast<std::size_t>(slot)];
        if (actor.kind==EntityKind::None || actor.health<=0 || actor.sleep_ticks<=0 ||
            actor.move_interval<=0 || distance(actor.cell,focus)>18 ||
            (game.tick+static_cast<unsigned>(slot)*17)%48!=0 || cosmetics.sprites.size()>=2048) continue;
        SpriteParticle mote;mote.sprite=Sprite::SleepZ;mote.owner={slot,actor.generation};
        mote.motion=ParticleMotion::Drift;mote.layer=ParticleLayer::Foreground;
        mote.x=static_cast<float>(actor.cell.x)+.75F;mote.y=static_cast<float>(actor.cell.y)+.1F;
        mote.vx=.0025F;mote.vy=-.008F;mote.width=mote.height=.55F;mote.alpha=.8F;
        mote.life=mote.span=80;cosmetics.sprites.push_back(mote);++count;
    }
}
