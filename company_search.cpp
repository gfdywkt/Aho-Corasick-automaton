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

char data[maxm];//���ڵ���ַ�
int leftchild[maxm];//���ڵ����Ⱥ����λ��
int rightchild[maxm];//���ڵ����Ⱥ����λ��
int fail[maxm];//���ڵ���AC�Զ����㷨�е�failָ��
int father[maxm];//���ڵ�ĸ���
int f[maxm];//���ýڵ�Ϊ�ؼ��ʵĽ�β��ָ������ؼ����е�λ��
int n;
int m;
unsigned int t;
string s[maxn];//����Ĺؼ���
string ss;
int len[maxn];//����Ĺؼ��ʳ���

int total;
int number[maxn];//��¼�˴��������Ĺؼ�����Ŷ���

//��������P��ʵ���Ͼ����ڽ������ڵ�P
int x[maxm];//�������ã�i�������λ�ã�PS���е������
int y[maxm];//�������ã�i�������λ�ã�
int z[maxm];//�������ã�i���������е���ȣ�


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
            printf("��ѯ�� %d �� \n", row);
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
        printf("���ݿ����ӳɹ���\n");
        mysql_query(my_connection,"set names utf8");
    }
    else{
        printf("���ݿ�����ʧ�ܣ�\n");
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
	//��������P��ʵ���Ͼ����ڽ������ڵ�P
    while (p < q){//����������δ�����������
        i = x[p];//����P����ָ��
        k = z[p];//����P�����
        if (x[p] == y[p] && int(s[x[p]].length()) == z[p]){//�������P����ָ���ַ�����ָ���ַ�����Ҷ�Ӧ�Ĺؼ��ʳ��ȵ��ڵ�ǰ�������
            p++;//�鿴��һ������
            continue;
        }
        while (i <= y[p]){//��iС������P����ָ��ʱ
            j = i;
            while (j < y[p] && s[i][k] == s[j+1][k]){//��ָ��jָ��Ĺؼ��ʵ��ַ���j+1ָ��Ĺؼ����ַ����ʱ
                j++;
            }
            if (s[i][k] == NULL){//�����Щ�ַ����ǿ���ô�⼸���ؼ����Ѿ��������ˣ������ٷָ�����
                i = j+1;
                continue;
            }
            data[q] = s[i][k];//�ϲ���Щ�ַ�����ֵ�����ڵ�Q
            father[q] = p;//��¼���ǵĸ�����P
            x[q] = i;//�ָ������Q����ָ��
            y[q] = j;//�ָ������Q����ָ��
            z[q] = z[p] + 1;//�ָ������Q�����
            q++;//���м�1
            i = j+1;
        }
        p++;//P�������������һ������
    }
    m = q;
    p = 0;
    q = 0;
	//�������Ҷ���
    while (q < m-1){
        if (father[q+1] != p) p++;//��һλ���ܳ������ԼӸ��ж�
        leftchild[p] = q+1;
        while (father[q+1] == p) q++;//һֱ�ҵ����һ��������P�Ľڵ�
        rightchild[p] = q;
        p++;
    }
	//int f[maxm];//���ýڵ�Ϊ�ؼ��ʵĽ�β��ָ������ؼ����е�λ��
    for (i = 0;i < m;i++) f[i] = -1;//��ȫ����Ϊ���ǹؼ��ʵĽ�β
    for (i = 0;i < n;i++) f[getfs(i)] = i;//getfs(i)����i�ڵ�����Ӧ�Ĺؼ�����ţ�û����Ϊ0
    fail[0] = -1;//����0�ڵ�
	//failָ��Ĺ�������kmp
    for (i = 1;i < m;i++){
        if (father[i] == 0) fail[i] = 0;//������0��failָ��0

        else{
            p = fail[father[i]];//��ʱָ�������׵�failָ��
            while (p != -1){//P��Ϊ���ڵ�
                for (j = leftchild[p];j <= rightchild[p];j++){//ö�ٶ���
                    if (data[j] == data[i]){//���i�ڵ���P�ڵ�Ķ�����ĳһ���ڵ�j��ͬ��
                        fail[i] = j;//i�ڵ��failָ��ָ��j�ڵ�
                        p = 0;//break�Ժ�p=fail[p]��Ҳ����p=-1����ֱ���˳�whileѭ��
                        break;
                    }
                }
                p = fail[p];//��pΪ0�����˳�whileѭ������p��Ϊ0����ǰfailָ�벻ƥ�䣬��Ѱ��ǰp�ڵ��fail�ڵ㣬��һ���kmp��ȫ����
            }
        }
    }
}

void auto_ac_init(char *filename){
    readfile(filename);
    maketree();
}

