#pragma once
#include "../game.hpp"
#include <span>

// Geometry only: deterministic integer rasterization, clipped to the supplied
// grid. Material policy, reservations and connectivity belong to the caller.
struct RasterShape { std::vector<Cell> cells; bool truncated=false; };
RasterShape raster_polygon(std::span<const Cell> vertices,int width,int height,std::size_t limit=8192);
RasterShape raster_line(Cell a,Cell b,int radius,int width,int height,std::size_t limit=8192);
// Four-connected portion of a proposed footprint, clipped by a caller-owned mask.
std::vector<Cell> connected_raster(Cell start,std::span<const Cell> cells,
    std::span<const std::uint8_t> allowed,int width,int height);
