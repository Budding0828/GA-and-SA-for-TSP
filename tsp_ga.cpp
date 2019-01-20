
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <algorithm>
using namespace std;

#define MAXGENS 500  // 最大进化代数
#define POPSIZE 100  // 种群数目
#define PXOVER 0.6   // 交叉概率
#define PMUTATION 0.1 // 变异概率
#define N 26 // 染色体长度(这里即为城市个数)
#define TESTTIME  30    //测试次数

int pop[POPSIZE][N];	 // 种群
int fitness[POPSIZE];

int globalBest[N];		// 最佳路线
int bestFitness = 0x7FFFFFFF; // 最短路径长度

int map[N][N];			//记录地图数据	

const char* filepath = "C://Users//56989//Desktop//dataset.txt";
ifstream infile;

//读取数据
void readData()
{
	infile.open(filepath);
	assert(infile.is_open()); //若失败,则输出错误消息,并终止程序运行 
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			infile >> map[i][j];
		}
	}
	infile.close();
}

// 种群初始化
void init()
{
	int num = 0;
	while (num < POPSIZE)
	{
		for (int i = 0; i<POPSIZE; i++)
			for (int j = 0; j<N; j++)
				pop[i][j] = j;
		num++;
		for (int i = 0; i<N - 1; i++)
		{
			for (int j = i + 1; j<N; j++)
			{
				int temp = pop[num][i];
				pop[num][i] = pop[num][j];
				pop[num][j] = temp; // 交换第num个个体的第i个元素和第j个元素
				num++;
				if (num >= POPSIZE)
					break;
			}
			if (num >= POPSIZE)
				break;
		}

		while (num < POPSIZE)
		{
			double r1 = ((double)rand()) / (RAND_MAX + 1.0);
			double r2 = ((double)rand()) / (RAND_MAX + 1.0);
			int p1 = (int)(N*r1); // 位置1
			int p2 = (int)(N*r2); // 位置2
			int temp = pop[num][p1];
			pop[num][p1] = pop[num][p2];
			pop[num][p2] = temp;    // 交换基因位置
			num++;
		}
	}

	//	for (int i = 0; i<POPSIZE; i++)
	//	{
	//		for (int j = 0; j<N; j++)
	//		{
	//			pop[i][j] = j;
	//		}
	//		random_shuffle(pop[i], pop[i] + N);
	//	}
}


int findMin()
{
	int mindis = fitness[0];
	int minindex = 0;

	for (int i = 1; i<POPSIZE; i++)
	{
		if (fitness[i] < mindis)
		{
			mindis = fitness[i];
			minindex = i;
		}
	}

	return minindex;

}

// 计算路径长度
double pathLen(int * arr)
{
	int totlen = 0;
	for (int i = 0; i < N - 1; i++)
	{
		totlen += map[arr[i]][arr[i + 1]];
	}
	totlen += map[arr[N - 1]][arr[0]];	//回到出发城市
	return totlen;
}

// 选择操作
void select()
{
	double pick;
	int choice_arr[POPSIZE][N];
	double fit_pro[POPSIZE];
	double sum = 0;
	double fit[POPSIZE]; // 适应度函数数组(距离的倒数)
	for (int j = 0; j<POPSIZE; j++)
	{
		fit[j] = 1.0 / fitness[j];
		sum += fit[j];
	}
	for (int j = 0; j<POPSIZE; j++)
	{
		fit_pro[j] = fit[j] / sum; // 概率数组
	}
	// 开始轮盘赌
	for (int i = 0; i<POPSIZE; i++)
	{
		pick = ((double)rand()) / RAND_MAX; // 0到1之间的随机数  
		for (int j = 0; j<POPSIZE; j++)
		{
			pick = pick - fit_pro[j];
			if (pick <= 0)
			{
				for (int k = 0; k<N; k++)
					choice_arr[i][k] = pop[j][k]; // 选中一个个体
				break;
			}
		}

	}
	for (int i = 0; i<POPSIZE; i++)
	{
		for (int j = 0; j<N; j++)
			pop[i][j] = choice_arr[i][j];
	}
}

//交叉操作
void crossover()
{
	double pick;
	double pick1, pick2;
	int choice1, choice2;
	int pos1, pos2;
	int temp;
	int conflict1[N]; // 冲突位置
	int conflict2[N];
	int num1, num2;
	int index1, index2;
	int move = 0; // 当前移动的位置
	while (move<N - 1)
	{
		pick = ((double)rand()) / RAND_MAX; // 用于决定是否进行交叉操作
		if (pick > PXOVER)
		{
			move += 2;
			continue; // 本次不进行交叉
		}
		// 采用部分映射杂交
		choice1 = move; // 用于选取杂交的两个父代
		choice2 = move + 1; // 注意避免下标越界
		pick1 = ((double)rand()) / (RAND_MAX + 1.0);
		pick2 = ((double)rand()) / (RAND_MAX + 1.0);
		pos1 = (int)(pick1*N); // 用于确定两个杂交点的位置
		pos2 = (int)(pick2*N);
		while (pos1 > N - 2 || pos1 < 1)
		{
			pick1 = ((double)rand()) / (RAND_MAX + 1.0);
			pos1 = (int)(pick1*N);
		}
		while (pos2 > N - 2 || pos2 < 1)
		{
			pick2 = ((double)rand()) / (RAND_MAX + 1.0);
			pos2 = (int)(pick2*N);
		}
		if (pos1 > pos2)
		{
			temp = pos1;
			pos1 = pos2;
			pos2 = temp; // 交换pos1和pos2的位置
		}
		for (int j = pos1; j <= pos2; j++)
		{
			temp = pop[choice1][j];
			pop[choice1][j] = pop[choice2][j];
			pop[choice2][j] = temp; // 逐个交换顺序
		}
		num1 = 0;
		num2 = 0;
		if (pos1 > 0 && pos2 < N - 1)
		{
			for (int j = 0; j <= pos1 - 1; j++)
			{
				for (int k = pos1; k <= pos2; k++)
				{
					if (pop[choice1][j] == pop[choice1][k])
					{
						conflict1[num1] = j;
						num1++;
					}
					if (pop[choice2][j] == pop[choice2][k])
					{
						conflict2[num2] = j;
						num2++;
					}
				}
			}
			for (int j = pos2 + 1; j<N; j++)
			{
				for (int k = pos1; k <= pos2; k++)
				{
					if (pop[choice1][j] == pop[choice1][k])
					{
						conflict1[num1] = j;
						num1++;
					}
					if (pop[choice2][j] == pop[choice2][k])
					{
						conflict2[num2] = j;
						num2++;
					}
				}

			}
		}
		if ((num1 == num2) && num1 > 0)
		{
			for (int j = 0; j<num1; j++)
			{
				index1 = conflict1[j];
				index2 = conflict2[j];
				temp = pop[choice1][index1]; // 交换冲突的位置
				pop[choice1][index1] = pop[choice2][index2];
				pop[choice2][index2] = temp;
			}
		}
		move += 2;
	}
}

// 变异操作
// 变异策略采取随机选取两个点，将其对换位置
void mutation()
{
	double pick, pick1, pick2;
	int pos1, pos2, temp;
	for (int i = 0; i<POPSIZE; i++)
	{
		pick = ((double)rand()) / RAND_MAX; // 用于判断是否进行变异操作
		if (pick > PMUTATION)
			continue;
		pick1 = ((double)rand()) / (RAND_MAX + 1.0);
		pick2 = ((double)rand()) / (RAND_MAX + 1.0);
		pos1 = (int)(pick1*N); // 选取进行变异的位置
		pos2 = (int)(pick2*N);
		while (pos1 > N - 1)
		{
			pick1 = ((double)rand()) / (RAND_MAX + 1.0);
			pos1 = (int)(pick1*N);
		}
		while (pos2 > N - 1)
		{
			pick2 = ((double)rand()) / (RAND_MAX + 1.0);
			pos2 = (int)(pick2*N);
		}
		temp = pop[i][pos1];
		pop[i][pos1] = pop[i][pos2];
		pop[i][pos2] = temp;
	}
}

//精英更新
void elitist()
{
	int best, worst;
	int bestIndex, worstIndex;

	best = fitness[0];
	worst = fitness[0];

	bestIndex = 0;
	worstIndex = 0;

	//找出最好和最坏
	for (int i = 0; i < POPSIZE; i++)
	{
		if (fitness[i] < best)
		{
			best = fitness[i];
			bestIndex = i;
		}
		else if (fitness[i] > worst)
		{
			worst = fitness[i];
			worstIndex = i;
		}
	}

	//要不更新最好
	if (best < bestFitness)
	{
		for (int i = 0; i < N; i++)
		{
			globalBest[i] = pop[bestIndex][i];
		}
		bestFitness = best;
	}
	else//要不更新最坏
	{
		for (int i = 0; i < N; i++)
		{
			pop[worstIndex][i] = globalBest[i];
		}
		fitness[worstIndex] = bestFitness;
	}
}


int main(void)
{
	
	readData();
	time_t start, finish;
	start = clock();
	double aveLen = 0.0;
	int timeTime = 0;
	while (timeTime < TESTTIME)
	{
		srand((unsigned)time(NULL));	// 初始化随机数种子
		init();

		//更新fitness
		for (int j = 0; j<POPSIZE; j++)
		{
			fitness[j] = pathLen(pop[j]);
		}

		int minindex = findMin();
		for (int j = 0; j<N; j++)
			globalBest[j] = pop[minindex][j];			// 最短路径序列
		bestFitness = fitness[minindex];

		for (int i = 0; i<MAXGENS; i++)
		{
			select();		// 选择
			crossover();	//交叉
			mutation();		//变异

			for (int j = 0; j<POPSIZE; j++)
				fitness[j] = pathLen(pop[j]); // 距离数组

			elitist();	//添加一个精英选择
			minindex = findMin();

			if (fitness[minindex] < bestFitness)
			{
				bestFitness = fitness[minindex];		// 更新最短路径
				for (int j = 0; j<N; j++)
					globalBest[j] = pop[minindex][j]; // 更新全局最短路径序列
			}
		}

		timeTime++;
		aveLen += bestFitness;
		printf("第%d次计算，最短路径长度为:%d\n", timeTime, bestFitness);
	}



	
	finish = clock(); // 计算结束
	double duration = ((double)(finish - start)) / CLOCKS_PER_SEC; // 计算耗时
	printf("遗传算法\n");
	/*
	for (int i = 0; i < N; i++)
	{
		cout << globalBest[i] << " ";
	}
	cout << endl;
	printf("最短路径长度为:%d\n", bestFitness);
	*/
	printf("程序平均耗时为:%lf秒.\n", duration/TESTTIME);
	printf("平均结果为：%2f\n", aveLen / 30);
	return 0;
}