#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <memory.h>
#include <time.h>
#include <unistd.h>

// data structure for the board
struct board {
  int size;// boards are square
  int seed;
  int *cells;// pointer to row-major array of cells 
};

// create a new empty board with specified parameters
struct board *board_create(int size, int seed) {
  struct board *ret = (struct board*)malloc(sizeof(struct board));
  ret->size=size;
  ret->cells=(int*)calloc(size*size,sizeof(int));
  ret->seed=seed;
  return ret;
}

// retrieve value of cell X,Y. no bounds checking
int board_get(struct board *board, int x, int y) {
  assert((unsigned)(x)<(unsigned)(board->size));
  assert((unsigned)(y)<(unsigned)(board->size));
  return board->cells[y * board->size + x];
}

// Set cell X,Y to VAL. no bounds checking
void board_set(struct board *board, int x, int y, int val) {
  assert((unsigned)(x)<(unsigned)(board->size));
  assert((unsigned)(y)<(unsigned)(board->size));
  board->cells[y * board->size + x] = val;
}

// Generate a new board:
// Start with a standard configuration; perform shuffles; erase cells
void board_generate(struct board *board) {
  srand(board->seed);
  /* initial configuration */
  for (int y=0; y<board->size; y++) {
    for (int x=0; x<board->size; x++) {
      board_set(board,x,y,y * board->size + x + 1);
    }
  }
  /* swap rows, columns, labels */
  int ncell=board->size * board->size;
  for (int iter=0; iter<ncell * 1000; iter++) {
    int dig1=rand() % ncell;
    int dig2=rand() % ncell;
    int t = board->cells[dig1];
    board->cells[dig1] = board->cells[dig2];
    board->cells[dig2] = t;
  }
  // erase 1
  int i=rand() % board->size;
  int j=rand() % board->size;
  if (board_get(board,i,j)!=0) {
    board_set(board,i,j,0);
  }
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
  float rightedge=595;/*points*/
  float topedge=842;
  float margin=25;
  float xscale=(rightedge-margin*2)/(size+1.f);
  float yscale=(topedge-margin*4)/(size+2.f);// why 4??
  float scale=xscale < yscale ? xscale : yscale;
  float fontsize = size >= 60 ? 0.2f : size>=30 ? 0.4f : 0.55f;
  printf("%%!PS-Adobe-3.0\n");
  printf("<< /PageSize [%f %f] >> setpagedevice\n",rightedge,topedge);
  printf("%f %f translate\n",margin,margin*3); // why 3??
  printf("%f %f scale\n",scale,scale);
  printf("/Times-Roman findfont %f scalefont setfont\n",fontsize);
  float boldsize=1.f / scale;// 1 point
  printf("0 0 setlinewidth\n");
  /* print the grid */
  for (int x=0; x<=size; x++) {
    ps_lineto(x,0, x,size);
  }
  for (int y=0; y<=size; y++) {
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
  printf("0 %f moveto (missingnum  size:%d  seed:%d) show\n",board->size+0.3f, board->size, board->seed);
  printf("showpage\n");
}

// main function
// pipe output to lp to print!  
int main(int argc, char **argv) {
  int size =argc >1 ? atoi(argv[1]) : 10;
  int seed  =argc >2 ? atoi(argv[2]) : time(0);
  struct board *board = board_create(size,seed);
  board_generate(board);
  board_ps(board);
  return 0;
}

