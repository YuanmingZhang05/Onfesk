#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include "train.h"
/*
You may replace the "train.h" with "benchmark.h" for inputting parameters without modifying the codes.  
*/
const int batch_size = 10000;
int parse_svm(std::fstream &fin, std::vector<std::vector<double>> &data, std::vector<std::vector<int>> &indices, std::vector<int> &labels)
{
    std::string line;
    int n_samples = 0;
    int i = 0;
    while(i < batch_size)
    {
        if(std::getline(fin, line))
        {
            std::stringstream sline(line);
            std::vector<double> v;
            std::vector<int> indices_current;
            int cat, index, n_features = 0;
            double elem;
            sline >> cat;
            labels[i] = cat;
            while (sline >> index)
            {
                sline.get();
                sline >> elem;
                v.push_back(abs(elem));
                indices_current.push_back(index + 1);
                n_features++;
            }
            if (n_features)
            {
                data[i] = v;
                indices[i] = indices_current;
                i++;
            }
        }
        else
        {   
            data.resize(i);
            indices.resize(i);
            labels.resize(i);
            return i;
        }
    }
    return batch_size;
}
int main(int argc, char **argv)
{
    std::vector<std::vector<double>> data(batch_size);
    std::vector<std::vector<int>> indices(batch_size);
    std::vector<int> labels(batch_size);
    int k = 256, m = 32; //The sketch length k and the approximate one-hot representation length m.
    double p = 1.; //The tuning parameter for the kernel space.
    Onfesk::UpdateStrategy method = Onfesk::UpdateStrategy::PA;
    OnfeskClassifier::SketchType sketch_type = OnfeskClassifier::SketchType::FastGM;
    OnfeskClassifier classifier(k, m, p, method, sketch_type);
    std::fstream fin("input_file_dir", std::fstream::in);  //Replace “input_file_dir” with the correct file dir.
    while(parse_svm(fin, data, indices, labels) == batch_size)
        classifier.evaluate(data, indices, labels);
    classifier.evaluate(data, indices, labels);
    std::cout << (double)classifier.nerrors_ / classifier.nprocessed_;
}
