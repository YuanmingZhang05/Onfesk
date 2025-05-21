# Building Accurate and Interpretable Online Classifiers on Edge Devices

This repository contains our implementation of Onfesk, which builds accurate and interpretable online classifiers on edge devices, while providing model interpretability. 
Specifically, [Onfesk_code.cpp](./Onfesk_code.cpp) is a version that can be compiled independently.
The rest are the codes used in all experiments. 
The codes must be built with the Intel(R) oneMKL library, which can be obtained through:
```url
https://www.intel.com/content/www/us/en/developer/tools/oneapi/onemkl.html
```
Onfesk's random variables generation is based on [MurmurHash3](https://dl.acm.org/doi/abs/10.5555/3295222.3295407)

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

### Baseline Methods
On four non-image datasets, we use Linear methods, Kernel methods, and two MLP methods as baselines.
DL methods except for two MLP methods are designed for image dataset, thus, we only use them as baselines on MNIST dataset.
|   Method   |                         Description                          |               Reference                | Method Type                |
| :--------: | :----------------------------------------------------------: | :------------------------------------: | :--------: |
|   WM-Sketch|  WM-Sketch applies the Count-Sketch technique to condense the linear model’s weights.       |         [Code](https://github.com/stanford-futuredata/wmsketch) [Paper](https://arxiv.org/abs/1711.02305)                                            |Linear Method|
| AWM-Sketch | improved method for WM-Sketch, employing a heap to enhance the precision of the estimations |         [Code](https://github.com/stanford-futuredata/wmsketch) [Paper](https://arxiv.org/abs/1711.02305)                                            |Linear Method|
| Feature Hashing | Feature Hashing employs random projection to map high-dimensional vectors into a lower-dimensional space |        [Code](https://github.com/LIBOL/KOL)   [Paper](https://arxiv.org/abs/0902.2206)                                           |Kernel Method|
| Forgetron | Forgetron suits the memory budget by limiting the number of stored examples and discarding the rest. |         [Code](https://github.com/LIBOL/KOL) [Paper](https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/DekelShSi06.pdf)    |Kernel Method|
|  Projectron++   |  Projectron++ limits its memory usage by projecting stored examples into a new space.          | [Code](https://github.com/LIBOL/KOL) [Paper](https://icml.cc/Conferences/2008/papers/355.pdf)                                                |Kernel Method|
|  Ahpatron   |                   Ahpatron approximates the Perceptron with an aggressive and novel updating rule.            | [Code](https://github.com/alg4ml/Ahpatron) [Paper](https://arxiv.org/abs/2312.07032)                                              |Kernel Method|
|   POMDR     |                    POMDR integrates the optimistic mirror descent and the approximate linear dependence condition                   |  [Code](https://github.com/JunfLi-TJU/OKL-Hinge) [Paper](https://arxiv.org/abs/2212.12989)                 |Kernel Method|          
|  MLP-fp32   |       A standard multi-layer perceptron model learned in an online manner while using 32-bit floatingpoint precision for each neuron                  |                                                                                       |DL Method|
|  MLP-fp32   |                 A standard multi-layer perceptron model learned in an online manner while using 16-bit floatingpoint precision for each neuron                  |                                                                             |DL Method|
|     VGG-5   |                  This simplified yet effective CNN architecture|  [Paper](https://arxiv.org/abs/1409.1556)                                                                                                                                      |DL Method|
| MobileNets  |    MobileNets adopts the depthwise separable convolutions to reduce the memory cost of the VGG model| [Code](https://github.com/lyk125/MobileNet-1) [Paper](https://arxiv.org/abs/1704.04861)                                                     |DL Method|
| SpinalNet   |    SpinalNet divides inputs into several splits for each layer to reduce parameters in DNN models.| [Code](https://github.com/dipuk0506/SpinalNet) [Paper](https://arxiv.org/abs/2007.03347)                                                     |DL Method|
| EtinyNet   |EtinyNet is an extremely tiny CNN backbone for ML models on MCUs.  |[Code](https://github.com/aztc/SpinalNet) [Paper](https://ojs.aaai.org/index.php/AAAI/article/view/20387)                                                     |DL Method|


### Experiments on Real-world MCUs 
We also compare our Onfesk against DL baselines on the STM32 MCUs, 
where all experiments are conducted on ST Edge AI Developer Cloud which can be obtained through:
```url
https://stm32ai-cs.st.com/home
```
We evaluated the average inference time of each method for a data example across several real-world MCUs, including the [STM32G474](https://www.st.com/en/
microcontrollers-microprocessors/stm32g4x4.html),  [STM32F469](https://www.st.com/en/
microcontrollers-microprocessors/stm32l469-479.html), and [STM32L4R9](https://www.st.com/en/
microcontrollers-microprocessors/stm32l4r9-s9.html).
It is important to note that we omit the training time for each method due to the online setting. 
For Onfesk, the inference time for a data example exactly equals its processing time during the training stage. 
In contrast, the DL baselines require significantly more time and memory to process a data example during training because of the extra computation for the backward and intermediate activation,
often making on-device training infeasible. Specifically, a method’s average inference time is obtained by dividing the number of processed data examples into its total run time.

