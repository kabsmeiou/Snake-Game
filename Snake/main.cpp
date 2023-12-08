#include <bits/stdc++.h>
#include "windows.h"
#include <conio.h>

const int row = 25;
const int col = 40;
long long score = 0;
short level = 1;
short counter = 1;
int speed = 140;
char U = 'U', D = 'D', L = 'L', R = 'R';

//buff related counters
int bonusCounter = 0;
int buffSwitch = 0;
int dizzyTimer = 0;

bool gameOver;
bool spawnPerk;

std::vector<std::vector<int>> gameGrid(row, std::vector<int>(col, 0));
std::vector<std::vector<int>> emptyCopy(row, std::vector<int>(col, 0)); //initialize empty grid as the idea is to reset gameGrid every iteration
std::stack<char> snakeMovement;
std::list<std::pair<int, int>> snakeBody;
std::queue<std::pair<int, int>> foodPlacement;
std::pair<int, int> lastBody;
std::pair<int, int> perkLocation;
std::vector<int> perks{1, -1, 3, 1, 5};

//prototypes
void mapObjects();
void showGrid();
void randomizePerk();
void handleInput();
void dfs(std::vector<std::vector<bool>> &visited, int xx, int yy);
void startGame();

/*
//////////////////////////////////////
the function for showing the mapped
objects
//////////////////////////////////////
*/
void showGrid() {
    system("cls");

    std::cout << " --------------------------------------\n";
    std::cout << "|\t      Snake Game               |" << '\n';
    std::cout << "| Level: " << level << (level == 10 ? "" : " ") << "                            |\n";
    std::string stringScore = "| ";
    stringScore += "Score: ";
    stringScore += std::to_string(score);
    for (int i = stringScore.size(); i < col; i++) {
        stringScore += (i == col - 1 ? "|" : " ");
    }
    std::cout << stringScore;
    std::cout << '\n';
    std::string perkStatus = "| Perk: ";
    if (buffSwitch == 0) {
        perkStatus += "NONE";
    } else if (buffSwitch == 1) {
        perkStatus += "Body Shrink";
    } else if (buffSwitch == 2) {
        perkStatus += "Score Frenzy";
    } else if (buffSwitch == -1) {
        perkStatus += "ERROR!!!(" + std::to_string(dizzyTimer) + ")";
    }

    for (int i = perkStatus.size(); i < col; i++) {
        perkStatus += (i == col - 1 ? "|" : " ");
    }

    std::cout << perkStatus << '\n';
    for (int r = 0; r < row; r++) {
        for (int c = 0; c < col; c++) {
            if (r == 0 || r == row - 1) {
                std::cout << (c == 0 || c == col - 1 ? ' ' : '_');
            } else if (c == 0 || c == col - 1) {
                std::cout << '|';
            } else if (!gameGrid[r][c]) {
                std::cout << ' ';
            } else if (gameGrid[r][c] == 1) {
                std::cout << 'o';
            } else if (gameGrid[r][c] == 2) {
                std::cout << '$';
            } else if (gameGrid[r][c] == 10) {
                std::cout << 'O';
            } else if (gameGrid[r][c] == 7) {
                std::cout << '?';
            }
        }
        std::cout << '\n';
    }

    if (gameOver) {
        std::cout << "\t     Bonk! Game Over.            " << '\n';
    }
}

/*
//////////////////////////////////////
Map the coordinates generated for food
and body. Modify the coordinates of
the food if there are collisions
with the body.
//////////////////////////////////////
*/
void mapObjects() {
    gameGrid = emptyCopy;
    std::pair<int, int> food = foodPlacement.front();
    int cur = 1;
    if (dizzyTimer > 0) {
        dizzyTimer--;
        if (dizzyTimer == 0) {
            buffSwitch = 0;
            U = 'U', D = 'D', R = 'R', L = 'L';
        }
    }
    //this is a separate loop just to check the food placement and make sure it will not collide with the body of the snake
    for (auto coordinates : snakeBody) {
        if (cur > 1 && coordinates.first == food.first && coordinates.second == food.second) {
            foodPlacement.pop();
            food = foodPlacement.front();
            foodPlacement.push(food);
            break;
        }
        cur++;
    }

    gameGrid[food.first][food.second] = 2;

    std::pair<int, int> head = snakeBody.front();
    std::map<std::pair<int, int>, int> bodyCollision;
    for (auto coordinates : snakeBody) {
        gameGrid[coordinates.first][coordinates.second] = 1;

        //this is an algorithm that counts the coordinates in our snake body.
        //if the count of the pair is > 2 that means we have duplicate coordinates for the body
        //there will be two cases. first, its because we ate a food and pushed back the tail of the snake
        //or there is a collision between the head and the snake's body.
        if ((++bodyCollision[coordinates] >= 2 && coordinates == head)) {
            gameOver = true;
        }
    }

    if (spawnPerk) {
        gameGrid[perkLocation.first][perkLocation.second] = 7;
    }

    //if current number is 1 that means the current coordinate is the head.
    //now, if head == food coordinates then we have eaten the food.
    //pop the food and emplace the last part of the snake that we wouldve deleted otherwise
    if (head.first == food.first && head.second == food.second) {

        gameGrid[lastBody.first][lastBody.second] = 1;
        snakeBody.emplace_back(lastBody);
        foodPlacement.pop();

        if (bonusCounter > 0) {
            score += (1 * level * (speed * 0.8)) * 3;
            bonusCounter--;
            if (bonusCounter == 0) buffSwitch = 0;
        } else {
            score += (1 * level * (speed * 0.8));
        }
        counter++;

        if (spawnPerk) {
            spawnPerk = false;
        }

        //speed algorithm(increase when the size of the snake is divisible by 5)
        if (counter % 5 == 0) {
            level++;

            spawnPerk = true;
            int x = rand() % (row - 2) + 1;
            int y = rand() % (col - 2) + 1;
            perkLocation = {x, y};

            speed = 120 - ((level - 1) * 10);
        }

    } else if (head.first == perkLocation.first && head.second == perkLocation.second && spawnPerk) {
        spawnPerk = false;
        randomizePerk();
    }

    gameGrid[head.first][head.second] = 10;
}

/*
//////////////////////////////////////
perk randomizer
//////////////////////////////////////
*/
void randomizePerk() {
    int pos = rand() % 5;
    if (perks[pos] == 1) {
        int body = 3;
        buffSwitch = 1;
        while (body--) {
            snakeBody.pop_back();
            mapObjects();
            showGrid();
            Sleep(500);
        }
        buffSwitch = 0;
    } else if (perks[pos] == 3 || perks[pos] == 5) {
        bonusCounter = 3;
        buffSwitch = 2;
    } else {
        buffSwitch = -1;
        dizzyTimer = 30;
        U = 'D', D = 'U', R = 'L', L = 'R';
    }
}

/*
//////////////////////////////////////
dfs algorithm for checking the
outermost layer of the matrix
//////////////////////////////////////
*/
void dfs(std::vector<std::vector<bool>> &visited, int xx, int yy) {

    if (xx < 0 || yy < 0 || xx >= row || yy >= col || visited[xx][yy] || gameOver) {
        return;
    }

    visited[xx][yy] = true;

    if (xx == 0 || yy == 0 || xx == row - 1 || yy == col - 1) { //condition to make sure we are currently visiting the border of the grid

        if (gameGrid[xx][yy] == 10) {
            gameOver = true;
        }

        dfs(visited, xx + 1, yy); //down
        dfs(visited, xx, yy + 1); //right
        dfs(visited, xx - 1, yy); //up
        dfs(visited, xx, yy - 1); //left
    }

}

/*
//////////////////////////////////////
check the border for collisions
using dfs
//////////////////////////////////////
*/
void checkBorders() {

    std::vector<std::vector<bool>> visited(row, std::vector<bool>(col, false));
    dfs(visited, 0, 0);

}

/*
//////////////////////////////////////
randomize coordinates for food.
//////////////////////////////////////
*/
void generateFood() {

    int x = rand() % (row - 2) + 1;
    int y = rand() % (col - 2) + 1;

    if (foodPlacement.size() < 5) {
        foodPlacement.push({x, y});
    }
}

/*
//////////////////////////////////////
create the coordinates for body.
increment x or y and push on the list.
if the game just got started, place
body at the middle
//////////////////////////////////////
*/
void generateBody() {

    if (snakeBody.empty()) {
        snakeBody.emplace_back(std::make_pair(row / 2, col / 2));
    }

    if (!snakeMovement.empty()) {
        char move = snakeMovement.top();
        std::pair<int, int> head = snakeBody.front();
        lastBody = head;
        if (move == 'U') {
            snakeBody.emplace_front(std::make_pair(head.first - 1, head.second));
            snakeBody.pop_back();
        } else if (move == 'D') {
            snakeBody.emplace_front(std::make_pair(head.first + 1, head.second));
            snakeBody.pop_back();
        } else if (move == 'L') {
            snakeBody.emplace_front(std::make_pair(head.first, head.second - 1));
            snakeBody.pop_back();
        } else if (move == 'R') {
            snakeBody.emplace_front(std::make_pair(head.first, head.second + 1));
            snakeBody.pop_back();
        }
    }
}

/*
//////////////////////////////////////
handling the keyboard hits(input to
control the snake)
//////////////////////////////////////
*/
void handleInput() {
    if (_kbhit()) {
        while (true) {
            if (GetAsyncKeyState(VK_UP)) {
                if (snakeMovement.empty() || snakeMovement.top() != D) snakeMovement.push(U);
            } else if (GetAsyncKeyState(VK_LEFT)) {
                if (snakeMovement.empty() || snakeMovement.top() != R) snakeMovement.push(L);
            } else if (GetAsyncKeyState(VK_RIGHT)) {
                if (snakeMovement.empty() || snakeMovement.top() != L) snakeMovement.push(R);
            } else if (GetAsyncKeyState(VK_DOWN)) {
                if (snakeMovement.empty() || snakeMovement.top() != U) snakeMovement.push(D);
            }
            break;
        }
    }
}

/*
//////////////////////////////////////
the function that serves as the menu
and the base of the gameOver boolean
variable.
//////////////////////////////////////
*/
void startGame() {
    system("cls");

    std::cout << "============ SNAKE ============";
    std::cout << "\n\n\n\n";
    std::cout << "==== Press Enter to Start =====";
    std::cout << "\n\n\n\n";

    gameOver = false;
    spawnPerk = false;

    printf("\e[?25l"); //removing console cursor
    srand(time(0));

    while (!gameOver) {
        handleInput();
        generateBody();
        generateFood();
        mapObjects();
        checkBorders();
        showGrid();
        Sleep(speed);
    }

    //buffer to let users press enter before the game closes so they can see their scores and levels
    std::string buffer;
    std::getline(std::cin, buffer);
}

signed main() {
    startGame();
    return 0;
}
