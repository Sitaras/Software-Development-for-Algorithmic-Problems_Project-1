#ifndef LSH_H
#define LSH_H


int uniform_distribution(int , int );
double rand_gen();
double normalRandom();
double dot_product(double *, double *);
int mod(int , int );
void generateH(h_function *);
int computeH(h_function ,Vector );
void generateG(g_function *);
int computeG(g_function ,Vector );
LSH initializeLSH(int );
#endif
