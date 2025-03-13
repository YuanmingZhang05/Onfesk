#include<iostream>
#include<cmath>
#include"onfesk.h"
class Benchmark
{
public:
	int k_, m_, nprocessed_, nerrors_;
	double p_;
	Onfesk::UpdateStrategy method_;
	Onfesk *classifier_;
	Benchmark(int k, int m, double p, Onfesk::UpdateStrategy method);
	~Benchmark();
	void evaluate(std::vector<std::vector<double>> &data, std::vector<std::vector<int>> &indices, std::vector<int> &labels);
};
