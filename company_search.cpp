#include <iostream>
using namespace std;
#include<fstream>
#include<sstream>
#include<string>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<mysql/mysql.h> 
#include<regex.h>
#define maxn 5000000
#define maxm 50000000
#define HOST "10.146.19.233"
#define USERNAME "crawler"
#define PASSWORD "xxxxxx"
#define DATABASE "crawler_all"
#define C_content 13
#define C_us_id 20
#define C_number 100000

char data[maxm];//树节点的字符
int leftchild[maxm];//树节点儿子群的左位置
int rightchild[maxm];//树节点儿子群的右位置
int fail[maxm];//树节点在AC自动机算法中的fail指针
int father[maxm];//树节点的父亲
int f[maxm];//若该节点为关键词的结尾，指向输入关键词中的位置
int n;
int m;
unsigned int t;
string s[maxn];//输入的关键词
string ss;
int len[maxn];//输入的关键词长度

int total;
int number[maxn];//记录此次搜索到的关键词序号队列

//处理区间P，实际上就是在建立树节点P
int x[maxm];//建立树用（i区间的左位置）PS：有点像快排
int y[maxm];//建立树用（i区间的右位置）
int z[maxm];//建立树用（i区间在树中的深度）


MYSQL *my_connection;
int res;
MYSQL_RES *res_ptr;
MYSQL_FIELD *field;
MYSQL_ROW result_row;
int row,column;

void mysql_connect_init();
void auto_ac(char* content);
void auto_ac_init(char* filename);
void readfile(char* filename);
bool mins(string x,string y);
void maketree();
int getfs(int x);
void check(string id,string num,string end);
string itoc(int x);
void sort(int x,int y);

int main(int argc,char* argv[]){
    mysql_connect_init();
	mysql_autocommit(my_connection,0);
    auto_ac_init(argv[2]);
    int j = atoi(argv[1]);
    int p,q;
    string cp,cq,ce;
    p = C_number * j + 1;
    q = C_number * (j + 1);
    cp = itoc(p);
    cq = itoc(q);
	ce = argv[2];
    cout<< cp << " " <<cq << endl;
    check(cp,cq,ce);
	//mysql_autocommit(my_connection,1);
    mysql_close(my_connection);
}

string itoc(int x){
    string p;
    stringstream q;
    q << x;
    q >> p;
    return p;
}

void auto_ac(char* content){
    string ss = content;
    bool flag;
    int p,q,i;
    q = 0;
    for (p = 0;p < int(ss.length());p++){
        flag = false;
        while (!flag){
            for (i = leftchild[q];i <= rightchild[q];i++)
                if (ss[p] == data[i]){
                    flag =true;
                    q = i;
                    break;
                }
            if (flag) break;
            if (q!=0) q = fail[q];
            if (q==0) break;
        }
        if (f[q] != -1){
			number[total] = f[q];
			total++;	
		}
    }
}

void check(string id,string num,string end){
	string filename ="./data/"+ id + "-" + num + "." + end;
	ofstream fout(filename.c_str(),ios::binary);
	char ch[100000];
    string result;
	string error;
	string TABLES = "crawl2";
    string mysql_getid = id;
    string mysql_getnum = num;
    string mysql_us_id;
    string mysql_get = "select * from " + TABLES + " where us_id >= " + mysql_getid +" and us_id <= " + mysql_getnum;
    //string mysql_get = "select * from crawl_enter1 where us_id = 3";
    res = mysql_query(my_connection,mysql_get.c_str());
    if (res){
		error = mysql_error(my_connection);
        cout << error << endl;
    }
    else{
        res_ptr = mysql_store_result(my_connection);
        if (res_ptr){
            column = mysql_num_fields(res_ptr);
            row = mysql_num_rows(res_ptr);
            printf("查询到 %d 行 \n", row);
            for (int i=1;i<row+1;i++){    
                result_row = mysql_fetch_row(res_ptr);
				result = result_row[C_content];
				if (result.length()<=90){
					continue;
				}
				total = 0;
                auto_ac(result_row[C_content]);
				if (total == 0) continue;
				result = result_row[C_us_id];
				for (int j = 0; j < result.length();j++){
					ch[0] = result[j];
					fout.write(ch,1);
				}
				for (int j = 0; j < total ;j++){
					ch[0] = '\t';
					fout.write(ch,1);
					result = s[number[j]];
					for (int k = 0;k < result.length();k++){
						ch[0] = result[k];
						fout.write(ch,1);
					}	
				}
				ch[0] = '\n';
				fout.write(ch,1);
            } 
        }
        else printf("no result!");
    }
}

void mysql_connect_init(){
    my_connection = mysql_init(NULL);
    if (NULL != mysql_real_connect(my_connection,HOST,USERNAME,PASSWORD,DATABASE,0,NULL,0)){
        printf("数据库连接成功！\n");
        mysql_query(my_connection,"set names utf8");
    }
    else{
        printf("数据库连接失败！\n");
    }
}

void readfile(char *filename){
    ifstream fin(filename ,ios::binary);
    if (!fin){
        printf("open file wrong!");
        return;
    }
    n = 0;
    t = 0;
    while (getline(fin,ss)){ 
        s[n] = ss.substr(0,ss.length());
        if (s[n].length() > t) t = s[n].length();  
        n++;
    }
}

int getfs(int x){
    int lens = int(s[x].length());
    int i = 0;
    int j = 0;
    int k = 0;
    while (k <= lens - 1){
        for (i = leftchild[j];i <= rightchild[j];i++){
            if (data[i] == s[x][k]){
                j = i;
                k++;
                break;
            }        
        }
    }
    return j;
}

void maketree(){
    x[0] = 0;
    y[0] = n-1;
    z[0] = 0;
    int p,q,i,j,k;
    p = 0;
    q = 1;
	//处理区间P，实际上就是在建立树节点P
    while (p < q){//队列中仍有未处理过的区间
        i = x[p];//区间P的左指针
        k = z[p];//区间P的深度
        if (x[p] == y[p] && int(s[x[p]].length()) == z[p]){//如果区间P的左指针字符与右指针字符相等且对应的关键词长度等于当前树的深度
            p++;//查看下一个区间
            continue;
        }
        while (i <= y[p]){//当i小于区间P的右指针时
            j = i;
            while (j < y[p] && s[i][k] == s[j+1][k]){//当指针j指向的关键词的字符与j+1指向的关键词字符相等时
                j++;
            }
            if (s[i][k] == NULL){//如果这些字符都是空那么这几条关键词已经处理完了，不用再分割区域
                i = j+1;
                continue;
            }
            data[q] = s[i][k];//合并这些字符并赋值给树节点Q
            father[q] = p;//记录他们的父亲是P
            x[q] = i;//分割后区间Q的左指针
            y[q] = j;//分割后区间Q的右指针
            z[q] = z[p] + 1;//分割后区间Q的深度
            q++;//队列加1
            i = j+1;
        }
        p++;//P处理过，处理下一个区间
    }
    m = q;
    p = 0;
    q = 0;
	//整理左右儿子
    while (q < m-1){
        if (father[q+1] != p) p++;//第一位可能出错，所以加个判断
        leftchild[p] = q+1;
        while (father[q+1] == p) q++;//一直找到最后一个父亲是P的节点
        rightchild[p] = q;
        p++;
    }
	//int f[maxm];//若该节点为关键词的结尾，指向输入关键词中的位置
    for (i = 0;i < m;i++) f[i] = -1;//先全部认为不是关键词的结尾
    for (i = 0;i < n;i++) f[getfs(i)] = i;//getfs(i)代表i节点所对应的关键词序号，没有则为0
    fail[0] = -1;//修正0节点
	//fail指针的规则，类似kmp
    for (i = 1;i < m;i++){
        if (father[i] == 0) fail[i] = 0;//父亲是0，fail指向0

        else{
            p = fail[father[i]];//暂时指向他父亲的fail指针
            while (p != -1){//P不为根节点
                for (j = leftchild[p];j <= rightchild[p];j++){//枚举儿子
                    if (data[j] == data[i]){//如果i节点与P节点的儿子中某一个节点j相同，
                        fail[i] = j;//i节点的fail指针指向j节点
                        p = 0;//break以后p=fail[p]，也就是p=-1可以直接退出while循环
                        break;
                    }
                }
                p = fail[p];//若p为0，则退出while循环。若p不为0，则当前fail指针不匹配，找寻当前p节点的fail节点，这一点跟kmp完全相似
            }
        }
    }
}

void auto_ac_init(char *filename){
    readfile(filename);
    maketree();
}

