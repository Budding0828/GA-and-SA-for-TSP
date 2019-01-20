
// tsp_sa.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <math.h>
using namespace std;

#define T0 50000.0		//初始化温度
#define Tk (1e-8)		//终止温度
#define d  0.98			// 退火系数
#define L 1000			// 每个温度时的迭代次数，即链长
#define N 26			// 城市数量
#define TESTTIME  30    //测试次数

int solution[N];		// 用于存放一个解

int map[N][N];			//记录地图数据				

const char* filepath = "C://Users//56989//Desktop//dataset.txt";
ifstream infile;

//读取数据
void readData()
{
	infile.open(filepath);
	assert(infile.is_open()); //若失败,则输出错误消息,并终止程序运行 
	for (int i = 0; i < 26; i++)
	{
		for (int j = 0; j < 26; j++)
		{
			infile >> map[i][j];
		}
	}
}

// 计算路径长度
int pathLen(int * arr)
{
	int totlen = 0;
	for (int i = 0; i < N-1; i++)
	{
		totlen += map[arr[i]][arr[i + 1]];
	}
	totlen += map[arr[N-1]][arr[0]];	//回到出发城市
	return totlen;
}

// 初始化函数
void initSolution()
{
	for (int i = 0; i<N; i++)
		solution[i] = i;  // 初始化一个解
}

// 产生一个新解
// 此处采用随机交叉两个位置的方式产生新的解
void genAnotherSolu()
{
	double r1 = ((double)rand()) / (RAND_MAX + 1.0);
	double r2 = ((double)rand()) / (RAND_MAX + 1.0);
	int pos1 = (int)(N*r1); //第一个交叉点的位置
	int pos2 = (int)(N*r2);

	int temp = solution[pos1];
	solution[pos1] = solution[pos2];
	solution[pos2] = temp;   // 交换两个点
}

// 主函数
int main(void)
{
	readData();				//读取数据
	printf("读数据完成\n\n");
	srand((unsigned)time(NULL)); 

	time_t start, finish;	//计算时间
	start = clock();		//开始计算时间
	
	int tempsolu[N];		//保留原来的解
	int tempLen = 0;

	double aveLen = 0.0;	//平均长度

	for (int testtime = 1; testtime <= TESTTIME; testtime++)
	{
		double T;
		T = T0;					//初始温度

		initSolution();			//初始化一个解
		int soluLen = pathLen(solution);

		while (T > Tk)
		{
			//printf("进入.\n");
			for (int i = 1; i <= L; i++)
			{
				memcpy(tempsolu, solution, N * sizeof(int)); // 复制数组，保留原来的解
				genAnotherSolu();							 // 产生新解

				tempLen = pathLen(tempsolu);
				soluLen = pathLen(solution);

				int dif = soluLen - tempLen;

				if (dif >= 0)//原来的解更好
				{
					double ran = ((double)rand()) / (RAND_MAX);
					if (exp(-dif / T) <= ran)   // 保留原来的解
					{
						memcpy(solution, tempsolu, N * sizeof(int));
					}
				}
			}
			T = T * d; // 降温

		}
		aveLen += pathLen(solution);
		printf("第%d次计算完成，所得路径长度为: %d\n", testtime, pathLen(solution));

	}
	aveLen = aveLen / 30;

	finish = clock(); 

	double duration = ((double)(finish - start)) / CLOCKS_PER_SEC;	// 计算时间
	
	printf("程序运行耗时:%lf秒.\n", duration);
	printf("重复30次，求得的最优路径平均值为：%2f.\n", aveLen);

	/*
	printf("模拟退火算法\n");
	printf("路径如下：\n");
	for (int i = 0; i<N ; i++)  // 输出路径
	{
		printf("%d ", solution[i]);
	}

	printf("\n最优路径长度为:%d\n", pathLen(solution));
	*/

	

	return 0;
}