#include"train.h"
OnfeskClassifier::OnfeskClassifier(int k, int m, double p, Onfesk::UpdateStrategy method, SketchType sketch_type):
	k_(k), m_(m), nerrors_(0), nprocessed_(0), p_(p), method_(method), sketch_type_(sketch_type)
{
	classifier_ = new Onfesk(k, m);
}
OnfeskClassifier::~OnfeskClassifier()
{
	delete classifier_;
}
void OnfeskClassifier::evaluate(std::vector<std::vector<double>> &data, std::vector<std::vector<int>> &indices, std::vector<int> &labels)
{
	int n = data.size();
	nprocessed_ += n;
	for (int i = 0; i < n; i++)
	{
		std::vector<double> x_distorted;
		std::transform(data[i].begin(), data[i].end(), std::inserter(x_distorted, x_distorted.begin())
			, [=](double x_i) {return pow(x_i, p_); });
		std::vector<int> sketch;
		switch (sketch_type_)
		{
		case SketchType::FastGM:
			sketch = fastgm(k_, x_distorted, indices[i]);
			break;
		case SketchType::ICWS:
			sketch = ICWS(k_, x_distorted, indices[i]);
			break;
		}
		if (!classifier_->update(sketch, labels[i], method_))
			nerrors_ += 1;
	}
}