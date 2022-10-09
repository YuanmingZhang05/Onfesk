#include<iostream>
#include<cmath>
#include"onfesk.h"
class OnfeskClassifier
{
public:
	enum class SketchType { FastGM, ICWS };
	int k_, m_, nprocessed_, nerrors_;
	double p_;
	Onfesk::UpdateStrategy method_;
	Onfesk *classifier_;
	SketchType sketch_type_;
	OnfeskClassifier(int k, int m, double p, Onfesk::UpdateStrategy method, SketchType sketch_type);
	~OnfeskClassifier();
	void evaluate(std::vector<std::vector<double>> &data, std::vector<std::vector<int>> &indices, std::vector<int> &labels);
};
