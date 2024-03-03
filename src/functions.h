#ifndef MZ_FUNCTIONS_H
#define MZ_FUNCTIONS_H

#include <memory>
#include <vector>

#include "classes.h"

std::vector<std::shared_ptr<Entity>> generateMaze(std::vector<std::vector<GridNode>>& maze, int mazeWidth, int mazeHeight, int cellSize, int wallSize, int horizOffset, int vertOffset, int seed);
Point discoverEnd(std::vector<std::vector<GridNode>>& maze, int mazeSize);
float Lerp(float start, float end, float amount);

#endif