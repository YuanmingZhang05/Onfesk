#include"onfesk.h"
Onfesk::Onfesk(int k, int m) 
	:k_(k), m_(m), 
	weights_(std::vector<std::vector<double>>(k, std::vector<double>(m, 0.)))
{
}
/* 
In what follows, we implement four different online update strategies, 
including the Percepton, the Passive-Aggressive (PA), the SVM, and the Logistic-Regression (LR).
*/
bool Onfesk::update(std::vector<int> s, int label, UpdateStrategy method)
{
	std::vector<int> proj(k_);
	for (int i = 0; i < k_; i++)
	{
		proj[i] = uniformint_hash(s[i], i, 0, m_);
	}
	double p = 0.;
	for (int i = 0; i < k_; i++)
	{
		p += weights_[i][proj[i]];
	}
	bool correct = std::signbit(p) == std::signbit((double)label);
	double loss, lr = 0.1, grad, weight_decay = 1e-5;
	switch (method)
	{
	case UpdateStrategy::Perceptron:
		if (!correct)
		{
			for (int i = 0; i < k_; i++)
			{
				weights_[i][proj[i]] += label;
			}

		}
		break;
	case UpdateStrategy::PA:
		loss = std::max(0., 1. - label * p); 
		lr = loss / k_;
		if (loss > 0.)
		{
			for (int i = 0; i < k_; i++)
			{
				weights_[i][proj[i]] += label * lr;
			}
		}
		break;
	case UpdateStrategy::SVM:
		loss = std::max(0., 1. - label * p);
		for (int i = 0; i < k_; i++)
			for (int j = 0; j < m_; j++)
				weights_[i][j] *= 1 - weight_decay * lr;
		if (loss > 0.)
		{
			for (int i = 0; i < k_; i++)
			{
				weights_[i][proj[i]] += label * lr;
			}
		}
		break;
	case UpdateStrategy::LR:		
		for (int i = 0; i < k_; i++)
			for (int j = 0; j < m_; j++)
				weights_[i][j] *= 1 - weight_decay * lr;
		grad = exp(-label * p) / (1 + exp(-label * p));
		for (int i = 0; i < k_; i++)
		{
			weights_[i][proj[i]] += label * grad * lr;
		}
	}
	return correct;
}
