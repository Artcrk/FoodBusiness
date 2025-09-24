#include "core.hpp"

/**
 * @file roots.cpp
 * @brief 餐饮管理系统中央模块 - 整体控制
 * @author Artcrk
 * @date 2025-09-24
 *
 * @todo 完善C模式的功能,指引在ShowMenu内.
 * 
 * @todo 完成对搜索功能的优化,引入哈希存储和标签化思想.(wish in v0.5)
 *
 * @todo 使用Qt包装这个控制台程序,以完成一整个真正的现代化程序.(wish in v2.0)
 */

double Rst::Version = 1.0;
std::pair<int, int> hourRange[6] = { {0,4},{4,9},{9,12},{12,15},{15,19},{19,23} };//不能使用圆括号,会被解析为逗号表达式!
std::string RangeHello[6] = {"深夜","早晨","上午","中午","下午","夜晚"};//时间.

//读取database.dll中的数据,主要初始化datas和calcRecord
//前面会存储一个int变量(4字节),用于存储datas容器的大小.
//在读取完成datas之后,会有一个int存储daily,之后就是daily.daily之后有一个int存储timeRange,以此类推.
bool datainit() {
	std::ifstream FileIn("database.dll");
	if (!FileIn.is_open()) {
		std::cout << ">:database文件打开失败!若第一次使用该软件请忽略." << std::endl;
		FileIn.close();
		return false;
	}
	FileIn.seekg(0, std::ios::end);
	auto length = FileIn.tellg();
	FileIn.seekg(0, std::ios::beg);
	if (length == 0) {
		std::cout << ">:文件为空" << std::endl;
		FileIn.close();
		return false;
	}

	//------datas

	int datas_col,datas_row;
	FileIn >> datas_col;//xx xx str dou str dou ...
	FileIn >> datas_row;
	if (datas_col != 0 || datas_row != 0) {
		if (datas_col > 9 || datas_row > 9) {
			std::cout << ">:当前数据存储异常,读取终止." << std::endl;
			FileIn.close();
			return false;
		}
		Rst::dapx = datas_col;
		Rst::dapy = datas_row;
		for (int i = 0; i <= datas_col; i++) {
			for (int j = 0; j < 10; j++) {
				if (i == datas_col && j == datas_row) {
					break;
				}
				//格式:" string double";

				std::string s = "";
				double d = 0.0;

				FileIn >> s;
				//Inportant information:由于未知原因,在读取后的赋值操作会导致乱码,
				// 这让程序不能正常读取string内容,但是加上std::move可以解决该问题<<------但是,为什么?
				Rst::datas[i][j].name = std::move(s);//读取string
				//Rst::datas[i][j].name = s;
				FileIn >> d;
				Rst::datas[i][j].cost = d;//读取double.

				Rst::datas[i][j].occupied = true;
			}
		}
		std::cout << ">>+:成功读取" << 10 * datas_col + datas_row << "个餐品数据." << std::endl;
	}
	else {
		std::cout << ">:未检测到餐品存储数据." << std::endl;
	}
	//------
	 
	//用于获取一整个timedata数据.必须保证格式正确.
	//-->>>> days total_cost total_income pure_income avg_income avg_pure_income total_number otherdata
	auto Gettimedata = [&](Rst::timedata & td) {
		std::string tmps;
		FileIn >> tmps;
		td.days = std::move(tmps);
		FileIn >> td.total_cost;
		FileIn >> td.total_income;
		FileIn >> td.pure_income;
		FileIn >> td.avg_income;
		FileIn >> td.avg_pure_income;
		FileIn >> td.total_number;

		FileIn >> tmps;
		td.otherdata = std::move(tmps);
		};


	int daily_size, timeRange_size;
	FileIn >> daily_size;
	auto& da = Rst::CRecord.Saved_daily;
	auto& tr = Rst::CRecord.Saved_timeRange;
	std::string date;
	//daily->
	//xx { date {TD} }
	if (daily_size != 0) {
		da.reserve(daily_size);//不是realloc,不需要乘以sizeof.
		for (int i = 0; i < daily_size; i++) {
			Rst::timedata td;
			date.clear();
			FileIn >> date;
			Gettimedata(td);
			Rst::CRecord.SaveDaily(Rst::daily(td, date));
		}
		std::cout << ">>+:成功读取" << daily_size << "个单日期数据!" << std::endl;
	}
	else {
		std::cout << ">:未检测到日数据." << std::endl;
	}
	FileIn >> timeRange_size;

	if (timeRange_size != 0) {
		tr.reserve(timeRange_size);

		//timeRange->
		//xx date (TD) xx dateRange ... //onlydate,if not,this param will be ignore.
		for (int i = 0; i < timeRange_size; i++) {
			std::vector<std::string> name;
			int rangeNum = 0;
			Rst::timedata td;
			date.clear();
			FileIn >> date;
			Gettimedata(td);
			FileIn >> rangeNum;
			std::string tmp;
			for (int j = 0; j < rangeNum; j++) {
				FileIn >> tmp;
				name.emplace_back(std::move(tmp));//tmp的指针指向地址改变,已经相当于一个新的空变量.
			}
			Rst::CRecord.SaveRange(Rst::timeRange(td, date, name));
		}
		std::cout << ">>+:成功读取" << timeRange_size << "个时间集合统计数据!" << std::endl;
	}
	else {
		std::cout << ">:未检测到时间集合统计数据." << std::endl;
	}
	FileIn.close();
	return true;
}

void init() {
	Rst::consoleColor = Rst::getcolor();
	Rst::setcolor(B | I);
	//std::fill(std::begin(Rst::datas), std::end(Rst::datas), 0);//仅用于一维数组
	memset(Rst::datas, 0, sizeof(Rst::datas));//语法:目标 + 想要初始化成为的数值 + 目标大小 . 不要记错!
	Rst::CRecord.datainit();
	Rst::dapx = 0, Rst::dapy = 0;
	std::string TimeNow = Rst::timeCatch();
	std::cout << "读取系统时间成功!"+TimeNow << std::endl;
	std::regex_match(TimeNow, Rst::match, Rst::pattern_Time);
	int H = std::stoi(Rst::match[4]);//注意stoi不是string内部方法
	for (int i = 0; i < 6; i++) {
		if (H >= hourRange[i].first && H < hourRange[i].second) {
			std::cout << "成功检测时间:" << TimeNow << std::endl;
			Rst::setcolor(G | B | I);
			std::cout << RangeHello[i] << "好,Aethryrn!" << std::endl;
			break;
		}
	}
	Rst::setcolor(B | I);
	printf("\n\n{--->即将进行数据读取初始化...\n");
	if (datainit()) {
		std::cout << ">+<读取数据成功!" << std::endl;
	}
	else {
		std::cout << ">-<数据初始化失败,请检查文件database.dll" << std::endl;
	}
	Rst::setcolor(Rst::consoleColor);
	return;
}

bool save() {//待完成.

	std::ofstream FileOut("database.dll");
	if (!FileOut.is_open()) {
		std::cout << "打开文件失败!" << std::endl;
		FileOut.close();
		return false;
	}
	int col_datas = Rst::dapx;
	int row_datas = Rst::dapy;
	
	FileOut << col_datas << " " << row_datas;


	for (int i = 0; i <= col_datas; i++) {
		for (int j = 0;j <10; j++) {//此处的10表示餐品行的最大值.
			//if (i < col_datas && j >= 10) {
			//	break;//前面的列需要正常输出.这里的判断条件与上面的重叠,故删去.
			//}
			if (i == col_datas && j == row_datas) {
				break;//最后一行的终止退出,不包括指向的地点,因为dapx,dapy指向的永远是有效地址后的第一个空地址.
			}
			FileOut << "\n" << Rst::datas[i][j].name << " " << Rst::datas[i][j].cost;//格式:" string double";
		}
	}
	//输出格式:保证最后一位非空格.
	FileOut << "\n";//避免全部变成一行,故换行.


	//用于输出一整个timedata数据.必须保证格式正确.
	//-->>>> days total_cost total_income pure_income avg_income avg_pure_income total_number otherdata
	auto Puttimedata = [&](const Rst::timedata& td) {//仅读
		FileOut << " " << td.days << " ";
		FileOut << td.total_cost << " ";
		FileOut << td.total_income << " ";
		FileOut << td.pure_income << " ";
		FileOut << td.avg_income << " ";
		FileOut << td.avg_pure_income << " ";
		FileOut << td.total_number << " ";
		if ((int)td.otherdata.size() == 0)//避免多空一个空格导致读取失败.
			FileOut << "NULL";
		else 
		FileOut << td.otherdata;
		};

	const auto& da = Rst::CRecord.Saved_daily;
	const auto& tr = Rst::CRecord.Saved_timeRange;
	int da_len = (int)da.size();
	int tr_len = (int)tr.size();
	
	//输出大小
	FileOut << da_len;//统一规范,末尾无空格或换行
	for (int i = 0; i < da_len; i++) {
		FileOut << "\n" << da[i].operationDate;
		Puttimedata(da[i].dailySave);
	}

	//输出格式:此处占用一个换行位.
	FileOut << "\n";

	FileOut << tr_len;

	//timeRange->
		//xx date (TD) xx dateRange ... //onlydate,if not,this param will be ignore.
	for (int i = 0; i < tr_len; i++) {
		int tr_multi_len = (int)tr[i].multiSave.size();
		FileOut << "\n" << tr[i].operationDate;
		Puttimedata(tr[i].totalData);
		FileOut  << " " << tr_multi_len;
		for (int j = 0; j < tr_multi_len; j++) {
			FileOut << " " << tr[i].multiSave[j];
		}
	}

	//输出格式:此处占用一个换行位.
	FileOut.close();
	return true;
}

void logUpdate() {
	std::ofstream FileOut("log.txt",std::ios::app);//只追加
	if (!FileOut.is_open()) {
		std::cout << "存入日志失败" << std::endl;
		return;
	}
	else {
		std::cout << ">>_日志记录成功!" << std::endl;
	}
	std::string t = Rst::timeCatch();
	FileOut << "Opened at " << t << ",in Version:v" <<std::format("{:.1f}", Rst::Version) << std::endl;
}

int main() {
	//取
	init();
	logUpdate();
	(void)_getch();

	//逻辑部分
	//
	Rm ongoing = Rm::MAINMENU;//并不最优选择,但留作纪念.
	int debugMode = 0;
	while (ongoing != Rm::EXIT) {
		system("cls");
		Rst::ShowMenu(Cs::MAINMENU);
		Rm ret = static_cast<Rm>(_getch() - '0');
		switch (ret) {
		case(Rm::EXIT):
			if (save()) {
				std::cout << "保存成功!" << std::endl;
			}
			else {
				std::cout << "保存失败!" << std::endl;
			}
			ongoing = Rm::EXIT;
			break;
		case(Rm::REGISTER):
			Rst::Register();
			break;
		case(Rm::CALCULATE):
			Rst::Calcrlate();
			break;
		case(Rm::DEBUG): {
			debugMode++;
			system("cls");
			std::cout << std::endl << "再按" << 4 - debugMode << "次进入DEBUG模式" << std::endl;
			if (debugMode > 3) {
				std::cout << "\n*进入DEBUG模式!*" << std::endl;
				Rst::Debug();
				debugMode = 0;
			}
			break;
		}
		default:
			system("cls");
			break;
		}
	}
	return 0;
}
