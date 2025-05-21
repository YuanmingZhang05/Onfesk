#include<vector>
#include<cmath>
#include"sketching.h"
class Onfesk
{
public:
	std::vector<std::vector<double>> weights_;
	enum class UpdateStrategy { Perceptron, PA, SVM, LR }; \\ online update methods
	Onfesk(int k, int m); \\ k is the sketch length and m is the approximate one-hot reprensentaion length
	bool update(std::vector<int> s, int label, UpdateStrategy method);
};
