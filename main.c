#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <memory.h>
#include <time.h>
#include <unistd.h>

/* sudoku board generator 
*/

// data structure for the board
struct board {
  int size;// boards are square
  int seed;
  int nerase;// number of squares to be erased
  int xchunk;// size of each block in X direction (y direction is just size/xchunk) 
  int *cells;// pointer to row-major array of cells 
  int valid;// successfully generated?
};

int imin(int x, int y) {
  return x<y ? x:y;
}

// create a new empty board with specified parameters
struct board *board_create(int size, int xchunk, int nerase, int seed) {
  struct board *ret = (struct board*)malloc(sizeof(struct board));
  ret->size=size;
  ret->cells=(int*)calloc(size*size,sizeof(int));
  ret->xchunk=xchunk;
  ret->seed=seed;
  ret->valid=0;
  ret->nerase=nerase;
  return ret;
}

// delete a board
void board_delete(struct board *board) {
  free(board->cells);
  free(board);
}

// retrieve value of cell X,Y. no bounds checking
int board_get(struct board *board, int x, int y) {
  assert((unsigned)(x)<(unsigned)(board->size));
  assert((unsigned)(y)<(unsigned)(board->size));
  return board->cells[y * board->size + x];
}

// Calculate value of y chunk (height of each subblock)
int ychunk(struct board *board) {
  return board->size / board->xchunk;
}

int xchunk(struct board *board) {
  return board->xchunk;
}

// Set cell X,Y to VAL. no bounds checking
void board_set(struct board *board, int x, int y, int val) {
  assert((unsigned)(x)<(unsigned)(board->size));
  assert((unsigned)(y)<(unsigned)(board->size));
  board->cells[y * board->size + x] = val;
}

// Calculate whether storing VAL at location NEWX,NEWY is valid.
// Consider it valid if location is the same as X,Y. 
// If collision is found, set BADX,BADY to NEWX,NEWY
int collision(struct board *board, int x, int y, int val, int newx, int newy, int *badx, int *bady) {
  int ret=0;
  if (!(x==newx && y==newy) && board_get(board,newx,newy) == val) {
    *badx=newx;
    *bady=newy;
    ret=1;
  }
  return ret;
}

// Calculate whether it's valid to store VAL at location X,Y
// If not, populate BADX,BADY with coordinate of conflicting cell
int valid_val(struct board *board, int x, int y, int val, int *badx, int *bady) {
  int ret=1;
  int left= x - x%xchunk(board);
  int bot= y - y%ychunk(board);
  /* check horizontal */
  for (int i=0; i<board->size; i++) {
    if (collision(board, x,y,val, (x+i) % board->size, y, badx, bady)) {
      ret=0;
      break;
    }
  }
  /* check vertical */
  for (int i=0; i<board->size; i++) {
    if (collision(board,x,y,val, x, (y+i) % board->size,badx,bady)) {
      ret=0;
      break;
    }
  }
  /* check my square */
  for (int i=left; i<left+xchunk(board); i++) {
    for (int j=bot; j<bot+ychunk(board); j++) {
      if (collision(board,x,y,val,i,j,badx,bady)==val) {
	ret=0;
	break;
      }
    }
  }
  return ret;
}

// Swap two digits on a board (part of shuffling)
void board_swap_val(struct board *board, int dig1, int dig2) {
  for (int y=0; y<board->size; y++) {
    for (int x=0; x<board->size; x++) {
      int val=board_get(board,x,y);
      int newval=val==dig1 ? dig2
	: val==dig2 ? dig1
	: val;
      board_set(board,x,y,newval);
    }
  }
}

// Swap two columns
void board_swap_col(struct board *board, int x1, int x2) {
  for (int y=0; y<board->size; y++) {
    int val1=board_get(board,x1,y);
    int val2=board_get(board,x2,y);
    board_set(board,x1,y,val2);
    board_set(board,x2,y,val1);
  }
}

// Swap two rows
void board_swap_row(struct board *board, int y1, int y2) {
  for (int x=0; x<board->size; x++) {
    int val1=board_get(board,x,y1);
    int val2=board_get(board,x,y2);
    board_set(board,x,y1,val2);
    board_set(board,x,y2,val1);
  }
}

// Check if a board contains a valid Sudoku solution.
void board_check(struct board *board) {
  for (int x=0; x<board->size; x++) {
    for (int y=0; y<board->size; y++) {
      int badx,bady;
      if (!valid_val(board,x,y,board_get(board,x,y),&badx,&bady)) {
	board->valid=0;
	fprintf(stderr,"invalid board generated at %d %d same as %d %d\n",x,y,badx,bady);
      }
    }
  }
}

// Perform erasure of random cells
void board_erase(struct board *board) {
  int nerase=0;
  nerase=board->nerase;
  if (nerase > board->size*board->size) {
    nerase = board->size*board->size;
  }
  while (nerase>0) {
    int i=rand() % board->size;
    int j=rand() % board->size;
    if (board_get(board,i,j)!=0) {
      board_set(board,i,j,0);
      nerase--;
    }
  }
}

// Generate a new board:
// Start with a standard configuration; perform shuffles; erase cells
void board_generate(struct board *board) {
  board->valid=1;
  srand(board->seed);
  /* initial configuration */
  for (int y=0; y<board->size; y++) {
    int start = (y / ychunk(board)) + xchunk(board) * y;
    for (int x=0; x<board->size; x++) {
      board_set(board,x,y,(start+x) % board->size + 1);
    }
  }
  /* swap rows, columns, labels */
  for (int iter=0; iter<board->size * board->size * 10; iter++) {
    int dig1=rand() % board->size;
    int dig2=rand() % board->size;
    switch(rand() % 3) {
    case 0: board_swap_val(board,dig1+1,dig2+1); break;
    case 1: if (dig1/ychunk(board) == dig2/ychunk(board)) board_swap_row(board,dig1,dig2); break;
    case 2: if (dig1%xchunk(board) == dig2%xchunk(board)) board_swap_col(board,dig1,dig2); break;
    }
  }
  board_check(board);
  board_erase(board);
}

// Draw line from XFROM,YFROM to XTO,YTO
void ps_lineto(float xfrom, float yfrom, float xto, float yto) {
  printf("newpath %f %f moveto %f %f lineto closepath stroke\n"
	 ,xfrom,yfrom,xto,yto);
}

// Generate a PostScript program for showing BOARD
// on a single page
void board_ps(struct board *board) {
  int size = board->size;
  int xmod = xchunk(board);
  int ymod = ychunk(board);
  if (xmod==1) xmod=ymod;
  float rightedge=595;/*points*/
  float topedge=842;
  float margin=25;
  float xscale=(rightedge-margin*2)/(size+1.f);
  float yscale=(topedge-margin*4)/(size+2.f);// why 4??
  float scale=xscale < yscale ? xscale : yscale;
  float fontsize = 0.55f;
  printf("%%!PS-Adobe-3.0\n");
  printf("<< /PageSize [%f %f] >> setpagedevice\n",rightedge,topedge);
  printf("%f %f translate\n",margin,margin*3); // why 3??
  printf("%f %f scale\n",scale,scale);
  printf("/Times-Roman findfont %f scalefont setfont\n",fontsize);
  float boldsize=1.f / scale;// 1 point
  /* print the grid */
  for (int x=0; x<=size; x++) {
    float b=(x % xmod)==0 ? boldsize : 0.f;
    printf("%f %f setlinewidth\n",b,b);
    ps_lineto(x,0, x,size);
  }

  for (int y=0; y<=size; y++) {
    float b=(y % ymod)==0 ? boldsize : 0.f;
    printf("%f %f setlinewidth\n",b,b);
    ps_lineto(0,y, size, y);
  }

  for (int x=0; x<board->size; x++) {
    for (int y=0; y<board->size; y++) {
      int val=board_get(board,x,y);
      if (val) { /* 0 = don't print */
	printf("%f %f moveto (%d) dup stringwidth pop 2 div neg 0 rmoveto show\n", x+0.5f, y+0.3f, val);
      }
    }
  }
  // show definition in 12 point font
  printf("/Times-Roman findfont %f scalefont setfont\n",12.f/scale);
  printf("0 %f moveto (sumaker  size:%d  seed:%d  xchunk:%d  erase:%d) show\n"
	 ,board->size+0.3f, board->size, board->seed, xchunk(board), board->nerase);
  printf("showpage\n");
}

// main function
// pipe output to lp to print!  
int main(int argc, char **argv) {
  int size =argc >1 ? atoi(argv[1]) : 9;
  int xchunk=argc >2 ? atoi(argv[2]) : 3;
  int nerase=argc >3 ? atoi(argv[3]) : 20;
  int seed  =argc >4 ? atoi(argv[4]) : time(0);
  struct board *board = board_create(size,xchunk,nerase,seed);
  if (size % xchunk != 0) {
    fprintf(stderr,"bad xchunk specified\n");
    board->valid=0;
  }
  board_generate(board);
  if (board->valid) {
    board_ps(board);
  }
  return board->valid ? 0:1;
}

