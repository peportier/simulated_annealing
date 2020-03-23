#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

struct Move { int src; int dst; };

static int N; /* the grid has N locations */
static int SSIDE; /* grid's side has length SSIDE */
static int *B; /* board */
static struct Move NEI[4] = {{0,0},{0,0},{0,0},{0,0}};
static int SNEI; /* current config has SNEI neighbors */
static int COST; /* 0: misplaced tiles ; 1: manhattan */
static float T0; /* initial temperature */
static float ALPHA; /* T[n+1] = ALPHA * T[n] ; ALPHA < 0 */
static float BETA; /* with M[n] the time spent at temperature T[n] */
                   /* M[n+1] = BETA * M[n] ; BETA > 0              */
static float BETA0; /* M[0] = BETA0 * MAXTIME */
static int MAXTIME; /* annealing stops when time exceeds MAXTIME */
static int D = 1; /* debug */

int
cost() {
  int d = 0;
  if(COST==0){
    for(int i=0 ; i<N ; i++) if(B[i]!=0 && B[i]!=i) d++;
  } else
  if(COST==1){
    for(int i=0 ; i<N ; i++) 
      if(B[i]!=0) 
        d+=abs(i/SSIDE - B[i]/SSIDE)+abs(i%SSIDE - B[i]%SSIDE);
  }
  return d;
}

void
printConfig() {
  printf("cost=%i\n", cost());
  for(int i=0;i<SSIDE;i++) {
    for(int j=0;j<SSIDE;j++) printf(" %2d |", B[i*SSIDE+j]);
    printf("\n");
  }
}

int
emptyLoc() {
  int i = 0;
  for( ; i < N ; i++ ) if( B[i] == 0 ) break;
  return i;
}

void
getMoves() {
  int z = emptyLoc();
  SNEI = 0;
  /* empty space not on the last column */
  if( (z+1) < N && (z+1) % SSIDE != 0 ) {
    NEI[SNEI].src = z; NEI[SNEI].dst = z+1; SNEI++;
  }
  /* empty space not on the first column */
  if( (z-1) >= 0 && (z-1) % SSIDE != (SSIDE-1) ) {
    NEI[SNEI].src = z; NEI[SNEI].dst = z-1; SNEI++;
  }
  /* empty space not on the last row */
  if( (z+SSIDE) < N ) {
    NEI[SNEI].src = z; NEI[SNEI].dst = z+SSIDE; SNEI++;
  }
  /* empty space not on the first row */
  if( (z-SSIDE) >= 0 ) {
    NEI[SNEI].src = z; NEI[SNEI].dst = z-SSIDE; SNEI++;
  }
}

void
printMove(int i) { printf("{%i,%i},", NEI[i].src, NEI[i].dst); }

void
printMoves() {
  printf("{");
  for( int i = 0 ; i < SNEI ; i++ ) printMove(i);
  printf("}\n");
}

void
scanParam() {
  scanf("COST = %i", &COST);
  scanf(" T0 = %f", &T0);
  scanf(" ALPHA = %f", &ALPHA);
  scanf(" BETA = %f", &BETA);
  scanf(" BETA0 = %f", &BETA0);
  scanf(" MAXTIME = %i", &MAXTIME);
}

int
scanConfig() {
  int r = scanf("%i", &N);
  if (r == 0 || r == EOF) return 0;
  SSIDE = (int) floor( sqrt( (double) N ) );
  free(B);
  B = malloc(N*sizeof(int));
  for (int i=0 ; i<N ; i++) scanf("%i", &B[i]);
  return 1;
}

void
doMove(struct Move *mv) {
  int tmp;
  tmp = B[mv->src]; B[mv->src] = B[mv->dst]; B[mv->dst] = tmp;
}

double
rand01() { return rand()/(double)RAND_MAX; }

/* Simulated annealing. Evaluation of a neighbor. */
int
saStep(int curCost, float temp) {
  int newCost, deltaCost, rndMove;
  getMoves();
  rndMove = rand() % SNEI;
  doMove(&NEI[rndMove]);
  newCost = cost();
  deltaCost = newCost - curCost; 
  if (deltaCost < 0) {
    curCost = newCost;
  } else {
    if (rand01() < exp(- deltaCost / temp)) {
      curCost = newCost;
    } else {
      doMove(&NEI[rndMove]); /* undo move */
    }
  }
  return curCost;
}

/* Simulated annealing */
void
sa() {
  int curCost = cost();
  int bestCost = curCost;
  float temp = T0;
  int elapsed = 0;
  /* m is aMount of time spent at current temp */
  int m = (int) floor(BETA0*MAXTIME); 
  int timer = m;
  while (elapsed<MAXTIME) {
    bestCost = curCost;
    while (timer!=0) {
      curCost = saStep(curCost, temp);
      if(curCost == 0) return;
      if(curCost < bestCost) bestCost = curCost;
      timer -= 1;
    }
    elapsed += m;
    m = (int) floor(m*BETA);
    timer = m;
    temp *= ALPHA;
    if(D){printf("At T=%f, elapsed: %i / %i, best: %i\n", 
                 temp, elapsed, MAXTIME, bestCost);}
  }
}

int
main()
{
  srand(time(0));
  scanParam();
  while (scanConfig()) {sa(); printConfig();}
  exit(0);
}
