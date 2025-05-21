# Building Accurate and Interpretable Online Classifiers on Edge Devices

This repository contains our implementation of Onfesk, which builds accurate and interpretable online classifiers on edge devices, while providing model interpretability. 
Specifically, Onfesk_code.cpp is a version that can be compiled independently.
The rest are the codes used in all experiments. 
The codes must be built with the Intel(R) oneMKL library, which can be obtained through:
```url
https://www.intel.com/content/www/us/en/developer/tools/oneapi/onemkl.html
```

### Datasets
Our experiments encompass five real-world datasets, including four non-image datasets (Webspam, URL, RCV1, Real-Sim) and one image dataset (MNIST). 
To suit the online setting, we select around 15, 000 examples with the largest length from each datasets and randomly shuffle them.
The Webspam dataset consists of web pages that were automatically collected from the Internet, which can be accessed through the following source: 
```url
https://www.csie.ntu.edu.tw/~cjlin/libsvmtools/datasets/binary.html#webspam
```
The URL dataset comprises both lexical and host-based features
associated with malicious and benign URLs, which can be referred through: 
```url
https://www.csie.ntu.edu.tw/~cjlin/libsvmtools/datasets/binary.html#url
```
The RCV1 dataset is a document-word collection derived from the Reuters Corpus, Volume 1, which can be obtained through:
```url
https://www.csie.ntu.edu.tw/~cjlin/libsvmtools/datasets/binary.html#rcv1.binary
```
The Real-Sim dataset consists of articles from various discussion groups (labeled ), which can be obtained through:
```url
https://www.csie.ntu.edu.tw/~cjlin/libsvmtools/datasets/binary.html#real-sim
```
The MNIST dataset contains 28 × 28 pixel images of handwritten digits, which can be referred through: 
```url
https://www.csie.ntu.edu.tw/~cjlin/libsvmtools/datasets/multiclass.html#mnist
```

### Methods implemented
|   Method   |                         Description                          |               Reference                |
| :--------: | :----------------------------------------------------------: | :------------------------------------: |
|   WM-Sketch|  WM-Sketch applies the Count-Sketch technique to condense the linear model’s weights.       |         [Code](https://github.com/stanford-futuredata/wmsketch) [Paper](https://arxiv.org/abs/1711.02305)                                            |
| AWM-Sketch | improved method for WM-Sketch, employing a heap to enhance the precision of the estimations |         [Code](https://github.com/stanford-futuredata/wmsketch) [Paper](https://arxiv.org/abs/1711.02305)                                            |
|   Feature Hashing | Feature Hashing employs random projection to map high-dimensional vectors into a lower-dimensional space |        [Code](https://github.com/LIBOL/KOL)   [Paper](https://arxiv.org/abs/0902.2206)                                           |
| Forgetron | Forgetron suits the memory budget by limiting the number of stored examples and discarding the rest. |         [Code](https://github.com/LIBOL/KOL) [Paper](https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/DekelShSi06.pdf)    |
|  Projectron++   |  Projectron++ limits its memory usage by projecting stored examples into a new space.          | [Code](https://github.com/LIBOL/KOL) [Paper](https://icml.cc/Conferences/2008/papers/355.pdf)|
|  Ahpatron   |                   Ahpatron approximates the Perceptron with an aggressive and novel updating rule by removing half of the
stored examples and preserving the informational essence
of the discarded examples via projection.                    | [Code](https://github.com/alg4ml/Ahpatron) [Paper](https://arxiv.org/abs/2312.07032) |
|    [HLL](https://dmtcs.episciences.org/3545/pdf)     |                    the HyperLogLog sketch                    | [hll.py](./baseline/hll.py) |
|     [FM](https://www.sciencedirect.com/science/article/pii/0022000085900418)     |                  the Flajolet-Martin sketch                  |  [fm.py](./baseline/fm.py)  |
|     [CL](https://research.google/pubs/pub49177/)     |                  the CascadingLegions                  |  [cl.py](./baseline/cl.py)  |
|     [LL](https://research.google/pubs/pub49177/)     |                  the LiquidLegions                  |  [ll.py](./baseline/ll.py)  |

We also 
```url
https://stm32ai-cs.st.com/home
```
