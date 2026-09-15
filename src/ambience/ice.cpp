#include "system.hpp"
#include "../world/water.hpp"

void place_ice_ambience(AmbientAudio& audio, const Game& game, Cell listener) {
    const auto add=[&](AmbientCue cue,Cell cell,bool global=false,Handle owner={},PropKind prop=PropKind::None) {
        add_ambient_source(audio,game,listener,cue,cell,global,owner,prop);
    };
    // OUTSIDE: Quiet local schedules, never game.rng or gameplay hearing events.
    add(AmbientCue::ThinWind,game.run.spawn,true);
    add(AmbientCue::DistantBell,game.run.spawn,true);
    add(AmbientCue::FarIceCall,game.run.spawn,true);
    for (const auto& site:make_ice_scenes(game)) {
        const auto scene_sound=[&](AmbientCue cue) {
            const auto before=audio.sources.size();
            add(cue,site.cell,false,{},site.prop);
            if (audio.sources.size()==before) return;
            audio.sources.back().scene=site; audio.sources.back().has_scene=true;
        };
        switch (site.kind) {
        case IceSceneKind::Window: scene_sound(AmbientCue::WindowWhistle); scene_sound(AmbientCue::TimberCreak); break;
        case IceSceneKind::Chain: scene_sound(AmbientCue::HangingChain); break;
        case IceSceneKind::Hatch: scene_sound(AmbientCue::SubmergedKnock); break;
        case IceSceneKind::Pipe: scene_sound(AmbientCue::PressureHiss); scene_sound(AmbientCue::PipeKnock); break;
        case IceSceneKind::Wheel: scene_sound(AmbientCue::Waterwheel); break;
        case IceSceneKind::Motor: scene_sound(AmbientCue::ObservatoryMotor); break;
        case IceSceneKind::Flag: scene_sound(AmbientCue::ClothFlutter); break;
        case IceSceneKind::Drip: scene_sound(AmbientCue::ThawDrip); break;
        case IceSceneKind::Chimney: scene_sound(AmbientCue::ChimneyDraft); break;
        }
    }
    for (int y=1;y<game.stage.height-1;++y)
        for (int x=1;x<game.stage.width-1;++x) {
            const Cell cell{x,y}; const auto& tile=game.stage.at_or_border(cell);
            if (tile.kind==TileKind::Ice) add(AmbientCue::IceGroan,cell);
            if (tile.kind==TileKind::Snow) add(AmbientCue::SnowSettling,cell);
            if (tile.kind==TileKind::Water || shallow_water(tile.kind)) {
                add(AmbientCue::UnderIce,cell); add(AmbientCue::SlushLap,cell);
            }
            if (tile.prop.kind==PropKind::LensCase && !tile.prop.broken)
                add(AmbientCue::GlassTinkle,cell,false,{},PropKind::LensCase);
        }
    for (int slot=0;slot<max_entities;++slot) {
        const auto& actor=game.entities[static_cast<std::size_t>(slot)];
        if (actor.kind==EntityKind::BoilerTank && actor.health>0)
            add(AmbientCue::BoilerIdle,actor.cell,false,{slot,actor.generation});
    }
}
