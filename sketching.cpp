#include"sketching.h"
/*
ICWS (Improved Consistent Weighted Sampling) method is considered as the state-of-the-art CWS algorithm.
We apply it to produce the GMM kernel's feature sketch.
*/
std::vector<int> ICWS(int k, std::vector<double> v, std::vector<int> indices)
{
	int n = v.size();
	std::vector<int> sketch(k);
	for (int j = 0; j < k; j++)
	{
		std::vector<double> a(n, 0.);
		for (int i = 0; i < n; i++)
		{
			double r = gamma21_hash((indices[i] << 1), j), c = gamma21_hash((indices[i] << 1) + 1, j), be = uniform_hash(indices[i], j),
				t = floor(log(v[i]) / r + be), y = exp(r * (t - be));
			a[i] = c / (y * exp(r));
		}
		sketch[j] = indices[std::min_element(a.begin(), a.end()) - a.begin()];
	}
	return sketch;
}

/*
We use the FastGM method to generate our GSM kernel's feature sketch.
Please refer to its original paper for more details.
Paper link: https://arxiv.org/abs/2302.05176
Code  link: https://github.com/YuanmingZhang05/FastGM
*/
std::vector<int> fastgm(int k, std::vector<double> v, std::vector<int> indices)
{
	int n = v.size(), R_scale = 0, n_empty = k;
	std::vector<int> R(n), n_generated(n, 0), s(k, -1);
	std::vector<std::vector<int>> position_map(n, std::vector<int>(k));
	std::iota(position_map[0].begin(), position_map[0].end(), 0);
	for (auto& m : position_map)
		m = position_map[0];
	std::vector<double> h(n, 0.), y(k, DBL_MAX);
	double sum_v = std::accumulate(v.begin(), v.end(), 0.);
	while (n_empty > 0)
	{
		R_scale += k;
		for (int i = 0; i < n; i++)
		{
			R[i] = ceil(R_scale * v[i] / sum_v);
			while (n_generated[i] < std::min(R[i], k))
			{
				int j = uniformint_hash(indices[i], n_generated[i], n_generated[i], k), pos = position_map[i][j];
				h[i] += exponential_hash(indices[i], n_generated[i]) / (v[i] * (k - n_generated[i]));
				std::swap(position_map[i][j], position_map[i][n_generated[i]]);
				n_generated[i] += 1;
				if (s[pos] < 0)
				{
					y[pos] = h[i];
					s[pos] = indices[i];
					n_empty -= 1;
				}
				else if (h[i] < y[pos])
				{
					y[pos] = h[i];
					s[pos] = indices[i];
				}
			}
		}
	}
	int argmax_y = std::max_element(y.begin(), y.end()) - y.begin();
	for (int i = 0; i < n; i++)
	{
		while (n_generated[i] < k)
		{
			int j = uniformint_hash(indices[i], n_generated[i], n_generated[i], k), pos = position_map[i][j];
			h[i] += exponential_hash(indices[i], n_generated[i]) / (v[i] * (k - n_generated[i]));
			std::swap(position_map[i][j], position_map[i][n_generated[i]]);
			n_generated[i] += 1;
			if (h[i] > y[argmax_y])
				break;
			if (h[i] < y[pos])
			{
				y[pos] = h[i];
				s[pos] = indices[i];
				if (pos == argmax_y)
					argmax_y = std::max_element(y.begin(), y.end()) - y.begin();
			}
		}
	}
	return s;
}
