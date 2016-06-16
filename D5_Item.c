#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<malloc.h>
#include<math.h>
#include<string.h>
#include<windows.h>

#define RR 1.0056
//此数值取决于道具Rank,为Rank/25000,修罗道具Rank为标准值+100,如修罗最强武器/秘宝都是40+100=140.考虑到没有人跑非最强道具,此数字不可在配置文件修改

char Stat[][4] = { "HP","SP","ATK","DEF","INT","RES","HIT","SPD" };
//能力值名称

struct item {
	int RARE;
	//稀有度

	int LV;
	//等级

	int KB;
	//击破点数

	__int64 STAT[8][2];
	//能力值与住人值数组

	double BASE[8];
	//基础值数组

}TRAP = { -1,-1,-1,{ 0 },{ 0 } };

void print(struct item x) {
	int i = 0;
	printf("道具稀有度:%d\n", x.RARE);
	printf("道具等级:%d\n", x.LV);
	printf("道具击破点数:%d\n", x.KB);
	for (i = 0; i<8; i++)
		printf("%-3s:%9I64d 基础值:%12.4lf 住人:%5I64d\n", Stat[i], x.STAT[i][0], x.BASE[i], x.STAT[i][1]);
	printf("道具能力值读取完毕\n\n");
}

double MLV(int lv) {
	if (lv <= 10)
		return 0.04*lv;
	if (lv <= 30)
		return 0.4 + 0.03*(lv - 10);
	if (lv <= 85)
		return 1 + 0.02*(lv - 30);
	if (lv <= 250)
		return 2.1 + 0.01*(lv - 85);
	if (lv <= 500)
		return 3.75 + 0.005*(lv - 250);
	if (lv <= 1000)
		return 5 + 0.0002*(lv - 500);
	else
		return 5.1 + 0.0001*(lv - 1000);
}


void UpdateG(struct item *trap, int growth, int nRARE, int nLV, int nKB) {
	int i;
	if (nRARE != 0)//0表示不更新
		(*trap).RARE = nRARE;
	if (nLV != 0)
		(*trap).LV = nLV;
	if (nKB != 0)
		(*trap).KB = nKB;
	while (growth>0) {
		for (i = 0; i<8; i++) {
			(*trap).BASE[i] = (*trap).BASE[i] * RR + (*trap).STAT[i][1] / 100000.;
			if (i == 0) {
				(*trap).BASE[i] = (*trap).BASE[i] * RR + (*trap).STAT[i][1] / 100000.;
				if ((*trap).BASE[0]>2e7)
					(*trap).BASE[0] = 2e7;
			}
			if (i>0)
				if ((*trap).BASE[i]>99999)
					(*trap).BASE[i] = 99999;
		}
		growth--;
	}
	for (i = 0; i<8; i++) {
		(*trap).STAT[i][0] = ((*trap).BASE[i] * (1 + MLV((*trap).LV) + (*trap).KB / 100.) + (*trap).LV)*(1 + (*trap).RARE / 100.);
	}
	printf("道具稀有度更新为%d,等级更新为%d,击破点数更新为%d后能力值为\n", (*trap).RARE, (*trap).LV, (*trap).KB);
	print(*trap);
	printf("更新完毕\n\n");
}

int NestCount(double cur, __int64 inno, __int64 max) {
	int i = 0;
	while (cur<max) {
		cur = RR*cur + inno / 100000.;
		i++;
	}
	return i;
}
void Check(struct item trap) {
	int i;
	int *point = (int*)malloc(sizeof(int) * 8);
	*point = NestCount(trap.BASE[0], trap.STAT[0][1], 2e7) / 2;
	printf("%-3s还需要%4d成长点达到成长上限\n", Stat[0], point[0]);
	for (i = 1; i<8; i++) {
		*(point + i) = NestCount(trap.BASE[i], trap.STAT[i][1], 99999);
		printf("%-3s还需要%4d成长点达到成长上限\n", Stat[i], point[i]);
	}
	printf("\n");
}

double CBase(__int64 STAT, struct item trap) {
	return (STAT / (1 + trap.RARE / 100.) - trap.LV) / (1 + MLV(trap.LV) + trap.KB / 100.);
}

int main() {

	int i;
	int Growth, RARE, LV, KB;

	//FILE *fp = fopen("ItemStat.txt", "r");
	//if (fp == NULL) {
	//	printf("初始文件读入错误\n");
	//	return 0;
	//}
	//fscanf(fp, "%d%d%d", &TRAP.RARE, &TRAP.LV, &TRAP.KB);

	//for (i = 0; i<8; i++) {
	//	fscanf(fp, "%I64d%I64d", &TRAP.STAT[i][0], &TRAP.STAT[i][1]);
	//	TRAP.BASE[i] = CBase(TRAP.STAT[i][0], TRAP);
	//}
	//fclose(fp);

	TRAP.RARE = GetPrivateProfileInt(TEXT("com"), TEXT("RARE"), 100,
		TEXT(".\\Config.ini"));
	TRAP.LV = GetPrivateProfileInt(TEXT("com"), TEXT("LV"), 500,
		TEXT(".\\Config.ini"));
	TRAP.KB = GetPrivateProfileInt(TEXT("com"), TEXT("KB"), 400,
		TEXT(".\\Config.ini"));

	TRAP.STAT[0][0] = GetPrivateProfileInt(TEXT("stat"), TEXT("HP"), -1,
		TEXT(".\\Config.ini"));
	TRAP.STAT[0][1] = GetPrivateProfileInt(TEXT("stat"), TEXT("HP_I"), 0,
		TEXT(".\\Config.ini"));

	TRAP.STAT[1][0] = GetPrivateProfileInt(TEXT("stat"), TEXT("SP"), -1,
		TEXT(".\\Config.ini"));
	TRAP.STAT[1][1] = GetPrivateProfileInt(TEXT("stat"), TEXT("SP_I"), 0,
		TEXT(".\\Config.ini"));

	TRAP.STAT[2][0] = GetPrivateProfileInt(TEXT("stat"), TEXT("ATK"), -1,
		TEXT(".\\Config.ini"));
	TRAP.STAT[2][1] = GetPrivateProfileInt(TEXT("stat"), TEXT("ATK_I"), 0,
		TEXT(".\\Config.ini"));

	TRAP.STAT[3][0] = GetPrivateProfileInt(TEXT("stat"), TEXT("DEF"), -1,
		TEXT(".\\Config.ini"));
	TRAP.STAT[3][1] = GetPrivateProfileInt(TEXT("stat"), TEXT("DEF_I"), 0,
		TEXT(".\\Config.ini"));

	TRAP.STAT[4][0] = GetPrivateProfileInt(TEXT("stat"), TEXT("INT"), -1,
		TEXT(".\\Config.ini"));
	TRAP.STAT[4][1] = GetPrivateProfileInt(TEXT("stat"), TEXT("INT_I"), 0,
		TEXT(".\\Config.ini"));

	TRAP.STAT[5][0] = GetPrivateProfileInt(TEXT("stat"), TEXT("RES"), -1,
		TEXT(".\\Config.ini"));
	TRAP.STAT[5][1] = GetPrivateProfileInt(TEXT("stat"), TEXT("RES_I"), 0,
		TEXT(".\\Config.ini"));

	TRAP.STAT[6][0] = GetPrivateProfileInt(TEXT("stat"), TEXT("HIT"), -1,
		TEXT(".\\Config.ini"));
	TRAP.STAT[6][1] = GetPrivateProfileInt(TEXT("stat"), TEXT("HIT_I"), 0,
		TEXT(".\\Config.ini"));

	TRAP.STAT[7][0] = GetPrivateProfileInt(TEXT("stat"), TEXT("SPD"), -1,
		TEXT(".\\Config.ini"));
	TRAP.STAT[7][1] = GetPrivateProfileInt(TEXT("stat"), TEXT("SPD_I"), 0,
		TEXT(".\\Config.ini"));

	for (i = 0; i<8; i++)
		TRAP.BASE[i] = CBase(TRAP.STAT[i][0], TRAP);

	print(TRAP);
	Check(TRAP);
	printf("三种议会分别给对应属性增加1/3/5点成长,自行把握开会次数以同时达到最强属性\n\n\n");
	printf("输入当前出道具界时系统提示的成长,稀有度,等级,击破点数,与进道具界时无变化则输入0,空格分开\n");


	scanf("%d%d%d%d", &Growth, &RARE, &LV, &KB);
	
	UpdateG(&TRAP, Growth, RARE, LV, KB);
	//更新后的成长点,稀有度,等级和击破点数
	
	Check(TRAP);
	//system("pause");
	return 1;
}
