#ifndef _DEBUG
#define main WinMain
#endif

#include <raylib.h>
#include <sstream>
#include <chrono>
#include <future>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <cmath>
#include <stack>

#include "classes.h"
#include "functions.h"

using namespace std;

enum Screen {
	MAINMENU,
	SEEDMENU,
	INSTRUCTIONS,
	DIFFICULTYSELECT,
	GAMEPLAY
};

int main() {

	srand(chrono::high_resolution_clock::now().time_since_epoch().count());

	Screen screen = MAINMENU;

	int seed, mazeWidth, mazeHeight, wallSize, cellSize, horizOffset, vertOffset, timeAllotted, curTime;
	int x = 0, y = 0, level = 1, genStatus = 0, menuStatus = 0, difficulty = 0;
	const int WIDTH = 1264, HEIGHT = 750, SPEED = 100000000;

	bool isCustomSeed;
	bool seedStatus = false;

	chrono::high_resolution_clock::time_point moveTimer, completeTimer, flickerTimer, roundTimer, failTimer, pasteTimer, delTimer, seedTimer;
	vector<shared_ptr<Entity>> fp, bp, mainMenu, difficultyScreen, seedMenu, instructionScreen;
	vector<vector<GridNode>> maze;
	TexturedEntity* eStart = nullptr, *eGoal = nullptr;
	Player* player = nullptr;
	Display* timer = nullptr;
	Point result = { 0, 0 };

	future<vector<shared_ptr<Entity>>> th1;
	future<Point> th2;

	Color menuCol = { 0, 0, 0, 255 }, bg = menuCol;

	string customSeed;

	const char* stringSeed = nullptr;

	Image icon = LoadImage("assets/Icon.png");
	InitWindow(WIDTH, HEIGHT, "Snail Maze Plus");
	SetWindowIcon(icon);
	SetTargetFPS(240);

	Texture snail = LoadTexture("assets/Snail.png"), start = LoadTexture("assets/Start.png"), goal = LoadTexture("assets/Goal.png"), logo = LoadTexture("assets/Logo.png");
	Font font = LoadFont("assets/snailfont.ttf");

	//main menu
	Text* menuSelector = new Text(WIDTH / 2 - 220, HEIGHT / 2 + 30, 30, WHITE, ">", CENTER, font);
	mainMenu.push_back(shared_ptr<Entity>(new TexturedEntity((WIDTH - 159*4) / 2, 100, 159*4, 37*4, logo, true)));
	mainMenu.push_back(shared_ptr<Entity>(menuSelector));
	mainMenu.push_back(shared_ptr<Entity>(new Text(WIDTH / 2 - 200, HEIGHT / 2 + 30, 30, WHITE, "PLAY", LEFT, font)));
	mainMenu.push_back(shared_ptr<Entity>(new Text(WIDTH / 2 - 200, HEIGHT / 2 + 90, 30, WHITE, "PLAY FROM SEED", LEFT, font)));
	mainMenu.push_back(shared_ptr<Entity>(new Text(WIDTH / 2 - 205, HEIGHT / 2 + 150, 30, WHITE, "INSTRUCTIONS", LEFT, font)));
	mainMenu.push_back(shared_ptr<Entity>(new Text(WIDTH / 2 - 205, HEIGHT / 2 + 210, 30, WHITE, "QUIT", LEFT, font)));

	//seed menu
	Text* seedSelector = new Text(WIDTH / 2 - 120, HEIGHT / 2 + 150, 30, WHITE, ">", CENTER, font);
	TextBox* seedBox = new TextBox(100, HEIGHT / 2 - 100, 30, WHITE, "ENTER SEED: ", YELLOW, font);
	Text* invalidSeed = new Text(WIDTH / 2, 150, 30, BLACK, "INVALID SEED!", CENTER, font);
	seedMenu.push_back(shared_ptr<Entity>(seedSelector));
	seedMenu.push_back(shared_ptr<Entity>(seedBox));
	seedMenu.push_back(shared_ptr<Entity>(invalidSeed));
	seedMenu.push_back(shared_ptr<Entity>(new Text(WIDTH / 2 - 100, HEIGHT / 2 + 150, 30, WHITE, "CONFIRM", LEFT, font)));
	seedMenu.push_back(shared_ptr<Entity>(new Text(WIDTH / 2 - 100, HEIGHT / 2 + 210, 30, YELLOW, "BACK", LEFT, font)));

	//instruction menu
	instructionScreen.push_back(shared_ptr<Entity>(new Text(275, 85, 45, WHITE, "W: NAVIGATE UP", LEFT, font)));
	instructionScreen.push_back(shared_ptr<Entity>(new Text(275, 170, 45, WHITE, "S: NAVIGATE DOWN", LEFT, font)));
	instructionScreen.push_back(shared_ptr<Entity>(new Text(275, 255, 45, WHITE, "A: NAVIGATE LEFT", LEFT, font)));
	instructionScreen.push_back(shared_ptr<Entity>(new Text(275, 330, 45, WHITE, "D: NAVIGATE RIGHT", LEFT, font)));
	instructionScreen.push_back(shared_ptr<Entity>(new Text(WIDTH / 2, 415, 45, WHITE, "ENTER: SELECT", CENTER, font)));
	instructionScreen.push_back(shared_ptr<Entity>(new Text(WIDTH / 2, 500, 45, WHITE, "CTRL+C: COPY CURRENT SEED", CENTER, font)));
	instructionScreen.push_back(shared_ptr<Entity>(new Display(WIDTH / 2, 650, 30, ">", WHITE, "BACK", YELLOW, CENTER, font)));

	//difficulty selection menu
	Text* difficultySelector = new Text(WIDTH / 2 - 120, HEIGHT / 2 - 90, 30, WHITE, ">", CENTER, font);
	difficultyScreen.push_back(shared_ptr<Entity>(difficultySelector));
	difficultyScreen.push_back(shared_ptr<Entity>(new Text(WIDTH / 2, 100, 50, WHITE, "SELECT DIFFICULTY", CENTER, font)));
	difficultyScreen.push_back(shared_ptr<Entity>(new Text(WIDTH / 2 - 100, HEIGHT / 2 - 90, 30, WHITE, "EASY", LEFT, font)));
	difficultyScreen.push_back(shared_ptr<Entity>(new Text(WIDTH / 2 - 100, HEIGHT / 2 - 30, 30, WHITE, "MEDIUM", LEFT, font)));
	difficultyScreen.push_back(shared_ptr<Entity>(new Text(WIDTH / 2 - 100, HEIGHT / 2 + 30, 30, WHITE, "HARD", LEFT, font)));
	difficultyScreen.push_back(shared_ptr<Entity>(new Text(WIDTH / 2 - 100, HEIGHT / 2 + 90, 30, WHITE, "EXPERT", LEFT, font)));
	difficultyScreen.push_back(shared_ptr<Entity>(new Text(WIDTH / 2 - 100, HEIGHT / 2 + 150, 30, YELLOW, "BACK", LEFT, font)));

	InitAudioDevice();
	Music gameplay = LoadMusicStream("assets/gameplay.mp3");
	Sound levelComplete = LoadSound("assets/levelend.ogg");

	while (!WindowShouldClose()) {
		if (screen == MAINMENU) {
			bg = menuCol;
			fp = mainMenu;
			isCustomSeed = false;
			customSeed = "";
			invalidSeed->m_color = BLACK;
			difficulty = 0;
			seedStatus = 0;
			genStatus = 0;
			level = 1;
			x = 0;
			y = 0;
			if (IsKeyPressed(KEY_S)) {
				menuStatus = menuStatus + 1 > 3 ? 0 : menuStatus + 1;
			}
			if (IsKeyPressed(KEY_W)) {
				menuStatus = menuStatus - 1 < 0 ? 3 : menuStatus - 1;
			}
			menuSelector->m_posY = (HEIGHT / 2 + 30) + 60 * menuStatus;
			if (IsKeyPressed(KEY_ENTER)) {
				if (menuStatus == 0) screen = DIFFICULTYSELECT;
				else if (menuStatus == 1) screen = SEEDMENU;
				else if (menuStatus == 2) screen = INSTRUCTIONS;
				else if (menuStatus == 3) goto Exit;
			}
		}
		else if (screen == DIFFICULTYSELECT) {
			fp = difficultyScreen;
			if (IsKeyPressed(KEY_S)) {
				difficulty = difficulty + 1 > 4 ? 0 : difficulty + 1;
			}
			if (IsKeyPressed(KEY_W)) {
				difficulty = difficulty - 1 < 0 ? 4 : difficulty - 1;
			}
			difficultySelector->m_posY = (HEIGHT / 2 - 90) + 60 * difficulty;
			if (IsKeyPressed(KEY_ENTER)) {
				if (difficulty < 4) screen = GAMEPLAY;
				else screen = MAINMENU;
			}
		}
		else if (screen == SEEDMENU) {
			fp = seedMenu;
			int key = GetCharPressed();
			if ((key > 47 && key < 58 && customSeed.length() != 1) || (customSeed.length() == 1 && key == 120)) customSeed += (char)key;
			if (IsKeyDown(KEY_BACKSPACE) && !customSeed.empty() && (chrono::high_resolution_clock::now() - delTimer).count() > 100000000) {
				customSeed.pop_back();
				delTimer = chrono::high_resolution_clock::now();
			}
			if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyDown(KEY_V) && (chrono::high_resolution_clock::now() - pasteTimer).count() > 250000000) {
				customSeed += GetClipboardText();
				pasteTimer = chrono::high_resolution_clock::now();
			}
			if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_S)) seedStatus = !seedStatus;
			seedBox->m_value = customSeed;
			seedSelector->m_posY = (HEIGHT / 2 + 150) + (seedStatus * 60);
			if (IsKeyPressed(KEY_ENTER)) {
				if (seedStatus) screen = MAINMENU;
				else {
					if (customSeed.length() != 0) {
						istringstream ss(customSeed);
						int sDifficulty, sSeed;
						char sX;
						ss >> sDifficulty >> sX >> sSeed;
						if ((sDifficulty != 0 && sDifficulty != 1 && sDifficulty != 2 && sDifficulty != 3) || (sX != 'x') || !ss.eof()) invalidSeed->m_color = YELLOW;
						else {
							difficulty = sDifficulty;
							seed = sSeed;
							isCustomSeed = true;
							screen = GAMEPLAY;
						}
					}
					else invalidSeed->m_color = YELLOW;
				}
			}
		}
		else if (screen == INSTRUCTIONS) {
			fp = instructionScreen;
			if (IsKeyPressed(KEY_ENTER)) screen = MAINMENU;
		}
		else if (screen == GAMEPLAY) {
			if (genStatus == 0) {
				if (difficulty == 0) {
					mazeWidth = 15;
					mazeHeight = 8;
					wallSize = 8;
					timeAllotted = 15;
				}
				else if (difficulty == 1) {
					mazeWidth = 30;
					mazeHeight = 16;
					wallSize = 4;
					timeAllotted = 60;
				}
				else if (difficulty == 2) {
					mazeWidth = 45;
					mazeHeight = 24;
					wallSize = 3;
					timeAllotted = 120;
				}
				else if (difficulty == 3) {
					mazeWidth = 60;
					mazeHeight = 32;
					wallSize = 2;
					timeAllotted = 240;
				}
				cellSize = WIDTH / mazeWidth;
				horizOffset = (WIDTH - cellSize * mazeWidth) / 2;
				vertOffset = (HEIGHT - cellSize * mazeHeight);
				if (!isCustomSeed) {
					seed = rand();
				}
				stringSeed = TextFormat("%01ix%01i", difficulty, seed);
				maze = vector<vector<GridNode>>(mazeWidth, vector<GridNode>(mazeHeight, GridNode()));
				th1 = async(generateMaze, ref(maze), mazeWidth, mazeHeight, cellSize, wallSize, horizOffset, vertOffset, seed);
				genStatus++;
			}
			else if (genStatus == 1 && th1.wait_for(0ms) == future_status::ready) {
				bp = th1.get();
				th2 = async(discoverEnd, ref(maze), mazeWidth * mazeHeight);
				genStatus++;
			}
			else if (genStatus == 2 && th2.wait_for(0ms) == future_status::ready) {
				result = th2.get();
				genStatus++;
			}
			else if (genStatus == 3) {
				curTime = timeAllotted;
				player = new Player( wallSize / 2 + horizOffset, wallSize / 2 + vertOffset, cellSize - wallSize, cellSize - wallSize, snail, false);
				eStart = new TexturedEntity( wallSize / 2 + horizOffset, wallSize / 2 + vertOffset, cellSize - wallSize, cellSize - wallSize, start, true);
				eGoal = new TexturedEntity((result.x * cellSize + wallSize / 2) + horizOffset, (result.y * cellSize + wallSize / 2) + vertOffset, cellSize - wallSize, cellSize - wallSize, goal, false);
				timer = new Display(WIDTH - 10, vertOffset / 2, 30, "TIME ", WHITE, TextFormat("%01i", curTime), YELLOW, RIGHT, font);
				bp.push_back(shared_ptr<Entity>(eStart));
				bp.push_back(shared_ptr<Entity>(player));
				bp.push_back(shared_ptr<Entity>(eGoal));
				bp.push_back(make_shared<Entity>(0, 0, WIDTH, vertOffset - 1, BLACK));
				bp.push_back(shared_ptr<Entity>(new Display(10, vertOffset / 2, 30, "SEED ", WHITE, stringSeed, YELLOW, LEFT, font)));
				bp.push_back(shared_ptr<Entity>(new Display(WIDTH - 300, vertOffset / 2, 30, "RD ", WHITE, TextFormat("%01i", level), YELLOW, RIGHT, font)));
				bp.push_back(shared_ptr<Entity>(timer));
				genStatus++;
			}
			else if (genStatus == 4 && (level == 0 || (chrono::high_resolution_clock::now() - completeTimer).count() > 3000000000)) {
				bg = { 0, 170, 255, 255 };
				fp = bp;
				PlayMusicStream(gameplay);
				roundTimer = chrono::high_resolution_clock::now();
				genStatus++;
			}
			else if (genStatus == 5) {
				UpdateMusicStream(gameplay);
				chrono::high_resolution_clock::time_point curTick = chrono::high_resolution_clock::now();
				if (x == result.x && y == result.y) {
					x = 0;
					y = 0;
					level++;
					fp.push_back(make_shared<Entity>(0, 0, WIDTH, HEIGHT, menuCol));
					fp.push_back(shared_ptr<Entity>(new Text(WIDTH / 2, HEIGHT / 2, 50, WHITE, "LEVEL COMPLETE!", CENTER, font)));
					if (!isCustomSeed) {
						completeTimer = curTick;
						genStatus = 0;
					}
					else seedTimer = curTick;
					StopMusicStream(gameplay);
					PlaySound(levelComplete);
				}
				if (curTime == -1) {
					curTime = -INT_MAX;
					fp.push_back(make_shared<Entity>(0, 0, WIDTH, HEIGHT, menuCol));
					fp.push_back(shared_ptr<Entity>(new Text(WIDTH / 2, HEIGHT / 2 - 60, 50, WHITE, "TIME UP!", CENTER, font)));
					if(!isCustomSeed) fp.push_back(shared_ptr<Entity>(new Display(WIDTH / 2, HEIGHT / 2 + 60, 50, "ROUNDS LASTED: ", WHITE, TextFormat("%01i", level - 1), YELLOW, CENTER, font)));
					failTimer = curTick;
					StopMusicStream(gameplay);
				}
				if (curTime == -INT_MAX && (curTick - failTimer).count() > 3000000000) {
					bg = menuCol;
					screen = MAINMENU;
				}
				if (isCustomSeed && (curTick - seedTimer).count() > 3000000000 && level == 2) {
					bg = menuCol;
					screen = MAINMENU;
				}
				if ((curTick - roundTimer).count() > 1000000000 && curTime > -1) {
					curTime--;
					timer->updateVal(max(curTime, 0));

					roundTimer = curTick;
				}
				if ((curTick - moveTimer).count() > SPEED && curTime > -1) player->m_canMove = true;
				if (IsKeyDown(KEY_W) && player->m_canMove && maze[x][y].m_north) {
					y--;
					player->m_canMove = false;
					moveTimer = curTick;
					player->m_oldPosX = player->m_posX;
					player->m_oldPosY = player->m_posY;
					player->m_rotation = -90;
					player->m_flipped = false;
				}
				if (IsKeyDown(KEY_S) && player->m_canMove && maze[x][y].m_south) {
					y++;
					player->m_canMove = false;
					moveTimer = curTick;
					player->m_oldPosX = player->m_posX;
					player->m_oldPosY = player->m_posY;
					player->m_rotation = 90;
					player->m_flipped = false;
				}
				if (IsKeyDown(KEY_A) && player->m_canMove && maze[x][y].m_west) {
					x--;
					player->m_canMove = false;
					moveTimer = curTick;
					player->m_oldPosX = player->m_posX;
					player->m_oldPosY = player->m_posY;
					player->m_rotation = 0;
					player->m_flipped = true;
				}
				if (IsKeyDown(KEY_D) && player->m_canMove && maze[x][y].m_east) {
					x++;
					player->m_canMove = false;
					moveTimer = curTick;
					player->m_oldPosX = player->m_posX;
					player->m_oldPosY = player->m_posY;
					player->m_rotation = 0;
					player->m_flipped = false;
				}
				if (IsKeyDown(KEY_C) && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))) SetClipboardText(stringSeed);
				if ((int)player->m_posX != (x * cellSize + wallSize / 2) + horizOffset) player->m_posX = Lerp(player->m_oldPosX, (x * cellSize + wallSize / 2) + horizOffset, min(1.0, (curTick - moveTimer).count() / (double)SPEED));
				else player->m_oldPosX = player->m_posX;
				if ((int)player->m_posY != (y * cellSize + wallSize / 2) + vertOffset) player->m_posY = Lerp(player->m_oldPosY, (y * cellSize + wallSize / 2) + vertOffset, min(1.0, (curTick - moveTimer).count() / (double)SPEED));
				else player->m_oldPosY = player->m_posY;
				if ((curTick - flickerTimer).count() > 500000000) {
					eStart->toggleFrame();
					eGoal->toggleFrame();
					flickerTimer = curTick;
				}
			}
		}
		BeginDrawing();
		ClearBackground(bg);
		for (auto i : fp) i->draw();
		EndDrawing();
	}

	Exit:

		CloseWindow();
		UnloadImage(icon);
		UnloadTexture(snail);
		UnloadTexture(start);
		UnloadTexture(goal);
		UnloadTexture(logo);

		UnloadMusicStream(gameplay);
		UnloadSound(levelComplete);
		CloseAudioDevice();

		return 0;
}