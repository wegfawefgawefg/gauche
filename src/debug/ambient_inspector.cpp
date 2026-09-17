#include "ambient_inspector.hpp"
#include "worldgen.hpp"
#include <imgui.h>
#include <algorithm>

AmbientInspector& ambient_inspector(){static AmbientInspector state;return state;}

void prepare_ambient_inspector(const Game& live,const AmbientAudio& audio,Cell listener) {
    auto& state=ambient_inspector();
    state.game=nullptr;state.audio=nullptr;
    if(!GAUCHE_DEV_MODE || (!state.visible && !state.overlay))return;
    const auto& viewer=worldgen_viewer();
    const bool preview=viewer.active && viewer.original && !viewer.trace.checkpoints.empty();
    const Game& game=preview ? *viewer.trace.checkpoints[static_cast<std::size_t>(viewer.checkpoint)].game : live;
    const auto report=preview ? viewer.original->generation_report : live.generation_report;
    const bool changed=preview!=state.preview || state.report.lock()!=report || (!preview && state.world_key!=audio.world_key);
    if(changed)state.selected=-1;
    if(preview && (changed || state.preview_game!=&game)) {
        state.preview_audio.sources.clear();
        place_ambience(state.preview_audio,game,game.run.spawn);
        state.preview_game=&game;state.selected=-1;
    }
    state.preview=preview;state.report=report;state.world_key=audio.world_key;
    state.game=&game;state.audio=preview ? &state.preview_audio : &audio;
    state.listener=preview ? game.run.spawn : listener;
}

namespace {
const char* mode_name(AmbientMode mode) {
    switch(mode){case AmbientMode::Loop:return "Loop";case AmbientMode::Enter:return "Enter";case AmbientMode::Occasional:return "Occasional";}
    return "Unknown";
}
bool voiced(const AmbientAudio& audio,int index) {
    for(const auto& voice:audio.loops)if(voice.source==index && voice.track && MIX_TrackPlaying(voice.track))return true;
    for(const auto& voice:audio.events)if(voice.source==index && voice.track && MIX_TrackPlaying(voice.track))return true;
    return false;
}
}

void draw_ambient_inspector() {
    auto& s=ambient_inspector();
    if(!s.visible)return;
    ImGui::SetNextWindowSize({540,560},ImGuiCond_FirstUseEver);
    if(ImGui::Begin("Ambient sound inspector",&s.visible)) {
        ImGui::Checkbox("Map overlay",&s.overlay);ImGui::SameLine();ImGui::Checkbox("Mute ambience",&s.mute);
        ImGui::Checkbox("Labels",&s.labels);ImGui::SameLine();ImGui::Checkbox("Hearing ranges",&s.ranges);
        ImGui::Checkbox("Entry boundaries",&s.triggers);ImGui::SameLine();ImGui::Checkbox("Selected source only",&s.selected_only);
        ImGui::Combo("Cue mode",&s.mode,"All\0Loops\0Enter triggers\0Occasional events\0");
        if(!s.game || !s.audio)ImGui::TextUnformatted("Source list available on the next frame.");
        else {
            ImGui::TextWrapped("%s",s.preview ? "Preview of selected generation checkpoint. Sources are derived from its terrain/props, not captured audio events. No playback or triggered history; Play inspects the real local scheduler." : "Live local listener. Trigger history belongs to this client, not shared gameplay. F1 hides panels; enabled map overlays stay visible.");
            ImGui::Text("%zu sources | listener %d,%d%s",s.audio->sources.size(),s.listener.x,s.listener.y,s.preview ? " (spawn)" : "");
            ImGui::TextWrapped("Cyan circles: near/far volume falloff. Orange diamonds: Manhattan entry distance. Global sounds cover the whole floor and have no distance boundary.");
            if(ImGui::BeginChild("sources",{0,170},ImGuiChildFlags_Borders)) {
                for(std::size_t i=0;i<s.audio->sources.size();++i) {
                    const auto& source=s.audio->sources[i];const auto& spec=ambient_specs[static_cast<std::size_t>(source.cue)];
                    if(s.mode && static_cast<int>(spec.mode)!=s.mode-1)continue;
                    const std::string label="#"+std::to_string(i)+" "+spec.name+" / "+mode_name(spec.mode)+(source.global ? " / global" : "");
                    if(ImGui::Selectable(label.c_str(),s.selected==static_cast<int>(i)))s.selected=static_cast<int>(i);
                }
            }ImGui::EndChild();
            if(s.selected>=0 && static_cast<std::size_t>(s.selected)<s.audio->sources.size()) {
                const auto& source=s.audio->sources[static_cast<std::size_t>(s.selected)];const auto& spec=ambient_specs[static_cast<std::size_t>(source.cue)];
                const Cell cell=ambient_source_cell(source,*s.game);
                ImGui::Text("%s at %d,%d",spec.name,cell.x,cell.y);
                ImGui::Text("Gain at listener %.3f (before volume/mute)",static_cast<double>(ambient_source_gain(source,*s.game,s.listener)));
                ImGui::Text("Near/far %.1f / %.1f | trigger %.1f",static_cast<double>(spec.near_radius),static_cast<double>(spec.far_radius),static_cast<double>(spec.trigger_radius));
                ImGui::Text("Rearms: %s | cooldown %.1fs",spec.rearm ? "yes" : "no",static_cast<double>(spec.cooldown));
                if(!s.preview)ImGui::TextWrapped("Inside %s | pending %s | entry consumed %s | voice %s | remaining cooldown %.1fs",
                    source.inside ? "yes" : "no",source.pending ? "yes" : "no",source.consumed ? "yes" : "no",voiced(*s.audio,s.selected) ? "playing" : "idle",static_cast<double>(source.cooldown));
                if(s.preview && ImGui::Button("Focus source")) {
                    auto& viewer=worldgen_viewer();viewer.render.camera={cell};viewer.zoom=1.5F;
                }
            }
            ImGui::TextWrapped("Mute changes volume only; live schedules keep running. Labels skip overlaps; select one source to isolate its boundaries.");
        }
    }ImGui::End();
}
