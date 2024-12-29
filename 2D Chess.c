#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//fast checkmate input: G7 G5 E2 E4 F7 F6 D1 H5 (fool's mate)
//scholar's mate:  E7 E5, E2 E4, F8 C5, B1 C3, D8 H4, G1 F3, H4 F2
//check without checkmate: f7 f5 e2 e4 a7 a5 d1 h5


// PIECE VARIABLE
typedef struct {
    char type;
    char color;
    char disp;
    int Y;
} pieces;

// CHESS BOARD
pieces board[8][8];
pieces temp;
pieces blank;

// TURN COUNTER
int turnCount = 0;

    char turn = 'W';


// FUNCTION TO READ CHESSBOARD FROM FILE
void reader(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Unable to open file %s\n", filename);
        exit(1);
    }
fscanf(file, " %c", &turn);
    for (int j = 0; j < 8; j++) {
        for (int i = 0; i < 8; i++) {
            fscanf(file, " %c %c %c %d", &board[i][j].type, &board[i][j].color, &board[i][j].disp, &board[i][j].Y);
        }
    }

    fclose(file);
}


// RESET ROUTINE
void reset() {
    reader("reset.txt");
}


// LOAD SAVED GAME
void reload() {
    reader("save.txt");
}


// SAVE GAME
void savegame() {
    FILE *file = fopen("save.txt", "w");
    if (file == NULL) {
        printf("Error: Unable to open save file.\n");
        return;
    }
fprintf(file, " %c\n", turn);
    for (int j = 0; j < 8; j++) {
        for (int i = 0; i < 8; i++) {
            fprintf(file, " %c\n %c\n %c\n %d\n", board[i][j].type, board[i][j].color, board[i][j].disp, board[i][j].Y);
        }
    }

    fclose(file);
}


// DISPLAY BOARD ON SCREEN
void display() {
    printf("\n        | A | B | C | D | E | F | G | H |\n");

    printf("\n         ");
    for (int i = 0; i < 8; i++) {
        printf("----");
    }
    printf("\n");
    char symbol;
    for (int j = 0; j < 8; j++) {
        printf("  %d |   |", j + 1);
        for (int i = 0; i < 8; i++) {
            symbol = board[i][j].disp;
            if (symbol != '_')
                printf(" %c |", symbol);
            else
                printf(" %c |", ' ');
        }
        printf("\n         ");
        for (int i = 0; i < 8; i++) {
            printf("----");
        }

        printf("\n");
    }

    printf("\n        | A | B | C | D | E | F | G | H |\n");
}


// BISHOP MOVEMENT
int bishop(int x, int y) {
    return (abs(x) == abs(y)) ? 1 : 0;
}


// ROOK MOVEMENT
int rook(int x, int y) {
    return (x == 0 || y == 0) ? 1 : 0;
}


// KNIGHT MOVEMENT
int knight(int x, int y) {
    x = abs(x);
    y = abs(y);
    return ((x == 2 && y == 1) || (x == 1 && y == 2)) ? 1 : 0;
}


// PAWN MOVEMENT
int pawn(int x, int y, char color, int destX, int destY) {
    if (color == 'W') { // White pawn logic
        if ((x == 1 || x == -1) && y == -1 && board[destX][destY].color == 'B') {
            return 1; // Capture move
        }
        if (x == 0 && y == -1 && board[destX][destY].type == '_') {
            return 1; // Single move forward
        }
        if (x == 0 && y == -2 && destY == 4 && board[destX][destY].type == '_' && board[destX][destY + 1].type == '_') {
            return 1; // Double move forward from initial position
        }
    } else if (color == 'B') { // Black pawn logic
        if ((x == 1 || x == -1) && y == 1 && board[destX][destY].color == 'W') {
            return 1; // Capture move
        }
        if (x == 0 && y == 1 && board[destX][destY].type == '_') {
            return 1; // Single move forward
        }
        if (x == 0 && y == 2 && destY == 3 && board[destX][destY].type == '_' && board[destX][destY - 1].type == '_') {
            return 1; // Double move forward from initial position
        }
    }
    return 0; // Invalid move
}


// KING MOVEMENT
int king(int x, int y, int xchosen, int ychosen) {
   return (abs(x) <= 1 && abs(y) <= 1) ? 1 : 0;
}

// QUEEN MOVEMENT
int queen(int x, int y) {
    return (rook(x, y) || bishop(x, y)) ? 1 : 0;
}

// CONVERT COORDINATES TO GRID INDEX
int xcoord(const char *input) {
    return (input[0] >= 'A' && input[0] <= 'H') ? (input[0] - 'A') : -1;
}

int ycoord(const char *input) {
    return (input[1] >= '1' && input[1] <= '8') ? (input[1] - '1') : -1;
}

// CHECK IF PIECE IS VALID
char piecereader(int row, int col, char turn) {
    if (board[row][col].type == '_') return '1';
    if (board[row][col].color != turn) return '2';
    return board[row][col].type;
}

// MOVE PIECE
void movePiece(int srcX, int srcY, int destX, int destY) {
    board[destX][destY] = board[srcX][srcY];
    board[srcX][srcY] = blank;
}


// MOVE RESULT
int mover(int row, int col, char turn) {

    if (board[row][col].type == '_') {
        return 1;
    }else if (board[row][col].color != turn) {

        return 2;
    }


    return 0;
}


// BISHOP PATH CHECKER


int BishopPath(int startX, int startY, int goalX, int goalY) {
    int deltaX = goalX - startX;
    int deltaY = goalY - startY;

    // Ensure the move is diagonal
    if (abs(deltaX) != abs(deltaY)) {
        return 3; // Not a valid diagonal move
    }

    // Determine the step directions
    int stepX = (deltaX > 0) ? 1 : -1;
    int stepY = (deltaY > 0) ? 1 : -1;

    // Check each square along the path (exclude the destination)
    for (int i = 1; i < abs(deltaX); i++) {
        int newX = startX + i * stepX;
        int newY = startY + i * stepY;

        if (board[newX][newY].type != '_') {
            return 0; // Path is blocked
        }
    }

    return 1; // Path is clear
}


// ROOK PATH CHECKER

int RookPath(int startX, int startY, int goalX, int goalY) {
    // Ensure the move is vertical or horizontal
    if (startX != goalX && startY != goalY) {
        return 3; // Not a valid rook move
    }

    // Determine the direction and check the path
    if (startX == goalX) { // Vertical move
        int stepY = (goalY > startY) ? 1 : -1;
        for (int i = startY + stepY; i != goalY; i += stepY) {
            if (board[startX][i].type != '_') {
                return 0; // Path is blocked
            }
        }
    } else if (startY == goalY) { // Horizontal move
        int stepX = (goalX > startX) ? 1 : -1;
        for (int i = startX + stepX; i != goalX; i += stepX) {
            if (board[i][startY].type != '_') {
                return 0; // Path is blocked
            }
        }
    }

    return 1; // Path is clear
}


// QUEEN PATH CHECKER



int QueenPath(int startX, int startY, int goalX, int goalY) {
    // Check both bishop and rook paths
    int checker = BishopPath(startX, startY, goalX, goalY);
    if (checker == 3) { // Not a diagonal move, try rook path
        checker = RookPath(startX, startY, goalX, goalY);
    }
    return checker; // Return result of the valid path checker
}


// PAWN PROMOTION CONDITION
int checkPromotionCondition(char turn, int x, int y) {
    int promotionRow = (turn == 'W') ? 0 : 7; // Top row for White, Bottom row for Black
    return (y == promotionRow && board[x][y].type == 'P') ? 1 : 0;
}


// PAWN PROMOTER


void promotePawn(char color, int x, int y) {
    char piece;
    printf("Pawn promotion! Enter the symbol of the piece to promote to (Q/R/B/N): ");

    while (1) {
        scanf(" %c", &piece);

        // Validate input
        if (piece == 'Q' || piece == 'R' || piece == 'B' || piece == 'K') {
            board[x][y].type = piece;          // Update the type of the pawn
            board[x][y].disp = (color == 'W') ? tolower(piece) : piece; // Display lowercase for white, uppercase for black
            printf("Pawn promoted to %c.\n", piece);
            break;
        } else {
            printf("Invalid input. Please enter Q, R, B, or K: ");
        }
    }
}

//notes:
//reminder: piecereader will read the selected coordinate and say if its a valid piecereader
//mover outputs whether the destination coordinates are valid


int moveableSquare(int x, int y, int newx, int newy)
{
    if (board[x][y].color == board[newx][newy].color)
        return 0;
    return 1;
}

//a function to tell you if a piece at x, y has a valid move to newx, newy (assuming checks don't exist)
int validPath(int x, int y, int newx, int newy, char type, char color)
{
    if (!moveableSquare(x, y, newx, newy)) return 0;

    int dx = newx - x;
    int dy = newy - y;

    switch (type)
    {
        case 'B':
            return bishop(dx, dy) && BishopPath(x, y, newx, newy);
        case 'R':
            return rook(dx, dy) && RookPath(x, y, newx, newy);
        case 'Q':
            return queen(dx, dy) && QueenPath(x, y, newx, newy);
        case 'C':
            return king(dx, dy, newx, newy);
        case 'K':
            return knight(dx, dy);
        case 'P':
            return pawn(dx, dy, color, newx, newy);
        default:
            return 0;
    }
}





//function that outputs 1 if your king can be attacked by an opponent piece
int inCheck(char yourColor)
{

    //locate this player's king
    int i, j, kingX = -1, kingY;

    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (board[i][j].type == 'C' && board[i][j].color == yourColor)
            {
                kingX = i;
                kingY = j;
                break;
            }
        }

        if (kingX != -1) break;

    }

    char oppColor = (yourColor == 'W') ? 'B':'W';

    //go through the board, find opponent pieces and see if they have a valid move to attack the king
    for (i = 0; i < 8; i ++)
    {
        for (j = 0; j < 8; j++)
        {
            if (board[i][j].color == oppColor && validPath(i, j, kingX, kingY, board[i][j].type, oppColor))
                return 1; //if there is a valid move for the opponent to capture your king
        }
    }

    return 0;
}

//



//a function to see if a move made by a player results in their king being in check
int selfCheck(int pieceX, int pieceY, int destX, int destY, char yourColor)
{


    //store the piece that is going to be moved and the piece currently at the destination
    pieces pieceMoved = board[pieceX][pieceY];
    pieces destPiece = board[destX][destY];

    //temporarily move the piece
    movePiece(pieceX, pieceY, destX, destY);

    //check if your king is in check after the move
    int isSelfCheck = inCheck(yourColor);

    //undo the move
    board[pieceX][pieceY] = pieceMoved;
    board[destX][destY] = destPiece;

    //return whether the king is in check
    return isSelfCheck;
}

//function to check if the piece selected has a valid move (i.e. the piece can go there physically and no check condition afterward)
int hasValidMove(int pieceX, int pieceY, char yourColor) {
    //check every possible coordinate as a possible destination coordination
    for (int destX = 0; destX < 8; destX++)
    {
        for (int destY = 0; destY < 8; destY++)
        {
            if (validPath(pieceX, pieceY, destX, destY, board[pieceX][pieceY].type, yourColor) &&
                !selfCheck(pieceX, pieceY, destX, destY, yourColor))
                return 1; //if valid move found

        }
    }
    return 0; //if no valid moves found
}

int youHaveValidMove(char yourColor)
{
    int i, j, destX, destY;

    //loop through every single square
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            //if any of your pieces has a valid move return 1
            if (board[i][j].color == yourColor && hasValidMove(i, j, yourColor))
                    return 1;
        }
    }

    return 0;
}



//checkmate condition check (is there a valid move and are you in check )
int isCheckmate(char yourColor)
{
    if (inCheck(yourColor) && !youHaveValidMove(yourColor))
        return 1;

    return 0;

}

//stalemate condition (is there a valid move and you are not in check)
int isStalemate(char yourColor)
{
    if (!inCheck(yourColor) && !youHaveValidMove(yourColor))
        return 1;

    return 0;
}






// MAIN GAME FUNCTION
int main() {
    blank.type = '_';
    blank.color = '_';
    blank.disp = '_';

    // Display menu
    int choice;
    do {
        printf("Enter a number (1, 2, or 3) to pick an option:\n");
        printf("1: Start New Game\n");
        printf("2: Reload Last Save\n");
        printf("3: Quit\n");
        scanf(" %d", &choice);

        if (choice == 1) {
            reset();
        } else if (choice == 2) {
            reload();
        } else if (choice == 3) {
            printf("Goodbye!\n");
            return 0;
        } else {
            printf("Invalid input. Please enter 1, 2, or 3.\n");
        }
    } while (choice < 1 || choice > 3);


    while (1) {
        display();
        printf("Turn: %s\n", (turn == 'W') ? "White" : "Black");

        char select[3];
        int xvalue, yvalue;
        char selected = 0;


            if (isCheckmate(turn))
            {
                printf("Checkmate! %s wins!\n", (turn == 'W') ? "Black" : "White");
                break; //end the game
            }

            if (isStalemate(turn))
            {
                printf("Stalemate! It's a draw.");
                break; //end the game

            }

            if (inCheck(turn))
                printf("Check! Protect your king! \n");

        // Select piece
        do {
            printf("Enter the coordinate of the piece you want to move (e.g., A2) \nor enter S to save the game upto this point: ");
            scanf(" %s", select);
            if (isalpha(select[0])) {
                select[0] = toupper(select[0]);
            }
            if(select[0] == 'S'){
                    savegame();
            printf("Game has been saved. Now enter the coordinate of the piece you want to move");
scanf(" %s", select);
if (isalpha(select[0])) {
                select[0] = toupper(select[0]);
            }
            }
            xvalue = xcoord(select);
            yvalue = ycoord(select);

            while (xvalue == -1 || yvalue == -1) {
                printf("Invalid input. This grid does not exist.\n");
               scanf(" %s", select);
if (isalpha(select[0])) {
                select[0] = toupper(select[0]);
            }
            }

            selected = piecereader(xvalue, yvalue, turn);
            if (selected == '1') {
                printf("No piece at this position.\n");
            } else if (selected == '2') {
                printf("You cannot select an opponent's piece.\n");
            } else {
                if (!hasValidMove(xvalue, yvalue, turn)) {
                    printf("This piece has no valid moves! \n");
                    selected = '1';
                } else
                    printf("You have selected %c.\n", selected);
            }
        } while (selected == '1' || selected == '2');

        // Move piece
        char movement[3];
        int xmoved, ymoved, xdiff, ydiff;
        do {
            printf("Enter the destination coordinate (e.g., A3): ");
            scanf(" %s", movement);
            if (isalpha(movement[0])) {
                movement[0] = toupper(movement[0]);
            }

            xmoved = xcoord(movement);
            ymoved = ycoord(movement);

            if (xmoved == -1 || ymoved == -1) {
                printf("Invalid input. This grid does not exist.\n");
                continue;
            }

            xdiff = xmoved - xvalue;
            ydiff = ymoved - yvalue;

            int validMove = 0;
            int checker = 1;
            if (selected == 'B') {validMove = bishop(xdiff, ydiff);
            checker = BishopPath(xvalue, yvalue, xmoved, ymoved);}
            else if (selected == 'R') {validMove = rook(xdiff, ydiff);
            checker = RookPath(xvalue, yvalue, xmoved, ymoved);}
            else if (selected == 'Q') {validMove = queen(xdiff, ydiff);
            checker = QueenPath(xvalue, yvalue, xmoved, ymoved);}
            else if (selected == 'C') validMove = king(xdiff, ydiff, xmoved, ymoved);
            else if (selected == 'K') validMove = knight(xdiff, ydiff);
            else if (selected == 'P') validMove = pawn(xdiff, ydiff, turn, xmoved, ymoved);


            int moved = mover(xmoved, ymoved, turn);

 	if (validMove == 1 && (moved ==1 || moved == 2) && checker == 1) {
                if(!selfCheck(xvalue, yvalue, xmoved, ymoved, turn)) {
                      if(moved == 2){
                            if(turn == 'W'){
                        printf("\n\nBlack %c has been eaten. \n\n", board[xmoved][ymoved].disp);
                        } else {
                        printf("\n\nWhite %c has been eaten. \n\n", board[xmoved][ymoved].disp);
                        }
                    }
                    movePiece(xvalue, yvalue, xmoved, ymoved);
                    if (selected == 'P' && checkPromotionCondition(turn, xmoved, ymoved)) {
                        promotePawn(turn, xmoved, ymoved);}
                    turn = (turn == 'W') ? 'B' : 'W';
                    turnCount++;

                    break;
                }
                else
                    printf("Your king will be in check after this move!");
                    continue;

            } else {
                printf("Invalid move. Try again.\n");
                continue;
            }





        } while (1);
    }

    return 0;
}
