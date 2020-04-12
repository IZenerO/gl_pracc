#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include <ios>
#include <string>
#include <algorithm>

int mix_waves (std::vector<float> & vec1, std::vector<float> & vec2) {
  std::transform(vec1.begin(), vec1.end(), vec2.begin(), vec1.begin(), std::plus<float>());
  std::ofstream endf;
  endf.open("data.csv", std::ios::trunc | std::ios::out);
  for (size_t i = 0; i < vec1.size(); ++i) {
    endf << vec1[i] << std::endl;
  }
}

int generator (uint64_t m_amp, float m_baseFreq, uint64_t m_time_s, uint m_sampleRate, uint m_offset, uint16_t choice, std::ofstream &csv_file) {
  if (choice > 2 || choice < 0) {
    return 0;
  }
  bool flag = false;
  std::vector<float> temp {};
  std::fstream fin ("data.csv");
  std::streampos current = fin.tellg();
  fin.seekg (0, fin.end);
  bool empty = !fin.tellg();
  fin.seekg (current, fin.beg);

  if (empty != true) {
    std::ifstream inf ("data.csv");
    std::string line {};
    while(getline(inf, line)) {
      if (line.size() > 0) {
        temp.push_back(stod(line));
      }
    }
    flag = true;
  }

  if (choice == 1) {
    std::ofstream otf;
    otf.open("data.csv", std::ios::trunc | std::ios::out);
    uint64_t cnt = m_time_s * m_sampleRate;
    std::vector<float> sine_vec {};

    for (uint64_t i = 0; i < cnt; ++i) {
      float sine_calc = static_cast<float> (m_amp * sin((2.f * M_PI * m_baseFreq)/ m_sampleRate * i) + m_offset);
      sine_vec.push_back(sine_calc);
      otf << sine_vec[i] << std::endl;
    }
    otf.close();
    if (flag == true) {
      mix_waves(temp, sine_vec);
    } else {
        return 0;
    }
  }
  if (choice == 2) {
    std::ofstream otf;
    otf.open("data.csv", std::ios::trunc | std::ios::out);
    uint64_t cnt = m_time_s * m_sampleRate;
    std::vector<float> triangle_vec {};

    for (size_t i = 0; i < cnt; ++i) {
      float triangle_calc = static_cast<float> ((m_amp * (2.f / M_PI)) * asin(sin((2 * M_PI * m_baseFreq) / m_sampleRate * i) + m_offset));
      triangle_vec.push_back(triangle_calc);
      otf <<  triangle_vec[i] << std::endl;
    }
    otf.close();
    if (flag == true) {
      mix_waves(temp, triangle_vec);
    } else {
        return 0;
    }
  }
}



int main () {
  uint16_t action = 1;
  std::ofstream otf ("data.csv");
  while (action != 0) {
    uint64_t amp;
    float baseFreq;
    uint64_t time_s;
    uint sampleRate;
    uint offset;
    uint16_t choice = 0;

    std::cout << "What do you want to generate? " 
     << "Type 1 for sinus or 2 for triangle. Type 0 for exit."  
     << std::endl;

    std::cin >> choice;

    switch (choice) {
    case 1:
      std::cout << "Enter amp, freq, time (s), smprate, offset" << std::endl;
      std::cin >> amp >> baseFreq >> time_s >> sampleRate >> offset;
      generator (amp, baseFreq, time_s, sampleRate, offset, choice, otf);
      break;
    case 2:
      std::cout << "Enter amp, freq, time (s), smprate, offset" << std::endl;
      std::cin >> amp >> baseFreq >> time_s >> sampleRate >> offset;
      generator (amp, baseFreq, time_s, sampleRate, offset, choice, otf);
      break;
    default:
      action = 0;
      break;
    }
  }
  return 0;  
}