// binomial_distribution
#include <iostream>
#include <random>

using namespace std;

double Binomial(int n, int s)
{
	double res;
	res = 1;
	for (int i = 1; i <= s; i++)
		res = (n - i + 1) * res / i;
		
	return res;
	
}
int main()
{
 	cout << Binomial(4,0) << endl;
	cout << Binomial(4,1) << endl;
	cout << Binomial(4,2) << endl;
	cout << Binomial(4,3) << endl;
	cout << Binomial(4,4) << endl;
	
	
}