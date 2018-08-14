#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;

struct Separator {
   double value;
   uint64_t ofs;
};

using BTreeLevel = vector<Separator>;

static BTreeLevel buildBTreeLevel(const vector<double>& data,unsigned pageSize)
{
   BTreeLevel result;
   result.push_back({data.front(),0});
   for (uint64_t pos=pageSize,limit=data.size();pos<limit;pos+=pageSize)
      result.push_back({data[pos],pos});
   if (result.back().ofs!=data.size())
      result.push_back({data.back(),data.size()-1});
   return result;
}

static BTreeLevel buildBTreeLevel(const BTreeLevel& data,unsigned pageSize)
{
   BTreeLevel result;
   result.push_back({data.front().value,0});
   for (uint64_t pos=pageSize,limit=data.size();pos<limit;pos+=pageSize)
      result.push_back({data[pos].value,pos});
   if (result.back().ofs!=data.size())
      result.push_back({data.back().value,data.size()-1});
   return result;
}

static inline uint64_t interpolate(double value,double min,double max,uint64_t from,uint64_t to)
{
   return ((from==to)||(min==max))?from:from+((to-from)*((value-min)/(max-min)));
}

static uint64_t lowerBound(const BTreeLevel& level,double value,uint64_t pos,uint64_t from,uint64_t to)
{
   double v=level[pos].value;
   auto base=level.data();
   if (value<v) {
      uint64_t step=1;
      uint64_t index=pos-1;
      while (index>=from) {
         if (value>base[index].value)
            return lower_bound(base+index,base+index+step,value,[](const Separator& a,double b) { return a.value<b; })-base;
         if ((index-=step)>to) break;
         step=step<<1;
      }
      return lower_bound(base+from,base+index+step,value,[](const Separator& a,double b) { return a.value<b; })-base;
   } else if (value>v) {
      uint64_t step=1;
      uint64_t index=pos+1;
      while (index<=to) {
         if (value<base[index].value)
            return lower_bound(base+index-step,base+index,value,[](const Separator& a,double b) { return a.value<b; })-base;
         index+=step;
         step=step<<1;
      }
      return lower_bound(base+index-step,base+to,value,[](const Separator& a,double b) { return a.value<b; })-base;
   } else {
      return pos;
   }
}

static uint64_t lookup(const vector<BTreeLevel>& btree,double value,uint64_t& leafFrom,uint64_t& leafTo)
{
   double lastMin=btree.back().front().value,lastMax=btree.back().back().value;
   unsigned from=0,to=btree.back().size();
   for (unsigned index=btree.size();index>0;) {
      auto& level=btree[--index];
      uint64_t start=interpolate(value,lastMin,lastMax,from,to);
      uint64_t desc=lowerBound(level,value,start,from,to);
      if (desc) {
         lastMin=level[desc-1].value; lastMax=level[desc].value;
         from=level[desc-1].ofs+1; to=level[desc].ofs;
      } else {
         lastMin=lastMax=level[desc].value;
         from=to=level[desc].ofs;
      }
   }
   leafFrom=from; leafTo=to;
   return interpolate(value,lastMin,lastMax,from,to);
}

[[maybe_unused]]static inline const double* lower_bound_bf(const double* base,const double* limit,double needle)
{
   auto n=(limit-base);
   auto lower=base;
   while (auto half=n/2) {
      auto middle=lower+half;
      lower=((*middle)<=needle)?middle:lower;
      n-=half;
   }
   return lower;
}

static uint64_t findKey(const vector<double>& level,double value,uint64_t pos,uint64_t from,uint64_t to)
{
   double v=level[pos];
   auto base=level.data();
   if (value<v) {
      uint64_t step=1;
      uint64_t index=pos-1;
      while (index>=from) {
         double v=base[index];
         if (value>v)
            return lower_bound(base+index,base+index+step,value/*,[](double a,double b) { return a<b; }*/)-base;
         if (value==v) return index;
         if ((index-=step)>to) break;
         step=step<<1;
      }
      return lower_bound(base+from,base+index+step,value/*,[](double a,double b) { return a<b; }*/)-base;
   } else if (value>v) {
      uint64_t step=1;
      uint64_t index=pos+1;
      while (index<=to) {
         double v=base[index];
         if (value<v)
            return lower_bound(base+index-step,base+index,value/*,[](double a,double b) { return a<b; }*/)-base;
         if (value==v) return index;
         index+=step;
         step=step<<1;
      }
      return lower_bound(base+index-step,base+to,value/*,[](double a,double b) { return a<b; }*/)-base;
   } else {
      return pos;
   }
}


int main(int argc,char* argv[])
{
   if (argc<3) {
      cerr << "Usage: " << argv[0] << " file lastinner" << endl;
      return 1;
   }
   unsigned lastInner=atoi(argv[2]);
   if (!lastInner) {
      cerr << "invalid node count" << endl;
      return 1;
   }

   vector<double> data;
   {
      ifstream in(argv[1],ifstream::binary);
      if (!in.is_open()) {
         cerr << "unable to open " << argv[1] << endl;
         return 1;
      }
      auto pos = in.tellg();
      in.seekg( 0, ios::end );
      auto size=in.tellg()-pos;
      in.seekg(0,ios::beg);

      unsigned elements=size/sizeof(double);
      data.resize(elements);
      in.read(reinterpret_cast<char*>(data.data()),elements*sizeof(double));
   }
   unsigned pageSize=(data.size()+lastInner-1)/lastInner;

   vector<BTreeLevel> btree;
   btree.push_back(buildBTreeLevel(data,pageSize));
   while (btree.back().size()>pageSize)
      btree.push_back(buildBTreeLevel(btree.back(),pageSize));

   uint64_t btreeSize=0;
   for (auto& l:btree)
      btreeSize+=sizeof(Separator)*l.size();
   cerr << "size " << (btreeSize>>10) << "KB" << endl;


   vector<double> lookupOrder=data;
   {
      mt19937 g;
      shuffle(lookupOrder.begin(), lookupOrder.end(), g);
   }

   cerr << "lookup" << endl;
   uint64_t sumDiff=0,maxDiff=0;
   auto start = chrono::steady_clock::now();
   for (auto& d:lookupOrder) {
      uint64_t leafFrom,leafTo;
      uint64_t pos=lookup(btree,d,leafFrom,leafTo);
      // cerr << leafFrom << " " << leafTo << " " << pos << endl;
      uint64_t real=findKey(data,d,pos,leafFrom,leafTo);
      uint64_t diff=(real<pos)?(pos-real):(real-pos);
      // cerr << real << " " << diff << " " << d << " " << data[real] << endl;
      if (diff>maxDiff) maxDiff=diff;
      sumDiff+=diff;
   }
   auto stop = chrono::steady_clock::now();
   cerr << "maximum diff: " << maxDiff << " avg diff: " << (static_cast<double>(sumDiff)/data.size()) << endl;
   double avgTotal=chrono::duration<double>(stop - start).count()*1000000000.0/data.size();
   cerr << "avg total lookup " << avgTotal << "ns" << endl;

   start = chrono::steady_clock::now();
   for (auto& d:lookupOrder) {
      uint64_t leafFrom,leafTo;
      uint64_t pos=lookup(btree,d,leafFrom,leafTo);
      if (pos>data.size()) { cerr << "invalid position!"; }
   }
   stop = chrono::steady_clock::now();
   double avgModel=chrono::duration<double>(stop - start).count()*1000000000.0/data.size();
   cerr << "avg model lookup " << avgModel << "ns" << endl;
   cerr << "avg search " << (avgTotal-avgModel) << "ns" << endl;

   start = chrono::steady_clock::now();
   for (auto& d:lookupOrder) {
      auto l=lower_bound(data.begin(),data.end(),d);
      if (l>data.end()) { cerr << "invalid position!"; }
   }
   stop = chrono::steady_clock::now();
   cerr << "avg search without any model " << chrono::duration<double>(stop - start).count()*1000000000.0/data.size() << "ns" << endl;
}
