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
    int pastrow;
    int pastcol;
};


//BLOCK LOGIC
class Block{
    public:
        //initializes and checks ITSELF
        Block();
        int Brandomizer(int rnColor);
        Position GetBlockPos() const { return blockPos; }
        int GetBlockAngle() const {return blockAngle;}
        int GetPastAngle() const {return pastAngle;}
        std::array<int, 16> GtBData(); //returns the current vector data for any given block type and orientation, crucial for blitting block onto grid
        int Rotate();
        void MoveDown(bool);
        void MoveLeft(bool);
        void MoveRight(bool);
        const bool GetBlockState();
        void SetBlockState(bool);
        
    private:
        bool active;
        Position blockPos;
        int blockType;
        int blockAngle;
        int pastAngle;
        static const std::array<std::array<std::array<int, 16>, 4>, 7> shapes;

};

const std::array<std::array<std::array<int, 16>, 4>, 7> Block::shapes = 
{{
    // 1: I
    {{
        {0,0,0,0, 1,1,1,1, 0,0,0,0, 0,0,0,0},
        {0,0,1,0, 0,0,1,0, 0,0,1,0, 0,0,1,0},
        {0,0,0,0, 0,0,0,0, 1,1,1,1, 0,0,0,0},
        {0,1,0,0, 0,1,0,0, 0,1,0,0, 0,1,0,0}
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
        {0,0,3,0, 0,3,3,0, 0,0,3,0, 0,0,0,0},
        {0,0,0,0, 0,0,3,0, 0,3,3,3, 0,0,0,0},
        {0,0,0,0, 0,3,0,0, 0,3,3,0, 0,3,0,0}
    }},
    // 4: S
    {{
        {0,0,0,0, 0,4,4,0, 4,4,0,0, 0,0,0,0},
        {0,4,0,0, 0,4,4,0, 0,0,4,0, 0,0,0,0},
        {0,0,0,0, 0,0,4,4, 0,4,4,0, 0,0,0,0},
        {0,0,0,0, 0,4,0,0, 0,4,4,0, 0,0,4,0}
    }},
    // 5: Z
    {{
        {0,0,0,0, 5,5,0,0, 0,5,5,0, 0,0,0,0},
        {0,0,5,0, 0,5,5,0, 0,5,0,0, 0,0,0,0},
        {0,0,0,0, 0,5,5,0, 0,0,5,5, 0,0,0,0},
        {0,0,0,0, 0,0,5,0, 0,5,5,0, 0,5,0,0}
    }},
    // 6: J
    {{
        {6,0,0,0, 6,6,6,0, 0,0,0,0, 0,0,0,0},
        {0,0,6,6, 0,0,6,0, 0,0,6,0, 0,0,0,0},
        {0,0,0,0, 0,0,0,0, 0,6,6,6, 0,0,0,6},
        {0,0,0,0, 0,6,0,0, 0,6,0,0, 6,6,0,0}
    }},
    // 7: L
    {{
        {0,0,7,0, 7,7,7,0, 0,0,0,0, 0,0,0,0},
        {0,0,7,0, 0,0,7,0, 0,0,7,7, 0,0,0,0},
        {0,0,0,0, 0,0,0,0, 0,7,7,7, 0,7,0,0},
        {0,0,0,0, 7,7,0,0, 0,7,0,0, 0,7,0,0}
    }}
}};

Block::Block(){
    active = true;
    blockAngle = 0;
    pastAngle = blockAngle;
    blockPos.col = 3;
    blockPos.row = 0;
    blockPos.pastrow = blockPos.row;
    blockPos.pastcol = blockPos.col;
}

int Block::Brandomizer(int rnColor){
    blockType = rnColor;
    return blockType;
}

int Block::Rotate(){
    pastAngle = blockAngle;
    blockAngle = (blockAngle + 1) % 4;

    return blockAngle;
}

void Block::MoveDown(bool inbounds){
    if ((inbounds) && (0 <= blockPos.row) && ((blockPos.row + 4) <= 19))
    {
        if ((blockPos.row + 5) < 0)
        {
            blockPos.row = blockPos.row;
        }
        else
        {
            blockPos.pastrow = blockPos.row;
            blockPos.row += 1;
        }
    }


}

void Block::MoveLeft(bool inbounds){
    if ((inbounds) && (0 <= blockPos.col) && ((blockPos.col + 4) <= 9))
    {
        if ((blockPos.col - 1) < 0)
        {
            blockPos.col = blockPos.col;
        }
        else
        {
            blockPos.pastcol = blockPos.col;
            blockPos.col -= 1;
        }
    }
    
}

void Block::MoveRight(bool inbounds){
    if ((inbounds) && (0 <= blockPos.col) && ((blockPos.col + 4) <= 9))
    {
        if ((blockPos.col + 5) > 9)
        {
            blockPos.col = blockPos.col;
        }
        else
        {
            blockPos.pastcol = blockPos.col;
            blockPos.col += 1;
        }
    }
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

struct CG_results
{
    bool LB_result;
    bool SC_result;
    bool BC_result;
};

class Grid {
    public:
        //initializes and checks ITSELF
        Grid();
        void Init();
        void PrintGrid();
        void UpdateGrid(const std::array<int, 16>, const bool SC_result, const bool LB_result, const int, const int);
        void CleanUp(const int, const int, const int, const int, const int, const int);
        void CheckGrid(const std::array<int, 16>, const int, const int);
        CG_results GetCGresults() const {return CGresults;}
    private:
        CG_results CGresults;
        int numRows;
        int numCols;
        int grid[20][10];
};

Grid::Grid(){
    numRows = 20;
    numCols = 10;
    CGresults.LB_result = false; //false by default bc its active at spawn
    CGresults.SC_result = true; //true by default bc its clear at spawn, could change depending on conditions but that is covered by first game loop iteration.
    CGresults.BC_result = true; // true by default bc its in bounds at spawn
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

void Grid::CheckGrid(const std::array<int, 16> blockShape, const int row, const int col)
{

    for (int i = row; i < (row + 4); i++){
        for (int j = col; j < (col + 4); j++){
            int relRow = i - row;
            int relCol = j - col; //relative row and col values

            if (blockShape[(4 * relRow) + relCol] != 0){

                if ((i >= (numRows - 1) || j >= (numCols - 1)|| j < 0)) // !!!!!!!!! CHECK THIS !!!!!!!!!!
                {    
                    CGresults.BC_result = false;
                    CGresults.SC_result = false; 

                }
                else
                {
                    CGresults.BC_result = true;
                    CGresults.SC_result = true;
                }

                if ((grid[i][j] != 0)) // add a closer check
                {
                    CGresults.LB_result = true;
                    CGresults.SC_result = false; 
                }
                else
                {
                    CGresults.LB_result = false;
                    CGresults.SC_result = true;
                }

            }

        }

    }
}



void Grid::UpdateGrid(const std::array<int, 16> blockShape,const bool SC_result, const bool LB_result, const int row, const int col) // draws the block onto the grid and also locks blocks into place
{
    
    if (SC_result) 
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

void Grid::CleanUp(const int pastrow, const int pastcol, const int row, const int col, const int angle, const int pastangle)
{
    if((pastrow != row) || (pastcol != col)){
        for (int i = pastrow; i < (pastrow + 4); i++)
        {
            for (int j = pastcol; j < (pastcol + 4); j++)
            {
                grid[i][j] = 0;
            }
        }
    }

    if((pastangle != angle))
    {
        for (int i = row; i < (row + 4); i++)
        {
            for (int j = col; j < (col + 4); j++)
            {
                grid[i][j] = 0;
            }
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
        void PrintBlockData();

        void Gravity();
        void KBMoveL();
        void KBMoveR();
        void KBRotate();
        
        //Grid Stuff
        void DrawGrid();
        void GetCheckdGrid();
        void TrigGridUpdate();
        void TrigCleanUp();
        


        ~Game();
    private:
    Grid board;
    Block* currentBlock;
    int score;
    bool gridstate;
    bool inbounds;
    bool lockblock;
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
    gridstate = board.GetCGresults().SC_result;
    inbounds = board.GetCGresults().BC_result;
    lockblock = board.GetCGresults().LB_result;
    
}

Block* Game::BlockSpawn()
{
    currentBlock = new Block;
    int rnColor = dist(gen);

    currentBlock->Brandomizer(rnColor);

    return currentBlock;
}

void Game::PrintBlockData()
{
    int row = currentBlock -> GetBlockPos().row;
    int col = currentBlock -> GetBlockPos().col;
    int pastrow = currentBlock -> GetBlockPos().pastrow;
    int pastcol = currentBlock -> GetBlockPos().pastcol;
    int Angle = currentBlock -> GetBlockAngle();
    int PAngle = currentBlock -> GetPastAngle();

    std::cout << "Current Block Pos { " << "Row: " << row << " Col: " << col << " PastRow: " << pastrow << " PastCol: " << pastcol << " Angle: " << Angle << " PastAngle: " << PAngle << " }" << "\n";

}

void Game::Gravity()
{
    currentBlock -> MoveDown((board.GetCGresults().BC_result));
    TrigCleanUp();
}

void Game::KBMoveL()
{
    currentBlock->MoveLeft((board.GetCGresults().BC_result));
    TrigCleanUp();
}

void Game::KBMoveR()
{
    currentBlock -> MoveRight((board.GetCGresults().BC_result));
    TrigCleanUp();
}

void Game::KBRotate()
{
    currentBlock->Rotate();
    TrigCleanUp();
}

void Game::DrawGrid()
{
    board.PrintGrid();
}

void Game::GetCheckdGrid()
{
    board.CheckGrid((currentBlock->GtBData()),(currentBlock->GetBlockPos().row),(currentBlock->GetBlockPos().col));
    gridstate = board.GetCGresults().SC_result;
    inbounds = board.GetCGresults().BC_result;
    lockblock = board.GetCGresults().LB_result;
}

void Game::TrigGridUpdate()
{
    board.UpdateGrid((currentBlock->GtBData()), gridstate, lockblock, (currentBlock->GetBlockPos().row), (currentBlock->GetBlockPos().col));
}

void Game::TrigCleanUp()
{
    board.CleanUp((currentBlock->GetBlockPos().pastrow),(currentBlock->GetBlockPos().pastcol), (currentBlock->GetBlockPos().row), (currentBlock->GetBlockPos().col), (currentBlock -> GetBlockAngle()), (currentBlock -> GetPastAngle()));
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
        game.PrintBlockData();
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

