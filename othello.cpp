///////////////////////////////////////////////////////////////////////////////////////////////
///         NAME        :   Gavin Dominique
///         DATE        :   October 12th, 2025
///     Assignment      : Assignment 3: An Intelligent Othello Player
/// PROGRAM DESCRIPTION : A C++ program that implements the 2-player board game Othello as a
///							terminal and ascii based experience. It includes a an Othello AI
///							implemented using the MiniMax algorithm with the option to 
///							enable AlphaBeta for faster searches.
///////////////////////////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <vector>
#include <cctype>
#include <random>
#include <fstream>
using namespace std;

//// function prototypes for the Othello implimentation
void initializeBoard();
void printBoardState(int message);
bool doMove(char turn, array<int, 2> move, char board[8][8]);
vector<string> getLegalMoves(char turn, char board[8][8]);
bool moveIsLegal(char turn, int row, int col, char board[8][8]);
array<int, 2> parseInput(string move);
string parseOutput(int r, int c);
bool checkWin(char board[8][8]);
int getPoints(char turn, char board[8][8]);
void play();
void playAI();

//// function prototypes for the mini-max algorithm
string find_best_move(char turn, int searchDepth);
int minimax(char root, char curTurn, char boardState[8][8], int searchDepth, bool debug, vector<string> checkedMoves);
int minimax_prune(char root, char curTurn, char boardState[8][8], int searchDepth, int alpha, int beta, bool debug, vector<string> checkedMoves);
int heuristic(char turn, char board[8][8]);
void printVector(vector<string> vec);

//// function prototypes for random helper functions
void info();
void copyBoard(char dest[8][8], char src[8][8]);
array<array<char, 8>, 8> formatBoard(char board[8][8]);
void saveGameTrace(const vector<array<array<char, 8>, 8>>& trace, char ai, string gameOutcome);

//// global variables
const int SIZE = 8;
char BOARD[SIZE][SIZE];

const int boardWeights[8][8] = {
    { 25, -5, 10, 2, 2, 10, -5, 25 },
    { -5, -10, -2, -2, -2, -2, -10, -5 },
    { 10, -2, 3, 1, 1, 3, -2, 10 },
    { 2, -2, 1, 0, 0, 1, -2, 2 },
    { 2, -2, 1, 0, 0, 1, -2, 2 },
    { 10, -2, 3, 1, 1, 3, -2, 10 },
    { -5, -10, -2, -2, -2, -2, -10, -5 },
    { 25, -5, 10, 2, 2, 10, -5, 25 }
};

const char EMPTY = '.';
const char BLACK = 'B';
const char WHITE = 'W';

char curPlayer = BLACK;

const int SEARCH_DEPTH = 6;
int statesViewed = 0;

// main program loop
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
		cout << "[1] to begin a 2-player game." << endl;
		cout << "[2] to play against the AI." << endl;
		cout << "[3] to view a program description." << endl;
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
			playAI();
		}
		else if (in == '3')
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

// program description screen function
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
+ Each player takes turns placing a disc of their color on the board.
+ They must be placed in a way that causes your opponents pieces to flip.
+ This means playing a piece that encloses enemy pieces between two of yours.
+ Game ends when neither player has any available moves.
+ Player with the most amount of their color wins!

An Othello AI, using the MiniMax algorithm, is also available for in-game assistance 
and to play against, for those seeking a challenge.)" << endl << endl;
		cout << "[Enter] to continue.";
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.get();

}

// game loop for two humans playing
void play()
{
	vector<array<array<char, 8>, 8>> gameTrace;

	curPlayer = BLACK;

	string input;
	bool playing = true;

	initializeBoard();	// starting board state
	gameTrace.push_back(formatBoard(BOARD));
	printBoardState(-1);

	while(playing)
	{
		// check if in a win state
		if (checkWin(BOARD))	// if in a win state
		{
			int blackPoints = getPoints(BLACK, BOARD);
			int whitePoints = getPoints(WHITE, BOARD);
			if (blackPoints == whitePoints)		// check if there is a tie
			{
				cout << "Game ends in a tie!!" << endl << endl;
				playing = false;
				saveGameTrace(gameTrace, EMPTY, "TIE GAME");
				cout << "[Enter] to return to the main menu.";
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cin.get();
				break;
			}
			else
			{
				string winner = (blackPoints > whitePoints) ? "Black" : "White";
				cout << winner << " has won the game!!" << endl << endl;
				playing = false;
				saveGameTrace(gameTrace, EMPTY, (winner + " WON"));
				cout << "[Enter] to return to the main menu.";
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cin.get();
				break;
			}
		}

		// check if there is a legal move
		vector<string> moves = getLegalMoves(curPlayer, BOARD);
		if (moves.size() == 0)
		{
			printBoardState(2);
			curPlayer = (curPlayer == BLACK) ? WHITE : BLACK;	// sets the next player
		}

		// handle i/o
		cout << "Enter a command : " << endl;
		cout << "To play a piece enter the format: D3" << endl;
		cout << "[1] - Return your possible moves." << endl;
		cout << "[2] - Ask an AI for your best move." << endl;
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
		else if (input == "1")	// prints board state with added available moves
		{
			printBoardState(0);
		}
		else if ((input == "2") || (input == "H"))	// uses the MiniMax algorithm to find the best move
		{
			printBoardState(3);
		}
		else // assume it was a move input
		{
			if (doMove(curPlayer, parseInput(input), BOARD))
			{
				gameTrace.push_back(formatBoard(BOARD));
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

// game loop for playing against the ai
void playAI()
{
	vector<array<array<char, 8>, 8>> gameTrace;
	curPlayer = BLACK;

	// randomly select the AI to play as either black or white
	mt19937 seed(random_device{}());								// this just creates the random device
	uniform_int_distribution<int> flip(0, 1);						// this defines the range of the random distribution

	char AI = (flip(seed) == 0) ? WHITE : BLACK;

	cout << "You will be playing as " << ((AI == BLACK) ? "WHITE" : "BLACK") << endl;
	cout << "[Enter] to continue.";
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	cin.get();

	string input;
	bool playing = true;

	initializeBoard();	// starting board state
	gameTrace.push_back(formatBoard(BOARD));
	printBoardState(-1);

	while(playing)
	{
		// check if in a win state
		if (checkWin(BOARD))	// if in a win state
		{
			int blackPoints = getPoints(BLACK, BOARD);
			int whitePoints = getPoints(WHITE, BOARD);
			if (blackPoints == whitePoints)		// check if there is a tie
			{
				cout << "Game ends in a tie!!" << endl << endl;
				playing = false;
				saveGameTrace(gameTrace, AI, "TIE GAME");
				cout << "[Enter] to return to the main menu.";
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cin.get();
				break;
			}
			else
			{
				string winner = (blackPoints > whitePoints) ? "Black" : "White";
				cout << winner << " has won the game!!" << endl << endl;
				playing = false;
				saveGameTrace(gameTrace, AI, (winner + " WON"));
				cout << "[Enter] to return to the main menu.";
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cin.get();
				break;
			}
		}

		// check if there is a legal move
		vector<string> moves = getLegalMoves(curPlayer, BOARD);
		if (moves.size() == 0)	// if the current player cannot move skip them
		{
			printBoardState(2);
			curPlayer = (curPlayer == BLACK) ? WHITE : BLACK;	// sets the next player
		}
		
		// if it is the AI's turn
		if (curPlayer == AI)
		{
			cout << "The AI is thinking. . ." << endl;

			// find its best move
			string bestMove = find_best_move(curPlayer, SEARCH_DEPTH);

			// execute the move
			doMove(curPlayer, parseInput(bestMove), BOARD);
			gameTrace.push_back(formatBoard(BOARD));

			// update player turn
			curPlayer = (curPlayer == BLACK) ? WHITE : BLACK;	// sets the next player
			printBoardState(-1);
		}
		else // if its the player's turn
		{
			// handle i/o
			cout << "Enter a command : " << endl;
			cout << "To play a piece enter the format: D3" << endl;
			cout << "[1] - Return your possible moves." << endl;
			cout << "[2] - Ask an AI for your best move." << endl;
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
			else if (input == "1")	// prints board state with added available moves
			{
				printBoardState(0);
			}
			else if ((input == "2") || (input == "H"))	// uses the MiniMax algorithm to find the best move
			{
				printBoardState(3);
			}
			else // assume it was a move input
			{
				if (doMove(curPlayer, parseInput(input), BOARD))
				{
					gameTrace.push_back(formatBoard(BOARD));
					curPlayer = (curPlayer == BLACK) ? WHITE : BLACK;	// sets the next player
					printBoardState(-2);	// special "AI is thinking" screen so the game feels more interactive
				}
				else
				{
					printBoardState(1);		// prints out an illegal move message
				}
			}
		}
	}
}

// returns the amount of points a player has based on a board state
int getPoints(char turn, char board[8][8])
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

// checks the current board state for a win condition
bool checkWin(char board[8][8])
{
	vector<string> blackMoves = getLegalMoves(BLACK, board);
	vector<string> whiteMoves = getLegalMoves(WHITE, board);

	if ((blackMoves.size() == 0) && (whiteMoves.size() == 0))	// if neither piece has available moves
	{
		return true;
	}

	return false;
}

// sets the initial values for the starting game board (just the 4 pieces in the middle)
void initializeBoard()
{
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			if ((i == 3 && j == 3) || (i == 4 && j == 4))		// sets the initial white pieces
			{
				BOARD[i][j] = WHITE;
			}
			else if ((i == 3 && j == 4) || (i == 4 && j == 3))	// sets the initial black pieces
			{
				BOARD[i][j] = BLACK;
			}
			else {BOARD[i][j] = EMPTY;}				// sets initial blank spots
		}
	}
}

// main print for the game loop
// prints the current turn and player standings
// prints a special message based on the int value given
void printBoardState(int message)
{
	int pointsB = getPoints(BLACK, BOARD);
	int pointsW = getPoints(WHITE, BOARD);

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
			cout << BOARD[i][j] << " ";
		}
		cout << "|" << endl;
	}
	
	cout << "--+-----------------+-" << endl;
	cout << endl;

	// print out selected message
	if (message == 0)	// 0 is the move help
	{
		cout << "--- Possible Moves ---" << endl;
		vector<string> possible = getLegalMoves(curPlayer, BOARD);
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
	else if (message == 3)	// 3 is the find best move option
	{
		string bestMove = find_best_move(curPlayer, SEARCH_DEPTH);
		cout << "--- Best move is " + bestMove + " ---";
	}
	else
	{
		cout << "----------------------";
	}
	cout << endl << endl;
}

// returns a vector containing all of the players legal moves based on a board state
vector<string> getLegalMoves(char turn, char board[8][8])
{
	vector<string> moves = {};

	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			if (moveIsLegal(turn, i, j, board))
			{
				moves.push_back(parseOutput(i, j));
			}
		}
	}

	return moves;
}

// parses an string in put into an array giving the moves row and col
// example : C3 becomes array(2, 2)
array<int, 2> parseInput(string move)
{
	move[0] = toupper(move[0]); // normalize letter
    int row = move[1] - '1';    // convert '1'-'8' → 0-7
    int col = move[0] - 'A';    // convert 'A'-'H' → 0-7
    return {row, col};
}

// parses a row and column input into the string move
// example : move(2, 2) becomes C3
string parseOutput(int r, int c)
{
	string cols[8] = {"A", "B", "C", "D", "E", "F", "G", "H"};
	return cols[c] + to_string(r+1);
}

// checks if a given move is legal for a player given a board state
bool moveIsLegal(char turn, int row, int col, char board[8][8])
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
bool doMove(char turn, array<int, 2> move, char board[8][8])
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

// function that returns a char[8][8] into an array<array<char, 8>, 8> so it can be saved into the game trace
array<array<char, 8>, 8> formatBoard(char board[8][8]) 
{
    array<array<char, 8>, 8> array;
    for (int i = 0; i < 8; ++i)
	{
        for (int j = 0; j < 8; ++j)
		{
            array[i][j] = board[i][j];
		}
	}

    return array;
}

// helper function that asks the user if they would like to save the game trace to a file
void saveGameTrace(const vector<array<array<char, 8>, 8>>& trace, char ai, string gameOutcome) 
{
	string input = "";
	cout << endl << "Do you want to save the game trace? [y/n]" << endl;
	cout <<  "(will overrite current game_trace.txt)" << endl;
	cout << "> ";
	cin >> input;
	cout << endl;

	transform(input.begin(), input.end(), input.begin(),::toupper);
	if (input == "Y") 
	{
		// creates output and checks if it was succesful
		ofstream output("game_trace.txt");
		if (!output) 
		{
			cerr << "Error: Could not open file for writing.\n";
			return;
		}

		// checks if the game was played against an ai and updates the trace accordingly
		if (ai != EMPTY)
		{
			output << "AI played as " << ai << endl;
		}

		output << "GAME OUTCOME -> " << gameOutcome << endl;

		for (size_t t = 0; t < trace.size(); ++t) {
			output << "Turn " << t + 1 << ":" << endl;
			output << "  | A B C D E F G H |" << endl;
			output << "--+-----------------+-" << endl;
			for (int i = 0; i < 8; ++i) {
				output << (i + 1) << " | ";
				for (int j = 0; j < 8; ++j)
					output << trace[t][i][j] << ' ';
				output << "|" << endl;
			}
			output << "--+-----------------+-" << endl << endl;
		}

		output.close();
		cout << "Game trace saved to 'game_trace.txt'." << endl << endl;
	}
	else { cout << "Game trace not saved." << endl << endl; }
}

// sets dest array to be a copy of src array
void copyBoard(char dest[8][8], char src[8][8])
{
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			dest[i][j] = src[i][j];
		}
	}
}

// uses the mini_max algorithm to find the current players best move
string find_best_move(char turn, int searchDepth)
{
	bool debug = false;
	bool prune = false;
	string input = "";
	cout << endl << "Do you want to print debug? [y/n]" << endl;
	cout << "> ";
	cin >> input;
	cout << endl;

	transform(input.begin(), input.end(), input.begin(),::toupper);
	if (input == "Y") { debug = true; }

	input = "";
	cout << endl << "Do you want to enable pruning? [y/n]" << endl;
	cout << "> ";
	cin >> input;
	cout << endl;

	transform(input.begin(), input.end(), input.begin(),::toupper);
	if (input == "Y") { prune = true;}

	vector<string> possibleMoves = getLegalMoves(turn, BOARD);
	vector<int> scores(possibleMoves.size());

	// reset the number of states viewed
	statesViewed = 0;

	for (int i = 0; i < possibleMoves.size(); i++)
	{
		char curState[8][8]; copyBoard(curState, BOARD);				// grab a copy of the current board state
		doMove(turn, parseInput(possibleMoves[i]), curState);			// do the move

		vector<string> checkedMoves;
		checkedMoves.push_back(possibleMoves[i]);

		if (prune) { scores[i] = minimax_prune(turn, ((turn == BLACK) ? WHITE : BLACK), curState, searchDepth - 1, -1000000, 1000000, debug, checkedMoves); }
		else { scores[i] = minimax(turn, ((turn == BLACK) ? WHITE : BLACK), curState, searchDepth-1, debug, checkedMoves);} 
	}

	if (debug)
	{
		// give the user a second to look at the debug output
		cout << "[Enter] to continue.";
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.get();
	}

	cout << "States examined : " << statesViewed << endl;

	// re-write it to maybe keep track of and return the best available move instead of just the score?
	vector<int> bestFound;
	int best = -1000000;

	for (int i = 0; i < scores.size(); i++) 
	{
		if (scores[i] > best) 
		{
			best = scores[i];
			bestFound = {i};
		} 
		else if (scores[i] == best) 
		{
			bestFound.push_back(i);
		}
	}

	// Randomly choose one of the best moves found
	mt19937 rng(std::random_device{}());
	uniform_int_distribution<int> dist(0, bestFound.size() - 1);
	return possibleMoves[bestFound[dist(rng)]];
}

// the player currently searching, the board state, the search depth, and if its a maximizing step
int minimax(char root, char curTurn, char boardState[8][8], int searchDepth, bool debug, vector<string> checkedMoves)
{
	statesViewed++;
	vector<string> possibleMoves = getLegalMoves(curTurn, boardState);

	// if at the end of the search or if the current state is a win state
	if (searchDepth == 0 || checkWin(boardState))
	{
		if (debug) 
		{ 
			// print out the moves viewed and its heurstic value
			printVector(checkedMoves); 
			cout << heuristic(root, boardState) << endl; 
		}

		// increase the number of states viewed by the size of moves checked
		// this logic SHOULD make sense, each child hit stores the moves checked before it
		// so the size of checked moves at the end of every node SHOULD properly update it
		//statesViewed += checkedMoves.size(); // this method did NOT work lmao
    	return heuristic(root, boardState);
	}

	// if there are no possible moves
	if (possibleMoves.empty()) 
	{
		// if opponent also has no moves then the game is over
		vector<string> oppMoves = getLegalMoves(((curTurn == BLACK) ? WHITE : BLACK), boardState);
		if (oppMoves.empty())
		{
			if (debug) 
			{ 
				// print out the moves viewed and its heurstic value
				printVector(checkedMoves); 
				cout << heuristic(root, boardState) << endl; 
			}

			return heuristic(root, boardState);
		}
		// if just you have nothing then L pass your turn nerd
		return minimax(root, ((curTurn == BLACK) ? WHITE : BLACK), boardState, searchDepth - 1, debug, checkedMoves);
	}

	int maxEval, eval, minEval;

	if (root == curTurn)
	{
		maxEval = -1000000;
		for (string move : possibleMoves)
		{
			char newState[8][8]; copyBoard(newState, boardState);											// copies the board state
			doMove(curTurn, parseInput(move), newState);													// does the chosen move
			checkedMoves.push_back(move);
			eval = minimax(root, ((curTurn == BLACK) ? WHITE : BLACK), newState, searchDepth - 1, debug, checkedMoves);	// conitnues to search deeper
			maxEval = max(maxEval, eval);																	// choose the best option
		}
		return maxEval;
	}

	else
	{
		minEval = 1000000;
		for (string move : possibleMoves)
		{
			char newState[8][8]; copyBoard(newState, boardState);											// copies the board state
			doMove(curTurn, parseInput(move), newState);													// does the chosen move
			checkedMoves.push_back(move);
			eval = minimax(root, ((curTurn == BLACK) ? WHITE : BLACK), newState, searchDepth - 1, debug, checkedMoves);	// conitnues to search deeper
			minEval = min(minEval, eval);																	// choose the best option
		}
		return minEval;
	}
}

// version of the minimax algorithm that implements alpha beta pruning. added this just so the code LOOKED cleaner
int minimax_prune(char root, char curTurn, char boardState[8][8], int searchDepth, int alpha, int beta, bool debug, vector<string> checkedMoves)
{
	statesViewed++;
	vector<string> possibleMoves = getLegalMoves(curTurn, boardState);

	// if at the end of the search or if the current state is a win state
	if (searchDepth == 0 || checkWin(boardState))
	{
		if (debug) 
		{ 
			// print out the moves viewed and its heurstic value
			printVector(checkedMoves); 
			cout << heuristic(root, boardState) << endl; 
		}

    	return heuristic(root, boardState);
	}

	// if there are no possible moves
	if (possibleMoves.empty()) 
	{
		// if opponent also has no moves then the game is over
		vector<string> oppMoves = getLegalMoves(((curTurn == BLACK) ? WHITE : BLACK), boardState);
		if (oppMoves.empty())
		{
			if (debug) 
			{ 
				// print out the moves viewed and its heurstic value
				printVector(checkedMoves); 
				cout << heuristic(root, boardState) << endl; 
			}

			return heuristic(root, boardState);
		}
		// if just you have nothing then L pass your turn nerd
		return minimax_prune(root, ((curTurn == BLACK) ? WHITE : BLACK), boardState, searchDepth - 1, alpha, beta, debug, checkedMoves);
	}

	int maxEval, eval, minEval;

	if (root == curTurn)
	{
		maxEval = -1000000;
		for (string move : possibleMoves)
		{
			char newState[8][8]; copyBoard(newState, boardState);																// copies the board state
			doMove(curTurn, parseInput(move), newState);																		// does the chosen move
			checkedMoves.push_back(move);
			eval = minimax_prune(root, ((curTurn == BLACK) ? WHITE : BLACK), newState, searchDepth - 1, alpha, beta, debug, checkedMoves);	// conitnues to search deeper
			maxEval = max(maxEval, eval);																						// choose the best option
			alpha = max(alpha, eval);
            if (beta <= alpha) 								// prune that shit baby
			{
				if (debug) { cout << " pruned " << endl; }
				break; 
			}
		}
		return maxEval;
	}

	else
	{
		minEval = 1000000;
		for (string move : possibleMoves)
		{
			char newState[8][8]; copyBoard(newState, boardState);																// copies the board state
			doMove(curTurn, parseInput(move), newState);																		// does the chosen move
			checkedMoves.push_back(move);
			eval = minimax_prune(root, ((curTurn == BLACK) ? WHITE : BLACK), newState, searchDepth - 1, alpha, beta, debug, checkedMoves);	// conitnues to search deeper
			minEval = min(minEval, eval);																						// choose the best option
			beta = min(beta, eval);
            if (beta <= alpha) 								// prune that shit baby
			{
				if (debug) { cout << " pruned " << endl; }
				break; 
			}
		}
		return minEval;
	}
}

// computes the heuristic for a given player and board state
// uses a predefined weight table for scoring
int heuristic(char root, char board[8][8])
{
    int score = 0;
	char enemy = (root == BLACK) ? WHITE : BLACK;

	// scores each cell based on its location
	for (int i = 0; i < 8; i++) 
	{
		for (int j = 0; j < 8; j++) 
		{
			if (board[i][j] == root)
			{
				score += boardWeights[i][j];
			}
			else if (board[i][j] == enemy)
			{
				score -= boardWeights[i][j];
			}
		}
	}

	return score;
}

// just a helper function for printing the debug for minimax
void printVector(vector<string> vec)
{
	for (int i = 0; i < vec.size(); i++)
	{
		cout << vec[i] << " -> ";
	}
}