#include "snake.hpp"
#include "behavior.hpp"
#include "attacks.hpp"

namespace {
void recover(Entity& snake,int ticks) {
    snake.label_a=SnakeRecover;snake.timer_a=ticks;snake.sprite=Sprite::Snake;snake.self_light={};
}
}
void init_snake(Entity& snake) {
    snake.health=snake.max_health=22;snake.impassable=true;snake.move_interval=23;
    snake.counter_a=snake.cell.x;snake.counter_b=snake.cell.y;recover(snake,35);
}
void interrupt_snake(Entity& snake) {
    if (snake.kind==EntityKind::Snake && snake.health>0) recover(snake,48);
}
void snake_timers(Entity& snake) {
    if (snake.kind!=EntityKind::Snake) return;
    if (snake.label_a==SnakeCoil && (snake.stun_ticks || snake.sleep_ticks || snake.vitals.rooted || snake.toss.ticks))
        recover(snake,48);
    if (snake.label_a==SnakeRecover && !snake.use_flash) snake.sprite=Sprite::Snake;
}
void step_snake(Game& game,int slot) {
    auto& snake=game.entities[static_cast<std::size_t>(slot)];
    if (snake.label_a==SnakeCoil) {
        if (snake.cell!=snake.point_a || snake.vitals.rooted || snake.toss.ticks) {recover(snake,48);return;}
        if (snake.timer_a) return;
        resolve_enemy_attack(game,slot,9,SoundId::SpiderBite);
        recover(snake,72);snake.sprite=Sprite::SnakeStrike;return;
    }
    if (snake.label_a==SnakeRecover && snake.timer_a) return;
    snake.label_a=SnakeHunt;
    const auto target=enemy_target(game,snake.cell,8);
    if (target && clear_attack_sight(game,snake.cell,target->cell)) {
        const Cell delta=target->cell-snake.cell;
        if (distance(snake.cell,target->cell)<=2 && (delta.x==0 || delta.y==0)) {
            snake.point_a=snake.cell;snake.point_b=target->cell;snake.facing=cardinal_toward(snake.cell,target->cell,snake.facing);
            snake.label_a=SnakeCoil;snake.timer_a=45;snake.sprite=Sprite::SnakeCoil;
            snake.self_light={18,22,8};emit_sound(game,SoundId::SpiderWarn,snake.cell);
        } else pursue(game,slot,target->cell);
    } else if (distance(snake.cell,{snake.counter_a,snake.counter_b})>4) approach(game,slot,{snake.counter_a,snake.counter_b});
    else if ((game.tick+snake.generation)%120==0) wander(game,slot);
}
