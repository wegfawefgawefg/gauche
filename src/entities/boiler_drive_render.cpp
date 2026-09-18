#include "boiler_drive_render.hpp"
#include "boiler_drive.hpp"
#include <cmath>

void draw_steam_drive(tr::Renderer* renderer,const GameGraphics& graphics,const Game& game,
    Cell cell,SDL_FRect rect,LightColor light,std::uint64_t tick) {
    const auto& prop=game.stage.at_or_border(cell).prop;
    const BoilerFeed* supply=nullptr;
    for (const auto& feed:game.boiler_feeds) if (feed.drive==cell && !feed.belts.empty()) {supply=&feed;break;}
    const bool running=supply && steam_drive_powered(game,*supply);
    tr::Texture* texture=texture_for(graphics,prop.broken ? Sprite::SteamDriveBroken : running ? Sprite::SteamDrive : Sprite::SteamDriveOff);
    tr::texture_color(texture,light.red,light.green,light.blue);
    tr::draw_texture(renderer,texture,nullptr,&rect);tr::texture_color(texture,1,1,1);
    if (prop.broken || !supply) return;
    const float pixel=rect.w/16;
    // Short rigid connection shows which vessel owns the drive. Long conveyor
    // transmission follows the saved belt itself, including its real breaks.
    const Cell inlet=supply->mount-cell;
    tr::set_color(renderer,light.red*.52F,light.green*.52F,light.blue*.38F,1);
    tr::line(renderer,rect.x+rect.w*.5F,rect.y+rect.h*.5F,
        rect.x+rect.w*(.5F+static_cast<float>(inlet.x)*.85F),rect.y+rect.h*(.5F+static_cast<float>(inlet.y)*.85F));
    const float angle=running ? static_cast<float>(tick%60)*.104719755F : .7F;
    const float cx=rect.x+pixel*9,cy=rect.y+pixel*8;
    for (int spoke=0;spoke<3;++spoke) {
        const float a=angle+static_cast<float>(spoke)*2.094395F;
        tr::line(renderer,cx,cy,cx+std::cos(a)*pixel*3,cy+std::sin(a)*pixel*3);
    }
}
