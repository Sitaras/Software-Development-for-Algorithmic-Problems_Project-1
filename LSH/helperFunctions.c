#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#define SIGMA 1.00
#define MI 0.00

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

double dot_product(double *v, double *u,int d){
    double result = 0.0;
    for (int i = 0; i < d; i++)
        result += v[i]*u[i];
    return result;
}

int mod(int a, int b){
  int r = a % b;
  return r < 0 ? r + b : r;
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

// int mod(int x, int y)
// {
//    int t = x - ((x / y) * y);
//    if (t < 0) t += y;
//    return t;
// }
