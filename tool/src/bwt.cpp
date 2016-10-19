#include <iostream>
#include <vector>
#include <list>

using namespace std;

struct vecpointer{
	int t;
	vector<int>::iterator p;

	vecpointer(int k,vector<int>::iterator q){
		t = k;
		p = q;
	}

	//終端文字が最も小さいという前提ならば無限ループにはならない
	bool operator<(vecpointer& right){
		vector<int>::iterator l = p;
		vector<int>::iterator r = right.p;
		for(;;){
			if((*l)<(*r)) return true;
			else if((*l)>(*r)) return false;
			l++;
			r++;
		}
	}
};

//BWT変換を行う vが入力数列、ansが出力文字列
void constructbwt(vector<int> &v,vector<int> &ans,int &n,int &sigma){
	list<vecpointer> vecplist; 
	int i=0;
	for(vector<int>::iterator p=v.begin();p!=v.end();p++){
		vecplist.push_back(vecpointer(i,p));
		i++;
	}
	vecplist.sort();
	for(list<vecpointer>::iterator p=vecplist.begin();p!=vecplist.end();p++){
		ans.push_back(v[((*p).t + (n-1))%n]);
	}
}

//入力部分
void inputvec(vector<int> &v,int &n,int &sigma){
	cin >> n >> sigma;
	for(int i=0;i<n;i++){
		int t;
		cin >> t;
		t %= sigma;
		v.push_back(t);
	}
}

int main(){
	vector<int> v;
	vector<int> ans;
	int n,sigma;
	inputvec(v,n,sigma);
	constructbwt(v,ans,n,sigma);
	cout << n << ' ' << sigma << endl;
	for(int i=0;i<ans.size();i++) cout << ans[i] << ' ';
	cout << endl;
	return 0;
}
