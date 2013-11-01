#include <iostream>
#include <vector>
using namespace std;

bool intPairSortFunction(const std::pair<int,int> &i, const std::pair<int,int> &j)
{
	return (i.second > j.second);
}
	
int main() 
{
	//Sort 100,000 items
	
	//Seed Random
	cout << "Seeding Random" << endl;
	srand (time(NULL));
	
	//Define Vector
	cout << "Defining Vector" << endl;
	std::vector<std::pair<int,int> > abc;
	
	//Add items to vector
	cout << "Adding items to Vector" << endl;
	for (int i = 0; i < 1228800; ++i)
	{
		std::pair<int,int> temp;
		temp.first = i;
		temp.second = rand() % 32000000 + 1;  
		abc.push_back(temp);
	}

	
	//Sort Vector
	cout << "Sorting Vector" << endl;
	std::stable_sort(abc.begin(), abc.end(), intPairSortFunction);
	
	//Print first 20 items
	cout << endl << "**First 256 Vectors**" << endl << endl;
	for (int i = 0; i < 256; ++i)
	{
		cout << "Key: " << abc[i].first << ", Value: " << abc[i].second << endl;
	}
	
}