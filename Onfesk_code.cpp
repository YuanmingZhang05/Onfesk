#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <chrono>
#include <bits/stdc++.h>
#include <cstdlib>
#include <cstdint>
#include <climits>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <math.h>
#include <numeric>
#include <sys/time.h>
#include <time.h>


using  namespace std;
void tic(uint64_t& s);
uint64_t toc(uint64_t s);
vector<default_random_engine> generator_vector;
float mean(const std::vector<float>& buf);
float median(std::vector<float>& buf);

struct NextBall{
    double x_i;
    int z_i;
    int m_i;
    int c;
    vector<int> pi_i;
};


typedef struct SparseExample {
  int32_t label;
  std::vector<std::pair<uint32_t, float> > features;
} SparseExample;

class SparseDataset {
 private:
  std::mt19937 prng_;

 public:
  uint32_t num_classes;
  uint32_t feature_dim;
  std::vector<SparseExample> examples;
  typedef typename std::vector<SparseExample>::const_iterator const_iterator;

  SparseDataset();
  SparseDataset(int32_t seed);
  ~SparseDataset();
  void seed(int32_t seed);
  uint32_t num_examples();
  const SparseExample& sample();
  const_iterator begin() const;
  const_iterator end() const;
};

SparseDataset read_libsvm(std::string& file_path);

float mean(const std::vector<float>& buf) {
  size_t n = buf.size();
  return std::accumulate(buf.begin(), buf.end(), 0.) / n;
}

float median(std::vector<float>& buf) {
  size_t n = buf.size();
  std::nth_element(buf.begin(), buf.begin() + n/2, buf.end());
  if (n % 2 == 1) return buf[n/2];
  std::nth_element(buf.begin(), buf.begin() + n/2 - 1, buf.begin() + n/2);
  return (buf[n/2 - 1] + buf[n/2]) / 2;
}

class HashFunction {
 public:
  virtual ~HashFunction() = default;
  virtual void hash(uint32_t* out, uint32_t x) = 0;
};

// tabulation hashing
static const size_t THASH_CHUNK_BITS = 8;
static const size_t THASH_NUM_CHUNKS = 32 / THASH_CHUNK_BITS;
static const size_t THASH_CHUNK_CARD = 1 << THASH_CHUNK_BITS;

class TabulationHash : public HashFunction {
 private:
  uint32_t** table_;
  uint32_t copies_;

 public:
  TabulationHash(uint32_t copies, int32_t seed);
  ~TabulationHash() override;
  void hash(uint32_t* out, uint32_t x) override;
};



class CountSketch {

 public:
  static const uint32_t MAX_LOG2_WIDTH = 31;

 private:
  const uint32_t depth_;
  const uint32_t input_width_;
  uint32_t width_mask_;
  float** weights_;
  TabulationHash hash_fn_;
  std::vector<uint32_t> hash_buf_;
  std::vector<uint32_t> hash_buf_fa_;
  std::vector<uint32_t> hash_buf_fb_;
  std::vector<float> weight_buf_;
  uint64_t t_ = 0;
  float bias_ = 0;

 public:
  CountSketch(uint32_t log2_width, uint32_t depth, int32_t seed);
  ~CountSketch();
  float get(uint32_t key, uint32_t feature_id );
  float get_mean(uint32_t key, uint32_t feature_id );
  float mypredict(uint32_t key, vector<int> sketch);
  void myupdate(uint32_t key, vector<int> sketch, int label);
  float get_line_weights(uint32_t line_num, uint32_t col_num);

};


// tabulation hashing
TabulationHash::TabulationHash(uint32_t copies, int32_t seed)
 : copies_{copies} {
  table_ = (uint32_t**) calloc(THASH_NUM_CHUNKS, sizeof(uint32_t*));
  table_[0] = (uint32_t*) calloc(THASH_NUM_CHUNKS * THASH_CHUNK_CARD * copies_, sizeof(uint32_t));
  std::mt19937 prng(seed);
  for (int i = 0; i < THASH_NUM_CHUNKS; i++) {
    table_[i] = table_[0] + i * THASH_CHUNK_CARD * copies_;
    for (int j = 0; j < THASH_CHUNK_CARD * copies_; j++) {
      table_[i][j] = prng();
    }
  }
}

TabulationHash::~TabulationHash() {
  free(table_[0]);
  free(table_);
}

void TabulationHash::hash(uint32_t* out, uint32_t x) {
  memset(out, 0, copies_ * sizeof(uint32_t));
  for (int i = 0; i < THASH_NUM_CHUNKS; i++) {
    uint32_t c = (x >> (i * THASH_CHUNK_BITS)) & (THASH_CHUNK_CARD - 1);
    uint32_t *hashes = table_[i] + c * copies_;
    for (int j = 0; j < copies_; j++) {
      out[j] ^= hashes[j];
    }
  }
}

CountSketch::CountSketch(
    uint32_t log2_width,
    uint32_t depth,
    int32_t seed)
 : depth_{depth}, input_width_{log2_width},
   hash_fn_(depth, seed),
   hash_buf_(depth, 0),
   hash_buf_fa_(depth, 0),
   hash_buf_fb_(depth, 0),
   weight_buf_(depth, 0) {

  if (log2_width > CountSketch::MAX_LOG2_WIDTH) {
    throw std::invalid_argument("Invalid sketch width");
  }

  uint32_t width = 1 << log2_width;
  width_mask_ = width - 1;

  weights_ = (float**) calloc(depth, sizeof(float*));
  weights_[0] = (float*) calloc(width * depth, sizeof(float));
  for (int i = 0; i < depth; i++) {
    weights_[i] = weights_[0] + i * width;
  }
}


CountSketch::~CountSketch() {
  free(weights_[0]);
  free(weights_);
}

float CountSketch::get(uint32_t key, uint32_t feature_id) {
  hash_fn_.hash(hash_buf_fa_.data(), key);
  hash_fn_.hash(hash_buf_fb_.data(), key+1);

  for (int i = 0; i < depth_; i++) {    
    uint32_t fa = hash_buf_fa_[i];
    uint32_t fb = hash_buf_fb_[i];
    uint32_t hash_val = fa* feature_id + fb;
    weight_buf_[i] = weights_[i][hash_val % width_mask_];
  }

  return mean(weight_buf_);
}

float CountSketch::get_mean(uint32_t key, uint32_t feature_id) {
  hash_fn_.hash(hash_buf_fa_.data(), key);
  hash_fn_.hash(hash_buf_fb_.data(), key+1);
  //hash_fn_.hash(hash_buf_.data(), key);

  for (int i = 0; i < depth_; i++) {    
    uint32_t fa = hash_buf_fa_[i];
    uint32_t fb = hash_buf_fb_[i];

    uint32_t hash_val = fa* feature_id + fb;
    //uint32_t h = hash_buf_[i];
    //int sgn = (h >> 31) ? +1 : -1;
    weight_buf_[i] = weights_[i][hash_val];
  }
  float weight_mean = mean(weight_buf_);
  float weight_diff = 0;
  for (int i = 0; i < depth_; i++) {    
      float dx =(float(weight_buf_[i]) - weight_mean);
      //cout << "****************************" << dx << endl;
      weight_diff = weight_diff + dx*dx;
  }
  //cout << "****************************" << weight_diff << endl;
  return weight_diff;

}

float CountSketch::mypredict(uint32_t key, vector<int> sketch) {
  hash_fn_.hash(hash_buf_fa_.data(), key);
  hash_fn_.hash(hash_buf_fb_.data(), key+1);
  float active_weight = 0;

  for (int i = 0; i < depth_; i++) {
    uint32_t fa = hash_buf_fa_[i];
    uint32_t fb = hash_buf_fb_[i];
    uint32_t hash_val = (fa* sketch[i] + fb);
    active_weight = active_weight + weights_[i][hash_val % width_mask_];
  }
   if (active_weight >= 1) return 1;
   else return 0;
}

void CountSketch::myupdate(uint32_t key, vector<int> sketch, int label) {
  hash_fn_.hash(hash_buf_fa_.data(), key);
  hash_fn_.hash(hash_buf_fb_.data(), key+1);
  

  for (int i = 0; i < depth_; i++) {
    uint32_t fa = hash_buf_fa_[i];
    uint32_t fb = hash_buf_fb_[i];
    uint32_t hash_val = fa* sketch[i] + fb;
    weights_[i][hash_val % width_mask_] += label * 1;
  }

}  

float CountSketch::get_line_weights(uint32_t line_num, uint32_t col_num){
  return weights_[line_num][col_num];
}




SparseDataset::SparseDataset()
 : prng_(std::chrono::system_clock::now().time_since_epoch().count()),
   num_classes{0},
   feature_dim{0} { }

SparseDataset::SparseDataset(int32_t seed)
 : prng_(seed),
   num_classes{0},
   feature_dim{0} { }

SparseDataset::~SparseDataset() { };

void SparseDataset::seed(int32_t seed) {
  prng_.seed(seed);
}

uint32_t SparseDataset::num_examples() {
  return examples.size();
}

const SparseExample& SparseDataset::sample() {
  uint32_t idx = prng_() % examples.size();
  return examples[idx];
}

SparseDataset::const_iterator SparseDataset::begin() const {
  return examples.begin();
}

SparseDataset::const_iterator SparseDataset::end() const {
  return examples.end();
}


SparseDataset read_libsvm(std::string& file_path) {
  SparseDataset dataset;
  std::ifstream ifs(file_path);
  std::string line;
  std::string delim = ":";
  std::set<int> classes;

  if (!ifs.is_open()) {
    throw std::runtime_error("Failed to read " + file_path);
  }
  //cout << file_path << endl;
  while (std::getline(ifs, line)) {
    std::istringstream ls(line);
    std::string label_str, pair_str;
    ls >> label_str;

    SparseExample example;
    example.label = std::stoi(label_str);
    if (example.label == -1) {
      example.label = 0;  // normalize -1/+1 to 0/1 for LIBSVM datasets
    }
    classes.insert(example.label);

    while (ls >> pair_str) {
      size_t pos = pair_str.find(delim);
      uint32_t k = (uint32_t) std::stol(pair_str.substr(0, pos));
      float v = std::stof(pair_str.substr(pos+1, std::string::npos));
      example.features.emplace_back(std::pair<uint32_t, float>(k, v));
      if (k >= dataset.feature_dim) dataset.feature_dim = k + 1;
    }
    dataset.examples.push_back(example);
  }

  dataset.num_classes = classes.size();
  return dataset;
}


void GetNextBalls(vector<NextBall> &vec, int i, int k, int round_num){
  
    default_random_engine generator = generator_vector[i];
//    getnextball_times = getnextball_times+1;
    double Q_k = k * 0.1;
    uniform_int_distribution<int> distribution1(0, RAND_MAX);
    double u = (distribution1(generator)+1) / double(RAND_MAX);
    //cout << i << " : " << u << endl;
    double x=0, z=0;
    int j=0;

    if(vec[i].m_i > Q_k){
    	//getnextball_hash_times=getnextball_hash_times+1;
        x = - log(u);
        j = distribution1(generator) % k;
        z = 1;
    }
    else{
    	//getnextball_gamma_times=getnextball_gamma_times+1;
        z =  floor(log(u)/log(1- double(vec[i].m_i)/k)) + 1;
        gamma_distribution <double> distribution2(z,1.0);
        x = distribution2(generator);
        j = distribution1(generator) % vec[i].m_i;
        //cout << "Permutation: " << j << ";" << i << ";" << z_i << ";" << vw_i << ";" << (i)+(z_i)+vw_i << endl;
    }
    if(j < vec[i].m_i){
        int temp = vec[i].pi_i[vec[i].m_i-1];
        vec[i].pi_i[vec[i].m_i-1] = vec[i].pi_i[j];
        vec[i].pi_i[j] = temp;
        vec[i].m_i -= 1;
    }

    vec[i].x_i += x;
    vec[i].z_i += z;
    vec[i].c = vec[i].pi_i[vec[i].m_i];
    generator_vector[i] = generator;
}

vector<int> BBM_Basic(const vector<double> data, const vector<int> pos,const uint32_t k, int roundnum){
    vector<double> yVec(k, -1);
    vector<int> Sketch(k, -1);

    for(int i=0; i<data.size(); i++){
        srand((pos[i]<<8) + roundnum);
        for (int j = 0; j < k; j++) {
            double u = rand() / double(RAND_MAX);
            double exp = -log(u) / data[i];
            if(yVec[j] == -1 or exp < yVec[j]) {
                yVec[j] = exp;
                Sketch[j] = pos[i];
            }
        }
    }
    return Sketch;
}

vector<int> FIPS( const vector<double> data, const vector<int> pos,const uint32_t k, int roundnum){
    long long int r = 0, k_star = k;
    vector<double> yVec(k, -1);
    vector<int> Sketch(k, -1);
    double v_sum;
    vector<NextBall> BallVec;
    vector<int> pi_ieach;
    NextBall Ball;

    for (int i=0;i<k;i++){
        pi_ieach.push_back(i);
    }

    for(uint32_t i=0;i<data.size();i++) {
        v_sum = v_sum + data[i];
        Ball.x_i = 0;
        Ball.z_i = 0;
        Ball.m_i = k;
        Ball.pi_i = pi_ieach;
        BallVec.push_back(Ball);
    }
//	int con = 0;
    while (k_star != 0) {
        r = r + k;
//        vector<int> r_i_list;

        for (int i=0;i<data.size();i++){
            double v_i = data[i];

            if(BallVec[i].m_i == k) {
                default_random_engine generator((pos[i] << 16) + roundnum);
                //cout << (i << 16) + roundnum << "\t" << generator << endl;
                generator_vector.push_back(generator);
            }
            if(v_i == 0) continue;
            uint64_t r_i = floor(r * v_i / v_sum);
//    	  k_star  = k_star -1;

            while(BallVec[i].m_i > 0 and BallVec[i].z_i <= r_i){

                GetNextBalls(BallVec, i, k, roundnum);

                double b_i = BallVec[i].x_i /(k*v_i);
                int c = BallVec[i].c;
                if (yVec[c] < 0 ){
                    yVec[c] = b_i;
                    k_star = k_star - 1;
                    Sketch[c] = pos[i];
                    continue;
                }
                if (b_i < yVec[c]){
                    yVec[c] = b_i;
                    Sketch[c] = pos[i];
                }
            }
        }
    }

    int j_star;
    double max = yVec[0];
    for (int i = 0; i < yVec.size(); i++) {
        if (max < yVec[i]){
            max = yVec[i];
            j_star = i;
        }
    }
    vector<int> N;
    int Nonempty = 0;
    for (int i = 0; i < data.size(); i++) {
        if(BallVec[i].m_i == 0) N.push_back(-1);
        else {
            N.push_back(1);
            Nonempty += 1;
        }
    }
        for(int i=0; i!=N.size();i++){
            if(N[i] == -1) continue;
            double v_i = data[i];

            while(BallVec[i].m_i > 0){
                GetNextBalls(BallVec, i, k, roundnum);
                int c = BallVec[i].c;

                double b_i = BallVec[i].x_i /(k*v_i);
                if (BallVec[i].m_i == 0 or yVec[j_star] < b_i){
                    N[i] = -1;
                    Nonempty--;
                    break;
                }
                if (b_i < yVec[c]){
                    yVec[c] = b_i;
                    Sketch[c] = pos[i];
                    if(c == j_star){
                        int max = yVec[0];
                        for (int j_i = 0; j_i < yVec.size(); j_i++) {
                            if (max < yVec[j_i]){
                                max = yVec[j_i];
                                j_star = j_i;
                            }
                        }
                    }
                }
            }

        }
    generator_vector.clear();
    return Sketch;
}


int main(int argc,char* argv[])
{
  clock_t startt,endt;
  SparseDataset train_dataset;
  string train_path;
  train_path =argv[1];
  train_dataset = read_libsvm(train_path);
  // train_path = ""; //dataset_path
  // train_dataset = read_libsvm(train_path);
  vector<int> tempX;
  vector<double> tempW;
  map<int,int> ID_Map;
  srand((int)time(0));
  int run_time = rand();
  
  string k_string=argv[2]; 
    int k= atoi(k_string.c_str());
  // const uint32_t k = 256;

  int32_t seed = 1;
  string w_string=argv[3]; 
  uint32_t log2_width = atoi(w_string.c_str()); 
  // uint32_t log2_width = 8;
  uint32_t train_err_count = 0;
  uint32_t train_count = 0;
  uint32_t depth = k;
  CountSketch model(log2_width,depth,seed);
  int32_t counter = 0;
  startt=clock();
  for (auto &ex : train_dataset) {
    for (auto &ele: ex.features){
    tempX.emplace_back(ele.first);
    //ID_Map[ele.first] = 0;
    tempW.emplace_back(fabsf(ele.second));
    }
    
    vector<int> Sketch_fips = FIPS(tempW,tempX,k,run_time);
    counter ++;
    float label_confidence;
    float line_weights;
    label_confidence = model.mypredict(run_time,Sketch_fips);
    train_count++;
    if (bool(label_confidence) != ex.label){
        train_err_count++;      
       if (ex.label == 0){model.myupdate(run_time,Sketch_fips, -1);}
       else {model.myupdate(run_time,Sketch_fips, 1);}       
    }
    tempW.clear();
    tempX.clear();
  }
endt=clock();
/*
  vector<int> importance_list;
  //float max_model_weight = model.get_max(run_time);
  //cout << "Max weight :"<< max_model_weight << endl;
  std::map < int, int > ::reverse_iterator iter;  
  for(iter = ID_Map.rbegin(); iter != ID_Map.rend(); iter++){
    	// cout<<iter->first<<"  "<<iter->second<<endl; 
      float feature_importance = model.get(run_time, iter->first);
      if(feature_importance != 0){
         cout << iter->first << "---"<< feature_importance << endl;
        //  cout << "Feature_ID:"<< feature_id << "Feature_Importance:"<< feature_importance << endl;
      }
  } 
*/

  double totalt=(double)(endt-startt)/CLOCKS_PER_SEC;
  double err=(double)train_err_count/train_count;
  double budget=(double)k*(1<<log2_width)/1024;
  cout << train_path<<","<<"train_pars: k--," << k <<","<<"log2_width,"<<log2_width<<","<<train_err_count << "," << train_count <<","<< "err_rate," << err<<","<< "budget," << budget<<","<<"total_time,"<<totalt<< "\r\n";


  return 0;
}