#include <stdio.h>
#include <string.h>
#include "laplace.h"
#include "csvpackage.h"
#include <time.h>
extern int rand();
extern void srand(unsigned);
/*
函数功能：	对传入的csv文件进行处理，提取其中数据并生成拉普拉斯分布的噪音进行加噪
输入参数说明：
path		csv文件的存储位置
beta		拉普拉斯分布参数
seed	    长整型指针变量， *seed 为伪随机数的种子
*/
int csv_analysis(char* path, double beta, long int seed)
{
	FILE *original_file = fopen(path,"r+"); //读取指定路径的数据集
	struct Animals * original_data = NULL;
	original_data = csv_parser(original_file);
	int sum=0,i=0;
	double x = 0;
	while(original_data[i].name)  //循环为原始数据集内各条数据去除标识（动物名）、生成拉普拉斯噪音并加噪
	{
		if(original_data[i].carrots+x>=55)
		{
			sum++;
		}
		i++;
	}
	x = laplace_data(beta,&seed); //产生拉普拉斯随机数
	sum += x;
	// printf("Added noise: %f\n", x);
	// printf("Added noise:%f\t%s %d\t%f\n",x,"Animal",i+1,original_data[i].carrots+x); //此处分别列出了每条具体添加的噪音和加噪的结果。当投入较少预算时，可能会出现负数
	// printf("Animals which carrots cost > 55 (Under DP): %d\n",sum); //输出加噪后的数据集中，每日食用胡萝卜大于55的动物个数
	return sum;
}

/*
参数表：
seed	    长整型指针变量， *seed为伪随机数的种子
sen			数据集的敏感度
x			用于储存拉普拉斯分布噪音的临时变量
beta		隐私预算，在输入后根据公式转换为拉普拉斯分布参数
*/
int main()
{
	long int seed;
	int sen = 1;  //对于一个单属性的数据集，其敏感度为1
	double beta = 0.1;
	int query_number = 20; //支持查询次数为20轮
	int query_n;
	srand((unsigned)time( NULL )); //生成基于时间的随机种子（srand方法）
	printf("Under privacy budget %f, sanitized original data with fake animal name and laplace noise:\n",beta);
	printf("Please input the query times:");
	scanf("%d", &query_n);
	if(query_n < 1 || !query_n)
	{
		query_n = 1;
	}
	// beta = 0.1;
	// printf("Please input laplace epsilon:");
	// scanf("%lf", &beta);
	// if(beta<=0 || !beta)//当输入的beta值无效时，默认设定beta值为1
	// {
	// 	beta = 1.0;
	// } 
	// printf("Under privacy budget %f, sanitized original data with fake animal name and laplace noise:\n",beta);
	beta = beta / query_number; //每次查询分配的预算
	beta = sen / beta; //拉普拉斯机制下，实际公式的算子beta为敏感度/预算
	double* result_origin = (double*)malloc(query_n *sizeof(double));
	double sum_res1 = 0;
	for (int i = 0; i < query_n; i++) {
		seed = rand() % 10000 + 10000; //随机种子产生
		double temp = csv_analysis("./zoo.csv", beta, seed); //先调用原始数据集
		result_origin[i] = temp;
		sum_res1 += result_origin[i];
	}
	printf("Average result(origin): \t%f\n",sum_res1 / query_n); //打印平均值
	// seed = rand()%10000+10000; //随机种子产生
	// csv_analysis("./zoo.csv",beta,seed); //先调用原始数据集
	printf("==================Using neighbour dataset==================\n");
	double* result_change = (double*)malloc(query_n *sizeof(double));
	double sum_res2 = 0;
	for (int i = 0; i < query_n; i++) {
		seed = rand() % 10000 + 10000; //随机种子产生
		double temp = csv_analysis("./zoo_nb.csv", beta, seed); //先调用原始数据集
		result_change[i] = temp;
		sum_res2 += result_change[i];
	}
	printf("Average result(neighbour): \t%f\n",sum_res2 / query_n); //打印平均值
	
	// seed = rand()%10000+10000; //随机种子产生
	// csv_analysis("./zoo.csv",beta,seed); //先调用原始数据集
	// printf("==================Using neighbour dataset==================\n");
	// seed = rand()%10000+10000; //随机种子更新
	// csv_analysis("./zoo_nb.csv",beta,seed); //再调用相邻数据集
	return 0;
}

