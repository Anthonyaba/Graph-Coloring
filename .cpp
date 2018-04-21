#include<iostream>
#include<cstring>
#include<stdlib.h>
#include<fstream>
#include<string>
#include <ctime>

using namespace std;
const int n = 500;
const int m = 48;
const int k= 50;  //种群数
const int MAXexchange = 1000000;
const int MAXiter = 1000;    
const int MAXjinhua = 10000;  //对新生成解s0的优化次数
int a[n][n]; //邻接矩阵  i,j为 第i节点与j节点有边相连
int b[n][m][k]; //  节点颜色表
int c[n][k]; // 第i个节点颜色为 c n
int  newi , newj ;
int d ; //冲突改变量
int M[n][m];
int new_M[n][m];  //对孩子解的仇人表
int f, new_best_f;//目标函数
int best_f[k];
int iter ; //记录迭代次数
int T[n][m];
int R_d[10000000][2];    //对领域动作的记录表
int R_tabud[10000000][2]; 
int r1_c[n], r2_c[n];
int dd[m], R_dd[m];  //R dd对dd的记录表
int new_c[n];
//int new_b[n][m];
//生成表a[][]
void initialize_a()
{
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++) //初始化节点矩阵
			a[i][j] = 0;
	int v, w;
	ifstream infile;
	//ofstream outfile;


	infile.open("F:\\11\\\instances\\DSJC500.5.col", ios::in);
	if (!infile.is_open())
		cout << "open failure" << endl;
	while (!infile.eof())
	{
		infile >> v >> w;
		a[v - 1][w - 1] = 1;
		a[w - 1][v - 1] = 1;

	}
	infile.close();
	//outfile.open()
	//for (int i = 0; i < 50; i++)
	//	for (int j = 0; j < 50; j++)
	//		cout << a[i][j] << ' ';

};   


void initialize(int k1) {  //初始化  k1为对第 k1个初始解的生成
	newi = 0, newj = 0;
	iter = 0;
	
	f = 0;   //

	for (int i = 0; i < n; i++)
		c[i][k1] = rand() % m;              //初始化对应节点颜色

	for (int i = 0; i < n; i++)     //对b矩阵置0
		for (int j = 0; j < m; j++)
			b[i][j][k1] = 0;

	for (int i = 0; i < n; i++)        //初始化仇人表
		for (int j = 0; j < m; j++)
			M[i][j] = 0;

	for (int i = 0; i < n; i++)    //依据 a c两表生成仇人表M
		for (int j = 0; j < n; j++)
		{
			if (a[i][j] == 1)
			{
				int j1 = c[j][k1];
				M[i][j1]++;
			}

			//cout << M[i][j]<<" ";
			//if (j == m - 1)cout << "\n";
		}

	for (int i = 0; i < 10000; i++)       //记录表的初始化
	{
		for (int j = 0; j < 2; j++)
		{
			R_tabud[i][j] = 0;
			R_d[i][j] = 0;
		}
	}


	for (int i = 0; i < n; i++)   //初始化禁忌表
		for (int j = 0; j < m; j++)
			T[i][j] = 0;

	for (int i = 0; i < n; i++)    //初始化冲突数
		for (int j = 0; j < n; j++)
			if (a[i][j] == 1 && c[i][k1] == c[j][k1])
				f++;
	f = f / 2;
	best_f[k1] = f;

	cout << f << " ";

}

void new_initialize() {
	//禁忌步骤初始化
	
		newi = 0, newj = 0,iter =0;
		f = 0;
		for (int i = 0; i < n; i++)        //初始化仇人表
			for (int j = 0; j < m; j++)
				new_M[i][j] = 0;
		for (int i = 0; i < n; i++)    //依据 a c两表生成新仇人表M
			for (int j = 0; j < n; j++)
			{
				if (a[i][j] == 1)
				{
					int j1 = new_c[j];
					new_M[i][j1]++;
				}
			}
		for (int i = 0; i < m; i++)
			R_dd[i] = 0;
		for (int i = 0; i < 1000000; i++)       //记录表的初始化
		{
			for (int j = 0; j < 2; j++)
			{
				R_tabud[i][j] = 0;
				R_d[i][j] = 0;
			}
		}


		for (int i = 0; i < n; i++)   //初始化禁忌表
			for (int j = 0; j < m; j++)
				T[i][j] = 0;

		for (int i = 0; i < n; i++)    //初始化冲突数
			for (int j = 0; j < n; j++)
				if (a[i][j] == 1 && new_c[i] == new_c[j])
					f++;
		f = f / 2;
		new_best_f = f;
	
}

void getmove(int k1)
{

	d = 10000;//初始为最大整x

	int tmp;//临时变量
	int tabu_d = 10000;
	int count = 0, tabu_count = 0;
	int A = best_f[k1] - f;
	int c_color;          //当前结点颜色
	int *h_color;        //邻接颜色表行首指针
	int *h_tabu;           //禁忌表行首指针
	int c_color_table;//当前结点颜色表的值
	for (int i = 0; i < n; i++) {
		c_color = c[i][k1];
		h_color = M[i];
		c_color_table = h_color[c_color];  //冲突值

		if (h_color[c_color] > 0) {   //M[i][j]冲突节点

			h_tabu = T[i];
			for (int j = 0; j < m; j++) {
				if (c_color != j) {
					tmp = h_color[j] - c_color_table;  //△

					if (h_tabu[j] <= iter) {

						if (tmp <= d) {
							if (tmp < d) {
								count = 0;
								d = tmp;
							}
							count++;
							R_d[count - 1][0] = i;
							R_d[count - 1][1] = j;
						}
					}
					else {                  //禁忌移动
						if (tmp <= tabu_d) {
							if (tmp < tabu_d) {
								tabu_d = tmp;
								tabu_count = 0;
							}
							tabu_count++;
							R_tabud[tabu_count - 1][0] = i;
							R_tabud[tabu_count - 1][1] = j;

						}
					}
				}
			}
		}
	}
	tmp = 0;
	if (tabu_d < A && tabu_d < d) {
		d = tabu_d;
		tmp = rand() % tabu_count;       //选取记录表中的随机一个动作
		newi = R_tabud[tmp][0];
		newj = R_tabud[tmp][1];
	}
	else
	{
		/*if (count == 0)
		tmp = count;
		else*/
		tmp = rand() % count;
		newi = R_d[tmp][0];
		newj = R_d[tmp][1];
	}
}

void new_getmove(int c[],int M[][m])
{

	d = 10000;//初始为最大整x

	int tmp;//临时变量
	int tabu_d = 10000;
	int count = 0, tabu_count = 0;
	int A = new_best_f - f;
	int c_color;          //当前结点颜色
	int *h_color;        //邻接颜色表行首指针
	int *h_tabu;           //禁忌表行首指针
	int c_color_table;//当前结点颜色表的值
	for (int i = 0; i < n; i++) {
		c_color = c[i];
		h_color = M[i];
		c_color_table = h_color[c_color];  //冲突值

		if (h_color[c_color] > 0) {   //M[i][j]冲突节点

			h_tabu = T[i];
			for (int j = 0; j < m; j++) {
				if (c_color != j) {
					tmp = h_color[j] - c_color_table;  //△

					if (h_tabu[j] <= iter) {

						if (tmp <= d) {
							if (tmp < d) {
								count = 0;
								d = tmp;
							}
							count++;
							R_d[count - 1][0] = i;
							R_d[count - 1][1] = j;
						}
					}
					else {                  //禁忌移动
						if (tmp <= tabu_d) {
							if (tmp < tabu_d) {
								tabu_d = tmp;
								tabu_count = 0;
							}
							tabu_count++;
							R_tabud[tabu_count - 1][0] = i;
							R_tabud[tabu_count - 1][1] = j;

						}
					}
				}
			}
		}
	}
	tmp = 0;
	if (tabu_d < A && tabu_d < d) {
		d = tabu_d;
		tmp = rand() % tabu_count;       //选取记录表中的随机一个动作
		newi = R_tabud[tmp][0];
		newj = R_tabud[tmp][1];
	}
	else
	{
		/*if (count == 0)
		tmp = count;
		else*/
		tmp = rand() % count;
		newi = R_d[tmp][0];
		newj = R_d[tmp][1];
	}
}

void makemove(int k1) {
	int oldj = c[newi][k1];
	c[newi][k1] = newj;

	f = f + d;
	if (f < best_f[k1]) {
		best_f[k1] = f;
	};



	for (int i = 0; i< n; i++)    //更新仇人表
	{
		if (a[newi][i] == 1) {
			M[i][oldj] --;
			M[i][newj] ++;
		}
	}

	//cout<<f<<" "<<best_f<<" " << M[newi][oldj] << " " << M[newi][newj] << endl;
	//b[newi][oldj]--;
	//b[newi][newj]++;

	T[newi][oldj] = iter + f + rand() % 11;  //更新禁忌表

}

void new_makemove(int c[],int M[][m]) {
	int oldj = c[newi];
	c[newi] = newj;

	f = f + d;
	if (f < new_best_f) {
		new_best_f = f;
	};



	for (int i = 0; i< n; i++)    //更新仇人表
	{
		if (a[newi][i] == 1) {
			M[i][oldj] --;
			M[i][newj] ++;
		}
	}

	//cout<<f<<" "<<best_f<<" " << M[newi][oldj] << " " << M[newi][newj] << endl;
	//b[newi][oldj]--;
	//b[newi][newj]++;

	T[newi][oldj] = iter + f + rand() % 11;  //更新禁忌表

}


void set_c(int b[][m],int *c1)  //c1 应为 r_c[]
{
	for (int i = 0; i < n; i++)
		for (int j=0; j < m; j++)
			if (b[i][j] == 1) c1[i] = j;//b==j  写错
}   
//set c可用性未知

void set_d(int c1[n],int d1[]) {
	for (int i = 0; i < m; i++)
		d1[i] = 0;
	int tmp;
	for (int j = 0; j < n; j++)
	{
		tmp = c1[j];
		d1[tmp]++;
	}
}

int select() {
	int tmp = 0, count = 0;
	for (int i = 0; i <= m - 1; i++)
		if (dd[tmp] <= dd[i])
		{
			if (dd[tmp] < dd[i])
			{
				count = 0;
				tmp = i;
			}
			count++;
			R_dd[count - 1] = i;
		}
	int a = rand() % count;
	tmp = R_dd[a];
	return tmp;
}

int select_worst_f()
{
	int tmp = 0;
	for (int i = 0; i <= k-1; i++)
		if (best_f[tmp] < best_f[i])
			tmp = i ;
	return tmp;
}


void exchange()
{
	int r1 = rand() % k;
	int r2 = rand() % k;

	{	for (int i = 0; i < n; i++)
		new_c[i] = 0;

	//上方完成对 new_b c M的初始化


	for (int i = 0; i < n; i++)
		r1_c[i] = c[i][r1];

	for (int i = 0; i < n; i++)
		r2_c[i] = c[i][r2];

	}

	for (int i = 0; i < m; i++)
	{
		int t1 = i % 2;
		if (!t1) //对r1矩阵选出节点最多的颜色
		{
			set_d(r1_c, dd);
			int tmp = select();
			for (int j = 0; j < n; j++)
				if (r1_c[j] == tmp)
				{
					new_c[j] = i;
					r1_c[j] = -1;
					r2_c[j] = -1;
				};
		};

		if (t1) //对r2矩阵选出节点最多的颜色
		{
			set_d(r2_c, dd);
			int tmp = select();
			for (int j = 0; j < n; j++)
				if (r2_c[j] == tmp)
				{
					new_c[j] = i;
					r2_c[j] = -1;
					r1_c[j] = -1;
				};
		};


	};

	for (int i = 0; i < n; i++)
	{
		if (r1_c[i] != -1)
		{
			int tmp = rand() % m;
			new_c[i] = tmp;
			r2_c[i] = -1;
		}
	};
	for (int i = 0; i < n; i++)
		if (r2_c[i] != -1)
		{
			int tmp = rand() % m;
			new_c[i] = tmp;
		}
}

void main() {
	srand((unsigned)time(0));
	
	clock_t start, finish;
	start = clock();
	

	initialize_a();
	for(int i=0;i<k;i++){
		
		initialize(i);
		while (iter < MAXiter && best_f[i] > 0) {

			
			getmove(i);
			makemove(i);
			iter++;
			
		/*cout << f << " "
		<<M[newi][newj]<<" "<<M[newi][oldj]<<"\n"; */
	    }
		
		cout << best_f[i] << endl;
		//set_b(i, b);//设置k个初始解的b矩阵
		if (!(best_f[i] > 0)) goto end1;
	};

	
	for (int i = 0; i < MAXexchange; i++)
	{
		exchange();
		new_initialize();

		while (iter < MAXjinhua && new_best_f > 0) {   //对新解优化次数
			new_getmove(new_c, new_M);
			new_makemove(new_c, new_M);
			iter++;

		}
		cout << new_best_f << "\n";
		if (!(new_best_f > 0)) break;

		int TMP = select_worst_f();

	
		cout << TMP << "\n";
		if (new_best_f < best_f[TMP])
		{
			for (int i = 0; i < n; i++)
				c[i][TMP] = new_c[i];
			
			best_f[TMP] = new_best_f;
		}
		cout << select_worst_f();
	}
end1:
	finish = clock();
	
	cout<< finish - start << "(ms) ";
	
	/*
	cout << iter << " "
		<< best_f << " "
		<< finish - start << "(ms) ";

	
	for (int i = 0; i< n; i++)
	{
		cout << c[i] << " ";
	};

	*/

	ofstream outfile;

	
	outfile.open("F:\\11\\\instances\\1.txt", ios::app);
	if (!outfile.is_open())
		cout << "open failure" << endl;
	outfile << k << "   " << finish - start << "(ms) ";
	
	/*
	outfile << "DSJC500.5.col, "
		<< iter << ", "
		<< best_f << ", "
		<< finish - start << "(ms), " << "(" << m << ")";

	for (int i = 0; i< n; i++)
	{
		outfile << c[i] << " ";
	};*/
	outfile << endl;
	outfile.close();
	
	system("pause");
	
}
