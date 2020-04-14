#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#define D 0             /* debug */

static int N;           /* chessboard has N*N squares               */
static int SDIAG;       /* chessboard has SDIAG diagonals           */
static int *B;          /* B[i] is the row of the queen on col i    */
static int *D1;         /* main diagonals                           */
static int *D2;         /* anti diagonals                           */
static float T0;        /* initial temperature                      */
static float ALPHA;     /* T[n+1] = ALPHA * T[n] ; ALPHA < 0        */
static int MAXTIME;     /* annealing stops after at most MAXTIME    */
static float BETA;      /* M[n]= time spent at temperature T[n]     */
                        /* M[n+1]= BETA * M[n] ; BETA > 0           */
static float BETA0;     /* M[0] = BETA0 * MAXTIME                   */
static float MINTEMP;   /* smallest temperature attained            */
static int MAXCOST;     /* max of min of costs at each temperature  */
static int MINCOST;     /* min of all costs encountered             */
static char TITLE[100]; /* title of the plot                        */

int /* number of pairs of queens attacking each other */
cost() {
  int k, d = 0;
  for (int i=0;i<SDIAG;i++) {D1[i]=0; D2[i]=0;}
  for (int i=0;i<N;i++) {
    k = (i-B[i])+N-1; /* all cases of the main diagonal controlled by 
                         queen i have the same k                    */
    if (D1[k]==0) D1[k]++; else d++;
    k = (i+B[i]);     /* all cases of the anti diagonal controlled by 
                         queen i have the same k                    */
    if (D2[k]==0) D2[k]++; else d++;
  }
  return d;
}

void
printConfig() {
  for (int i=0;i<N;i++) printf("%i ", B[i]); printf("\n");
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
  SDIAG = 2*N-1;
  free(B);
  B = malloc(N*sizeof(int));
  for (int i=0 ; i<N ; i++) scanf("%i", &B[i]);
  scanf("\n"); /* ready to scan the next config line */
  free(D1); D1 = malloc(SDIAG*sizeof(int));
  free(D2); D2 = malloc(SDIAG*sizeof(int));
  return 1;
}

void /* swap rows of queens on columns i and j */
doMove(int i, int j) { int t = B[i]; B[i] = B[j]; B[j] = t; }

double /* random number between 0 and 1 */
rand01() { return rand()/(double)RAND_MAX; }


int /* Simulated annealing. Evaluation of a random neighbor. */
saStep(int curCost, float temp) {
  int newCost, deltaCost, rndMoveI, rndMoveJ;
  rndMoveJ = rndMoveI = rand() % N;
  while (rndMoveJ == rndMoveI) rndMoveJ = rand() % N;
  doMove(rndMoveI, rndMoveJ);
  newCost = cost();
  deltaCost = newCost - curCost; 
  if (deltaCost < 0) {
    curCost = newCost;
  } else {
    if (rand01() < exp(- deltaCost / temp)) {
      curCost = newCost;
    } else {
      doMove(rndMoveI, rndMoveJ); /* undo move */
    }
  }
  return curCost;
}


void /* simulated annealing */
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

void /* copy the content of file s in file d */
fcopy(FILE *s, FILE *d) {int c; while ((c=getc(s))!=EOF) putc(c,d);}

void /* load a prelude of postscript functions */
initPlot() {
  FILE *fpPlot = fopen("plot.ps", "w");
  FILE *fp = fopen("base.ps", "r"); fcopy(fp, fpPlot); fclose(fp);
  fclose(fpPlot);
}

void /* plot cost against temperature */
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
  while (scanConfig()) {sa(); plot(); printConfig();}
  exit(0);
}
