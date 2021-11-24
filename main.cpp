/*
 *
 * Distributed under terms of the MIT license.
 */
#include<cstdio>
#include<iostream>
#include<fstream>
#include<cmath>
#include<algorithm>
#include<set>
#include<map>
#include<queue>
#include<string>
#include<cstring>
#include<cstdlib>
#include<cassert>
#include<vector>
#include<unordered_map>
#include"simulator.h"
#include"utils.h"
#define rep(i,a,b) for (int i=a;i<=b;i++)
#define pb push_back
#define ws wss
using namespace std;
typedef long long LL;
typedef pair<int,int> PII;
template<class T> void upmax(T &a,T b) { if (a<b) a=b;}
template<class T> void upmin(T &a,T b) { if (a>b) a=b;}

//-----------------------
Config config;
int vis[320000];
int main(int argc, char *argv[])
{
	if (read_config(config, argc, argv)<0) return -1;
	process("data.csv");

//----------------------------------------------------	
	freopen(config.file, "w", stdout);
		
	double * B=new double[num_bidders];
	int * T=new int[num_bidders];
	double * r=new double[num_bidders];


	for (int cas=0;cas<100;cas++)
	{
		for (int i=0;i<320000;i++) vis[i] = 0;
		for (int i=0;i<50000 ;i++) vis[i] = 1;
		random_shuffle(vis, vis+300000);


		init(num_bidders, B, T, r, config);

		Strategy * stg[10];
		int stg_cnt = 6;

		stg[0]	= new BasicPrimal_Dual_ver1(num_bidders, B, T, r, "BA1");
		stg[1]	= new BasicPrimal_Dual_ver2(num_bidders, B, T, r, "BA2");
		stg[2]	= new BasicPrimal_Dual_ver3(num_bidders, B, T, r, "BA3");

		stg[3]	= new ContractFirstAllocation(num_bidders, B, T, r, "CA");
		stg[4]	= new GreedyAllocation	(num_bidders, B, T, r, "GA");
		stg[5]	= new AdAllocation	 	(num_bidders, B, T, r, "propose");

		for (int i=0;i<stg_cnt;i++)
			simulator(stg[i], vis, ad_per_video);
		/*
		simulator(BA2, vis, ad_per_video);
		simulator(BA3, vis, ad_per_video);
		simulator(CA , vis, ad_per_video);
		simulator(GA , vis, ad_per_video);
		simulator(propose ,vis, ad_per_video);
*/

		puts("parameters:");
		printf("B: (%d, %d)\n", config.B_base, config.B_range);
		printf("T: (%d, %d)\n", config.T_base, config.T_range);
		printf("r: (%d, %d)\n", config.r_base, config.r_range);
		puts("");

	//	double p11,p12,p13,p2,p3,p4,p5;

		for (int i=0;i<stg_cnt;i++)
			stg[i]->get_profit();
				/*
		p11= BA1->get_profit();
		p12= BA2->get_profit();
		p13= BA3->get_profit();
		p2 = CA->get_profit();
		p3 = GA->get_profit();
		p4 = propose->get_profit();
		*/
	}
}
/*
 * 10 10
 * 8 8
 * --------
 * 10 10
 * 12 10
 * 12 12
 * */
