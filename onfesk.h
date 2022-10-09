#include<vector>
#include<cmath>
#include"sketching.h"
class Onfesk
{
public:
	std::vector<std::vector<double>> weights_;
	enum class UpdateStrategy { Perceptron, PA, SVM, LR };
	int k_, m_;
	Onfesk(int k, int m);
	bool update(std::vector<int> s, int label, UpdateStrategy method);
};
