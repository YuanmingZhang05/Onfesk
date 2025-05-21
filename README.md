Onfesk_code.cpp is a version that can be compiled independently.
The rest are the codes used in all experiments. 

# Building Accurate and Interpretable Online Classifiers on Edge Devices

This repository contains our implementation of Onfesk, which builds provides the intersection cardinality estimation of static and streaming sets respectively, while satisfying Local Differential Privacy (LDP). 
Onfesk must be built with the Intel(R) oneMKL library (available at https://www.intel.com/content/www/us/en/developer/tools/oneapi/onemkl.html).
Both of our SetXor and SetXorDyn construct a compact bit matrix and performs bitwise XOR operations when adding elements or merging sketches. Additionally, they incorporate a random response mechanism to satisfy Differential Privacy (DP) and estimates the intersection cardinality through the estimation of the difference set cardinality. Moreover, we conducted extensive experiments to compare the performance of our SetXor with the method [SFM](https://arxiv.org/pdf/2302.02056.pdf) and the state-of-the-art method [LL](https://research.google/pubs/pub49177/). 

### Datasets
Our experiments encompass five real-world datasets, including four non-image datasets and the webpage visits dataset in the static case, and the network traffic datasets and the BitcoinHeist dataset in the streaming case, to evaluate different methods. For the synthetic datasets, we randomly generate unique data from a 32-bit integer space, which is used to construct both static and streaming datasets. As for the network origin-destination traffic dataset, we utilize the AbileneTM dataset from the first week, which can be accessed through the following source: 
```url
https://www.cs.utexas.edu/~yzhang/research/AbileneTM/
```
For the webpage visits dataset, we estimate the common users who visit two different categories’ webpage, which can be referred through: 
```url
https://doi.org/10.24432/C5390X
```
And for the BitcoinHeist dataset, we construct sketches to record the Bitcoin transaction records, which can be obtained through:
```url
https://archive.ics.uci.edu/dataset/526/bitcoinheistransomwareaddressdataset
```
For the network traffic dataset, we utilize network flows captured from July 3 to July 7, 2017, which can be referred through: 
```url
https://www.unb.ca/cic/datasets/ids-2017.html
```

### Methods implemented
|   Method   |                         Description                          |               Reference                |
| :--------: | :----------------------------------------------------------: | :------------------------------------: |
|   SetXor   |                  the original SetXor sketch                  |         [setxor.py](setxor.py)         |
| SetXor-IVW | improved method for SetXor, providing more accurate estimation |         [setxor.py](setxor.py)         |
|   SetXorDyn   |                  the original SetXorDyn sketch                  |         [setxor_Dyn.py](setxor_Dyn.py)         |
| SetXorDyn-IVW | improved method for SetXorDyn, providing more accurate estimation |         [setxor_Dyn.py](setxor_Dyn.py)         |
|  [SFM-Sym](https://arxiv.org/abs/2302.02056)   |                SFM with deterministic merging                | [sfm.py](./baseline/sfm.py) |
|  [SFM-Xor](https://arxiv.org/abs/2302.02056)   |                   SFM with random merging                    | [sfm.py](./baseline/sfm.py) |
|    [HLL](https://dmtcs.episciences.org/3545/pdf)     |                    the HyperLogLog sketch                    | [hll.py](./baseline/hll.py) |
|     [FM](https://www.sciencedirect.com/science/article/pii/0022000085900418)     |                  the Flajolet-Martin sketch                  |  [fm.py](./baseline/fm.py)  |
|     [CL](https://research.google/pubs/pub49177/)     |                  the CascadingLegions                  |  [cl.py](./baseline/cl.py)  |
|     [LL](https://research.google/pubs/pub49177/)     |                  the LiquidLegions                  |  [ll.py](./baseline/ll.py)  |
