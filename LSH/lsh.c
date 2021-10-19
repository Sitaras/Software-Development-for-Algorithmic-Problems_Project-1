#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "../Vector/vector.h"
#include "../hashTable/hashTable.h"

#define SIGMA 1.00
#define MI 0.00


extern int w;
extern int d;
extern int k;
extern int hashTableSize;

int uniform_distribution(int rangeLow, int rangeHigh) {
    double myRand = rand()/(1.0 + RAND_MAX);
    int range = rangeHigh - rangeLow + 1;
    int myRand_scaled = (myRand * range) + rangeLow;
    return myRand_scaled;
}

double rand_gen() {
   // return a uniformly distributed random value
   return ( (double)(rand()) + 1. )/( (double)(RAND_MAX) + 1. );
}
double normalRandom() {
   // return a normally distributed random value
   double v1=rand_gen();
   double v2=rand_gen();
   return (cos(2*3.14*v2)*sqrt(-2.*log(v1)))*SIGMA+MI;
}

double dot_product(double *v, double *u){
    double result = 0.0;
    for (int i = 0; i < d; i++)
        result += v[i]*u[i];
    return result;
}

// double randn (){
//   double mu = 0;
//   double sigma = 1;
//   double U1, U2, W, mult;
//   static double X1, X2;
//   static int call = 0;
//
//   if (call == 1){
//       call = !call;
//       return (mu + sigma * (double) X2);
//     }
//   do{
//       U1 = -1 + ((double) rand () / RAND_MAX) * 2;
//       U2 = -1 + ((double) rand () / RAND_MAX) * 2;
//       W = pow (U1, 2) + pow (U2, 2);
//     }
//   while (W >= 1 || W == 0);
//
//   mult = sqrt ((-2 * log (W)) / W);
//   X1 = U1 * mult;
//   X2 = U2 * mult;
//
//   call = !call;
//
//   return (mu + sigma * (double) X1)/RAND_MAX;
// }

int mod(int a, int b){
  int r = a % b;
  return r < 0 ? r + b : r;
}

// int mod(int x, int y)
// {
//    int t = x - ((x / y) * y);
//    if (t < 0) t += y;
//    return t;
// }


typedef struct hfunc{
  double *v;
  double t;
}h_function;

typedef struct gfunc{
  h_function *h_functions;
  int *r;
  int m;
}g_function;

typedef struct lsh_n{
  g_function *g_fun;
  HashTable *hts;
  int l;
}lshNode;
typedef lshNode *LSH;

void generateH(h_function *hfun){
  hfun->v=malloc(d*sizeof(double));
  for(int i=0;i<d;i++){
    hfun->v[i] = normalRandom();
  }
  hfun->t=uniform_distribution(0,w);
}

void destroyH(h_function h){
  free(h.v);
}

int computeH(h_function hfun,Vector vector){
  double pv = dot_product(hfun.v,getCoords(vector));
  double temp = (double) (pv+hfun.t)/(double)w;
  return floor(temp);
}

void generateG(g_function *gfun){
  gfun->h_functions = malloc(k*sizeof(h_function));
  for(int i=0;i<k;i++){
     generateH(&gfun->h_functions[i]);
  }
  gfun->r = malloc(k*sizeof(int));
  for(int i=0;i<k;i++){
     gfun->r[i]=rand();

  }
  gfun->m=(UINT_MAX-4);
}

void destroyG(g_function g){
  for(int i=0;i<k;i++){
    destroyH(g.h_functions[i]);
  }
  free(g.h_functions);
  free(g.r);
}

int computeG(g_function gfun,Vector p){
  int sum = 0;
  for(int i=0;i<k;i++){
    sum += mod(gfun.r[i]*computeH(gfun.h_functions[i],p),gfun.m);
  }
  return mod(mod(sum,gfun.m),hashTableSize);
}

LSH initializeLSH(int l){
  LSH tempLSH = malloc(sizeof(lshNode));
  tempLSH->g_fun = malloc(l*sizeof(g_function));
  tempLSH->hts = malloc(l*sizeof(HashTable));
  for(int i=0;i<l;i++){
     generateG(&(tempLSH->g_fun[i]));
     tempLSH->hts[i] = htInitialize(hashTableSize);
  }
  tempLSH->l=l;
  return tempLSH;
}

void destroyLSH(LSH lsh){
  for(int i=0;i<lsh->l;i++){
     destroyG(lsh->g_fun[i]);
     htDelete(lsh->hts[i]);
  }
  free(lsh);
}
