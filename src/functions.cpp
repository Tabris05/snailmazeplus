#include <memory>
#include <vector>
#include <stack>

#include "classes.h"
#include "functions.h"
using namespace std;

vector<shared_ptr<Entity>> generateMaze(vector<vector<GridNode>>& maze, int mazeWidth, int mazeHeight, int cellSize, int wallSize, int horizOffset, int vertOffset, int seed) {
	srand(seed);
	vector<shared_ptr<Entity>> screen;
	int xStart = rand() % mazeWidth, yStart = rand() % mazeHeight;
	maze[xStart][yStart].m_visited = true;
	int x[4] = { xStart, xStart, xStart, xStart }, y[4] = { yStart, yStart, yStart, yStart }, visited = 1;
	stack<Point> prevVisited[4];
	while (visited != mazeWidth * mazeHeight) {
		for (int i = 0; i < 4; i++) {
			vector<Point> validDirections;
			if (x[i] - 1 >= 0 && !maze[x[i] - 1][y[i]].m_visited) {
				validDirections.push_back({ -1, 0 });
			}
			if (x[i] + 1 < mazeWidth && !maze[x[i] + 1][y[i]].m_visited) {
				validDirections.push_back({ 1, 0 });
			}
			if (y[i] - 1 >= 0 && !maze[x[i]][y[i] - 1].m_visited) {
				validDirections.push_back({ 0, -1 });
			}
			if (y[i] + 1 < mazeHeight && !maze[x[i]][y[i] + 1].m_visited) {
				validDirections.push_back({ 0, 1 });
			}
			int size = validDirections.size();
			Point moveDir;
			if (size > 0) {
				prevVisited[i].push({x[i], y[i]});
				moveDir = validDirections[rand() % size];

				float rectX, rectY, rectW, rectH;
				rectX = moveDir.x < 0 ? (x[i] - 1) * cellSize + wallSize / 2 : x[i] * cellSize + wallSize / 2;
				rectY = moveDir.y < 0 ? (y[i] - 1) * cellSize + wallSize / 2 : y[i] * cellSize + wallSize / 2;
				rectW = moveDir.x == 0 ? cellSize - wallSize : (cellSize * 2 - wallSize);
				rectH = moveDir.y == 0 ? cellSize - wallSize : cellSize * 2 - wallSize;
				screen.push_back(make_shared<Entity>(rectX + horizOffset, rectY + vertOffset, rectW, rectH, Color{ 0, 0, 255, 255 }));

				maze[x[i]][y[i]].m_north = moveDir.y == -1 ? true : maze[x[i]][y[i]].m_north;
				maze[x[i]][y[i]].m_south = moveDir.y == 1 ? true : maze[x[i]][y[i]].m_south;
				maze[x[i]][y[i]].m_east = moveDir.x == 1 ? true : maze[x[i]][y[i]].m_east;
				maze[x[i]][y[i]].m_west = moveDir.x == -1 ? true : maze[x[i]][y[i]].m_west;

				x[i] += moveDir.x;
				y[i] += moveDir.y;

				maze[x[i]][y[i]].m_north = moveDir.y == 1 ? true : maze[x[i]][y[i]].m_north;
				maze[x[i]][y[i]].m_south = moveDir.y == -1 ? true : maze[x[i]][y[i]].m_south;
				maze[x[i]][y[i]].m_east = moveDir.x == -1 ? true : maze[x[i]][y[i]].m_east;
				maze[x[i]][y[i]].m_west = moveDir.x == 1 ? true : maze[x[i]][y[i]].m_west;
			}
			else {
				if (prevVisited[i].size() > 0) {
					moveDir = prevVisited[i].top();
					prevVisited[i].pop();
					x[i] = moveDir.x;
					y[i] = moveDir.y;
				}
			}
			if (!maze[x[i]][y[i]].m_visited) {
				maze[x[i]][y[i]].m_visited = true;
				visited++;
				if (visited == mazeWidth * mazeHeight) return screen;
			}
		}
	}
}

Point discoverEnd(vector<vector<GridNode>>& maze, int mazeSize) {
	int x = 0, y = 0, depth = 0, maxDepth = 0, visited = 0;
	stack<Point> prevVisited;
	Point result = { 0, 0 };
	while (visited < mazeSize) {
		if (maze[x][y].m_north && maze[x][y - 1].m_visited) {
			prevVisited.push({ x, y });
			visited++;
			y--;
			maze[x][y].m_visited = false;
			depth++;
			if (depth > maxDepth) {
				depth = maxDepth;
				result = { x, y };
			}
		}
		else if (maze[x][y].m_south && maze[x][y + 1].m_visited) {
			prevVisited.push({ x, y });
			visited++;
			y++;
			maze[x][y].m_visited = false;
			depth++;
			if (depth > maxDepth) {
				depth = maxDepth;
				result = { x, y };
			}
		}
		else if (maze[x][y].m_west && maze[x - 1][y].m_visited) {
			prevVisited.push({ x, y });
			visited++;
			x--;
			maze[x][y].m_visited = false;
			depth++;
			if (depth > maxDepth) {
				depth = maxDepth;
				result = { x, y };
			}
		}
		else if (maze[x][y].m_east && maze[x + 1][y].m_visited) {
			prevVisited.push({ x, y });
			visited++;
			x++;
			maze[x][y].m_visited = false;
			depth++;
			if (depth > maxDepth) {
				depth = maxDepth;
				result = { x, y };
			}
		}
		else {
			x = prevVisited.top().x;
			y = prevVisited.top().y;
			prevVisited.pop();
			depth--;
		}
	}
	return result;
}

float Lerp(float start, float end, float amount) {
	return start + amount * (end - start);
}