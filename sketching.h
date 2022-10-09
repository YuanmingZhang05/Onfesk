#include<vector>
#include<functional>
#include<numeric>
#include<mkl.h>
#include<algorithm>
#include<cmath>
#include<cfloat>
//#include<boost/math/special_functions/gamma.hpp>
#include"hashing.h"
std::vector<int> fastgm(int k, std::vector<double> v, std::vector<int> indices);
std::vector<int> ICWS(int k, std::vector<double> v, std::vector<int> indices);
//std::vector<int> fastgm_alpha(int k, double alpha, std::vector<double> v, std::vector<int> indices);
