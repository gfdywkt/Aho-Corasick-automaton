#include <iostream> 
using namespace std; 
#include <fstream>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define maxn 5000000

unsigned int t;
int n;
int m;
string s[maxn];
string ss;

void read(char* filename){
	ifstream fin(filename,ios::binary);
	if (!fin){
		cerr << "没有找到文件" << filename << endl;
        return;
	}
	n = 0;
	t = 0;
	while (getline(fin,ss)){
		s[n] = ss.substr(0,int(ss.length()));
		if (s[n].length() > t) t = s[n].length();
		n++;
	}
    fin.close();
}

bool mins(string s1,string s2){
	int l1 = s1.length();
	int l2 = s2.length();
	int i=0;
	while (i<=l1&&i<=l2){
		if (s1[i]<s2[i]) return true;
		if (s1[i]>s2[i]) return false;
		i++;
	}
	if (l1>=l2) return false;
	return true;
}
/*
void paixu(int x,int y){
	if (x>=y) return;
	string s0=s[0];
	int p = x;
	int q = y;
	while (p<q){
		while (mins(s0,s[q])&&p<q) q--;
		s[p] = s[q];
		while (mins(s[p],s0)&&p<q) p++;
		s[q] = s[p];
	}
	s[p] = s0;
	paixu(x,p-1);
	paixu(p+1,y);
}
*/
void paixu(int left,int right){
	if (left >= right) return;
	int i = left - 1,j = right + 1;
	string mid = s[left + ( rand()%(right - left + 1) )];
	string xx;
	while(i < j){
		i++;
		j--;
		while (mins(s[i],mid)) i++;
		while (mins(mid,s[j])) j--;
		if (i < j){
			xx = s[i];
			s[i] = s[j];
			s[j] = xx;
		}
	}
	paixu(left,j);
	paixu(j+1,right);
}

void writeline(char* filename){
    strcat(filename,".sort");
	ofstream fout(filename,ios::binary);
	char ch[1000000];
	int len;
	printf("%d\n",n);
	for (int i=0;i<n;i++)
	{
        //while (s[i]==s[i+1]&&i+1<n){
        //    i += 1;
        //}
		len = int(s[i].length());
		for (int j=0;j<len;j++) ch[j]=s[i][j];
		ch[len] = '\n';
		//fout.write(ch,3);
		fout.write(ch,len+1);
	}
}

int main(int argc,char* argv[]){
	srand((unsigned)time(NULL));
	read(argv[1]);
	paixu(0,n-1);
	writeline(argv[1]);
}
