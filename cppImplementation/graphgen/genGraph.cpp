#include "jngen.h"
#include<bits/stdc++.h>
using namespace std;

int main(int argc,char** argv)
{
//auto g = Graph::random(10, 20).connected().allowMulti().g().shuffled();
//Graph g2 = Graph::randomStretched(100, 200, 2, 5);
	cout << Graph::random(atoi(argv[1]),atoi(argv[2])).connected() << endl;
	return 0;
}
