#pragma once

#include <cstddef>

enum class SoundId : std::size_t {
    ApeScream, BallBounce1, BallBounce2, BallBounce3, BallBounce4,
    BallDrop, BallHitPaddle, BallHitPaddleEr, BallWallBounce,
    Confirm, Death, Explosion, Explosion1, Explosion2, Explosion3,
    AnimalCrush1, AnimalCrush2, HitBlock1, LevelLoss, LevelStart,
    LevelWin, SmallLaser, SturdyBlockBouncedOn, SuperConfirm,
    Step1, Step2, BoxBreak, BlockLand, ZombieGrowl1, ZombieGrowl2,
    ZombieScratch1, Punch1, ClothRip, CantUse, Chick, Hen, Rooster,
    DistantTrainSound, RailPlace, TrainPassing, Drop,
    Count,
};
