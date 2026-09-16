#include "worldgen_sidebar.hpp"
#include <algorithm>
#include <string>

namespace {
constexpr float width=196;
std::vector<int> children(const GenerationReport& report,int feature) {
    std::vector<int> rows;
    for(std::size_t i=0;i<report.components.size();++i)
        if(static_cast<int>(report.components[i].feature)==feature)rows.push_back(static_cast<int>(i));
    return rows;
}
int selected_row(const std::vector<int>& rows,int value) {
    const auto found=std::find(rows.begin(),rows.end(),value);
    return found==rows.end() ? 0 : static_cast<int>(found-rows.begin());
}
void text(SDL_Renderer* renderer,float y,const std::string& label,int indent=0) {
    const int chars=23-indent;
    const auto clipped=label.size()>static_cast<std::size_t>(chars) ? label.substr(0,static_cast<std::size_t>(chars-2))+".." : label;
    SDL_RenderDebugText(renderer,6+static_cast<float>(indent*8),y,clipped.c_str());
}
void wrapped(SDL_Renderer* renderer,float& y,std::string label,int lines) {
    while(!label.empty() && lines-->0) {
        std::size_t end=std::min<std::size_t>(23,label.size());
        if(end<label.size()) {const auto space=label.rfind(' ',end);if(space!=std::string::npos && space>0)end=space;}
        text(renderer,y,label.substr(0,end));y+=10;label.erase(0,end);
        while(!label.empty() && label.front()==' ')label.erase(0,1);
    }
}
const char* status(GenerationOutcome outcome) {
    switch(outcome) {
    case GenerationOutcome::Built:return "+";case GenerationOutcome::Reserved:return "R";
    case GenerationOutcome::Missed:return "-";case GenerationOutcome::Ineligible:return "X";
    case GenerationOutcome::Failed:return "!";case GenerationOutcome::Suppressed:return "S";
    case GenerationOutcome::Selected:return "*";case GenerationOutcome::Pending:return "?";
    }
    return "?";
}
}

void focus_worldgen_selection(WorldGenViewer& v,const GenerationReport& report,bool component) {
    if(report.geometry_omitted)return;
    if(component && v.selected_component>=0 && static_cast<std::size_t>(v.selected_component)<report.components.size()) {
        const auto& c=report.components[static_cast<std::size_t>(v.selected_component)];
        Cell low=!c.guide.empty() ? c.guide.front() : !c.cells.empty() ? c.cells.front() : c.anchor;
        Cell high=low+Cell{1,1};
        const auto extend=[&](Cell p){low.x=std::min(low.x,p.x);low.y=std::min(low.y,p.y);high.x=std::max(high.x,p.x+1);high.y=std::max(high.y,p.y+1);};
        for(Cell p:c.guide)extend(p);
        for(Cell p:c.cells)extend(p);
        focus_worldgen_bounds(v,low,high);return;
    }
    if(v.selected_feature<0)return;
    const auto* d=feature_decision(report,static_cast<GenerationFeature>(v.selected_feature));
    if(!d || d->regions.empty())return;
    Cell low=d->regions.front().low,high=d->regions.front().high;
    for(const auto& r:d->regions){low.x=std::min(low.x,r.low.x);low.y=std::min(low.y,r.low.y);high.x=std::max(high.x,r.high.x);high.y=std::max(high.y,r.high.y);}
    focus_worldgen_bounds(v,low,high);
}

void focus_worldgen_bounds(WorldGenViewer& v,Cell low,Cell high,float max_zoom) {
    const float panel=v.sidebar_visible ? width : 0;
    v.zoom=std::clamp(std::min((600-panel)/(8*static_cast<float>(std::max(1,high.x-low.x)+4)),
        280/(8*static_cast<float>(std::max(1,high.y-low.y)+4))),.08F,max_zoom);
    v.render.camera={static_cast<float>(low.x+high.x)*.5F-panel/(16*v.zoom),static_cast<float>(low.y+high.y)*.5F};
}

bool worldgen_sidebar_event(const SDL_Event& event,WorldGenViewer& v) {
    const bool key=event.type==SDL_EVENT_KEY_DOWN,pad=event.type==SDL_EVENT_GAMEPAD_BUTTON_DOWN;
    if(!key&&!pad)return false;
    const auto pressed=[&](SDL_Keycode k,SDL_GamepadButton b){return (key&&event.key.key==k)||(pad&&event.gbutton.button==b);};
    if(pressed(SDLK_TAB,SDL_GAMEPAD_BUTTON_BACK)) {
        if(key&&event.key.repeat)return true;
        v.sidebar_focus=!v.sidebar_focus;
        if(v.sidebar_focus){v.sidebar_visible=true;if(v.selected_feature<0)v.selected_feature=0;}
        return true;
    }
    if(!v.sidebar_focus || v.trace.checkpoints.empty())return false;
    const auto& report=v.trace.checkpoints[static_cast<std::size_t>(v.checkpoint)].report;
    if(pressed(SDLK_ESCAPE,SDL_GAMEPAD_BUTTON_EAST)){v.sidebar_focus=false;return true;}
    if(pressed(SDLK_RETURN,SDL_GAMEPAD_BUTTON_SOUTH)){focus_worldgen_selection(v,report,v.sidebar_children);return true;}
    const auto rows=children(report,v.selected_feature);
    if(pressed(SDLK_RIGHT,SDL_GAMEPAD_BUTTON_DPAD_RIGHT)) {
        if(!rows.empty()){v.sidebar_children=true;if(std::find(rows.begin(),rows.end(),v.selected_component)==rows.end())v.selected_component=rows.front();}
        return true;
    }
    if(pressed(SDLK_LEFT,SDL_GAMEPAD_BUTTON_DPAD_LEFT)){v.sidebar_children=false;v.selected_component=-1;return true;}
    const int direction=pressed(SDLK_UP,SDL_GAMEPAD_BUTTON_DPAD_UP) ? -1 : pressed(SDLK_DOWN,SDL_GAMEPAD_BUTTON_DPAD_DOWN) ? 1 : 0;
    if(direction) {
        if(v.sidebar_children) {
            if(!rows.empty())v.selected_component=rows[static_cast<std::size_t>(std::clamp(selected_row(rows,v.selected_component)+direction,0,static_cast<int>(rows.size())-1))];
        } else {
            v.selected_feature=std::clamp(v.selected_feature+direction,0,static_cast<int>(generation_rules.size())-1);v.selected_component=-1;
        }
        return true;
    }
    return false;
}

void draw_worldgen_sidebar(SDL_Renderer* renderer,const WorldGenViewer& v,const GenerationReport& report) {
    if(!v.sidebar_visible)return;
    SDL_SetRenderDrawColor(renderer,12,17,18,245);const SDL_FRect panel{0,24,width,297};SDL_RenderFillRect(renderer,&panel);
    SDL_SetRenderDrawColor(renderer,v.sidebar_focus ? 255 : 145,v.sidebar_focus ? 226 : 170,130,255);
    text(renderer,29,v.sidebar_children ? "CHILD ROLLS" : "FLOOR ROLLS");
    text(renderer,40,v.sidebar_focus ? "D-pad select | A focus" : "Back/Tab: browse rolls");
    const auto rows=children(report,v.selected_feature);
    const int count=v.sidebar_children ? static_cast<int>(rows.size()) : static_cast<int>(generation_rules.size());
    const int selected=v.sidebar_children ? selected_row(rows,v.selected_component) : v.selected_feature;
    const int first=std::clamp(selected-5,0,std::max(0,count-11));
    for(int n=first;n<std::min(count,first+11);++n) {
        const float y=56+static_cast<float>((n-first)*12);
        if(n==selected){SDL_SetRenderDrawColor(renderer,60,67,45,255);const SDL_FRect box{2,y-1,width-4,12};SDL_RenderFillRect(renderer,&box);}
        SDL_SetRenderDrawColor(renderer,n==selected ? 255 : 185,n==selected ? 235 : 194,n==selected ? 145 : 181,255);
        if(v.sidebar_children) {
            const auto& c=report.components[static_cast<std::size_t>(rows[static_cast<std::size_t>(n)])];
            int depth=0,parent=c.parent;
            while(parent>=0&&static_cast<std::size_t>(parent)<report.components.size()&&depth<3){++depth;parent=report.components[static_cast<std::size_t>(parent)].parent;}
            text(renderer,y,c.slot+": "+c.choice,depth);
        } else {
            const auto& rule=generation_rules[static_cast<std::size_t>(n)];const auto* d=feature_decision(report,rule.feature);
            text(renderer,y,std::string(status(d ? d->outcome : GenerationOutcome::Pending))+" "+rule.name);
        }
    }
    SDL_SetRenderDrawColor(renderer,170,184,177,255);
    text(renderer,192,std::to_string(count ? first+1 : 0)+"-"+std::to_string(std::min(count,first+11))+" / "+std::to_string(count));
    float y=205;
    if(v.sidebar_children && !rows.empty()) {
        const auto& c=report.components[static_cast<std::size_t>(rows[static_cast<std::size_t>(selected_row(rows,v.selected_component))])];
        wrapped(renderer,y,c.slot+": "+c.choice,3);
        wrapped(renderer,y,c.result,3);
        text(renderer,y,"Cells "+std::to_string(c.placed)+" | ticket "+std::to_string(c.ticket)+"/"+std::to_string(c.total));
    } else if(v.selected_feature>=0) {
        const auto& rule=generation_rules[static_cast<std::size_t>(v.selected_feature)];const auto* d=feature_decision(report,rule.feature);
        wrapped(renderer,y,rule.name,2);
        wrapped(renderer,y,d ? generation_outcome_name(d->outcome) : "Pending",2);
        if(d)wrapped(renderer,y,d->variant.empty() ? d->reason : d->variant,4);
    } else {wrapped(renderer,y,"+ built | R reserved",2);wrapped(renderer,y,"- missed | X ineligible",2);}
    SDL_SetRenderDrawColor(renderer,195,197,173,255);
    text(renderer,299,v.sidebar_focus ? "Right: child  Left: up" : "Start/F1: weights/info");
    text(renderer,310,v.sidebar_focus ? "B/Back/Tab: map" : "+ built  ! failed  ? next");
}
