#include "prompts.hpp"
#include "text.hpp"
#include <algorithm>
#include <cmath>

float draw_prompt(SDL_Renderer* renderer, float x, float y, const InputPrompt& prompt) {
    const bool shape = prompt.face >= SDL_GAMEPAD_BUTTON_LABEL_CROSS;
    const float width = shape ? 14 : std::max(14.0F, 6 * static_cast<float>(prompt.label.size()) + 6);
    const SDL_FRect shadow{x + 1, y + 2, width, 12}, face{x, y, width, 12};
    SDL_SetRenderDrawColor(renderer, 3, 5, 4, 255); SDL_RenderFillRect(renderer, &shadow);
    SDL_SetRenderDrawColor(renderer, 45, 49, 45, 255); SDL_RenderFillRect(renderer, &face);
    SDL_Color color{215, 211, 187, 255};
    if (prompt.gamepad) {
        switch (prompt.face) {
        case SDL_GAMEPAD_BUTTON_LABEL_A: case SDL_GAMEPAD_BUTTON_LABEL_TRIANGLE: color = {116, 206, 140, 255}; break;
        case SDL_GAMEPAD_BUTTON_LABEL_B: case SDL_GAMEPAD_BUTTON_LABEL_CIRCLE: color = {235, 115, 111, 255}; break;
        case SDL_GAMEPAD_BUTTON_LABEL_X: case SDL_GAMEPAD_BUTTON_LABEL_CROSS: color = {131, 180, 239, 255}; break;
        case SDL_GAMEPAD_BUTTON_LABEL_Y: color = {234, 209, 102, 255}; break;
        case SDL_GAMEPAD_BUTTON_LABEL_SQUARE: color = {218, 151, 211, 255}; break;
        default: break;
        }
    }
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderRect(renderer, &face);
    if (shape) {
        const float cx = x + width * .5F, cy = y + 6;
        if (prompt.face == SDL_GAMEPAD_BUTTON_LABEL_CROSS) {
            SDL_RenderLine(renderer, cx-3, cy-3, cx+3, cy+3); SDL_RenderLine(renderer, cx+3, cy-3, cx-3, cy+3);
        } else if (prompt.face == SDL_GAMEPAD_BUTTON_LABEL_TRIANGLE) {
            const SDL_FPoint points[]{{cx,cy-4},{cx+4,cy+3},{cx-4,cy+3},{cx,cy-4}};
            SDL_RenderLines(renderer, points, 4);
        } else if (prompt.face == SDL_GAMEPAD_BUTTON_LABEL_SQUARE) {
            const SDL_FRect box{cx-3,cy-3,6,6}; SDL_RenderRect(renderer, &box);
        } else {
            SDL_FPoint points[13];
            for (int i=0; i<13; ++i) { const float angle=static_cast<float>(i)*6.2831853F/12;
                points[i]={cx+std::cos(angle)*3.5F,cy+std::sin(angle)*3.5F}; }
            SDL_RenderLines(renderer, points, 13);
        }
    } else small_ui_text(renderer, x+3, y+2, prompt.label, color.r, color.g, color.b);
    return width;
}

void draw_action_hint(SDL_Renderer* renderer, float x, float y, Action action, std::string_view label) {
    const float width = draw_prompt(renderer, x, y, action_prompt(action));
    small_ui_text(renderer, x+width+5, y+2, label, 218, 198, 152);
}
