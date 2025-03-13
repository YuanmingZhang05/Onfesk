#include"benchmark.h"
Benchmark::Benchmark(int k, int m, double p, Onfesk::UpdateStrategy method):
	k_(k), m_(m), nerrors_(0), nprocessed_(0), p_(p), method_(method)
{
	classifier_ = new Onfesk(k, m);
}
Benchmark::~Benchmark()
{
	delete classifier_;
}
void Benchmark::evaluate(std::vector<std::vector<double>> &data, std::vector<std::vector<int>> &indices, std::vector<int> &labels)
{
	int n = data.size();
	nprocessed_ += n;
	for (int i = 0; i < n; i++)
	{
		std::vector<double> x_distorted;
		std::transform(data[i].begin(), data[i].end(), std::inserter(x_distorted, x_distorted.begin())
			, [=](double x_i) {return pow(x_i, p_); });
		std::vector<int> sketch = fastgm(k_, x_distorted, indices[i]);
		if (!classifier_->update(sketch, labels[i], method_))
			nerrors_ += 1;
	}
}