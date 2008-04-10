#include "mt19937ar.h"

int frand(int (*p)(int,int,int,int),int max,int m,int base) {
	int x=rand()%max,y=rand()%max;
	if(m<0 && x<=p(max,m,base,y))return x;
	if(m>0 && x>=p(max,m,base,y))return x;
	if(m==0)return x;
	return frand(p,max,m,base);
}

int slopeP(int max,int m,int base,int x) {
	return (int)(((10/(double)m)*(x-50)+base)/100*max);
}
