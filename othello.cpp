#include <iostream>
#include <vector>
#include <cctype>
using namespace std;

// function prototypes
void initializeBoard();
void printBoardState(int message);
bool doMove(char turn, array<int, 2> move);
vector<string> getLegalMoves(char turn);
bool moveIsLegal(char turn, int row, int col);
array<int, 2> parseInput(string move);
string parseOutput(int r, int c);
bool checkWin();
int getPoints(char turn);
void info();
void play();

// global variables
const int SIZE = 8;
char board[SIZE][SIZE];

const char EMPTY = '.';
const char BLACK = 'B';
const char WHITE = 'W';

char curPlayer = BLACK;

int main()
{
	bool running = true;
	while (running)
	{
		cout << "\033[2J\033[1;1H";		// clears the screen
		cout << "-+-------------------------------------------------------------------+-" << endl;
		cout << R"(    ___       ___       ___       ___       ___       ___       ___   
   /\  \     /\  \     /\__\     /\  \     /\__\     /\__\     /\  \  
  /::\  \    \:\  \   /:/__/_   /::\  \   /:/  /    /:/  /    /::\  \ 
 /:/\:\__\   /::\__\ /::\/\__\ /::\:\__\ /:/__/    /:/__/    /:/\:\__\
 \:\/:/  /  /:/\/__/ \/\::/  / \:\:\/  / \:\  \    \:\  \    \:\/:/  /
  \::/  /   \/__/      /:/  /   \:\/  /   \:\__\    \:\__\    \::/  / 
   \/__/               \/__/     \/__/     \/__/     \/__/     \/__/  )" << endl;
		cout << endl << "-+-------------------------------------------------------------------+-" << endl;
		
		cout << "Programmed by: Gavin Dominique" << endl << endl;
		
		char in;
		cout << "[1] to begin a game." << endl;
		cout << "[2] to view a program description." << endl;
		cout << "[0] to quit the program. " << endl;
		cout << endl << "> ";
		cin >> in;
		if (in == '1')
		{
			cout << "\033[2J\033[1;1H";		// clears the screen
			play();							// begins the game
		}
		else if (in == '2')
		{
			info();
		}
		else if (in == '0')
		{
			running = false;
		}
	}

	cout << endl;
	return 0;
}

void info()
{
	cout << "\033[2J\033[1;1H";		// clears the screen
	cout << "-+-------------------------------------------------------------------+-" << endl;
	cout << R"(    ___       ___       ___       ___       ___       ___       ___   
   /\  \     /\  \     /\__\     /\  \     /\__\     /\__\     /\  \  
  /::\  \    \:\  \   /:/__/_   /::\  \   /:/  /    /:/  /    /::\  \ 
 /:/\:\__\   /::\__\ /::\/\__\ /::\:\__\ /:/__/    /:/__/    /:/\:\__\
 \:\/:/  /  /:/\/__/ \/\::/  / \:\:\/  / \:\  \    \:\  \    \:\/:/  /
  \::/  /   \/__/      /:/  /   \:\/  /   \:\__\    \:\__\    \::/  / 
   \/__/               \/__/     \/__/     \/__/     \/__/     \/__/  )" << endl;
	cout << endl << "-+-------------------------------------------------------------------+-" << endl << endl;

	cout << R"(Implementation of the classic 2-player board game Othello!
Here are the basic game rules:
  1. Each player takes turn place a disc of their color on the board.
  2. They must be placed in a way that causes your opponents pieces to flip.
  3. This means playing a piece that encloses enemy pieces between two of yours.
  4. Game ends when neither player has any available moves.
  5. Player with the most amount of their color wins!

Future implantation will add a functional Othello AI that can be played against.)" << endl;
		cout << "[Enter] to continue.";
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.get();

}

void play()
{
	string input;
	bool playing = true;

	initializeBoard();	// starting board state
	printBoardState(-1);

	while(playing)
	{
		// check if in a win state
		if (checkWin())	// if in a win state
		{
			int blackPoints = getPoints(BLACK);
			int whitePoints = getPoints(WHITE);
			if (blackPoints == whitePoints)		// check if there is a tie
			{
				cout << "Game ends in a tie!!" << endl << endl;
				playing = false;
				cout << "[Enter] to continue.";
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cin.get();
				break;
			}
			else
			{
				string winner = (blackPoints > whitePoints) ? "Black" : "White";
				cout << winner << " has won the game!!" << endl << endl;
				playing = false;
				cout << "[Enter] to continue.";
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cin.get();
				break;
			}
		}

		// check if there is a legal move
		vector<string> moves = getLegalMoves(curPlayer);
		if (moves.size() == 0)
		{
			printBoardState(2);
			curPlayer = (curPlayer == BLACK) ? WHITE : BLACK;	// sets the next player
		}

		// handle i/o
		cout << "Enter a command : " << endl;
		cout << "To play a piece enter the format: D3" << endl;
		cout << "[1] - Return your possible moves." << endl;
		//cout << "[2] - AI helper" << endl;
		cout << "[0] - Quit to the main menu." << endl;
		cout << "> ";
		cin >> input;
		cout << endl;

		// converts the input to uppercase 
		transform(input.begin(), input.end(), input.begin(),::toupper);

		// check what the input was
		if ((input == "0") || (input == "Q"))		// quit the game
		{
			playing = false;
			break;
		}
		else if ((input == "1") || (input == "H"))	// prints board state with added available moves
		{
			printBoardState(0);
		}
		else // assume it was a move input
		{
			if (doMove(curPlayer, parseInput(input)))
			{
				curPlayer = (curPlayer == BLACK) ? WHITE : BLACK;	// sets the next player
				printBoardState(-1);	// no special messages
			}
			else
			{
				printBoardState(1);		// prints out an illegal move message
			}
		}
	}
}

int getPoints(char turn)
{
	int points = 0;

	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			if (board[i][j] == turn)
			{
				points++;
			}
		}
	}

	return points;
}

bool checkWin()
{
	vector<string> blackMoves = getLegalMoves(BLACK);
	vector<string> whiteMoves = getLegalMoves(WHITE);

	if ((blackMoves.size() == 0) && (whiteMoves.size() == 0))	// if neither piece has available moves
	{
		return true;
	}

	return false;
}

void initializeBoard()
{
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			if ((i == 3 && j == 3) || (i == 4 && j == 4))		// sets the initial white pieces
			{
				board[i][j] = WHITE;
			}
			else if ((i == 3 && j == 4) || (i == 4 && j == 3))	// sets the initial black pieces
			{
				board[i][j] = BLACK;
			}
			else {board[i][j] = EMPTY;}				// sets initial blank spots
		}
	}
}

void printBoardState(int message)
{
	int pointsB = getPoints(BLACK);
	int pointsW = getPoints(WHITE);

	cout << "\033[2J\033[1;1H";		// clears the screen
	cout << "Current player : " << ((curPlayer == BLACK) ? "Black" : "White")
		<< " | Points : B = " << pointsB << " & W = " << pointsW << endl;
	
	cout << "  | A B C D E F G H |" << endl;
	cout << "--+-----------------+-" << endl;

	for (int i = 0; i < SIZE; i++)
	{
		cout << (i + 1) << " | ";
		for (int j = 0; j < SIZE; j++)
		{
			cout << board[i][j] << " ";
		}
		cout << "|" << endl;
	}
	
	cout << "--+-----------------+-" << endl;
	cout << endl;

	// print out selected message
	if (message == 0)	// 0 is the move help
	{
		cout << "--- Possible Moves ---" << endl;
		vector<string> possible = getLegalMoves(curPlayer);
		for (int i = 0; i < possible.size(); i++)
		{
			cout << possible[i] << " ";
		}
	}
	else if (message == 1)	// 1 is the illegal move
	{
		cout << "---- Illegal Move ----";
	}
	else if (message == 2)	// 2 is the skip move notice
	{
		cout << "No valid moves, turn was skipped.";
	}
	else
	{
		cout << "----------------------";
	}
	cout << endl << endl;
}

vector<string> getLegalMoves(char turn)
{
	vector<string> moves = {};

	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			// for each cell on the board
			if (moveIsLegal(turn, i, j))
			{
				moves.push_back(parseOutput(i, j));
			}
		}
	}

	return moves;
}

array<int, 2> parseInput(string move)
{
	move[0] = toupper(move[0]); // normalize letter
    int row = move[1] - '1';    // convert '1'-'8' → 0-7
    int col = move[0] - 'A';    // convert 'A'-'H' → 0-7
    return {row, col};
}

string parseOutput(int r, int c)
{
	string cols[8] = {"A", "B", "C", "D", "E", "F", "G", "H"};
	return cols[c] + to_string(r+1);
}

bool moveIsLegal(char turn, int row, int col)
{
	// if row or column out of bounds or the cell is occupied
	if ((row < 0 || row >= SIZE) || (col < 0 || col >= SIZE) || (board[row][col] != EMPTY))
	{
		return false;	// the move is ILLEGAL
	}

	// fucking logic for legal moves idk yet
	//// what to look for in every direction =
	// space + x enemy piece(s) + my piece
	// my piece + x enemy piece(s) + space

	// fucking ace for stack overflow dude holy shit
	// https://stackoverflow.com/questions/44736696/othello-algorithm-for-finding-the-available-options-to-play
	// used the pseudo code from one of the answers in the post above to help figure it out

	// all possible move directions that need to be checked
	int directions[8][2] = {
		{-1, -1}, { 1, 0}, {1, -1},
		{0,  -1},		   {0,  1},
		{-1,  1}, {-1, 0}, {1,  1}
	};

	for (int i = 0; i < 8; i++)
	{
		int r = row + directions[i][0];
		int c = col + directions[i][1];

		bool seenOpponent = false;

		// while the checked position is in bounds and the piece is not your own
		char enemy = (turn == BLACK) ? WHITE : BLACK;
		while ((r >= 0 && r < SIZE) && (c >= 0 && c < SIZE) && (board[r][c] == enemy))
		{
			r += directions[i][0];
			c += directions[i][1];
			seenOpponent = true;
		}

		if ((r >= 0 && r < SIZE) && (c >= 0 && c < SIZE) && (board[r][c] == turn) && seenOpponent)
		{
			return true; // the move is LEGAL
		}
	}

	return false; // the move is ILLEGAL
}

// function that takes in the current player and the move's row and column
// it then attempts to do the move and flip enemy pieces if needed
bool doMove(char turn, array<int, 2> move)
{
	int row = move[0];
	int col = move[1];
	int valid  = false;

	// if the move is out of bounds or the cell is occupied
	if ((row < 0 || row >= SIZE) || (col < 0 || col >= SIZE) || (board[row][col] != EMPTY))
	{
		cout << "Move is not legal!" << endl << endl;	// then the move is not legal
		return valid;	// exit the function
	}

	// all possible move directions that need to be checked
	int directions[8][2] = {
		{-1, -1}, {1,  0}, {1, -1},
		{0,  -1},		   {0,  1},
		{-1,  1}, {-1, 0}, {1,  1}
	};

	for (int i = 0; i < 8; i++)	// for each possible move direction
	{
		int r = row + directions[i][0];	// get the initial row to check
		int c = col + directions[i][1]; // get the initial column to check

		bool hasEnemy = false;

		// while the checked position is in bounds and the piece is not your own
		char enemy = (turn == BLACK) ? WHITE : BLACK;
		while ((r >= 0 && r < SIZE) && (c >= 0 && c < SIZE) && (board[r][c] == enemy))
		{
			r += directions[i][0];	// iterate the r and c values
			c += directions[i][1];
			hasEnemy = true;		// check that we've seen an enemy piece
		}

		// if the final position is in bounds and the piece is our own and we've seen an enemy piece
		if ((r >= 0 && r < SIZE) && (c >= 0 && c < SIZE) && (board[r][c] == turn) && hasEnemy)
		{
			board[row][col] = turn;					// set the move position to our piece

			int flip_r = row + directions[i][0];	// get the initial row to flip
			int flip_c = col + directions[i][1];	// get the initial row to flip

			// while the position to flip is within bounds and its not one of our own pieces
			while ((flip_r >= 0 && flip_r < SIZE) && (flip_c >= 0 && flip_c < SIZE) && (board[flip_r][flip_c] != turn))
			{
				board[flip_r][flip_c] = turn;	// flip the piece
				flip_r += directions[i][0];		// iterate the r and c values
				flip_c += directions[i][1];
			}

			valid = true; // move is valid
		}
	}

	// if function was not exited then the move was not legal
	if (!valid)
	{
		cout << "Move is not legal!" << endl << endl;
	}

	return valid;
}