#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <termios.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <random>

//KEYBUNDS
class InputHandler {
public:
    using ActionCallback = std::function<void()>;

    // Bind key to a function
    void bindKey(char key, ActionCallback callback) {
        keyBindings[key] = callback;
    }

    // Called in gameloop when key is pressed
    void handleInput(char pressedKey) {
        auto it = keyBindings.find(pressedKey);
        if (it != keyBindings.end()) {
            it->second(); // Execute the action that is bound
        }
    }

private:
    std::unordered_map<char, ActionCallback> keyBindings;
};



//POSITION VALUES
struct Position{
    int row;
    int col;    
};

//BLOCK LOGIC
class Block{
    public:
        //initializes and checks ITSELF
        Block();
        int Brandomizer(int rnColor);
        Position GetBlockPos() const { return blockPos; }
        std::array<int, 16> GtBData(); //returns the current vector data for any given block type and orientation, crucial for blitting block onto grid
        int Rotate();
        void MoveDown();
        void MoveLeft();
        void MoveRight();
        const bool GetBlockState();
        void SetBlockState(bool);
        
    private:
        bool active;
        Position blockPos;
        int blockType;
        int blockAngle;
        static const std::array<std::array<std::array<int, 16>, 4>, 7> shapes;

};

const std::array<std::array<std::array<int, 16>, 4>, 7> Block::shapes = 
{{
    // 1: I
    {{
        {0,0,0,0, 1,1,1,1, 0,0,0,0, 0,0,0,0},
        {0,0,1,0, 0,0,1,0, 0,0,1,0, 0,0,1,0},
        {0,0,0,0, 1,1,1,1, 0,0,0,0, 0,0,0,0},
        {0,0,1,0, 0,0,1,0, 0,0,1,0, 0,0,1,0}
    }},
    // 2: O
    {{
        {0,0,0,0, 0,2,2,0, 0,2,2,0, 0,0,0,0},
        {0,0,0,0, 0,2,2,0, 0,2,2,0, 0,0,0,0},
        {0,0,0,0, 0,2,2,0, 0,2,2,0, 0,0,0,0},
        {0,0,0,0, 0,2,2,0, 0,2,2,0, 0,0,0,0}
    }},
    // 3: T
    {{
        {0,0,0,0, 3,3,3,0, 0,3,0,0, 0,0,0,0},
        {0,3,0,0, 3,3,0,0, 0,3,0,0, 0,0,0,0},
        {0,3,0,0, 3,3,3,0, 0,0,0,0, 0,0,0,0},
        {0,3,0,0, 0,3,3,0, 0,3,0,0, 0,0,0,0}
    }},
    // 4: S
    {{
        {0,0,0,0, 0,4,4,0, 4,4,0,0, 0,0,0,0},
        {4,0,0,0, 4,4,0,0, 0,4,0,0, 0,0,0,0},
        {0,0,0,0, 0,4,4,0, 4,4,0,0, 0,0,0,0},
        {4,0,0,0, 4,4,0,0, 0,4,0,0, 0,0,0,0}
    }},
    // 5: Z
    {{
        {0,0,0,0, 5,5,0,0, 0,5,5,0, 0,0,0,0},
        {0,5,0,0, 5,5,0,0, 5,0,0,0, 0,0,0,0},
        {0,0,0,0, 5,5,0,0, 0,5,5,0, 0,0,0,0},
        {0,5,0,0, 5,5,0,0, 5,0,0,0, 0,0,0,0}
    }},
    // 6: J
    {{
        {6,0,0,0, 6,6,6,0, 0,0,0,0, 0,0,0,0},
        {0,6,6,0, 0,6,0,0, 0,6,0,0, 0,0,0,0},
        {0,0,0,0, 6,6,6,0, 0,0,6,0, 0,0,0,0},
        {0,6,0,0, 0,6,0,0, 6,6,0,0, 0,0,0,0}
    }},
    // 7: L
    {{
        {0,0,7,0, 7,7,7,0, 0,0,0,0, 0,0,0,0},
        {0,7,0,0, 0,7,0,0, 0,7,7,0, 0,0,0,0},
        {0,0,0,0, 7,7,7,0, 7,0,0,0, 0,0,0,0},
        {7,7,0,0, 0,7,0,0, 0,7,0,0, 0,0,0,0}
    }}
}};

Block::Block(){
    active = true;
    blockAngle = 0;
    blockPos.col = 0;
    blockPos.row = 3;
}

int Block::Brandomizer(int rnColor){
    blockType = rnColor;
    return blockType;
}

int Block::Rotate(){
    blockAngle = (blockAngle + 1) % 4;
    return blockAngle;
}

void Block::MoveDown(){
    blockPos.row += 1;
}

void Block::MoveLeft(){
    blockPos.col -= 1;
}

void Block::MoveRight(){
    blockPos.col += 1;
}

std::array<int, 16> Block::GtBData()
{
    return shapes[blockType][blockAngle];
}

const bool Block::GetBlockState()
{
    return active;
}

void Block::SetBlockState(bool inp)
{
    active = inp;
}

//GRID LOGIC
class Grid {
    public:
        //initializes and checks ITSELF
        Grid();
        void Init();
        void PrintGrid();
        void UpdateGrid(const std::array<int, 16>, const bool CG_result, const int, const int);
        void CleanUp(const int, const int);
        bool CheckGrid(const std::array<int, 16>, const int, const int);
        bool GetCGresult(){return CG_result;}
    private:
        bool CG_result;
        int numRows;
        int numCols;
        int grid[20][10];
};

Grid::Grid(){
    numRows = 20;
    numCols = 10;
    CG_result = true;
    Init();
}

void Grid::Init(){
    for(int i = 0; i < numRows; i++){
        for(int j = 0; j < numCols; j++){
            grid[i][j] = 0;
            
        }
    }
}

void Grid::PrintGrid()
{
    std::string prBoard;
    for(int i = 0; i < numRows; i++){
        for(int j = 0; j < numCols; j++){
            prBoard += std::to_string(grid[i][j]) + " ";
        }
        prBoard += "\n";
    }
    std::cout << prBoard;
}

bool Grid::CheckGrid(const std::array<int, 16> blockShape, const int row, const int col)
{

    for (int i = row; i < (row + 4); i++){
        for (int j = col; j < (col + 4); j++){
            int relRow = i - row;
            int relCol = j - col; //relative row and col values

            if (blockShape[(4 * relRow) + relCol] != 0){

                if ((i >= numRows || j >= numCols || j < 0)) // !!!!!!!!! CHECK THIS !!!!!!!!!!
                {    
                    CG_result = false; 
                    return CG_result;
                }

                if ((grid[i][j] != 0)) // add a closer check
                {
                    CG_result = false; 
                    return CG_result;
                }

            }
        }
    }

    CG_result = true; 
    return CG_result;
}

void Grid::UpdateGrid(const std::array<int, 16> blockShape,const bool CG_result, const int row, const int col) // draws the block onto the grid and also locks blocks into place
{
    
    if (CG_result) 
    {
        for (int i = (row); i < (row + 4); i++) // starts at row above the already checked current block
        {
            for (int j = (col); j < (col + 4); j++) // starts at col zero to properly scan every space if the player were to move left or right as well
            {
                int relRow = i - row;
                int relCol = j - col; //relative row and col values
                
                if ((blockShape[(relRow * 4) + relCol]) != 0)
                {
                    grid[i][j] = blockShape[(relRow * 4) + relCol];
                }
                
            }
            
        }
        
    }
}

void Grid::CleanUp(const int row, const int col)
{
    for (int i = (row - 4); i < row; i++)
    {
        for (int j = col; j < col; j++)
        {
            grid[i][j] = 0;
        }
    }
}


//GAMELOGIC
class Game
{
    public:
        //initializes and manipulates BOTH GRID AND BLOCK, and checks ITSELF
        Game();
        void ChangeGamestate(bool farp)
        {   if (farp)
            {gamestate = true;}
            else
            {gamestate = false;}
        }
        //UpdateScore();
        bool GetGamestate()
        {
            return gamestate;
        }
        //Block Stuff
        Block* BlockSpawn();
        
        void Gravity();
        void KBMoveL();
        void KBMoveR();
        void KBRotate();
        
        //Grid Stuff
        void DrawGrid();
        void GetCheckdGrid();
        void TrigGridUpdate();
        


        ~Game();
    private:
    Grid board;
    Block* currentBlock;
    int score;
    bool gridstate;
    bool gamestate;
    std::mt19937 gen;
    std::uniform_int_distribution<int> dist;
};

Game::Game()
:gen(std::random_device{}()), dist(0, 6) //sort of preprocessing for constructor calls to get seed before game is constructed.
{

    score = 0;
    gamestate = false;
    currentBlock = nullptr;
    
}

Block* Game::BlockSpawn()
{
    currentBlock = new Block;
    int rnColor = dist(gen);

    currentBlock->Brandomizer(rnColor);

    return currentBlock;
}

void Game::Gravity()
{
    currentBlock -> MoveDown();
}

void Game::KBMoveL()
{
    currentBlock->MoveLeft();
}

void Game::KBMoveR()
{
    currentBlock -> MoveRight();
}

void Game::KBRotate()
{
    currentBlock->Rotate();
}

void Game::DrawGrid()
{
    board.PrintGrid();
}

void Game::GetCheckdGrid()
{
    gridstate = board.CheckGrid((currentBlock->GtBData()),(currentBlock->GetBlockPos().row),(currentBlock->GetBlockPos().col));
}

void Game::TrigGridUpdate()
{
    board.UpdateGrid((currentBlock->GtBData()),gridstate,(currentBlock->GetBlockPos().row),(currentBlock->GetBlockPos().col));
}

Game::~Game()
{
    delete currentBlock;
}
//TERMINAL CONFIG
// ---- Raw mode: set up ONCE, restore ONCE ----

struct termios orig_termios;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;

    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);  // add this
    //raw.c_oflag &= ~(OPOST);  <<removed to maintain \n usage                                  // add this
    raw.c_lflag &= ~(ICANON | ECHO | ISIG);                     // replace your existing line with this

    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    tcflush(STDIN_FILENO, TCIFLUSH);
}

//AI function make sure you learn how it works
char getch_instant() {
    char buf = 0;
    read(STDIN_FILENO, &buf, 1); // returns immediately; buf stays 0 if no key waiting
    return buf;
}



//MAIN/GAMELOOP
//bool gamestate = false;

int main(){
    enableRawMode();

    std::ios::sync_with_stdio(false); // faster cout, no C stdio interleaving issues

    std::cout << "\033[2J\033[1;1H";
    std::cout << "Press 'o' to start, 'q' to quit\n";
    std::cout.flush();

    Game game;
    game.BlockSpawn();
    InputHandler userIn;
    char PressedKey = 0;
    int frameCount = 0;

    userIn.bindKey('o', [&game]() {game.ChangeGamestate(true);});
    userIn.bindKey('q', [&game]() {game.ChangeGamestate(false);});
    userIn.bindKey('w', [&game](){game.KBRotate();});
    userIn.bindKey('s', [&game](){game.Gravity();}); // both w and s rotate the block
    userIn.bindKey('a', [&game](){game.KBMoveL();});
    userIn.bindKey('d', [&game](){game.KBMoveR();});
    //start game

    while (PressedKey == 0) {
        PressedKey = getch_instant();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    userIn.handleInput(PressedKey);

    while (game.GetGamestate())
    {
        PressedKey = getch_instant();

        std::cout << "\033[2J\033[1;1H"; //clear screen, move curser to row 1 col 1.
        std::cout << "Frame: " << frameCount++ << "\n"; //frame counter
        // simulate a real game loop event
        userIn.handleInput(PressedKey);
        game.GetCheckdGrid();
        game.TrigGridUpdate();
        game.DrawGrid(); //already updated
        

        std::this_thread::sleep_for(std::chrono::milliseconds(64)); //fps cap ~15 

        std::cout.flush(); //flushes cout queue
    }

    std::cout << "\033[2J\033[1;1HGame exited.\n"; // clear screen, move curser to row 1 col 1.
    std::cout.flush(); //flushes cout queue
    return 0;
}

