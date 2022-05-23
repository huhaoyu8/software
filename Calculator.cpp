#include<bits/stdc++.h>
#include<Windows.h>
using namespace std;

const int Max = 90000005;
bool Out = true;

bool Set();
void Show(vector<int>& a); 
void Add(vector<int>& a, vector<int>b);
void Add(vector<int>& a, int b);
void TranThem(string beg, string end, string str);
void TranNum();
vector<int> NumStl(int a);
void Multi(vector<int>& a, int b);
vector<int> GetNum(ifstream& in, int& Order);
void Menu();

bool Set() {
	while (1) {
		cout << "\t\t请设置输出方式\n";
		cout << "\t\t1.记事本输出\n";
		cout << "\t\t2.黑窗输出\n";
		string option;
		cin >> option;
		if (option == "1")return false;
		else if (option == "2")return true;
		else cout << "请仅输入单个阿拉伯数字，重试一下" << endl, Sleep(1500), system("cls");
	}
}

void Show(vector<int>&a) {
	if (Out) {
		bool start=false;
		for (auto it = a.end(); it != a.begin(); it++) {
			if (*it && start)start = true;
			else if(start)cout << *it;
		}
	}
	else {
		ofstream out("转换后.txt");
		for (auto it = a.begin(); it != a.end(); it++) {
			out << *it;
		}
		cout << "请到“转换后.txt”文件中查看";
	}
}

void Add(vector<int>& a, vector<int>b) {
	long long int MaxLen = max(a.size(), b.size());
	a.size() < b.size() ? a.resize(MaxLen) : b.resize(MaxLen);
	for (int i = 0; i < MaxLen; i++) {
		a[i] += b[i];
	}
	a.push_back(0);
	for (auto it = a.begin(); it != a.end() - 1; it++) {
		while (*it > 9) {
			*it -= 10;
			it++;
			*it++;
			it--;
		}
	}
}

void Add(vector<int>& a, int b) {
	a[0] += b;
	for (auto it = a.begin(); it != a.end() - 1; it++) {
		while (*it > 9) {
			*it -= 10;
			it++;
			*it++;
			it--;
		}
	}
}

void TranThem(string beg, string end, string str) {
	ifstream In210("TwoTen.txt");
	ifstream In810("EightTen.txt");
	ifstream In1610("OxTen.txt");

	vector<int>TranEnd(Max, 0);
	if (beg == "2" && end == "8") {
		if (str.size() % 3 == 1)str.push_back('0'), str.push_back('0');
		else if (str.size() % 3 == 2)str.push_back('0');
		for (int i = 2, j = 0; i <= str.size() - 1; i += 3) {
			TranEnd[j++] = ((str[i] - '0') * 4 + (str[i - 1] - '0') * 2 + (str[i - 2] - '0'));
		}
	}
	else if (beg == "2" && end == "16") {
		if (str.size() % 4 == 1)str.push_back('0'), str.push_back('0'), str.push_back('0');
		else if (str.size() % 4 == 2)str.push_back('0'), str.push_back('0');
		else if (str.size() % 4 == 3)str.push_back('0');
		for (int i = 3, j = 0; i <= str.size() - 1; i += 4) {
			TranEnd[j++] = ((str[i] - '0') * 8 + (str[i - 1] - '0') * 4 + (str[i - 2] - '0') * 2 + (str[i - 3] - '0'));
		}
	}
	else if (beg == "8" && end == "2") {
		int j = 0;
		for (auto it = str.begin(); it != str.end(); it++) {
			int TempNum = *it - '0', a = 0, b = 0, c = 0;
			if (TempNum >= 4)a = 1, TempNum -= 4;
			if (TempNum >= 2)b = 1, TempNum -= 2;
			if (TempNum >= 1)c = 1, TempNum -= 1;
			TranEnd[j++] = c, TranEnd[j++] = b, TranEnd[j++] = a;
		}
	}
	else if (beg == "8" && end == "16") {
		TranThem("8", "2", str);
		TranThem("2", "16", str);
	}
	else if (beg == "16" && end == "2") {
		int j = 0;
		for (auto it = str.begin(); it != str.end(); it++) {
			int TempNum = (*it >= '0' && *it <= '9') ? *it - '0' : *it - 'A' + 9, a = 0, b = 0, c = 0, d = 0;
			if (TempNum >= 8)TempNum -= 8, a = 1;
			if (TempNum >= 4)TempNum -= 4, b = 1;
			if (TempNum >= 2)TempNum -= 2, c = 1;
			if (TempNum >= 1)TempNum -= 1, d = 1;
			TranEnd[j++] = d, TranEnd[j++] = c, TranEnd[j++] = b, TranEnd[j++] = a;
		}
	}
	else if (beg == "16" && end == "8") {
		TranThem("16", "2", str);
		TranThem("2", "8", str);
	}
	else if (beg == "2" && end == "10") {
		for (int i = 1, j = 0; i < str.size(); i++) {
			if (str[i] == '1')Add(TranEnd, GetNum(In210, i));
		}
		if (str[0] = '1')Add(TranEnd, 1);
	}
	else if (beg == "8" && end == "10") {
		for (int i = 1, j = 0; i < str.size(); i++) {
			vector<int >TempV = GetNum(In810, i);
			Multi(TempV, str[i] - '0');
			if (str[i] != '0')Add(TranEnd, TempV);
		}
		if (str[0] != '0')Add(TranEnd, str[0] - '0');
	}
	else if (beg == "16" && end == "10") {
		for (int i = 1, j = 0; i < str.size(); i++) {
			vector<int >TempV = GetNum(In1610, i);
			int num = (str[i] > '0' && str[i] < '9') ? str[i] - '0' : str[i] - 'A' + 10;
			Multi(TempV, num);
			if (str[i] != '0')Add(TranEnd, TempV);
		}
		if (str[0] != '0')Add(TranEnd, (str[0] > '0' && str[0] < '9') ? str[0] - '0' : str[0] - 'A' + 10);
	}

	Show(TranEnd);
}

void TranNum() {
	string beg, end, str;
	do {		//监测输入是否达标
		cout << "请输入原进制(仅输入阿拉伯数字)" << endl;
		cin >> beg;
		cout << "请输入目标进制(仅输入阿拉伯数字)" << endl;
		cin >> end;
		cout << "请输入转换数(仅输入字符，勿输入正负号)" << endl;
		cin >> str;
		if ((beg != "2" && beg != "8" && beg != "10" && beg != "16") || (end != "2" && end != "8" && end != "10" && end != "16")) {
			cout << "请重新输入进制(阿拉伯数字表示)", Sleep(1500), system("cls");//重新输入
		}
		else break;
	} while (1);

		int num;
		if (beg.size() > 1)num = (beg[0] - '0') * 10 + (beg[1] - '0');
		else num = beg[0] - '0';
		for (int i = 0; i < str.size(); i++) {	//监测输入是否达标
			if (!('0' <= str[i] && str[i] <= ('0' + num -1) && num != 16) && !(num == 16 && ((str[i] >= '0' && str[i] <= '9') || (str[i] >= 'A' && str[i] <= 'F')))) {
				cout << "请重新输入要转换数";
				cin >> str;
				i = -1;
			}
		}

	reverse(str.begin(), str.end());
	TranThem(beg, end,str);
}

vector<int> NumStl(int a) {
	vector<int>TempVe;
	while (a) {
		TempVe.push_back(a % 10);
		a /= 10;
	}
	return TempVe;
}

void Multi(vector<int>& a, int b) {
	for (auto it = a.begin(); it != a.end(); it++) {
		*it *= b;
	}
	for (auto it = a.begin(); it != a.end() - 1; it++) {
		while (*it > 9) {
			*it -= 10;
			it++;
			*it++;
			it--;
		}
	}
}

vector<int> GetNum(ifstream &in,int &Order) {
	vector<int>ReadNum;
	string TempStream;
	int prefix = 0;
	for (int i=1;i!=Order;i++, getline(in, TempStream)) {
		auto it = TempStream.begin();
		while((* it) != '#')it++;
		it++;

		while (it != TempStream.end()) {
			ReadNum.push_back(*it - '0');
		}
		reverse(ReadNum.begin(), ReadNum.end());
	}
	return ReadNum;
}

void Menu() {
	bool state = true;
	string opt;
	while (state) {
		cout << "\t\t1.转换\n";
		cout << "\t\t2.设置\n";
		cout << "\t\t3.帮助\n";
		cout << "\t\t4.退出\n";
		cin >> opt;
		if (opt.length() == 1 && opt[0] == '1')	TranNum();
		else if (opt.length() == 1 && opt[0] == '2')Out = Set();
		else if (opt.length() == 1 && opt[0] == '3')cout << "此计算器用于二、八、十、十六各进制的转换\n仅支持约5千万位的十进制转换\n";
		else if (opt.length() == 1 && opt[0] == '4')state = false;
		else cout << "请仅输入单个阿拉伯数字，重试一下" << endl, Sleep(1500), system("cls");	//重新输入
	}
}

int main() {
	Menu();
	return 0;
}
