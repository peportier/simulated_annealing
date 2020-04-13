#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#define D 0 /* debug */

struct Move { int src; int dst; };

static int N;           /* grid has N locations */
static int SSIDE;       /* grid's side has length SSIDE */
static int *B;          /* board */
static struct Move NEI[4] = {{0,0},{0,0},{0,0},{0,0}};
static int SNEI;        /* current config has SNEI neighbors */
static float T0;        /* initial temperature */
static float ALPHA;     /* T[n+1] = ALPHA * T[n] ; ALPHA < 0 */
static int MAXTIME; /* annealing stops when time exceeds MAXTIME */
static float BETA; /* with M[n] the time spent at temperature T[n] */
                   /* M[n+1] = BETA * M[n] ; BETA > 0              */
static float BETA0;     /* M[0] = BETA0 * MAXTIME */
static float MINTEMP;   /* smallest temperature attained */
static int MAXCOST;     /* max of min of costs at each temp */
static int MINCOST;     /* min of all costs encountered */
static char TITLE[100]; /* plot title */

int
cost() { /* manhattan heuristic */
  int d = 0;
  for(int i=0 ; i<N ; i++) 
    if(B[i]!=0) 
      d+=abs(i/SSIDE - B[i]/SSIDE)+abs(i%SSIDE - B[i]%SSIDE);
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
emptyLoc() {int i=0; for(;i<N;i++) if(B[i]==0) break; return i;}

void
buildNeighborhood() {
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
scanParam() {
  scanf("T0 = %f\n", &T0);
  scanf("ALPHA = %f\n", &ALPHA);
  scanf("BETA = %f\n", &BETA);
  scanf("BETA0 = %f\n", &BETA0);
  scanf("MAXTIME = %i\n", &MAXTIME);
}

int
scanConfig() {
  fgets(TITLE, 100, stdin); strtok(TITLE,"\n");
  int r = scanf("%i", &N);
  if (r == 0 || r == EOF) return 0;
  SSIDE = (int) floor( sqrt( (double) N ) );
  free(B);
  B = malloc(N*sizeof(int));
  for (int i=0 ; i<N ; i++) scanf("%i", &B[i]);
  scanf("\n"); /* ready to scan the next config line */
  return 1;
}

void
doMove(struct Move *mv) {
  int tmp = B[mv->src]; B[mv->src] = B[mv->dst]; B[mv->dst] = tmp;
}

double
rand01() { return rand()/(double)RAND_MAX; }

/* Simulated annealing. Evaluation of a neighbor. */
int
saStep(int curCost, float temp) {
  int newCost, deltaCost, rndMove;
  buildNeighborhood();
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
  FILE *fp = fopen("plotTmp", "w");
  int solutionFound=0;
  int curCost = cost();
  int bestCost = MAXCOST = MINCOST = curCost;
  float temp = MINTEMP = T0; 
  int elapsed = 0;
  /* m is aMount of time spent at current temp */
  int m = (int) floor(BETA0*MAXTIME); 
  int timer = m;
  fprintf(fp, "%.2f %i mt\n", temp, bestCost);
  while (elapsed<MAXTIME && !solutionFound) {
    bestCost = curCost;
    while (timer!=0) {
      curCost = saStep(curCost, temp);
      if(curCost == 0) {bestCost = curCost; solutionFound=1; break;}
      else if(curCost < bestCost) bestCost = curCost;
      timer -= 1;
    }
    elapsed += m;
    if(D){printf("At T=%f, elapsed: %i / %i, best: %i\n", 
                 temp, elapsed, MAXTIME, bestCost);}
    fprintf(fp, "%.2f %i lt\n", temp, bestCost);
    m = (int) floor(m*BETA); /* More time spent at lower temp */
    timer = m; MINTEMP = temp; temp *= ALPHA;
    if(bestCost > MAXCOST) MAXCOST = bestCost;
    if(bestCost < MINCOST) MINCOST = bestCost;
  }
  fclose(fp);
}

void
fcopy(FILE *s, FILE *d) {int c; while ((c=getc(s))!=EOF) putc(c,d);}

void
initPlot() {
  FILE *fpPlot = fopen("plot.ps", "w");
  FILE *fp = fopen("base.ps", "r"); fcopy(fp, fpPlot); fclose(fp);
  fclose(fpPlot);
}

void
plot() {
  FILE *fp = fopen("plot.ps", "a");
  int xmin = (int) floor(MINTEMP), ymin = MINCOST;
  int xmax = (int) floor(T0), ymax = MAXCOST;
  fprintf(fp, "initScaleAndFont\n");
  fprintf(fp, "newpath\n");
  fprintf(fp, "/xmin %i def\n", xmin);
  fprintf(fp, "/ymin %i def\n", ymin);
  fprintf(fp, "/xmax %i def\n", xmax);
  fprintf(fp, "/ymax %i def\n", ymax);
  fprintf(fp, "drawFrame\n");
  fprintf(fp, "(Cost) ylabel\n");
  fprintf(fp, "(Temperature) xlabel\n");
  for (int i=xmin;i<xmax;i+=5) fprintf(fp, "%i xtick\n", i);
  for (int i=ymin;i<ymax;i+=5) fprintf(fp, "%i ytick\n", i);
  fprintf(fp, "(%s) title\n", TITLE);
  fprintf(fp, "stroke\n");
  FILE *fp2 = fopen("plotTmp", "r"); fcopy(fp2, fp); fclose(fp2);
  remove("plotTmp");
  fprintf(fp, "stroke\nshowpage\n");
  fclose(fp);
}

int
main() {
  srand(time(0)); scanParam(); initPlot();
  while (scanConfig()) {sa(); plot();}
  exit(0);
}
