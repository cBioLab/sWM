#include "wav.h"
#include "comm.h"

#define IS_F 0
#define IS_G 1

extern cybozu::RandomGenerator rg;

using namespace std;

int main(){
	ROT::SysInit();
	WAVM::Server s;
	WAVM::Client c;

	string pubfile = "../comm/pub.dat";
	string prvfile = "../comm/prv.dat"; 
	string wavelet = "../data/test/server_test.wm";
	string paramfile = "../comm/param.dat";
	string queryfile = "../data/test/client_test.que";
	string q_lenfile = "../comm/q_len.dat";
	string queryffile = "../comm/queryf.dat";
	string querygfile = "../comm/queryg.dat";
	string resultffile = "../comm/resultf.dat";
	string resultgfile = "../comm/resultg.dat";
	string ismatchfile = "../comm/ismatch.dat";


	s.readWVM(wavelet);

	s.makeLUTable(paramfile); //L0,L1確定->書き出し
	s.core = 1;
	c.core = 1;

	int e = 2;

	c.setQuery(queryfile,q_lenfile); //クエリセット、長さ書き出し

	c.setParam(paramfile,(char *)pubfile.c_str(),(char *)prvfile.c_str()); //鍵生成

	s.setParam(pubfile,q_lenfile); //L0,L1確定後,c->s pub+(m)


	int f = 0,g = s.text_len-1,t;
	int ranf0,ranf1,rang0,rang1;
	ranf0 = ranf1 = rang0 = rang1 = 0;
	int maxmatchlen = c.query_len;
	int isL = -1;

	cerr <<"f=" << f << " g=" << g << endl;
	for(int i=0;i<c.query_len;i++){ //c.query_len
		for(int j=0;j<s.lg_sigma;j++){ //s.lg_sigma

			ranf0 = rand();
			ranf1 = rand();
			if(i+1 == c.query_len && j+1 == s.lg_sigma){
				ranf0=0; ranf1=0;
			}

			rang0 = rand();
			rang1 = rand();
			if(i+1 == c.query_len && j+1 == s.lg_sigma){
				rang0=0; rang1=0;
			}

			t = ((c.query[c.query_len - i -1])>>j)&1;
			if(t){
				f += (c.B0)*(c.L1);
				g += (c.B0)*(c.L1);
			}

			s.updtLUTable();

			c.makeQuery(queryffile,(f/(c.L1))%(c.L0),f%(c.L1));

			s.setPrevFr();
			if(j==0) s.getOrgQuery(queryffile, IS_F); //外のループに関して1つ前のf,gを用いてisLongestできるようにした
			s.getResult(queryffile, ranf0, ranf1);
			s.storePrevFr();
			s.makeResFile(resultffile);

			c.makeQuery(querygfile,(g/(c.L1))%(c.L0),g%(c.L1));

			s.setPrevGr();
			if(j==0) s.getOrgQuery(querygfile, IS_G);
			s.getResult(querygfile, rang0, rang1);
			s.storePrevGr();
			s.makeResFile(resultgfile);

			c.decResult(resultffile, (f/c.L1)%c.L0);
			f = c.L1*c.t0 + c.t1;

			c.decResult(resultgfile, (g/c.L1)%c.L0);
			g = c.L1*c.t0 + c.t1;

			cerr <<"f=" << f << " g=" << g << endl;
			
		}
//		s.makeIsLongest(ismatchfile);
		s.makeIsELongest(ismatchfile,e);
		if(isL!=0){
			isL = c.chkIsLongest(ismatchfile);
			if(isL==0) maxmatchlen = i-1; 
		}
//		cerr << "isL=" << isL << endl;
	}

	c.makeQuery(queryffile,(f/(c.L1))%(c.L0),f%(c.L1));

	s.setPrevFr();
  s.getOrgQuery(queryffile, IS_F); //外のループに関して1つ前のf,gを用いてisLongestできるようにした

	c.makeQuery(querygfile,(g/(c.L1))%(c.L0),g%(c.L1));

	s.setPrevGr();
  s.getOrgQuery(querygfile, IS_G);

//	s.makeIsLongest(ismatchfile);
	s.makeIsELongest(ismatchfile,e);
	if(isL!=0){
		isL = c.chkIsLongest(ismatchfile);
		if(isL==0) maxmatchlen = c.query_len-1; 
	}

	cout << "f="<<f<<" g="<<g<<endl;
	cout << "maxmatchlen=" << maxmatchlen << endl; 
	return 0;
}
