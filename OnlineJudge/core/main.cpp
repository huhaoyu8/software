//https://burweisnote.blogspot.com/2017/08/execlpexecvp.html
#include <cstdio>
#include <syslog.h>
#include <cerrno>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <unistd.h>
#include <ctime>
#include <cmath>
#include <cctype>
#include <string>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include "okcalls64.h"
#include <filesystem>
#include <mysql/mysql.h>
static int cpu_compensation=1;
static std::string result[15]= {
    "PD", "PR", "CI", "RJ", "AC", "PE", "WA", "TLE", "MLE", "OLE", "RE", "CE", "CO", "TR", "MC"
};
static std::string langExt[25]={
    "c", "cc", "pas", "java", "rb", "sh", "py", "php", "pl", "cs", "m", "bas", "scm", "c", "cc", "lua", "js", "go",
    "sql", "f95", "m", "cob", "R", "sb3", "cj"
};
MYSQL *conn;
int call_counter[512] = {0};//记录系统调用
static int oi_mode=0;
static int record_call=0;
static long totalTime=0;
void init_call(const int& lang){
		int i;
	memset(call_counter, 0, sizeof(call_counter));
	if (record_call){
		// recording for debuging
		for (i = 0; i < 512; i++){ call_counter[i] = 0; }
	}
	else if (lang <= LANG_CPP|| lang == LANG_CLANG || lang == LANG_CLANGPP){for (i = 0; i == 0 || LANG_CV[i]; i++){ call_counter[LANG_CV[i]] = -1; }	}
	else if (lang == LANG_PASCAL){			for (i = 0; i == 0 || LANG_PV[i]; i++) call_counter[LANG_PV[i]] = -1;	}
	else if (lang == LANG_JAVA){		for (i = 0; i == 0 || LANG_JV[i]; i++) call_counter[LANG_JV[i]] = -1;	}
	else if (lang == LANG_RUBY){		for (i = 0; i == 0 || LANG_RV[i]; i++) call_counter[LANG_RV[i]] = -1;	}
	else if (lang == LANG_BASH){		for (i = 0; i == 0 || LANG_BV[i]; i++) call_counter[LANG_BV[i]] = -1;	}
	else if (lang == LANG_PYTHON){	for (i = 0; i == 0 || LANG_YV[i]; i++) call_counter[LANG_YV[i]] = -1;	}
	else if (lang == LANG_PHP){		for (i = 0; i == 0 || LANG_PHV[i]; i++) call_counter[LANG_PHV[i]] = -1;	}
	else if (lang == LANG_PERL){for (i = 0; i == 0 || LANG_PLV[i]; i++) call_counter[LANG_PLV[i]] = -1;	}
	else if (lang == LANG_CSHARP){for (i = 0; i == 0 || LANG_CSV[i]; i++) call_counter[LANG_CSV[i]] = -1;	}
	else if (lang == LANG_OBJC){			for (i = 0; i == 0 || LANG_OV[i]; i++) call_counter[LANG_OV[i]] = -1;	}
	else if (lang == LANG_FREEBASIC){			for (i = 0; i == 0 || LANG_BASICV[i]; i++) call_counter[LANG_BASICV[i]] = -1;	}
	else if (lang == LANG_SCHEME){			for (i = 0; i == 0 || LANG_SV[i]; i++) call_counter[LANG_SV[i]] = -1;	}
	else if (lang == LANG_LUA){		for (i = 0; i == 0 || LANG_LUAV[i]; i++) call_counter[LANG_LUAV[i]] = -1;	}
	else if (lang == LANG_JS){			for (i = 0; i == 0 || LANG_JSV[i]; i++) call_counter[LANG_JSV[i]] = -1;	}
	else if (lang == LANG_GO){		for (i = 0; i == 0 || LANG_GOV[i]; i++) call_counter[LANG_GOV[i]] = -1;	}
	else if (lang == LANG_SQL){		for (i = 0; i == 0 || LANG_SQLV[i]; i++) call_counter[LANG_SQLV[i]] = -1;	}
	else if (lang == LANG_FORTRAN){	for (i = 0; i == 0 || LANG_FV[i]; i++) call_counter[LANG_FV[i]] = -1;	}
	else if (lang == LANG_MATLAB){			for (i = 0; i == 0 || LANG_MV[i]; i++) call_counter[LANG_MV[i]] = -1;	}
	else if (lang == LANG_COBOL){		for (i = 0; i == 0 || LANG_CBV[i]; i++) call_counter[LANG_CBV[i]] = -1;	}
	else if (lang == LANG_R){ for (i = 0; i == 0 || LANG_RLV[i]; i++) call_counter[LANG_RLV[i]] = -1; }
	else if (lang == LANG_SB3){ for (i = 0; i == 0 || LANG_SB3V[i]; i++) call_counter[LANG_SB3V[i]] = -1; }
	else if (lang == LANG_CJ){ for (i = 0; i == 0 || LANG_CJV[i]; i++) call_counter[LANG_CJV[i]] = -1; }
	call_counter[SYS_execve % 512] = 1; //允许系统调用execve（加载并执行一个新的程序，替换当前进程的映像）
	if (lang == LANG_RUBY) call_counter[SYS_execve % 512] = 8;
	// printf("SYS_execve:%d [%d] \n",SYS_execve % 512, call_counter[SYS_execve % 512]);
}

//从mysql获取任务信息
void get_solution_info(int solutionID, int& problemID, std::string& userID, int& language, int& contestID){
	std::string sql="SELECT problem_id, user_id, language, contest_id FROM solution where solution_id="+std::to_string(solutionID);
	mysql_real_query(conn, sql.c_str(), strlen(sql.c_str()));
	MYSQL_RES* res = mysql_store_result(conn);
	MYSQL_ROW row = mysql_fetch_row(res);
	problemID = std::stoi(row[0], nullptr, 10);
	userID = row[1];
	language = std::stoi(row[2], nullptr, 10);
	if(row[3]==nullptr) contestID=0;
	else contestID = std::stoi(row[3]);
	if(res!=nullptr) {
		mysql_free_result(res);                         // free the memory
		res=nullptr;
	}
}

//从mysql获取题目信息
void get_problem_info(int problemID, int& timeLimit, int& memoryLimit, int& spj){
	const std::string sql="SELECT time_limit, memory_limit, spj FROM problem where problem_id="+std::to_string(problemID);
    mysql_real_query(conn, sql.c_str(), strlen(sql.c_str()));
    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res);
    timeLimit = std::stoi(row[0], nullptr, 10);
    memoryLimit = std::stoi(row[1], nullptr, 10);
    spj = std::stoi(row[4], nullptr, 10);
    if(res!=nullptr) {
        mysql_free_result(res);                         // free the memory
        res=nullptr;
    }
}

//从mysql获取任务
void get_solution(const int solutionID, const int language)
{
	std::string srcPth;
	std::string sql="SELECT source FROM solution where solution_id="+std::to_string(solutionID);
	mysql_real_query(conn, sql.c_str(), strlen(sql.c_str()));
	MYSQL_RES* res = mysql_store_result(conn);
	if (res!=nullptr)
	{
		MYSQL_ROW row = mysql_fetch_row(res);
		if (row!=nullptr)
		{
			srcPth="/Main."+langExt[language];
			FILE* srcFile=fopen((srcPth).c_str(),"we");
			fprintf(srcFile,"%s",row[0]);
			mysql_free_result(res);
			res=nullptr,row=nullptr;
			fclose(srcFile);
		}
	}
}

//添加compile error信息到mysql
void addceinfo_mysql(int solutionID)
{
	char sql[1 << 16];
	char ceinfo[1 << 16];
	FILE *fp = fopen("ce.txt", "re");
	snprintf(sql, (1 << 16) - 1, "DELETE FROM compileinfo WHERE solution_id=%d",
			 solutionID);
	mysql_real_query(conn, sql, strlen(sql));
	char* cend = ceinfo;
	while (fgets(cend, 1024, fp))
	{
		cend += strlen(cend);
		if (cend - ceinfo > 16384)
			break;
	}
	*cend = '\0';
	char* end = sql;
	strcpy(end, "INSERT INTO compileinfo VALUES(");
	end += strlen(sql);
	*end++ = '\'';
	end += sprintf(end, "%d", solutionID);
	*end++ = '\'';
	*end++ = ',';
	*end++ = '\'';
	end += mysql_real_escape_string(conn, end, ceinfo, strlen(ceinfo));
	*end++ = '\'';
	*end++ = ')';
	*end = 0;
	//      printf("%s\n",ceinfo);
	if (mysql_real_query(conn, sql, end - sql))
		printf("%s\n", mysql_error(conn));
	fclose(fp);
}

//添加运行结果到mysql
void update_solutoin_mysql(int solutionID, int result, int time, int memory,int sim,int sim_s_id,double pass_rate)
{
	if (result==OJ_TL&&memory==0)result=OJ_ML;
	if (result==OJ_AC)result=OJ_AC;
	std::string sql,judger;
	if (oi_mode)sql="UPDATE solution SET result="+std::to_string(result)+",time="+std::to_string(time)+",memory="+std::to_string(memory)+"pass_rate="+std::to_string(pass_rate)+"judger="+judger+"judgetime=now() WHERE solution_id="+std::to_string(solutionID);
	else sql="UPDATE solution SET result="+std::to_string(result)+",time="+std::to_string(time)+",memory="+std::to_string(memory)+"judger="+judger+",judgetime=now() WHERE solution_id="+std::to_string(solutionID);
	mysql_real_query(conn, sql.c_str(), sql.size());
	if (sim)
	{
		sql="insert into sim(s_id,sim_s_id) values("+std::to_string(solutionID)+"on duplicate key update sim_s_id="+std::to_string(sim_s_id)+"sim="+std::to_string(sim);
	}
	mysql_real_query(conn, sql.c_str(), sql.size());
}

//去除字符串开头的空白字符，并截断字符串到第一个空白字符处
void trim(char* c){
	char buf[4096];
	strcpy(buf, c);
	char* start = buf;
	while (isspace(*start)) start++;
	char* end = start;
	while (!isspace(*end)) end++;
	*end = '\0';
	strcpy(c, start);
}

//根据用户的提交与标准答案进行对比，计算分数，并生成差异输出
float raw_text_judge(const char* infile, const char* outfile, const char* userfile, float* total_mark){
	float mark = 0;
	int total = 0;
	FILE* in = fopen(infile, "r");
	if (fscanf(in, "%d", &total) != 1) return -1;
	fclose(in);
	FILE* out = fopen(outfile, "r");
	int num = 0;
	char* user_answer = nullptr;
	size_t buf_length;
	float m[total + 1];
	char* ans[total + 1];
	*total_mark = 0;
	for (int i = 1; i <= total; i++){
		ans[i] = nullptr;
		buf_length = 0;
		if (fscanf(out, "%d", &num) != 1) return -2;
		if (i == num){
			if (fscanf(out, "%*[^\[][%f]", &m[num]) != 1) return -3;
			*total_mark += m[num];
			const size_t ans_length = getline(&ans[i], &buf_length, out);
			for (int j = ans_length - 1; '\n' == ans[i][j] || '\r' == ans[i][j]; j--){ ans[i][j] = '\0'; }
			trim(ans[i]);
		}
		else{}
	}
	fclose(out);
	FILE* user = fopen(userfile, "r");
	FILE* df = fopen("diff.out", "a");
	for (int i = 1; i <= total; i++){
		user_answer = nullptr;
		buf_length = 0;
		if (fscanf(user, "%d", &num) == EOF) continue;
		const size_t user_length = getline(&user_answer, &buf_length, user);
		int j = 0;
		for (j = user_length - 1; '\n' == user_answer[j] || '\r' == user_answer[j]; j--){ user_answer[j] = '\0'; }
		trim(user_answer);
		if (num > 0 && num <= total){
			if (strcasecmp(ans[num], user_answer) == 0 || strcasecmp(ans[num], "*") == 0 || strcasecmp(ans[num], " *")
				== 0){ mark += m[num]; }
			else{ if (true) fprintf(df, "%d Answer:%s[You:%s] -%.1f\n", i, ans[i], user_answer, m[i]); }
			m[num] = 0;
		}
		else{ break; }
	}
	for (int i = 1; i <= total; i++){ free(ans[i]); }
	free(user_answer);
	fclose(user);
	fclose(df);
	return mark;
}

//判断是否是相同的任务
int same_subtask(const char* last, const char* cur){
	int i = 0;
	for (i = 0; last[i] != '.' && cur[i] != '.'; i++){ if (last[i] != cur[i]){ break; } }
	return last[i] == cur[i];
}

void prepare_files(const std::string& tmpFileName, std::string& infile, const int& p_id, const std::string& work_dir, std::string& outfile,
                   std::string& userfile, const int runnerID, const int& language){
	std::string fileName(tmpFileName,tmpFileName.size()-3);
	int j;  // 用于存储目标字符串的位置
	for (int i = j = 0; tmpFileName[i] != '\0'; ++i){  // 遍历源字符串 t
		if (tmpFileName[i] == '\''){  // 如果当前字符是单引号
			fileName[j++] = '\'';   // 将单引号字符 `'` 添加到目标字符串 s
			fileName[j++] = '\\';   // 接着添加反斜杠 `\`
			fileName[j++] = '\'';   // 再添加单引号字符 `'`，组成转义序列 `\'`
			fileName[j++] = '\'';   // 追加另一个单引号字符 `'`
			continue;        // 跳过当前循环，进入下一轮
		}
		fileName[j++] = tmpFileName[i];  // 如果不是单引号，将当前字符直接复制到目标字符串 s
	}
	fileName[j] = '\0';  // 确保目标字符串 s 以空字符 `\0` 结尾
	infile="/home/judge/"+std::to_string(p_id)+"/"+fileName+".in";
	std::string noip_file_name="/home/judge/"+std::to_string(p_id)+"/input.name";
	if (access(noip_file_name.c_str(), R_OK) != -1){
		FILE* fpname = fopen(noip_file_name.c_str(), "r");
		if (fscanf(fpname, "%s", noip_file_name)){
			system(("/bin/cp "+infile+" "+work_dir+"/"+basename(noip_file_name.data())).c_str()); // 如果存在input.name则复制测试数据
			system(("/usr/bin/chown judge "+work_dir+"/"+basename(noip_file_name.data())).c_str()); // 修改属主
		}
		fclose(fpname);
	}
	else if (language == LANG_RUBY)
			system(("/bin/cp "+infile+" "+work_dir+"/data.in").c_str());
	system(("/bin/cp /home/judge/"+std::to_string(p_id)+"/*.dic "+work_dir+"/ 2>/dev/null").c_str());
	system(("chown judge "+work_dir+"/*.dic ").c_str());
	outfile="/home/judge/"+std::to_string(p_id)+"/"+tmpFileName+".out";
	noip_file_name="/home/judge/"+std::to_string(p_id)+"/output.name";
	if (access(noip_file_name.c_str(), R_OK) != -1)
	{
		FILE* fpname = fopen(noip_file_name.c_str(), "r");
		if (fscanf(fpname, "%s", noip_file_name)){
			if (!strstr("noip_file_name", "//")){
				userfile="/home/judge/run"+std::to_string(runnerID)+"/"+basename(noip_file_name.data());
				system(("rm "+std::string(userfile)).c_str());
			}
		}
		fclose(fpname);
	}
	if (access(noip_file_name.c_str(), R_OK) != -1){
		FILE* fpname = fopen(noip_file_name.c_str(), "r");
		if (fscanf(fpname, "%s", noip_file_name)){
			if (!strstr("noip_file_name", "//")){
				userfile="/home/judge/run"+std::to_string(runnerID)+"/"+basename(noip_file_name.data());
				system(("rm "+std::string(userfile)).c_str());
			}
		}
		fclose(fpname);
	}
	else{
		userfile="/home/judge/run"+std::to_string(runnerID)+"/user.out";
		system(("rm "+std::string(userfile)).c_str());
	}
}
//从文件名中读取分数
int has_mark_in_name = 0;
int mark_of_name(const char* name){
	int mark;
	printf("reading mark from %s \n", name);
	if (sscanf(name, "%*[^\[][%d]", &mark) == 1){
		printf("reading mark %d \n", mark);
		has_mark_in_name = 1;
		return mark;
	}
	return 10;
}

//长度
size_t inFile(const dirent* dp){
	if (strlen(dp->d_name) <= 3 || strcmp(dp->d_name + strlen(dp->d_name) - 3, ".in") != 0) return 0;
	return strlen(dp->d_name) - 3;
}

//ZOJ--将 old 字符串中所有出现的 search 子字符串替换成 replace 字符串
char* str_replace(char* old, const char* search, const char* replace){
	char* s;
	const size_t r = strlen(replace);
	const size_t p = strlen(search);
	while (nullptr != (s = strstr(old, search))){
		char buf[4096];
		strcpy(buf, s + p);
		if (strlen(old) - strlen(s) + strlen(buf) > 4096) break;
		strcpy(s, replace);
		strcpy(s + r - p + 1, buf);
	}
	return old;
}

//ZOJ--判断是否为UTF8编码
bool is_str_utf8(const char* str){
	unsigned int nBytes = 0;
	unsigned char chr = *str;
	bool bAllAscii = true;
	for (unsigned int i = 0; str[i] != '\0'; ++i){
		chr = *(str + i);
		//判断是否ASCII编码,如果不是,说明有可能是UTF8,ASCII用7位编码,最高位标记为0,0xxxxxxx
		if (nBytes == 0 && (chr & 0x80) != 0){ bAllAscii = false; }
		if (nBytes == 0){
			//如果不是ASCII码,应该是多字节符,计算字节数
			if (chr >= 0x80){
				if (chr >= 0xFC && chr <= 0xFD){ nBytes = 6; }
				else if (chr >= 0xF8){ nBytes = 5; }
				else if (chr >= 0xF0){ nBytes = 4; }
				else if (chr >= 0xE0){ nBytes = 3; }
				else if (chr >= 0xC0){ nBytes = 2; }
				else{ return false; }
				nBytes--;
			}
		}
		else{
			//多字节符的非首字节,应为 10xxxxxx
			if ((chr & 0xC0) != 0x80){ return false; }
			//减到为零为止
			nBytes--;
		}
	}
	//违返UTF8编码规则
	if (nBytes != 0){ return false; }
	if (bAllAscii){
		//如果全部都是ASCII, 也是UTF8
		return true;
	}
	return true;
}

//zoj--安全打印,避免乱码
inline void fprintSafe(FILE* f, char* buf){
	if (is_str_utf8(buf)){
		str_replace(buf, "|", "丨");
		str_replace(buf, "[", "［");
		str_replace(buf, "]", "］");
		str_replace(buf, "(", "（");
		str_replace(buf, ")", "）");
		str_replace(buf, "*", "＊");
		str_replace(buf, "\r", "");
		str_replace(buf, "\n", "↩");
		fprintf(f, "%s", buf);
	}
	else{
		for (size_t i = 0; i < strlen(buf); i++){
			if (buf[i] == ' ') fprintf(f, "⬜");
			else if (buf[i] == '\r') continue;
			else if (buf[i] == '\n') fprintf(f, "↩");
			else if (isprint(buf[i])) fprintf(f, "%c", buf[i]);
			else if (buf[i] == EOF) fprintf(f, "⛔");     //Binary Code
			else fprintf(f, "`0x%02x` ", buf[i] & 0xff); //Binary Code
		}
	}
}

//ZOJ--获取文件名
const char* getFileNameFromPath(const char* path){
	for (int i = static_cast<int>(strlen(path)); i >= 0; i--){ if (path[i] == '/') return &path[i + 1]; }
	return path;
}

//ZOJ -- 比较两个文件 f1 和 f2，并以表格形式生成他们的差异
void make_diff_out_simple(FILE* f1, FILE* f2, char* prefix, int c1, int c2, const char* path){
	char buf1[4096];
	char buf2[4096];
	FILE* diff = fopen("diff.out", "a+");
	fprintf(diff, "%s\n--\n", getFileNameFromPath(path));
	fprintf(diff, "|Expected|Yours\n|--|--\n|...|...\n");
	int row = 0;
	while (!(feof(f1) && feof(f2))){
		row++;
		fprintf(diff, "|");
		if (row == 1){
			fprintSafe(diff, prefix);
			if (c1 != '\n'){
				buf1[0] = c1; // patch buf1 with c1
				if (!feof(f1) && fgets(buf1 + 1, 4096 - 2, f1)){ fprintSafe(diff, buf1); }
			}
			else{ fprintf(diff, "↩"); }
		}
		else if (!feof(f1) && fgets(buf1, 4096 - 1, f1)){ fprintSafe(diff, buf1); }
		else{
			fprintf(diff, "⛔"); // standard output ending
		}
		fprintf(diff, "|");
		if (row == 1){
			fprintSafe(diff, prefix);
			if (c2 == EOF){
				fprintf(diff, "⛔"); //Binary Code
			}
			else{
				buf2[0] = c2; // patch buf2 with c2
				if (!feof(f2) && fgets(buf2 + 1, 4096 - 2, f2)){
					fprintf(diff, "`");
					fprintSafe(diff, buf2);
					fprintf(diff, "`");
				}
			}
		}
		else if (!feof(f2) && fgets(buf2, 4096 - 1, f2)){ fprintSafe(diff, buf2); }
		fprintf(diff, "\n");
		if (row >= 5 || ftell(diff) > 128) break;
	}
	fprintf(diff, "\n\n");
	fclose(diff);
}

// ZOJ--找到下一个非空格字符或\n
void find_next_nonspace(int& c1, int& c2, FILE*& f1, FILE*& f2, int& ret){
	while ((isspace(c1)) || (isspace(c2))){
		if (c1 != c2){
			if (c2 == EOF){
				do{ c1 = fgetc(f1); }
				while (isspace(c1));
				continue;
			}
			if (c1 == EOF){
				do{ c2 = fgetc(f2); }
				while (isspace(c2));
				continue;
			}
			if (isspace(c1) && isspace(c2)){
				while (c2 == '\n' && isspace(c1) && c1 != '\n') c1 = fgetc(f1);
				while (c1 == '\n' && isspace(c2) && c2 != '\n') c2 = fgetc(f2);
			}
			else{
				ret = OJ_PE;
			}
		}
		if (isspace(c1)){ c1 = fgetc(f1); }
		if (isspace(c2)){ c2 = fgetc(f2); }
	}
}

//ZOJ
int compare_zoj(const char* file1, const char* file2, const char* infile, const char* userfile, double* spj_mark){
	int ret = OJ_AC;
	int c1, c2;
	char prefix[4096] = "";
	int preK = 0;
	FILE* f1 = fopen(file1, "re");
	FILE* f2 = fopen(file2, "re");
	if (!f1 || !f2){ ret = OJ_RE; }
	else
		for (;;){
			// Find the first non-space character at the beginning of line.
			// Blank lines are skipped.
			c1 = fgetc(f1);
			c2 = fgetc(f2);
			find_next_nonspace(c1, c2, f1, f2, ret);
			// Compare the current line.
			for (;;){
				// Read until 2 files return a space or 0 together.
				while ((!isspace(c1) && c1) || (!isspace(c2) && c2)){
					if (c1 == EOF && c2 == EOF){ goto end; }
					if (c1 == EOF || c2 == EOF){ break; }
					if (c1 != c2){
						// Consecutive non-space characters should be all exactly the ifconfig|grep 'inet'|awk -F: '{printf $2}'|awk  '{printf $1}'same
						ret = OJ_WA;
						goto end;
					}
					else if (preK < 4096 - 1){
						prefix[preK++] = c1;
						prefix[preK] = '\0';
					}
					else{
						preK = 0;
						prefix[preK] = '\0';
					}
					c1 = fgetc(f1);
					c2 = fgetc(f2);
				}
				find_next_nonspace(c1, c2, f1, f2, ret);
				preK = 0;
				prefix[preK] = '\0';
				if (c1 == EOF && c2 == EOF){ goto end; }
				if (c1 == EOF || c2 == EOF){
					ret = OJ_WA;
					goto end;
				}

				if ((c1 == '\n' || !c1) && (c2 == '\n' || !c2)){ break; }
			}
		}

	end:
	struct stat fileStat{};
	long out_size =0;
	if (stat(file1,&fileStat) ==-1) out_size= 0;
	else out_size = fileStat.st_size;

	const long user_now = ftell(f2);
	if ( user_now > 1 && out_size > user_now) *spj_mark = (user_now - 1.00) / out_size;

	if (ret == OJ_WA || ret == OJ_PE){ make_diff_out_simple(f1, f2, prefix, c1, c2, file1); }
	if (f1) fclose(f1);
	if (f2) fclose(f2);
	return ret;
}

//特判
int special_judge(char* oj_home, int problem_id, const std::string& infile, const std::string& outfile, const std::string&  userfile, double* spj_mark,
                  int spj){
	char spjpath[4096 / 2];
	char tpjpath[4096 / 2];
	char upjpath[4096 / 2];
	// prevent privileges settings caused spj fail in [issues686]
	system(("chgrp judge /home/judge/data/"+std::to_string(problem_id)+"/?pj "+infile+" "+outfile+" "+userfile).c_str());
	system(("chmod 751 /home/judge/data/"+std::to_string(problem_id)+"/?pj "+infile+" "+outfile+" "+userfile).c_str());
	sprintf(spjpath, "%s/data/%d/spj", oj_home, problem_id);
	sprintf(tpjpath, "%s/data/%d/tpj", oj_home, problem_id);
	sprintf(upjpath, "%s/data/%d/upj", oj_home, problem_id);

	const pid_t pid = fork();
	int ret = 0;
	if (pid == 0){
		struct rlimit LIM{}; // time limit, file limit& memory limit

		LIM.rlim_cur = 15;
		LIM.rlim_max = LIM.rlim_cur;
		setrlimit(RLIMIT_CPU, &LIM);
		alarm(0);
		alarm(10);

		// file limit
		LIM.rlim_max = (1024*1024)<<9 + (1024*1024);
		LIM.rlim_cur = (1024*1024)<<9;
		setrlimit(RLIMIT_FSIZE, &LIM);

		while (setgid(1536) != 0) sleep(1);
		while (setuid(1536) != 0) sleep(1);
		while (setresuid(1536, 1536, 1536) != 0) sleep(1);
		if (access(upjpath, X_OK) == 0){
			ret = execl(upjpath, upjpath, infile, outfile, userfile,NULL); // hustoj style 2
		}
		else if (access(tpjpath, X_OK) == 0){
			//ret = execute_cmd("%s/data/%d/tpj %s %s %s 2>> diff.out ", oj_home, problem_id, infile, userfile, outfile);    // testlib style
			ret = execl(tpjpath, tpjpath, infile, userfile, outfile, NULL);
			// testlib style: switch userfile and outfile position
		}
		else if (access(spjpath, X_OK) == 0){
			ret = execl(spjpath, spjpath, infile, outfile, userfile,NULL); // hustoj style 1
			//ret = execute_cmd("%s/data/%d/spj %s %s %s", oj_home, problem_id, infile, outfile, userfile);    // hustoj style
		}
		else if (spj == 2){}
		else{
			printf("spj tpj not found problem: %d\n", problem_id);
			ret = 1;
		}
		if (ret) exit(ret);
		exit(0);
	}
	int status;

	waitpid(pid, &status, 0);
	ret = WEXITSTATUS(status);
	if (access(upjpath, X_OK) == 0){
		printf("upj return: %d\n", ret);
		*spj_mark = ret / 100.0;
		if (ret == 100) ret = 0;
		else ret = 1;
		printf("spj_mark: %.2f ret: %d\n", *spj_mark, ret);
	}
	return ret;
}

//获取进程状态
int get_proc_status(int pid, const char* mark){
	char fn[4096], buf[4096];
	int ret = 0;
	sprintf(fn, "/proc/%d/status", pid);
	FILE* pf = fopen(fn, "re");
	const int m = strlen(mark);
	while (pf && fgets(buf, 4096 - 1, pf)){
		buf[strlen(buf) - 1] = 0;
		if (strncmp(buf, mark, m) == 0){ if (1 != sscanf(buf + m + 1, "%d", &ret)) printf("proc read fail\n"); }
	}
	if (pf) fclose(pf);
	return ret;
}

//清理可能存在的热加载目录
void umount(const std::string& work_dir){
    //  懒卸载意味着卸载操作会延迟到文件系统不再被使用时才真正发生，允许文件系统保持挂载状态，直到不再有进程使用它。
    if (chdir(work_dir.c_str())) exit(-1);
    system(("/bin/umount -l "+work_dir+"/usr 2>/dev/null").c_str()); //卸载用户目录
    if (!work_dir.empty())
        system(("/bin/umount -l "+work_dir+"/proc 2>/dev/null").c_str()); //卸载proc
    system(("/bin/umount -l "+work_dir+"/dev 2>/dev/null").c_str()); //卸载dev
    system(("/bin/umount -l "+work_dir+"/usr 2>/dev/null").c_str()); //卸载用户目录
    system ("/bin/umount -l usr dev");                      //懒卸载 usr 和 dev 目录下的挂载点
    system ("/bin/umount -l lib lib64");                    //懒卸载 lib 和 lib64 目录下的挂载点
    system(("/bin/rmdir "+work_dir+"/* ").c_str());                  //删除工作目录下的空目录
    system(("/bin/rmdir "+work_dir+"/log/* ").c_str());              //删除./log/工作目录下的空目录
}

//编译
bool compile(const int& language,const std::string& workDir){
    const pid_t pid=fork();
    if (pid==0){
        rlimit LIM{};
        int cpuTime=50;     //秒
        if (language==LANG_JAVA) cpuTime=60;
        LIM.rlim_max = cpuTime;            //硬限制，root用户可以通过setrlimit()来改变
        LIM.rlim_cur = cpuTime;            //软限制，进程可以通过调用setrlimit()来改变
        setrlimit(RLIMIT_CPU, &LIM);   //设置CPU时间限制（秒）（软限制）
        alarm(0);                      //取消之前的定时器
        if (cpuTime > 0) alarm(cpuTime);       //设置定时器，超时后向进程发送SIGALRM信号
        else alarm(1);                 //设置定时器，超时后向进程发送SIGALRM信号
        LIM.rlim_max = 500 * 1024*1024;   //硬限制，root用户可以通过setrlimit()来改变
        LIM.rlim_cur = 500 * 1024*1024;   //软限制，进程可以通过调用setrlimit()来改变
        setrlimit(RLIMIT_FSIZE, &LIM); //设置文件大小限制（字节）（软限制）
        if (language==LANG_PASCAL||language==LANG_JAVA||language==LANG_GO||language==LANG_RUBY||language==LANG_SB3)
            LIM.rlim_cur=(1024*1024)<<12, LIM.rlim_max=(1024*1024)<<12;//4GB
        else LIM.rlim_cur=(1024*1024)<<11, LIM.rlim_max=(1024*1024)<<11;//2GB
        if (language!=LANG_JAVA)setrlimit(RLIMIT_AS, &LIM);//设置进程数据段大小的最大值（字节）
        if (language==LANG_PASCAL&&language==LANG_FREEBASIC)stdout = freopen("ce.txt", "w", stdout); //重定向标准输出到ce.txt
        else stderr = freopen("ce.txt", "w", stderr); //重定向错误输出到ce.txt
        std::string cmd="/bin/chown judge "+workDir;    //改变文件所有者为judge
        std::string cmd2="/bin/chmod 750 "+workDir;     //改变文件权限为750
        system(cmd.c_str());
        system(cmd2.c_str());
        if (language!=LANG_JAVA&&language!=LANG_CSHARP&&language!=LANG_PYTHON&&language!=LANG_FREEBASIC&&language!=LANG_BASH&&language!=LANG_RUBY){
            if (access("usr",F_OK)==-1){
                system("mkdir -p root/.cache/go-build usr etc/alternatives proc tmp dev");//递归创建多个目录
                system("chown judge -R root tmp");      //tmp目录的所有者递归改为judge，用户组改为root
                system("mount -o bind /usr usr");   //将/usr目录挂载到usr
                system("mount -o remount,ro usr");  //将/usr目录挂载为只读
                system("ln -s usr/bin bin");        //创建符号链接
                system("ln -s usr/lib lib");        //创建符号链接
                system("ln -s usr/lib32 lib32");    //创建符号链接
                system("ln -s usr/libx32 libx32");          //创建符号链接
                system("cp /etc/alternatives/* etc/alternatives");  //复制/etc/alternatives/目录下的所有文件到etc/alternatives/
                system("cp /etc/fpc* etc/");            //复制fpc*有关的文件到etc/
                system("mount -o bind /proc proc");     //将/proc目录挂载到proc
                if (language!=LANG_C&&language!=LANG_CPP&&language!=LANG_PASCAL&&language!=LANG_PYTHON&&language!=LANG_OBJC&&language!=LANG_CLANG&&language!=LANG_CLANGPP){
                    system("mount -o bind /dev dev");   //将/dev目录挂载到dev
                    system("mount -o remount,ro dev");  //将/dev目录挂载为只读
                }
            }
        }
        while (setgid(1536)!=0)sleep(1);                //设置组ID为1536
        while (setuid(1536)!=0)sleep(1);                //设置用户ID为1536
        while (setresuid(1536,1536,1536)!=0)sleep(1);   //设置实际用户ID、有效用户ID、保存的用户ID为1536
        switch (language){
            case LANG_C:execlp("gcc","-fno-asm","-O2","-fmax-errors=10","-std=c17","-Wall","--static","-DONLINE_JUDGE","-o","Main","Main.c","-lm",nullptr);break;
            case LANG_CPP:execlp("g++","-fno-asm","-O2","-fmax-errors=10","-std=c++20","-Wall","--static","-DONLINE_JUDGE","-o","Main","Main.cc","-lm",nullptr);break;
            case LANG_PASCAL:execlp("fpc","Main.pas","-Cs32000000","-Sh","-O2","-Co","-Ct","-Ci",nullptr);break;
            case LANG_RUBY:execlp("ruby","-c","Main.rb",nullptr);break;
            case LANG_BASH:execlp("chmod","+rx","Main.sh",nullptr);break;
            case LANG_PYTHON:execlp("python3","-c","import py_compile; py_compile.compile(r'Main.py')",nullptr);break;
            case LANG_PHP:execlp("php","-l","Main.php",nullptr);break;
            case LANG_PERL:execlp("perl","-c","Main.pl",nullptr);break;
            case LANG_CSHARP:execlp("mcs","-coding:utf8","-warn:0","Main.cs",nullptr);break;
            case LANG_OBJC:execlp("gcc","-o","Main","Main.m","-fconstant-string-class=NSConstantString","-I","/usr/include/GNUstep/","-L","/usr/lib/GNUstep/Libraries/","-lobjc","-lgnustep-base",nullptr);break;
            case LANG_FREEBASIC:execlp("fbc","-lang","qb","Main.bas",nullptr);break;
            case LANG_CLANG:execlp("clang","Main.c","-o","Main","-ferror-limit=10","-fno-asm","-Wall","-lm","--static","-std=c99","-DONLINE_JUDGE",nullptr);break;
            case LANG_CLANGPP:execlp("clang++","Main.cc","-o","Main","-ferror-limit=10","-fno-asm","-Wall","-lm","--static","-std=c++20","-DONLINE_JUDGE",nullptr);break;
            case LANG_LUA:execlp("lua","-o","Main","Main.lua",nullptr);break;
            case LANG_GO:
                setenv("PYTHONIOENCODING","utf-8",1),setenv("USER","judge",1),setenv("GOCACHE","/tmp",1),setenv("PATH","/bin:/usr/bin:/opt/cangjie/bin",1),setenv("LD_LIBRARY_PATH","/opt/cangjie/runtime/lib/linux_x86_64_llvm:/opt/cangjie/tools/lib",1),setenv("GOCACHE","/tmp",1),setenv("LANG","zh_CN.UTF-8",1),setenv("LANGUAGE","zh_CN.UTF-8",1),setenv("LC_ALL","zh_CN.utf-8",1);
                execlp("go","build","-o","Main","Main.go",nullptr);break;
            case LANG_FORTRAN:execlp("f95","-static","-o","Main","Main.f95",nullptr);break;
            case LANG_COBOL:execlp("cobc","-x","-static","-o","Main","Main.cob",nullptr);break;
            case LANG_SB3:execlp("scratch-run","--check","Main.sb3",nullptr);break;
            case LANG_JAVA:execlp("javac","-J-Xms32m","-J-Xmx256m","-encoding","UTF-8","Main.java",nullptr);break;
            case LANG_CJ:execlp("/opt/cangjie/bin/cjc","--static-std","--static-libs","--diagnostic-format","noColor","-o","Main","Main.cj",nullptr);break;
            default:;
        }
        exit(0);
    }
    int status=0;
    waitpid(pid,&status,0);
    if (language==LANG_RUBY||language==LANG_BASH||language==LANG_PYTHON){
        if (std::filesystem::exists("ce.txt")&&std::filesystem::file_size("ce.txt")!=0)
            status=0;
    }
    system("/bin/umount -l bin usr lib lib64 etc/alternatives dev 2>/dev/null");
    umount(workDir);
    return status==0;
}

//运行
void run(const int& language, const std::string& workDir, const double& time_lmt, const int& usedtime, const int& mem_lmt,
                  const std::string& data_file_path, int p_id){
    //准备环境变量处理中文，如果希望使用非中文的语言环境，可能需要修改这些环境变量
    char* const envp[] = {
        (char* const)"PYTHONIOENCODING=utf-8", (char* const)"USER=judge", (char* const)"GOCACHE=/tmp",
        (char* const)"PATH=/bin:/usr/bin:/opt/cangjie/bin",
        (char* const)"LD_LIBRARY_PATH=/opt/cangjie/runtime/lib/linux_x86_64_llvm:/opt/cangjie/tools/lib",
        (char* const)"GOCACHE=/tmp", (char* const)"LANG=zh_CN.UTF-8", (char* const)"LANGUAGE=zh_CN.UTF-8",
        (char* const)"LC_ALL=zh_CN.utf-8", nullptr
    };
    nice(19);                  //设置进程优先级
    chdir(workDir.c_str()); //改变当前工作目录
    if (language==LANG_SQL){
        system(("usr/bin/sqlite3 %s/data.db < %s"+workDir+data_file_path).c_str());
        system(("usr/bin/sqlite3 %s/data.db < %s"+workDir).c_str());
        stdin = freopen("Main.sql", "r", stdin);
    }else{
        std::string noip_file_name="home/judge/data/"+ std::to_string(p_id)+"/input.name";
        if (p_id == 0 || access(noip_file_name.c_str(), R_OK) == -1)
            stdin = freopen(data_file_path.c_str(), "r",stdin);
    }
    system(("touch "+workDir+"/Main.out").c_str()); //创建文件
    system(("chown judge 760 "+workDir+"/user.out").c_str()); //改变文件所有者为judge
    stdout=freopen("user.out","w",stdout); //重定向标准输出到user.out
    stderr=freopen("user.err","w",stderr); //重定向错误输出到user.err
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);       // 子进程,被父进程跟踪
    while (setgid(1536) != 0) sleep(1);                //设置组ID为1536
    while (setuid(1536) != 0) sleep(1);                //设置用户ID为1536
    while (setresuid(1536, 1536, 1536) != 0) sleep(1); //设置实际用户ID、有效用户ID、保存的用户ID为1536
    rlimit LIM{};
    LIM.rlim_cur = ceil(time_lmt / cpu_compensation - usedtime / 1000.0f) + 1;
    LIM.rlim_max = LIM.rlim_cur + 1;
    setrlimit(RLIMIT_CPU, &LIM); //设置CPU时间限制（秒）（软限制）
    alarm(0);                    //取消之前的定时器
    if (time_lmt / cpu_compensation > 1) alarm(ceil(time_lmt / cpu_compensation) + 1);
    else alarm(1);
    // 设置文件大小限制
    LIM.rlim_max = (1024*1024)<<9 + (1024*1024);    //硬限制513MB，root用户可以通过setrlimit()来改变
    LIM.rlim_cur = (1024*1024)<<9;                  //软限制512MB，进程可以通过调用setrlimit()来改变
    setrlimit(RLIMIT_FSIZE, &LIM);
    //设置进程数限制
    switch (language){
        case LANG_GO:
        case LANG_CSHARP:
        case LANG_JAVA:
        case LANG_R:
        case LANG_SB3:    //GOlang, CSharp, Java, Ruby, Scratch3设置
            LIM.rlim_cur = LIM.rlim_max = 880;
            break;
        case LANG_RUBY:
        case LANG_PYTHON:
        case LANG_SCHEME:
        case LANG_JS:
        case LANG_CJ:
        case LANG_MATLAB:  //Ruby, Python3, Scheme, JavaScript, CangJie, MATLAB设置
            LIM.rlim_cur = LIM.rlim_max = 200;
            break;
        case LANG_BASH: //bash
            LIM.rlim_cur = LIM.rlim_max = 3;
            break;
        default: LIM.rlim_cur = LIM.rlim_max = 1;
    }

    setrlimit(RLIMIT_NPROC, &LIM);

    //设置栈大小(单位:字节)
    LIM.rlim_cur = (1024*1024) << 8, LIM.rlim_max = (1024*1024) << 8;
    setrlimit(RLIMIT_STACK, &LIM);
    //设置内存限制(单位:字节)
    LIM.rlim_cur = (1024*1024) * mem_lmt / 2 * 3,  LIM.rlim_max = (1024*1024) * mem_lmt * 2;
    //设置虚拟内存限制
    if (language==LANG_C||language==LANG_CPP||language==LANG_PASCAL||language==LANG_JAVA||language==LANG_OBJC||language==LANG_CLANG||language==LANG_CLANGPP||language==LANG_GO)
        setrlimit(RLIMIT_AS, &LIM);
    //运行程序
    if (language==LANG_C||language==LANG_CPP||language==LANG_PASCAL||language==LANG_OBJC||language==LANG_FREEBASIC||language==LANG_CLANG||language==LANG_CLANGPP||language==LANG_GO||language==LANG_FORTRAN||language==LANG_COBOL||language==LANG_CJ)
        execle("./Main","./Main",nullptr,envp);
    else if (language==LANG_JAVA)execle("/usr/bin/java","/usr/bin/java","-Xms32m","-Xmx256m","Main",nullptr,envp);
    else if (language==LANG_RUBY)execle("/usr/bin/ruby","/usr/bin/ruby","Main.rb",nullptr,envp);
    else if (language==LANG_BASH)execle("/bin/bash", "/bin/bash","Main.sh",nullptr,envp);
    else if (language==LANG_PYTHON)execle("/usr/bin/python3","/usr/bin/python3","Main.py",nullptr,envp);
    else if (language==LANG_PHP)execle("/usr/bin/php","/usr/bin/php","Main.php",nullptr,envp);
    else if (language==LANG_PERL)execle("/usr/bin/perl","/usr/bin/perl","Main.pl",nullptr,envp);
    else if (language==LANG_CSHARP)execle("/usr/bin/mono","/usr/bin/mono","--debug","Main.exe",nullptr,envp);
    else if (language==LANG_SCHEME)execle("/usr/bin/guile","/usr/bin/guile","Main.scm",nullptr,envp);
    else if (language==LANG_LUA)execle("/usr/bin/lua","/usr/bin/lua","Main.lua",nullptr,envp);
    else if (language==LANG_JS)execle("/usr/bin/node","/usr/bin/node","Main.js",nullptr,envp);
    else if (language==LANG_SQL)execle("/usr/bin/sqlite3","/usr/bin/sqlite3","data.db",nullptr,envp);
    else if (language==LANG_MATLAB)execl("/usr/bin/octave-cli","/usr/bin/octave-cli","-W","-q","-H","Main.m",nullptr);
    else if (language==LANG_R)execle("/usr/bin/Rscript","/usr/bin/Rscript","Main.R",nullptr,envp);
    else if (language==LANG_SB3)execle("/usr/bin/scratch-run","/usr/bin/scratch-run","Main.sb3",nullptr,envp);
    fflush(stderr);
    exit(0);
}

//监控
void watch(pid_t pidApp, const std::string& infile, int& ACFlag, int spj, const std::string& userfile, const std::string& outfile, int solution_id,
                    const int& language, long& maxMemory, int mem_lmt, long& usedtime, double time_lmt){
    long tempMemory=0;
	int status, sig, exitcode;
	char white_code[256] = {0}; // 信号白名单
	white_code[0] = 1;          // 成功退出
	white_code[5] = 1;			// SIGTRAP,Trace/breakpoint trap
	white_code[17] = 1;			// SIGCHLD,Child exited
	white_code[23] = 1;			// SIGCHLD,Urgent data is available at a socket
	white_code[133] = 1;

	user_regs_struct reg{};
	rusage ruse{};
	int tick = 0;
	int first = true;
	uintmax_t outFileSize = std::filesystem::file_size(outfile);
	while (true){
		tick++;
		// 检查子进程的使用情况
		wait4(pidApp, &status, __WALL, &ruse); //等待子进程切换内核态（调用系统API或者运行状态变化）
		if (first) ptrace(PTRACE_SETOPTIONS, pidApp, NULL, PTRACE_O_TRACESYSGOOD | PTRACE_O_TRACEEXIT);
		//设置跟踪选项并跟踪进程的系统调用，JVM GC 在使用前请求VM，所以使用内核页错误次数和页大小
		//jvm gc ask VM before need,so used kernel page fault times and page size
		if (language == LANG_JAVA || language == LANG_PHP || language == LANG_CSHARP || language == LANG_CLANG || language == LANG_CLANGPP
			|| language == LANG_JS || language == LANG_GO || language == LANG_MATLAB || language == LANG_COBOL || language == LANG_CJ ||
			language == LANG_SB3){
			tempMemory = ruse.ru_minflt * getpagesize();//java use pagefault
		}else{
			//other use VmPeak
			tempMemory = get_proc_status(pidApp, "VmPeak:") << 10;
		}
		if (tempMemory > maxMemory) maxMemory = tempMemory;
		//内存超限
		if (maxMemory > mem_lmt * (1024*1024)){
			if (ACFlag == OJ_AC) ACFlag = OJ_ML;
			ptrace(PTRACE_KILL, pidApp, NULL, NULL);
			break;
		}
		// 子进程已经退出 ，返回值不为0则判RE
		if (WIFEXITED(status)){
			exitcode = WEXITSTATUS(status);
			if (exitcode){
				ACFlag = OJ_RE;
				FILE* errFile = fopen("error.out", "a+");
				fprintf(errFile, "%s:%s\n", infile.c_str()+16, ("\t    non-zero return = " + std::to_string(exitcode)+"\n").c_str());
				fclose(errFile);
			}
			break;
		}
		//运行错误
		if ((language == LANG_C||language==LANG_CPP||language==LANG_PASCAL||language==LANG_JAVA|| language == LANG_BASH || language == LANG_CSHARP) && std::filesystem::exists("error.out") && !oi_mode){
			ACFlag = OJ_RE;
			ptrace(PTRACE_KILL, pidApp, NULL, NULL);
			break;
		}
		//输出超限
		struct stat f_stat{};
		stat(userfile.c_str(), &f_stat);
		if (((tick & 0xff) == 0x00) && (!spj) && f_stat.st_size > outFileSize * 2 + 1024){
			ACFlag = OJ_OL;
			ptrace(PTRACE_KILL, pidApp, NULL, NULL);
			break;
		}
		exitcode = WEXITSTATUS(status) % 256; // 退出状态
		if (!white_code[exitcode]){           // 非白名单信号
			if (ACFlag == OJ_AC){
				switch (exitcode){ // 根据退出的原因给出判题结果
					case SIGCHLD:  //子进程退出
					case SIGALRM:  //定时器到时
						alarm(0);
					case SIGKILL: //进程被杀死
					case SIGXCPU: //CPU时间限制
						ACFlag = OJ_TL;
						usedtime = time_lmt * 1000;
						break;
					case SIGXFSZ: //文件大小超限
						ACFlag = OJ_OL;
						break;
					default: // 其他情况(运行错误)
						ACFlag = OJ_RE;
				}
				FILE* errFile = fopen("error.out", "a+");
				fprintf(errFile, "%s:%s\n", infile.c_str()+16, strsignal(exitcode));
				fprintf(errFile, "%s:%s\n", ("if you can confirm the code is right and your system is somehow different from others , try adding: ' white_code[%d]=1; ' after judge_client:2888 for it, but don't panic just for a Segmentfault. "
					        +std::to_string(exitcode)).c_str(), strsignal(exitcode));
				fclose(errFile);
			}
			ptrace(PTRACE_KILL, pidApp, NULL, NULL); // 杀死出问题的进程
			break;
		}
		if (WIFSIGNALED(status)){   // 子进程被信号status终止
			sig = WTERMSIG(status); // 获取终止信号(提取子进程因信号终止而退出时的信号编号)
			if (ACFlag == OJ_AC){
				switch (sig){
					case SIGCHLD: //子进程退出
					case SIGALRM: alarm(0);
					case SIGKILL: //进程被杀死
					case SIGXCPU: //CPU时间限制
						ACFlag = OJ_TL;
						break;
					case SIGXFSZ: //文件大小超限
						ACFlag = OJ_OL;
						break; //运行错误
					default: ACFlag = OJ_RE;
				}
				FILE* errFile = fopen("error.out", "a+");
				fprintf(errFile, "%s:%s\n", infile.c_str()+16, strsignal(sig));
				fclose(errFile);
			}
			break;
		}
		long call_id = ptrace(PTRACE_GETREGS, pidApp, NULL, &reg);    //ptrace 获取子进程的寄存器内容
		call_id = (reg.orig_rax) % 512; // 获取系统调用号
		if (record_call){
			printf("new call id:%ld\n", call_id);
			call_counter[call_id]++;
			printf("call %ld: %d\n", call_id, call_counter[call_id]);
		}
		else if (call_counter[call_id]){ call_counter[call_id]--; }
		else{
			//do not limit JVM syscall for using different JVM 对于非法的系统调用，给出具体编号给管理员参考
			ACFlag = OJ_RE;
			char error[4096];
			sprintf(error,
			        "[ERROR] solution_id:%d called a Forbidden system call:%u [%u]\n"
			        " TO FIX THIS , ask admin to add the CALLID into corresponding LANG_XXV[] located at okcalls32/64.h ,\n"
			        "and recompile judge_client. \n" "if you are admin and you don't know what to do ,\n"
			        "中文解释查看知乎 https://zhuanlan.zhihu.com/p/24498599\n", solution_id, call_id,
			        static_cast<unsigned int>(reg.orig_rax));
			FILE* errFile = fopen("error.out", "a+");
			fprintf(errFile, "%s:%s\n", infile.c_str()+16,error);
			fclose(errFile);
			ptrace(PTRACE_KILL, pidApp, NULL, NULL);
		}
		call_id = 0;
		ptrace(PTRACE_SYSCALL, pidApp, NULL, NULL); // 继续等待下一次的系统调用或其他中断
		first = false;
	}
	ptrace(PTRACE_KILL, pidApp, NULL, NULL); // 杀死出问题的进程
	system(("kill -9 "+std::to_string(pidApp)).c_str());
	usedtime += (ruse.ru_utime.tv_sec * 1000 + ruse.ru_utime.tv_usec / 1000) * cpu_compensation;
	// 统计用户态耗时，在更快速的CPU上加以cpu_compensation倍数放大
	usedtime += (ruse.ru_stime.tv_sec * 1000 + ruse.ru_stime.tv_usec / 1000) * cpu_compensation;
	// 统计内核态耗时，在更快速的CPU上加以cpu_compensation倍数放大
}

//评测
void judge(int& ACflg, const int& usedtime, double time_lmt, int isSpecialJudge, const int p_id, const std::string& infile, const char* outfile,
					const std::string& userfile, int& PEflg, const int& language, const std::string& work_dir, long& maxMemory, int mem_lmt, int num_of_test,
					double* spj_mark){
	if (num_of_test == 0) num_of_test = 1.0;

	if (ACflg == OJ_AC){
		int real_limit = time_lmt * 1000;
		if (totalTime > real_limit) ACflg = OJ_TL; // 总时间超过
		if (usedtime > real_limit) ACflg = OJ_TL;   // 单点超过
	}

	if (maxMemory > mem_lmt * (1024*1024)) ACflg = OJ_ML; //issues79
	// compare
	if (ACflg == OJ_AC){
		int comp_res;
		if (isSpecialJudge){
			comp_res = special_judge("/home/judge", p_id, infile, outfile, userfile, spj_mark, isSpecialJudge);
			if (comp_res == 0) comp_res = OJ_AC;
			else comp_res = OJ_WA;
		}else{
			comp_res = compare_zoj(outfile, userfile.c_str(), infile.c_str(), userfile.c_str(), spj_mark);
		}
		if (comp_res == OJ_WA)	ACflg = OJ_WA;
		else if (comp_res == OJ_PE) PEflg = OJ_PE;
		ACflg = comp_res;
	}
	if ((ACflg == OJ_AC)) *spj_mark = 1.0;
	else if (*spj_mark > 0.99) *spj_mark = 0.99;
	//jvm popup messages, if don't consider them will get miss-WrongAnswer
	if (language == LANG_JAVA)	{
		int com_res=OJ_AC;
		system(("chmod 700 "+work_dir+"/error.out").c_str());
		com_res=system(("/bin/grep 'Exception'  "+work_dir+"/error.out").c_str());
		if (!com_res)  ACflg=OJ_RE;
		system(("cat "+work_dir+"/error.out").c_str());
		com_res=system(("/bin/grep 'java.lang.OutOfMemoryError'  "+work_dir+"/error.out").c_str());
		if (!com_res){
			printf("JVM need more Memory or Threads!");
			ACflg=OJ_ML;
			maxMemory=mem_lmt*(1024*1024);
		}
		com_res=system(("/bin/grep 'Could not create'  "+work_dir+"/error.out").c_str());
		if (!com_res){
			printf("jvm need more resource");
			ACflg=OJ_RE;
		}
	}
	if (language == LANG_PYTHON){
		int com_res=OJ_AC;
		com_res=system(("/bin/grep 'MemoryError'  "+work_dir+"/error.out").c_str());
		if (!com_res){
			printf("Python need more Memory!");
			ACflg=OJ_ML;
			maxMemory=mem_lmt*(1024*1024);
		}
	}
}

//判断是否已经运行
bool already_running(){
    char buf[16];                                                                                                 //缓冲区
    const int fd = open("/home/judge/run0/judge_client.pid", O_RDWR | O_CREAT, S_IRUSR |S_IWUSR|S_IRGRP|S_IROTH); //打开文件(如果不存在则创建)
    if (fd < 0){
        exit(1);
    }
    //如果文件锁定失败,则关闭文件描述符并返回1
    flock fl{};
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    if ((fcntl(fd, F_SETLK, &fl)) < 0){
        if (errno == EACCES || errno == EAGAIN){
            //如果错误号为EACCES或EAGAIN,则输出错误信息
            close(fd);
            return true;
        }
        exit(1);
    }
    //如果文件锁定成功,则关闭文件描述符并返回0
    if (ftruncate(fd, 0)) printf("close file fail 0 \n");                              //将文件大小截为0
    sprintf(buf, "%d", getpid());                                                      //将进程ID写入缓冲区
    if (write(fd, buf, strlen(buf) + 1) >= 4096) printf("buffer size overflow!...\n"); //将缓冲区写入文件
    return false;
}

//清理工作目录
void clearWorkDir(const std::string& workDir){
    umount(workDir);
    system(("mkdir "+workDir+"/log"+"2>/dev/null").c_str()); //创建log文件夹,不显示错误信息
    system(("/bin/mv "+workDir+"/* "+workDir+"/log/ 2>/dev/null").c_str()); //将work_dir目录下的所有文件移动到log目录中,不显示错误信息
    system(("/bin/rmdir "+workDir+"/log/* 2>/dev/null").c_str()); //尝试删除log目录下的所有空目录，忽略错误。如果目录不是空的，则不会删除。,不显示错误信息
    system(("/bin/rm -rf "+workDir+"/log/* 2>/dev/null").c_str()); //强制删除 log 目录下的所有文件和子目录,不显示错误信息
}
//获得测试数据目录中测试数据列表
int isInFile(const dirent * dp)
{
	const int l = strlen(dp->d_name);
	if (l <= 3 || strcmp(dp->d_name + l - 3, ".in") != 0)
		return 0;
	return l - 3;
}

int main(int argc, char** argv){
	//初始化--------------------
	umask(0077);
    int runnerID=std::stoi(argv[2],nullptr,10);   //定义运行ID
    const std::string workDir="/home/judge/run"+std::to_string(runnerID)+"/"; //定义工作目录
    std::string lockFile="home/judge/client"+std::to_string(runnerID)+".pid"; //定义锁文件
    while (already_running()){
        syslog(LOG_ERR | LOG_DAEMON, "This working directory is occupied !\n");
        sleep(3);
    }
    system(("mkdir -p "+workDir).c_str());    //创建工作目录
    system(("chown judge "+workDir).c_str()); //改变文件所有者为judge
    clearWorkDir(workDir);                    //清理可能存在的热加载目录
    if (chdir(workDir.c_str())) exit(-3); //如果切换目录失败，则退出
	//定义solution的变量
	std::string userID;
	int solutionID,problemID,contestID,language;
	get_solution_info(solutionID,problemID,userID,language,contestID);
	//定义problem的变量
	int timeLimit,memoryLimit,specialJudge;
	get_problem_info(problemID,timeLimit,memoryLimit,specialJudge);
	//定义结果的变量
	get_solution(solutionID,language);
    if (timeLimit <= 0) timeLimit = 1;
    if (language==LANG_PYTHON){
        system(("sed -i 's/import.*os//g' "+workDir+"/Main.py").c_str());
    }else if (language==LANG_SB3){
        system(("cp "+workDir+"/../data/"+std::to_string(problemID)+"/sb3/"+std::to_string(solutionID)+".sb3 "+"Main.sb3").c_str());
    }
    system(("chown judge "+workDir+"/Main.*").c_str()); //改变文件所有者为judge
    system(("chmod 750 "+workDir+"/Main.*").c_str());  //改变文件权限为750
    //JAVA和需要虚拟机的语言,获得额外的时间和内存
    if (language!=LANG_C||language!=LANG_CPP||language!=LANG_PASCAL||language!=LANG_OBJC||language==LANG_CLANG||language==LANG_CLANGPP||language!=LANG_GO){
        timeLimit = timeLimit + 5;
        memoryLimit = memoryLimit + 512;
        if (language==LANG_JAVA){
            system(("cp /home/judge/etc/java0.policy "+workDir+"/java.policy").c_str());
            system(("chmod 755 "+workDir+"/java.policy").c_str());
            system(("chown judge "+workDir+"/java.policy").c_str());
        }
    }
    //不要超过范围
    if (timeLimit > 300 || timeLimit < 0) timeLimit = 1;
    if (memoryLimit > 2048 || memoryLimit < 1) memoryLimit = 2048;
    //编译--------------------
    if (!compile(language,workDir)){
        //传递CE
    	addceinfo_mysql(solutionID);
		update_solutoin_mysql(solutionID, OJ_CE, 0, 0, 0, 0, 0.0);
		mysql_close(conn);
    	clearWorkDir(workDir);
        exit(0);
    }
	umount(workDir);
	//运行--------------------
    std::string fullPath="/home/judge/data/"+std::to_string(problemID);
	dirent** namelist;
	int namelist_len=scandir(fullPath.data(),&namelist,isInFile,alphasort);
	if (namelist_len==-1)mysql_close(conn),exit(-1);	//打开目录失败，退出
	//准备运行时环境
	if (language==LANG_BASH)system("busybox dos2unix Main.sh");

	int numOftest=namelist_len;
	int ACFlag=OJ_AC,finalFlag=OJ_AC;
	long usedTime=0, maxMemory=0;
	if (numOftest==0){
		//写入文件，没有测试数据
		FILE *ferr = fopen("error.out", "a+");
		fprintf(ferr, "%s:%s\n","no test data ", " no *.in file found");
		fclose(ferr);
		ACFlag=OJ_RE,finalFlag=OJ_RE;
	}
	size_t prelen=strlen("/home/judge/data/")+std::to_string(problemID).size()+1;
	if (prelen<strlen("/home/judge")+6)prelen=strlen("/home/judge")+6;
	//遍历测试数据
	std::string outFile,inFile,userFile;
	double spj_mark=0,pass_rate=0;
	dirent* dirp;
	float total_mark=0,get_mark =0,mark=0;
	int time_space_index = 0,minus_mark=0,PEFlag=OJ_AC,sim;
	char time_space_table[4096 * 100];                   //定义时间空间表
	for (int i = 0; (oi_mode || ACFlag == OJ_AC || ACFlag == OJ_PE) && i < namelist_len; i++)
	{
		char last_name[4096];
		usedTime = 0,spj_mark=0;
		dirp=namelist[i];
		if ((strlen(dirp->d_name) <= 3 || strcmp(dirp->d_name + strlen(dirp->d_name) - 3, ".in") != 0))continue;
		mark=mark_of_name(dirp->d_name);
		total_mark += mark;
		//准备数据
		prepare_files(dirp->d_name, inFile, problemID, workDir, outFile, userFile, runnerID, language);
		if (access((fullPath+"/output.txt").c_str(), R_OK) == -1){
			//out file does not exist
			std::string error="missing out file "+std::string(dirp->d_name)+", report to system administrator!\n";
			FILE *ferr = fopen("error.out", "a+");
			fprintf(ferr, "%s:%s\n",(fullPath+"/input.txt").c_str()+16, error.c_str());
			fclose(ferr);
			// writeFile_runTimeError(infile + prelen, error);
			ACFlag = OJ_RE;
		}
		init_call(language);
		pid_t pidApp=fork();
		if (pidApp==0){
			if (specialJudge==2)exit(0);
			run(language,workDir,timeLimit,usedTime,memoryLimit,fullPath+"/input.txt",problemID);
		}else{
			if (specialJudge!=2)
				watch(pidApp,inFile,ACFlag,specialJudge,userFile,outFile,solutionID,language,maxMemory,memoryLimit,usedTime,timeLimit);
			kill(pidApp,SIGKILL);
			totalTime+=usedTime;
			if (access("Main.out", R_OK ) == -1)ACFlag=OJ_WA;
			else judge(ACFlag,usedTime,timeLimit,specialJudge,problemID,inFile,outFile.c_str(),userFile,PEFlag,language,workDir,maxMemory,memoryLimit,numOftest,&spj_mark);
			// time_space_index += sprintf(time_space_table + time_space_index, "%s|%ld|%s/%.2f|%dk|%dms\n",
			// 								infile + prelen, get_file_size(infile), jresult[ACflg], spj_mark,
			// 								maxMemory / 1024, usedtime);
			usedTime = 0;
		}
		if (oi_mode)
		{
			if (ACFlag==OJ_AC)
			{
				++pass_rate;
				get_mark += mark;
				if (same_subtask(last_name, dirp->d_name))
				{	//相同的子任务
					if (minus_mark)	minus_mark += mark; //累计任务内积分
					else get_mark -= mark; //任务已经失败，扣除本次得分
				}else minus_mark = mark; //跨越任务，积分重新累计
			}else
			{	//相同子任务，初次失败
				if (minus_mark >= 0) get_mark -= minus_mark; //扣除任务内积分
				get_mark += mark * spj_mark;
				pass_rate += spj_mark;
				minus_mark = -1; //当前任务失败，标记
			}
			if (finalFlag < ACFlag){ finalFlag = ACFlag; }
			if ( ACFlag == OJ_TL) break;	//超时直接退出,避免"拒绝服务洪水攻击"
			ACFlag = OJ_AC;
		}
		strcpy(last_name, dirp->d_name);
	}
	if (ACFlag==OJ_AC && PEFlag==OJ_PE)ACFlag=OJ_PE;
	if(ACFlag==OJ_AC && finalFlag==OJ_AC&!oi_mode)sim	; //相似度	检测
	if ((oi_mode && finalFlag == OJ_RE)||ACFlag == OJ_RE)
	{
		FILE* df = fopen("error.out", "a");
		fprintf(df, "filename|size|result|memory|time\n--|--|--|--|--\n%s\n", time_space_table);
		fclose(df);
	}
	usedTime = totalTime;
	if (specialJudge != 2){
		if (oi_mode){
			if (numOftest > 0){ pass_rate /= numOftest; }
			if (total_mark > 0 && has_mark_in_name){
				pass_rate = get_mark;
				if (total_mark > 100) pass_rate /= total_mark;
				else pass_rate /= 100.0;
			}
			//OJ_FinalFlag,update_solution(finalFlag, maxMemory >> 10);
		}
		else{
			if (ACFlag == OJ_AC) pass_rate = 1.0;
			else pass_rate = 0.0;
			//AC, update_solution(ACflg, maxMemory >> 10);
		}
	}else{
		std::string srcPath="Main."+langExt[language];
		mark = raw_text_judge((fullPath+"/input.txt").c_str(), (fullPath+"/output.txt").c_str(), srcPath.c_str(), &total_mark);
		printf("raw_text_mark:%.1f\n", mark);
		if (mark >= 0 && mark <= total_mark) pass_rate = mark;
		pass_rate /= 100.0;
		if (mark == total_mark) finalFlag = ACFlag = OJ_AC;
		else finalFlag = ACFlag = OJ_WA;
		// finalFlag,update_solution(finalACflg, mark * 10);
	}
	FILE* df = fopen("diff.out", "a");
	fprintf(df, "filename|size|result|memory|time\n--|--|--|--|--\n%s\n", time_space_table);
	fclose(df);
	clearWorkDir(workDir);
	free(namelist);
    return 0;
}