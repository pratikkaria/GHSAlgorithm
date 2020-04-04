#include<bits/stdc++.h>
using namespace std;

template<typename T>
void printVector(vector<T> str)
{
  for(int i=0;i<str.size();i++)
    cout<<str[i]<<endl;
  cout<<"----------------"<<endl;
}
vector<int> processLine(string s)
{
  vector<int> out;
  s.erase( std::remove_if(s.begin(), s.end(),[](char ch){ return ch=='(' || ch ==')'; }),s.end());
  s.erase( std::remove_if(s.begin(), s.end(),[](char ch){ return ch==','; }),s.end());
  istringstream ss(s);
  while (ss)
  {
      string word;
      ss >> word;
      if(word=="")
        continue;
      out.push_back(atoi(word.c_str()));
  }
  return out;
}
int main(int argc,char** argv)
{
  std::ifstream file(argv[1]);
  int numNodes,flag=0;
  if (file.is_open())
  {
      std::string line;
      while (std::getline(file, line))
      {
        if(flag==0)
        {
          numNodes=atoi(line.c_str());
          flag=1;
          continue;
        }
        //printVector(processLine(line));
      }
      cout<<"Number of nodes:"<<numNodes<<endl;
      file.close();
  }
  return 0;
}
