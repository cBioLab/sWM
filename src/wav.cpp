#include "wav.h"

//#define DEBUG_MAIN

extern cybozu::RandomGenerator rg;

void WAVM::Client::setParam(std::string file, char *pubkf, char *prvkf)
{
	std::ifstream ifs(file.c_str(), std::ios::binary);
	ifs>>B0;
	ifs>>L0;
	ifs>>L1;
	ifs>>text_len;
	v_length = L0*L1;
	pubk=pubkf;
	prvk=prvkf;
	setParam(L0, L1, prvk, pubk);
	if(L0<L1)
		prv.setCache(0, L1+1); // set cache for prv
	else
		prv.setCache(0, L0+1);
}

void WAVM::Client::setQuery(std::string qfile,std::string qlenfile)
{
        std::ifstream ifs(qfile.c_str(), std::ios::binary);
	double sigma;
	ifs>>query_len;
	//ifs>>lg_sigma;
	ifs>>sigma;
	lg_sigma = log(sigma)/log(2);
	if(sigma-pow(2,lg_sigma)>0)
	  lg_sigma++;
	std::cerr <<"q_len="<<query_len<<" lg_sigma="<<lg_sigma<<std::endl;
	query.resize(query_len);
	for(int i=0;i<query_len;i++){
		ifs>>query[i];
	}
	std::ofstream ofs(qlenfile.c_str(), std::ios::binary);
	ofs<<query_len<<"\n";
}

int WAVM::Client::chkIsLongest(std::string match){
	Elgamal::CipherText c;
	std::ifstream ifs(match.c_str(), std::ios::binary);
	bool cc;	
	int res, line;
	ifs >> line;
	for(int i=0; i<line; i++){
		ifs >> c;
		res = prv.dec(c, &cc);
		#ifdef DEBUG
		std::cerr<<"res="<<res<<std::endl;
		#endif
		if(cc && res==0){
			return(0);
		}else{
		}
	}
	return(-1);
}

int WAVM::Server::retV(int idx){
	return(v[idx]);
}

void WAVM::Server::setParam(std::string file,std::string qlenfile)
{
	pubk=file;
	std::cerr<<pubk<<"\n";
	std::cerr<<"set v\n";
	setV(v, v_length, L0, L1);
	std::cerr<<"read key\n";
	readPubkey(pubk);
	std::ifstream ifs(qlenfile.c_str(), std::ios::binary);
	ifs>>query_len;
}

void WAVM::Server::readWVM(std::string wavefile)
{
	std::ifstream ifs(wavefile.c_str());
	ifs >> lg_sigma;
	ifs >> text_len;
	wvm.resize(lg_sigma);
	std::vector<int> vec(text_len*2);
	for (int j=0;j<lg_sigma;j++){
		for (int i=0;i<text_len*2;i++){
			ifs >> vec[i];
		}
		wvm[j] = vec;
	}
	std::cerr<<"lg_sigma="<<lg_sigma<<", text_len="<<text_len<<std::endl;
}

void WAVM::Server::makeLUTable(std::string file)
{
	int b;
	for(b=1;(((text_len/b)+1)/(double)b) > 8.0;b++){}
	B0 = b; //b-1にするかどうか
	L0 = 2*B0;
	L1 = text_len/B0+1;
	v_length = L0*L1;
	v = (int*)calloc(sizeof(int),v_length);
	std::ofstream ofs(file.c_str(), std::ios::binary);
	ofs<<B0<<"\n";
	ofs<<L0<<"\n";
	ofs<<L1<<"\n";
	ofs<<text_len<<"\n";
	std::cerr<<"B0="<<B0<<", L0="<<L0<<" ,L1="<<L1<<" blk="<<B0*L1<<"\n";
}

void WAVM::Server::updtLUTable(void)
{
	std::vector<int> vec = wvm[turn];
	int blk = L1 * B0;
	for(int i=0;i<text_len;i++){
		v[i] = vec[i];
		v[i+blk] = vec[i+text_len];
	}


	#ifdef DEBUG
	std::cerr << "v" << std::endl;
	for(int i=0;i<blk*2;i++) std::cerr << v[i] << " ";
	std::cerr << std::endl;
	#endif

	updtV(v, v_length, L0, L1);
	turn++;
	turn %= lg_sigma;
}

void WAVM::Server::setPrevFr(){
	prev_r0 = prev_fr0;
	prev_r1 = prev_fr1;
}
void WAVM::Server::setPrevGr(){
	prev_r0 = prev_gr0;
	prev_r1 = prev_gr1;
}
void WAVM::Server::storePrevFr(){
	prev_fr0 = prev_r0;
	prev_fr1 = prev_r1;
}
void WAVM::Server::storePrevGr(){
	prev_gr0 = prev_r0;
	prev_gr1 = prev_r1;
}

//index 0:fp 1:ep
void WAVM::Server::getOrgQuery(std::string& query, int index){
	std::ifstream ifs(query.c_str(), std::ios::binary);

	Elgamal::CipherText c;
	CipherTextVec::iterator it;
	CipherTextVec::iterator start;

	pub.enc(c, prev_r0, rg);
	c.neg();
	if(index==0){
		ifs >> efp;
		efp.add(c);
		efq.resize(L1);
		it = efq.begin();
		start = efq.begin();
	}else{
		ifs >> egp;
		egp.add(c);
		egq.resize(L1);
		it = egq.begin();
		start = egq.begin();
	}

	for(int i=0;i<(L1-prev_r1);i++)
		it++;
		
	for(int i=0;i<L1;i++){
		if(i+(L1-prev_r1)==L1){
			it = start;
		}
		ifs >> c;
		*it=c;
		it++;
	}
}


void WAVM::Server::makeIsLongest(std::string match){
	std::ofstream ofs(match.c_str(), std::ios::binary);

	Elgamal::CipherText c;
	Elgamal::CipherText d;	
	Elgamal::CipherText res;	
	pub.enc(c,0,rg);
	pub.enc(d,L0,rg);

	omp_set_nested(1);
#pragma omp parallel for
	for(int i=0;i<L1;i++){
		egq[i].neg();
		efq[i].add(egq[i]);
		efq[i].mul(rand());
	}
	for(int i=0;i<L1;i++)
		c.add(efq[i]);

	c.add(efp);
	egp.neg();
	c.add(egp);

	CipherTextVec tmp;

	Zn rn;
	rn.setRand(rg);
	res = c;
	res.mul(rn);
	tmp.push_back(res);

	rn.setRand(rg);
	res = c;
	res.add(d);
	res.mul(rn);
	tmp.push_back(res);

	rn.setRand(rg);
	res = c;
	d.neg();
	res.add(d);
	res.mul(rn);
	tmp.push_back(res);

	ofs << tmp.size() << "\n";
	random_shuffle(tmp.begin(),tmp.end());
	for(int i=0;i<tmp.size();i++){
		ofs << tmp[i];
		ofs << "\n";
	}
}

void WAVM::Server::compIsELongest(int offset, int flg, CipherTextVec& tmp){
	Elgamal::CipherText c;
	Elgamal::CipherText d;	
	Elgamal::CipherText e;
	Elgamal::CipherText res;	

	Elgamal::CipherText egp_t=egp, efp_t=efp;
	CipherTextVec egq_t, efq_t;
	egq_t.resize(L1);
	efq_t.resize(L1);
	
	pub.enc(c,0,rg);
	pub.enc(d,L0,rg);
	pub.enc(e,1,rg);

	for(int i=0;i<L1;i++){
		efq_t[(i+offset)%L1] = efq[i];
		egq_t[i] = egq[i];
	} 
	if(flg==0){
		//same f_0
		for(int i=0; i<offset;i++)
			efq_t[i] = c;
		
	}else{
		//f_0 = f_0+1
		for(int i=offset; i<L1;i++)
			efq_t[i] = c;
		efp_t.add(e);
	}

	omp_set_nested(1);
#pragma omp parallel for
	for(int i=0;i<L1;i++){
		egq_t[i].neg();
		efq_t[i].add(egq_t[i]);
		efq_t[i].mul(rand());
	}
	for(int i=0;i<L1;i++)
		c.add(efq_t[i]);

	c.add(efp_t);
	egp_t.neg();
	c.add(egp_t);

	Zn rn;
	rn.setRand(rg);
	res = c;
	res.mul(rn);
	tmp.push_back(res);

	rn.setRand(rg);
	res = c;
	res.add(d);
	res.mul(rn);
	tmp.push_back(res);

	rn.setRand(rg);
	res = c;
	d.neg();
	res.add(d);
	res.mul(rn);
	tmp.push_back(res);

}

void WAVM::Server::makeIsELongest(std::string match, int thr){
	std::ofstream ofs(match.c_str(), std::ios::binary);
	CipherTextVec tmp;

	compIsELongest(0, 0, tmp);
	for(int i=1; i<thr; i++){
		compIsELongest(i, 0, tmp);
		compIsELongest(i, 1, tmp);
	}
	
	ofs << tmp.size() << "\n";
	random_shuffle(tmp.begin(),tmp.end());
	for(int i=0;i<tmp.size();i++){
		ofs << tmp[i];
		ofs << "\n";
	}
}
