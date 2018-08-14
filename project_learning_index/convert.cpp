#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
//4 input: <commend> <mode=float> <input file> <output file>
int main(int argc,char* argv[])
{
   if (argc<4) {
      cerr << "usage: " << argv[0] << " file" << endl;
      return 1;
   }
   string mode=argv[1];
   vector<pair<double,double>> cdf;
   if (mode=="float") {
      ifstream in(argv[2],ifstream::binary);
      vector<float> data;
      auto pos = in.tellg();
      in.seekg( 0, ios::end );
      auto size=in.tellg()-pos;
      in.seekg(0,ios::beg);

      unsigned elements=size/sizeof(float);
      data.resize(elements);
      in.read(reinterpret_cast<char*>(data.data()),elements*sizeof(float));

      sort(data.begin(),data.end());

      vector<double> data2;
      for (auto d:data)
         data2.push_back(d);
      ofstream out(argv[3],ofstream::binary);
      out.write(reinterpret_cast<char*>(data2.data()),sizeof(double)*data2.size());
      return 0;


      {
         unsigned pos=0;
         float last=data.front()+1;
         for (float d:data) {
            if (d!=last)
               cdf.push_back({d,pos});
            pos++;
            last=d;
         }
         cdf.push_back({data.back()+1,pos});
      }
   } else if (mode=="text") {
      vector<double> data;
      ifstream in(argv[2]);
      if (!in.is_open()) {
         cerr << "unable to open " << argv[1] << endl;
         return 1;
      }
      double v;
      while (in>>v)
         data.push_back(v);
      sort(data.begin(),data.end());
      ofstream out(argv[3],ofstream::binary);
      out.write(reinterpret_cast<char*>(data.data()),sizeof(double)*data.size());
      return 0;

      {
         unsigned pos=0;
         double last=data.front()+1;
         for (double d:data) {
            if (d!=last)
               cdf.push_back({d,pos});
            pos++;
            last=d;
         }
         cdf.push_back({data.back()+1,pos});
      }

   }

   ofstream out(argv[3],ofstream::binary);
   out.write(reinterpret_cast<char*>(cdf.data()),sizeof(pair<double,double>)*cdf.size());
}
//---------------------------------------------------------------------------
