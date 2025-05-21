#include<vector>
#include<functional>
#include<numeric>
#include<mkl.h>
#include<algorithm>
#include<cmath>
#include<cfloat>
#include"hashing.h"
std::vector<int> fastgm(int k, std::vector<double> v, std::vector<int> indices); \\ feature sketch generation method of GSM kernel
std::vector<int> ICWS(int k, std::vector<double> v, std::vector<int> indices); \\ feature sketch generation method of GMM kernel
