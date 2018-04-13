#include<iostream>
#include<cstring>
#include<stdlib.h>
#include<fstream>
#include<string>
#include <ctime>

using namespace std;
const int n = 1000;
const int m = 89;
int a[n][n]; //邻接矩阵  i,j为 第i节点与j节点有边相连
			 //int b[n][m]; //
int c[n]; // 第i个节点颜色为 c n
int  newi = 0, newj = 0;
int d = 0; //冲突改变量
int M[n][m];
int f, best_f;//目标函数
int iter = 0; //记录迭代次数
int T[n][m];
int R_d[10000000][2];    //对领域动作的记录表
int R_tabud[10000000][2];


void firststep() {  //初始化
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++) //初始化节点矩阵
			a[i][j] = 0;
	int v, w;
	ifstream infile;
	//ofstream outfile;


	infile.open("F:\\11\\\instances\\DSJC1000.5.col", ios::in);
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



	for (int i = 0; i < n; i++)
		c[i] = rand() % m;              //初始化对应节点颜色



	for (int i = 0; i < n; i++)        //初始化仇人表
		for (int j = 0; j < m; j++)
			M[i][j] = 0;

	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
		{
			if (a[i][j] == 1)
			{
				int j1 = c[j];
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
			if (a[i][j] == 1 && c[i] == c[j])
				f++;
	f = f / 2;
	best_f = f;

	cout << f << " ";

}

void getmove()
{

	d = 10000;//初始为最大整

	int tmp;//临时变量
	int tabu_d = 10000;
	int count = 0, tabu_count = 0;
	int A = best_f - f;
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




void makemove() {
	int oldj = c[newi];
	c[newi] = newj;

	f = f + d;
	if (f < best_f) {
		best_f = f;
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

void main() {
	srand((unsigned)time(0));
	clock_t start, finish;
	start = clock();
	firststep();
	while (iter < 100000000 && best_f > 0) {


		getmove();
		makemove();
		iter++;
		/*cout << f << " "
		<<M[newi][newj]<<" "<<M[newi][oldj]<<"\n"; */
	}
	finish = clock();

	cout << iter << " "
		<< best_f << " "
		<< finish - start << "(ms) ";

	for (int i = 0; i< n; i++)
	{
		cout << c[i] << " ";
	};

	ofstream outfile;


	outfile.open("F:\\11\\\instances\\新建文本文档.txt", ios::app);
	if (!outfile.is_open())
		cout << "open failure" << endl;

	outfile << "DSJC1000.5.col, "
		<< iter << ", "
		<< best_f << ", "
		<< finish - start << "(ms), " << "(" << m << ")";

	for (int i = 0; i< n; i++)
	{
		outfile << c[i] << " ";
	};
	outfile << endl;
	outfile.close();
	system("pause");

}
