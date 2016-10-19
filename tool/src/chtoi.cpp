#include <iostream>
#include <string>

using namespace std;

int main(){
	string str;
	cin >> str;
	cout << str.size()+1 << " " << 27 << endl;
	for(int i=0;i<str.size();i++){
		cout << str[i]-'a'+1 << " ";
	}
	cout << 0 << endl;
	return 0;
}
