#include <vector>
#include <cmath>
#include <iostream>
using namespace std;

class wavelet
{
public:
	wavelet(int n, int sigma, vector<int> T);
	~wavelet();
	void show_tables();
	int n, sigma, log_sigma;
	vector< vector <int> > rank_tables;
};

wavelet::wavelet(int n, int sigma, vector<int> T)
{
	this->n = n;
	this->sigma = sigma;
	this->log_sigma = (int)ceil(log2(sigma));
	rank_tables = vector< vector <int> >(log_sigma, vector<int>(2*(n+1), 0));
	int i = 0;
	vector<int> prevT = T;
	vector<int> nextT;
	for (size_t i = 0; i < log_sigma; i++)
	{
		nextT.clear();
		vector <int> &rank_tb = rank_tables[i];
		int bit_mask = 1 << i;
		int  nz = 0;
		
		for (size_t p = 0; p < n; p++)
		{
			if ((prevT[p] & bit_mask) == 0) {
				nextT.push_back(prevT[p]);
				nz++;
				rank_tb[p + 1] = rank_tb[p] + 1; //1の領域の初期化も兼ねるためrank_tb[n]まで書き込む
			}
			else
			{
				rank_tb[p + 1] = rank_tb[p];
			}
		}

		rank_tb[n + 1] = nz;
		for (size_t p = 0; p < n; p++)
		{
			if ((prevT[p] & bit_mask) != 0) {
				nextT.push_back(prevT[p]);
				rank_tb[p + n + 2] = rank_tb[p + n + 1] + 1;
			}
			else
			{
				rank_tb[p + n + 2] = rank_tb[p + n + 1];
			}
		}
		prevT = nextT;
	}
}

wavelet::~wavelet()
{
}

void wavelet::show_tables() {
	cout << log_sigma << endl;
	cout << n+1 << endl;
	for(vector<int> rank_tb: rank_tables)
	{
		for (size_t i = 0; i < 2*(n+1); i++)
		{
			cout << rank_tb[i] << " ";
		}
		cout << endl;
	}
}

int main()
{
	int n;
	int sigma;
	std::cin >> n;
	std::cin >> sigma;
	vector<int> T(n);
	for(int i = 0; i < n; i++){
		std::cin >> T[i];
	}
	wavelet w(n, sigma, T);
	w.show_tables();
    return 0;
}

