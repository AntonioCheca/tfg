#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include "vec3.h"

using namespace std;


int main(){
  ofstream o("rmse_ellipse_grande.txt");
  ifstream original("original_ellipse_grande.ppm");
  string s;
  int nx_o, ny_o;

  vec3 v;

  original >> s;

  original >> nx_o >> ny_o;
  vector<vector<vec3> > orig;
  original >> s;
  for(int i = 0; i < nx_o; i++){
    vector<vec3> row;
    for(int j = 0; j < ny_o; j++){
      original >> v;
      row.push_back(v);
    }
    orig.push_back(row);
  }
  original.close();

  for(int i = 1; i <= 20; i++){
    cout << i << endl;
    ifstream f("ellipse_area_grande_" + to_string(i) + ".ppm");
    double squared_dif = 0;
    f >> s;
    int nx, ny;
    int nc;
    f >> nx >> ny >> nc;
    for(int j = 0; j < nx; j++){
      for(int k = 0; k < ny; k++){
        f >> v;
        squared_dif += (v-orig[j][k]).squared_length();
      }
    }
    o << i*5 << " " << sqrt(squared_dif/(nx*ny)) << endl;
    f.close();
  }
  o.close();
}
