#include "wav.h"
#include "comm.h"
#include <sys/time.h>

extern cybozu::RandomGenerator rg;

using namespace std;

double get_wall_time(){
	struct timeval time;
	if (gettimeofday(&time, NULL)){
		return 0;
	}
	return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

int main(int argc,char *argv[]){
	double elapsed_time;
	double start_sec = get_wall_time();
	double s_time,e_time,calc_time=0;
	s_time = start_sec;

	ROT::SysInit();
	WAVM::Client c;

	c.core = 4;
	int p = 23456;
	string queryfile;
	string hostname = "localhost";

	string pubfile = "../comm/client/pub.dat";
	string prvfile = "../comm/client/prv.dat"; 
	string paramfile = "../comm/client/param.dat";
	string q_lenfile = "../comm/client/q_len.dat";
	string queryffile = "../comm/client/queryf.dat";
	string querygfile = "../comm/client/queryg.dat";
	string resultffile = "../comm/client/resultf.dat";
	string resultgfile = "../comm/client/resultg.dat";
	string ismatchfile = "../comm/client/ismatch.dat";

	//-h hostname -p port -f queryfile -c core
	int opt;
	while((opt = getopt(argc, argv, "h:p:f:c:")) != -1){
		switch(opt){
			case 'h':
				hostname = optarg;
				break;
			case 'p':
				p = atoi(optarg);
				break;
			case 'f': 
				queryfile = optarg;
				break;
			case 'c': 
				c.core = atoi(optarg);
				break;
			default:
				fprintf(stderr,"error! \'%c\' \'%c\'\n", opt, optopt);
				return -1;
				break;
		}
	}

	c.setQuery(queryfile,q_lenfile); //クエリセット、長さ書き出し

	int sendsize = 0;
	int recvsize = 0;

	e_time = get_wall_time();
	calc_time += e_time-s_time;

	int sock = prepCSock((char *)hostname.c_str(),p);

	sendsize+=sendFile(sock,(char *)q_lenfile.c_str());
	recvsize+=recvFile(sock,(char *)paramfile.c_str());

	s_time = get_wall_time();
	c.setParam(paramfile,(char *)pubfile.c_str(),(char *)prvfile.c_str()); //鍵生成
	e_time = get_wall_time();
	calc_time += e_time-s_time;

	sendsize+=sendFile(sock,(char *)pubfile.c_str());

	s_time = get_wall_time();

	int f = 0,g = c.text_len-1,t;
	int maxmatchlen = c.query_len;
	int isL = -1;

	#ifdef DEBUG	
	cerr <<"f=" << f << " g=" << g << endl;
	#endif

	e_time = get_wall_time();
	calc_time += e_time-s_time;

	for(int i=0;i<c.query_len;i++){
		for(int j=0;j<c.lg_sigma;j++){
			s_time = get_wall_time();

			t = ((c.query[c.query_len - i -1])>>j)&1;
			if(t){
				f += (c.B0)*(c.L1);
				g += (c.B0)*(c.L1);
			}

			c.makeQuery(queryffile,(f/(c.L1))%(c.L0),f%(c.L1));

			e_time = get_wall_time();
			calc_time += e_time-s_time;

			sendsize+=sendFile(sock,(char *)queryffile.c_str());
			recvsize+=recvFile(sock,(char *)resultffile.c_str());

			s_time = get_wall_time();

			c.makeQuery(querygfile,(g/(c.L1))%(c.L0),g%(c.L1));

			e_time = get_wall_time();
			calc_time += e_time-s_time;

			sendsize+=sendFile(sock,(char *)querygfile.c_str());
			recvsize+=recvFile(sock,(char *)resultgfile.c_str());

			s_time = get_wall_time();

			c.decResult(resultffile, (f/c.L1)%c.L0);
			f = c.L1*c.t0 + c.t1;

			c.decResult(resultgfile, (g/c.L1)%c.L0);
			g = c.L1*c.t0 + c.t1;
			#ifdef DEBUG
			cerr <<"f=" << f << " g=" << g << endl;
			#endif

			e_time = get_wall_time();
			calc_time += e_time-s_time;
		}
		recvsize+=recvFile(sock,(char *)ismatchfile.c_str());

		s_time = get_wall_time();
		if(isL!=0){
			isL = c.chkIsLongest(ismatchfile);
			if(isL==0) maxmatchlen = i-1; 
		}
		e_time = get_wall_time();
		calc_time += e_time-s_time;
	}

	s_time = get_wall_time();
	c.makeQuery(queryffile,(f/(c.L1))%(c.L0),f%(c.L1));
	c.makeQuery(querygfile,(g/(c.L1))%(c.L0),g%(c.L1));
	e_time = get_wall_time();
	calc_time += e_time-s_time;

	sendsize+=sendFile(sock,(char *)queryffile.c_str());
	sendsize+=sendFile(sock,(char *)querygfile.c_str());
	recvsize+=recvFile(sock,(char *)ismatchfile.c_str());

	s_time = get_wall_time();
	if(isL!=0){
		isL = c.chkIsLongest(ismatchfile);
		if(isL==0) maxmatchlen = c.query_len-1; 
	}
	e_time = get_wall_time();
	calc_time += e_time-s_time;

	closeSock(sock);

//	cerr << "f="<<f<<" g="<<g<<endl;
	cerr << "maxmatchlen=" << maxmatchlen << endl;

	cerr << "client calc_time=" << calc_time << endl;
/*
	cout << "sendsize="<<(double)sendsize/(1024*1024)<<endl; 
	cout << "recvsize="<<(double)recvsize/(1024*1024)<<endl;
*/
//	cout << "communication size="<<(double)(sendsize+recvsize)/(1024*1024)<<endl;

	double end_sec = get_wall_time();
	elapsed_time = end_sec - start_sec;
//	cout << "elaplsed time=" << elapsed_time << endl;
/*
	cout << elapsed_time << endl;
	cout << calc_time << endl;
	cout <<(double)(sendsize+recvsize)/(1024*1024)<<endl;
*/
	return 0;
}
