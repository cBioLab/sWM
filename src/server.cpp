#include "wav.h"
#include "comm.h"
#include <sys/time.h>

#define IS_F 0
#define IS_G 1

extern cybozu::RandomGenerator rg;

using namespace std;

double get_wall_time(){
	struct timeval time;
	if (gettimeofday(&time, NULL)){
		return 0;
	}
	return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

int main(int argc,char* argv[]){
	double s_time,e_time,calc_time=0;
	s_time = get_wall_time();

	ROT::SysInit();
	WAVM::Server s;

	int ranf0,ranf1,rang0,rang1;
	ranf0=ranf1=rang0=rang1 =0;
	int sock;

	string pubfile = "../comm/server/pub.dat";
	string paramfile = "../comm/server/param.dat";
	string q_lenfile = "../comm/server/q_len.dat";
	string queryffile = "../comm/server/queryf.dat";
	string querygfile = "../comm/server/queryg.dat";
	string resultffile = "../comm/server/resultf.dat";
	string resultgfile = "../comm/server/resultg.dat";
	string ismatchfile = "../comm/server/ismatch.dat";

	string wavelet;
	s.core = 4; //default

	int opt;
	int p = 23456;
	int epsilon = 0;
	//-p port -f WMfile -c core -e epsilon
	while((opt = getopt(argc, argv, "p:f:c:e:")) != -1){
		switch(opt){
			case 'p':
				p = atoi(optarg);
				break;
			case 'f': 
				wavelet = optarg;
				break;
			case 'c': 
				s.core = atoi(optarg);
				break;
			case 'e': 
				epsilon = atoi(optarg);
				break;
			default:
				fprintf(stderr,"error! \'%c\' \'%c\'\n", opt, optopt);
				return -1;
				break;
		}
	}

	sock = prepSSock(p);

	s.readWVM(wavelet);
	s.makeLUTable(paramfile); //L0,L1確定->書き出し

	e_time = get_wall_time();
	calc_time += e_time-s_time;

	sock = acceptSSock(sock);
	recvFile(sock,(char *)q_lenfile.c_str());
	sendFile(sock,(char *)paramfile.c_str());
	recvFile(sock,(char *)pubfile.c_str());

	s_time = get_wall_time();
	
	s.setParam(pubfile,q_lenfile); //L0,L1確定後,c->s pub+(m)

	e_time = get_wall_time();
	calc_time += e_time-s_time;

	for(int i=0;i<s.query_len;i++){
		for(int j=0;j<s.lg_sigma;j++){
			s_time = get_wall_time();
			ranf0 = rand();
			ranf1 = rand();
/*
			if(i+1 == s.query_len && j+1 == s.lg_sigma){
				ranf0=0; ranf1=0;
			}
*/
			rang0 = rand();
			rang1 = rand();
/*
			if(i+1 == s.query_len && j+1 == s.lg_sigma){
				rang0=0; rang1=0;
			}
*/
			s.updtLUTable();

			e_time = get_wall_time();
			calc_time += e_time-s_time;

			recvFile(sock,(char *)queryffile.c_str());

			s_time = get_wall_time();

			s.setPrevFr();
			if(j==0) s.getOrgQuery(queryffile, IS_F);
			s.getResult(queryffile, ranf0, ranf1);
			s.storePrevFr();
			s.makeResFile(resultffile);

			e_time = get_wall_time();
			calc_time += e_time-s_time;

			sendFile(sock,(char *)resultffile.c_str());
			recvFile(sock,(char *)querygfile.c_str());

			s_time = get_wall_time();

			s.setPrevGr();
			if(j==0) s.getOrgQuery(querygfile, IS_G);
			s.getResult(querygfile, rang0, rang1);
			s.storePrevGr();
			s.makeResFile(resultgfile);

			e_time = get_wall_time();
			calc_time += e_time-s_time;

			sendFile(sock,(char *)resultgfile.c_str());
		}
		s_time = get_wall_time();
		if(epsilon == 0) s.makeIsLongest(ismatchfile);
		else s.makeIsELongest(ismatchfile,epsilon);
		e_time = get_wall_time();
		calc_time += e_time-s_time;

		sendFile(sock,(char *)ismatchfile.c_str());
	}

	recvFile(sock,(char *)queryffile.c_str());

	s_time = get_wall_time();
	s.setPrevFr();
	s.getOrgQuery(queryffile, IS_F);
	e_time = get_wall_time();
	calc_time += e_time-s_time;

	recvFile(sock,(char *)querygfile.c_str());

	s_time = get_wall_time();
	s.setPrevGr();
	s.getOrgQuery(querygfile, IS_G);
	e_time = get_wall_time();
	calc_time += e_time-s_time;

	s_time = get_wall_time();
	if(epsilon == 0) s.makeIsLongest(ismatchfile);
	else s.makeIsELongest(ismatchfile,epsilon);
	e_time = get_wall_time();
	calc_time += e_time-s_time;

	sendFile(sock,(char *)ismatchfile.c_str());

	closeSock(sock);

	cerr << "server calc_time=" << calc_time << endl;
	cout << calc_time <<endl;

	return 0;
}
