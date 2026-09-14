#pragma once

#include <cstddef>
#include <array>
#include <string_view>

enum class SoundId : std::size_t {
    ApeScream, BallBounce1, BallBounce2, BallBounce3, BallBounce4,
    BallDrop, BallHitPaddle, BallHitPaddleEr, BallWallBounce,
    Confirm, Death, Explosion, Explosion1, Explosion2, Explosion3,
    AnimalCrush1, AnimalCrush2, HitBlock1, LevelLoss, LevelStart,
    LevelWin, SmallLaser, SturdyBlockBouncedOn, SuperConfirm,
    Step1, Step2, BoxBreak, BlockLand, ZombieGrowl1, ZombieGrowl2,
    ZombieScratch1, Punch1, ClothRip, CantUse, Chick, Hen, Rooster,
    DistantTrainSound, RailPlace, TrainPassing, Drop,
    FirePanic, FireTrample, FireOut,
    Count,
};

inline constexpr std::array<std::string_view, static_cast<std::size_t>(SoundId::Count)> sound_names{
    "ape_scream", "ball_bounce1", "ball_bounce2", "ball_bounce3", "ball_bounce4",
    "ball_drop", "ball_hit_paddle", "ball_hit_paddle_er", "ball_wall_bounce",
    "confirm", "death", "explosion", "explosion1", "explosion2", "explosion3",
    "animal_crush1", "animal_crush2", "hit_block1", "level_loss", "level_start",
    "level_win", "small_laser", "sturdy_block_bounced_on", "super_confirm",
    "step1", "step2", "box_break", "block_land", "zombie_growl1", "zombie_growl2",
    "zombie_scratch1", "punch1", "cloth_rip", "cant_use", "chick", "hen", "rooster",
    "distant_train_sound", "rail_place", "train_passing", "drop", "fire_panic", "fire_trample", "fire_out",
};
