#pragma once
#include "route.hpp"
#include <memory>
#include <string>

struct GenerationCheckpoint {
    std::string name;
    std::unique_ptr<Game> game;
    std::vector<RoomPlan> rooms;
    GenerationReport report;
    bool detail=false;
    int component=-1;
};
struct GenerationTraceOptions {
    bool details=false;
    int feature=-1; // -1: every instrumented feature.
    int every=1;
};
// Local observation only. Fine captures cannot consume the coarse pass slots.
struct GenerationTrace {
    static constexpr std::size_t limit=36,detail_limit=32;
    static constexpr std::size_t detail_byte_limit=48U*1024U*1024U;
    GenerationTraceOptions options;
    std::vector<GenerationCheckpoint> checkpoints;
    std::size_t coarse_count=0,detail_count=0,detail_seen=0,bytes=0,detail_bytes=0;
    bool truncated=false,detail_truncated=false;
    void reset();
    void capture(const char* name,const Game& game,const FloorPlan& plan);
    void capture_detail(const char* name,GenerationFeature feature,int component,const Game& game,const FloorPlan& plan);
};
// Records the resolved attempt on normal scope exit, including continue/return
// after a rejection or rollback. No gameplay object depends on this guard.
struct GenerationStep {
    GenerationTrace* trace;
    const Game& game;
    const FloorPlan& plan;
    const char* name;
    GenerationFeature feature;
    int component;
    ~GenerationStep() {if (trace) trace->capture_detail(name,feature,component,game,plan);}
};
