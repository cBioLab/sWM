#include "rot.h"

namespace WAVM{
	class Server : public ROT::Server
	{
		int prev_fr0, prev_fr1;
		int prev_gr0, prev_gr1;
		CipherTextVec efq, egq;
		Elgamal::CipherText efp, egp;
		int turn; //WVAの何段目か（通信回数）
	public:
		int *v; //lookuptables
		std::vector< std::vector<int> > wvm;
		int query_len;
		std::string pubk;
		int lg_sigma, text_len;
		int v_length, B0, L0, L1;
		std::vector<int> pos;
		int retV(int idx);
		void readWVM(std::string wavefile);
		void setParam(std::string file,std::string qlenfile);
		void updtLUTable(void);
		void makeLUTable(std::string file);
		void setPrevFr();
		void setPrevGr();
		void storePrevFr();
		void storePrevGr();
		void getOrgQuery(std::string& query, int index); //index == 0 ? f : g
		void makeIsLongest(std::string match);
		void compIsELongest(int offset, int flg, CipherTextVec& tmp);
		void makeIsELongest(std::string match, int thr);

		Server(){
			prev_fr0=0; prev_fr1=0;
			prev_gr0=0; prev_gr1=0;
			turn = 0;
			ROT::Server();
		}
	};

	class Client : public ROT::Client
	{
		using ROT::Client::setParam;

	public:
		int lg_sigma;
		std::vector<int> query;
		int query_len;
		std::string prvk;
		std::string pubk;
		int text_len;
		int v_length, B0, L0, L1;
		void setParam(std::string file, char *pubkf, char *prvkf);
		void setQuery(std::string qfile,std::string qlenfile);
		int chkIsLongest(std::string match);
	};
}
