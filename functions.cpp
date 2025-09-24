#include "core.hpp"

/**
 * @file functions.cpp
 * @brief 餐饮管理系统核心运转模块 - 逻辑运行与调控
 * @author Artcrk
 *
 */

namespace Rst { // B04
	bis datas[10][10];//餐品数据存储,
	calcRecord CRecord;//全局计算数据存储容器.

	int dapx, dapy;
	double Inf = 1e-4;//误差限制.

	HANDLE set_consoleColor = GetStdHandle(STD_OUTPUT_HANDLE);//颜色调用权限.
	std::regex pattern_size(R"((>|<|>=|<=|==)(-?\d+(\.?\d+)?))");//b26<<-----
	std::regex pattern_date(R"((\d{4})-(\d{1,2})-(\d{1,2}))");//年月日.
	std::regex pattern_Time(R"((\d{4})-(\d{2})-(\d{2})_(\d{2}):(\d{2}):(\d{2}))");
	std::regex pattern_sumCalc(R"(([axd]{1})(\d+))");//用于捕获suncalc使用的操作类型.
	std::string sizeSymbol[5] = { ">","<",">=","<=","==" };
	std::smatch match;//B27<<------B36<<-----再度使用会自动清空哦!
	WORD consoleColor = R | G | B;
	//*以下函数弃用,仅使用另一个重载,这是由于enum colorful设计的巧妙性,以及另一个重载适配性更好.但是留在这里,我还没写笔记呢
	// 
	//void setcolor(std::initializer_list<colorful> colors) {//使用initializer_list来捕获可变长度参数列表,需要注意由此方式包装的参数不能通过[]来访问,但是可以使用其自带的迭代器来访问.B39<<----
	//	WORD ColorMix = 0;//按照颜色的二进制位掩码逻辑,初始化为一个0是没问题的;B38<<-----
	//	for (const auto& i : colors) {//自动捕获列表,原理类似迭代器,刚好可以用于init_list的情况B37<<-----
	//		ColorMix |= i;//或等于B36<<-------
	//	}
	//	SetConsoleTextAttribute(set_consoleColor, ColorMix);
	//}

	/* @brief 用于快速设置控制台颜色;
	 * @param colors 想要放入的颜色(R,G,B,I,YELLOW,PINK,CYAN}也就是红绿蓝,亮色,黄色洋红青色,可以自由位运算..
	 *
	 * @note 示例:
	 *   setcolor(R|G|I);//黄色亮色.
	 *   setcolor(YELLOW);//黄色纯色.
	 */
	void setcolor(WORD COLOR) {//右值引用不能隐式转换B42<<-------------
		SetConsoleTextAttribute(set_consoleColor, COLOR);
	}

	/*
	 * @brief 获取当前屏幕的颜色
	 *
	 * @return 当前屏幕颜色的低四位WORD值.
	 */
	WORD getcolor() {//B40<<------获取当前屏幕颜色的流程!
		CONSOLE_SCREEN_BUFFER_INFO csbi;//<<step1
		GetConsoleScreenBufferInfo(set_consoleColor, &csbi);//step2
		return csbi.wAttributes & 0x0F;//step3,避免捕获高位(背景色)
	}

	//若当前控制台颜色不是xx,则设置为xx.
	void resetColor(WORD AIMCOLOR) {
		if (getcolor() != AIMCOLOR) {
			setcolor(AIMCOLOR);//切记三元运算符的后两位返回类型必须相同!!
		}
	}

	/**
	 * @brief 用于更新类型为timedata*的目标数据
	 *
	 * @param t_p 指向timedata类型的指针
	 * @param income 收入
	 * @param cost 成本
	 * @param mode 更新状态 ADD/ERASE.(default = Tdm::ADD)
	 *
	 * @return 运行成功状态
	 */
	bool timedataUpdate(timedata* t_p, double& income, double& cost, Tdm mode) {
		//检验异常条件
		if (t_p == nullptr || income < 0.0 || cost < 0.0) {
			return false;
		}
		if (mode == Tdm::ERASE && t_p->total_number < 1) {
			return false;
		}
		const int changeWay = static_cast<int>(mode);
		//对数据的整合计算
		t_p->total_number += changeWay;
		t_p->total_income += income * changeWay;
		t_p->pure_income += changeWay * (income - cost);
		t_p->total_cost += cost * changeWay;
		if (t_p->total_number > 0) {
			t_p->avg_pure_income = t_p->pure_income / t_p->total_number;
			t_p->avg_income = t_p->total_income / t_p->total_number;
		}
		else {
			t_p->avg_pure_income = 0;
			t_p->avg_income = 0;
		}
		return true;
	}

	/**
	 * @brief 餐品空缺位指针增减函数
	 *
	 * @param x 用于调控Addxy的行为,x可为ADD,SUB.(default = ADD)
	 * @return 返回函数的运行状态
	 *
	 * @throw 无显式异常,但在加减操作失败时返回false
	 *
	 *
	 */
	bool Addxy(Am x) {//实现对数据库指针xy的加减操作.//B03
		int sx = dapx, sy = dapy;
		bool st = true;
		int mode = (x == Am::ADD) ? 1 : -1;
		dapx += (dapy + mode) < 0 ? -1 : 0;
		dapx += (dapy + mode) > 9 ? 1 : 0;
		dapy = (dapy + mode + 10) % 10;
		(dapx < 0 || dapx>9) ? dapx = sx, dapy = sy, st = false : 0;//上界突破情况或着下界突破情况
		return st;
	}

	//用于比较任意两个timedata变量.建议右侧为较新的日期
	bool Compare(timedata& left, timedata& right) {
		setcolor(YELLOW);
		//打印比例为21:26:26:26:1
		//默认最后一个'|'要单独打印.
		//r:右边~    l:左边需要展示的数值
		auto print = [](double& r, double& l, const std::string& type) {//注意,这里加上const就能引用常量.constexpr是编译时常量,不能在运行期间修改,与const不同.
			double _sub = r - l;
			autoPrint(type, ' ', 21, Ed::LEFT, 1, true, Ag::LEFT, 10);
			autoPrint(std::format("{:.1f}", l), ' ', 26, Ed::LEFT, 1, true);
			autoPrint(std::format("{:.1f}", r), ' ', 26, Ed::ALL, 1, true);
			if (_sub > 0 + Inf) {
				resetColor(G | I);
				autoPrint(std::format("+{:.2f}", _sub), ' ', 26, Ed::EMPTY, 1, true);//增长时绿色,减少时红色.不变时白色.
			}
			else {
				if (_sub < 0 - Inf) {
					resetColor(R | I);
				}
				else {
					resetColor(R | G | B);
				}
				autoPrint(std::format("{:.2f}", _sub), ' ', 26, Ed::EMPTY, 1, true);
			}
			setcolor(YELLOW);
			printf("|\n");
			};
		autoPrint("%比较结果%", '=', WIDTH, Ed::EMPTY);

		autoPrint("", ' ', 21, Ed::LEFT, 1, true, Ag::LEFT, 10);
		autoPrint("左值:" + left.days, ' ', 26, Ed::LEFT, 1, true);
		autoPrint("右值:" + right.days, ' ', 26, Ed::ALL, 1, true);
		autoPrint("比较差值", ' ', 26, Ed::EMPTY, 1, true);//增长时绿色,减少时红色.不变时白色.
		printf("|\n");

		print(right.total_number, left.total_number, "总单数");
		print(right.total_income, left.total_income, "总收入");
		print(right.pure_income, left.pure_income, "总净收入");
		print(right.total_cost, left.total_cost, "总成本");
		print(right.avg_income, left.avg_income, "均收入");
		print(right.avg_pure_income, left.avg_pure_income, "均净收入");
		autoPrint("", '=', WIDTH, Ed::EMPTY);
		return true;
	}

	//@brief 打印对应数据库存储的元素
	bool ShowNowDataBase() {//数据展示,有另外两个重载.

		int i = 0, j = 0;
		if (!datas[i][j].occupied) {

			std::cout << "\n#数据库中没有存储数据!" << std::endl;
			return false;
		}
		for (i = 0; i < 10; i++) {
			if (!datas[i][0].occupied) {
				i = 20;
				break;
			}
			std::cout << std::format("===< {} >\n", i + 1);
			for (j = 0; j < 10; j++) {
				if (!datas[i][j].occupied) {
					j = 20;
					break;
				}
				std::cout << std::format("#<{}-{}>: {} $:{:.3f}\n", i + 1, j + 1, datas[i][j].name, datas[i][j].cost);//用于格式化输出数据文件.
			}
			std::cout << "\n";
		}
		return true;
	}

	//展示第index_x行的数据
	bool ShowNowDataBase(int index_x) {
		if (!datas[index_x][0].occupied) {
			std::cout << std::format("\n#<{}-*>中没有存储数据!", index_x + 1) << std::endl;
			return false;
		}
		std::cout << std::format("===< {} >\n", index_x + 1);
		for (int j = 0; j < 10; j++) {
			if (!datas[index_x][j].occupied) {
				j = 20;
				break;
			}

			std::cout << std::format("#<{}-{}>: {} $:{:.3f}\n", index_x + 1, j + 1, datas[index_x][j].name, datas[index_x][j].cost);
		}
		return true;
	}

	//展示index_x,index_y项的数据.
	bool ShowNowDataBase(int index_x, int index_y) {
		if (!datas[index_x][index_y].occupied) {
			std::cout << std::format("\n#<{}-{}>中没有存储数据!", index_x + 1, index_y + 1) << std::endl;
			return false;
		}
		std::cout << std::format("#<{}-{}>: {} $:{:.3f}\n", index_x + 1, index_y + 1, datas[index_x][index_y].name, datas[index_x][index_y].cost);
		return true;
	}

	/**
	 * @brief 键盘快捷键查找
	 *
	 * @return 查找到的项的坐标
	 *
	 * @note 目前只适用于[10][10]的数组大小
	 *
	 */
	coordInt CDserach() {//用于寻找对应的项,比较快速,相当于快捷键查找.
	ser_a1:
		system("cls");
		std::cout << "\nSerach模式下, 你可以通过按下ESC键快速退出该模式.\n";
		bool able01 = ShowNowDataBase();
		coordInt cod = { 0,0 };
		if (able01 == false) {
			std::cout << "\n数据库无数据存储,按任意键退出搜索模式";
			(void)_getch();
			return { -1,-1 };
		}
		int a1 = (int)_getch();
		if (a1 == 27) {
			return { -1,-1 };
		}
		else if ((int)(a1 - '0') - 1 < 0 || (int)(a1 - '0') - 1 > 9) {
			goto ser_a1;
		}
	ser_a2:
		system("cls");
		std::cout << "寻找到以下序列\n";
		bool able02 = ShowNowDataBase((int)(a1 - '0') - 1);
		if (able02 == false) { std::cout << "\n该处无数据存储,按任意键返回上一步"; }
		char a2 = _getch();
		if (a2 == 27) {
			return { -1,-1 };
		}
		if (a2 == (int)' ' || !able02 || ((int)(a2 - '0') - 1 < 0 || (int)(a2 - '0') - 1 > 9)) {
			goto ser_a1;
		}
		system("cls");
		std::cout << "寻找到项\n";
		bool able03 = ShowNowDataBase((int)(a1 - '0') - 1, (int)(a2 - '0') - 1);
		if (able03 == false) { std::cout << "\n该处无数据存储,按任意键返回上一步"; }
		else { std::cout << "确定选择该项吗?"; }
		char a3 = _getch();
		if (a3 == 27) {
			return { -1,-1 };
		}
		if (a3 == (int)' ' || !able03) {
			goto ser_a2;
		}
		system("cls");
		cod.X = (int)(a1 - '0') - 1, cod.Y = (int)(a2 - '0') - 1;
		return cod;
	}

	/**
	 * @brief 自动对齐打印函数
	 *
	 * @param text 待打印字符串,支持中英文
	 * @param fills 空隙填充符号(default = ' ')
	 * @param width 打印宽度(default = 100)
	 * @param L_R_edge 是否打印左右边框'|'.使用LRedge枚举常量(EMPTY,LEFT,RIGHT,ALL).(default = ALL)
	 * @param repeat 重复打印次数(default = 1);
	 * @param Right_API 是否向右拓展(default = false)
	 * @param alignment 对齐方式,使用AlignmentMode枚举常量MIDDLE,LEFT,RIGHT.(default = MIDDLE);
	 * @param percent 期望对齐的空格的百分比(default = 0),该参数不会在MIDDLE模式下被使用.
	 * @param saveOutput 是否保存输出而不输出.(default = false)
	 *
	 * @note 你经常能够看见我连用几个autoPrint来输出,这是用于连续打印几个并列表格.
	 *
	 * @return 本次产生的输出字符串,以及start和lengthS的数据.当出现异常时返回空字符串.
	 *
	 * @throw 无显式异常,但可能因为 文本长度过长 /重复次数小于0 /宽度小于0 而返回false
	 *
	 */
	outputString autoPrint(const std::string& text, const char& fills, int width, Ed L_R_edge, int repeat, bool Right_API, Ag alignment, int percent, bool saveOutput) {
		std::string totalOutput = "";

		int lengthS = static_cast<int>(text.length());//事实证明,中文字符的实际宽度占用就是2字节B23
		int start = 0;
		int percentWidth = ((width - 2) * percent / 100);
		switch (alignment) {
		case(Ag::MIDDLE):
			start = (width - lengthS) / 2 + 1;
			break;
		case(Ag::LEFT):
			start = 2;
			start += percentWidth;
			break;
		case(Ag::RIGHT):
			start = width - lengthS;
			start -= percentWidth;
			break;
		default:
			return { "",0,0 };
		}
		if (start < 2 || repeat <= 0 || width <= 2 || lengthS + start > width) {
			std::cout << "Error!打印字符过长/重复次数异常/宽度异常." << std::endl;
			return { "",0,0 };
		}
		else {
			while (repeat--) {
				L_R_edge == Ed::ALL || L_R_edge == Ed::LEFT ? totalOutput += "|" : totalOutput += fills;
				for (int i = 2; i < width; i++) {
					if (i == start) {
						totalOutput += text;
						i += lengthS;
					}
					i < width ? totalOutput += fills : "";//因为i++的缘故,保证每一次++都能够输出一个数据,否则将会在末尾少一个空格.
				}
				L_R_edge == Ed::ALL || L_R_edge == Ed::RIGHT ? totalOutput += "|" : totalOutput += fills;
				Right_API ? "" : totalOutput += "\n";
			}
		}
		if (saveOutput == false) {
			std::cout << totalOutput;//实际上优化了输出的流程,避免多次调用输出流
		}
		return { totalOutput, start, lengthS };
	}

	/**
	 * @brief 使用autoPrint完成预定义的菜单界面打印
	 *
	 * @param mode Cs类型的参数,选定要打印的菜单类型
	 * @param tmp 在CTR(calculate time range)模式下使用,是当前的数据.
	 *
	 * @details 在开头会自动打印一个换行x
	 *
	 */
	void ShowMenu(Cs mode, timedata* tmp) {

		printf("\n");
		switch (mode) {
		case(Cs::CALCRLATEMENU): {
			resetColor(R | G);
			autoPrint("", '-', WIDTH, Ed::EMPTY);
			autoPrint("{++++>计算模式菜单<++++}");
			autoPrint("", ' ', WIDTH, Ed::ALL, 2);

			autoPrint("< 1 > 计算某段时间的数据", ' ', 50, Ed::LEFT, 1, true, Ag::LEFT, 20);
			autoPrint("< 2 > 整合几个TD的数据", ' ', 50, Ed::RIGHT, 1, false, Ag::LEFT, 10);

			autoPrint("< 3 > 所有操作历史记录(仅读)", ' ', 50, Ed::LEFT, 1, true, Ag::LEFT, 20);
			autoPrint("< 4 > 计算结果汇总展示", ' ', 50, Ed::RIGHT, 1, false, Ag::LEFT, 10);

			autoPrint("< 5 > 单个日期比较", ' ', 50, Ed::LEFT, 1, true, Ag::LEFT, 20);
			autoPrint("< 6 > 整合数据比较", ' ', 50, Ed::RIGHT, 1, false, Ag::LEFT, 10);

			autoPrint("< ESC > 退出");
			autoPrint("", ' ', WIDTH, Ed::ALL, 2);
			autoPrint("", '-', WIDTH, Ed::EMPTY);
			break;
		}
		case(Cs::MAINMENU): {
			resetColor(CYAN);
			autoPrint("", '-', WIDTH, Ed::EMPTY);
			autoPrint("{[  _>主菜单<_  ]}");
			autoPrint("", ' ', WIDTH, Ed::ALL, 2);

			autoPrint("< 1 > 登记&注册&查看模式", ' ', 50, Ed::LEFT, 1, true, Ag::LEFT, 20);
			autoPrint("< 2 > 计算模式", ' ', 50, Ed::RIGHT, 1, false, Ag::LEFT, 10);

			autoPrint("< ESC > 退出");
			autoPrint("", ' ', WIDTH, Ed::ALL, 2);
			autoPrint("", '-', WIDTH, Ed::EMPTY);
			break;
		}
		case(Cs::REGISTERMENU): {
			resetColor(PINK);
			autoPrint("", '-', WIDTH, Ed::EMPTY);
			autoPrint("{---->注册模式菜单<----}");
			autoPrint("", ' ', WIDTH, Ed::ALL, 2);

			autoPrint("< 1 > 新录入", ' ', 50, Ed::LEFT, 1, true, Ag::LEFT, 20);
			autoPrint("< 2 > 删除", ' ', 50, Ed::RIGHT, 1, false, Ag::LEFT, 10);

			autoPrint("< 3 > 修改", ' ', 50, Ed::LEFT, 1, true, Ag::LEFT, 20);
			autoPrint("< 4 > 查看全部", ' ', 50, Ed::RIGHT, 1, false, Ag::LEFT, 10);

			autoPrint("< ESC > 退出");
			autoPrint("", ' ', WIDTH, Ed::ALL, 2);
			autoPrint("", '-', WIDTH, Ed::EMPTY);
			break;
		}
		case(Cs::CTR): {
			if (tmp == nullptr) {
				std::cout << "调用CTR面板时数据指针为空!";
				break;
			}
			resetColor(B | I);
			autoPrint("日期|< " + tmp->days + " >|日期", '-', WIDTH, Ed::EMPTY, 1);
			autoPrint("", ' ', WIDTH, Ed::ALL, 2);
			autoPrint("<' '>使用已录入数据进行新增,该操作会直接启动搜索程序.", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 20);
			autoPrint("<'z'>快捷录入一个临时数据,无需录入名字.", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 20);
			autoPrint("<'s'>快捷存档并计入计算,需要输入名字,数据会自动存储.", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 20);
			autoPrint("<'c'>查看历史录入记录,包括进行删除与修改操作.", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 20);
			autoPrint("< ESC >保存并退出(仅保存统计数据,不保存操作历史).", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 20);
			autoPrint("", ' ', WIDTH, Ed::ALL, 2);
			autoPrint("", '-', WIDTH, Ed::EMPTY);
			std::cout << "当前数据情况如下:" << std::endl;
			setcolor(YELLOW);
			//第一行
			autoPrint(std::format("[总收入>: {:.2f}]", tmp->total_income), '=', 40, Ed::ALL, 1, true, Ag::LEFT, 5);
			autoPrint(std::format("[总成本>: {:.2f}]", tmp->total_cost), '=', 30, Ed::RIGHT, 1, true, Ag::LEFT, 10);
			autoPrint(std::format("[总单数>: {:.0f}]", tmp->total_number), '=', 30, Ed::RIGHT, 1, false, Ag::LEFT, 10);
			//第二行
			autoPrint(std::format("[总净收入>: {:.1f}]", tmp->pure_income), '=', 40, Ed::ALL, 1, true, Ag::LEFT, 5);
			autoPrint(std::format("[单平均收入>: {:.2f}]", tmp->avg_income), '=', 30, Ed::RIGHT, 1, true, Ag::LEFT, 10);
			autoPrint(std::format("[单平均净收入>: {:.2f}]", tmp->avg_pure_income), '=', 30, Ed::RIGHT, 1, false, Ag::LEFT, 10);
			break;
		}
		case(Cs::RECORDMENU): {
			resetColor(B | I);
			autoPrint("#>>>当前处于检索模式!<<<#", '-', WIDTH, Ed::EMPTY);
			autoPrint("", ' ', WIDTH, Ed::ALL, 2);
			autoPrint("_在检索模式下,你可以做如下操作:", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 5);
			autoPrint("< 1 > 使用'>' '<' '<=' '>=' '=='来寻找对应*金额*的数据.(收入和成本均会检索)", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 5);
			autoPrint("示例:\">=200.0\",输入则会得到所有income/cost大于200.0或与200.0误差小于1e-5的数据.", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 5);
			autoPrint("< 2 > 直接使用数字与来搜索对应编号的数据.", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 5);
			autoPrint("示例:\"12\",输入后将会得到编号为12的数据.", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 5);
			autoPrint("< 3 > 使用名称模糊搜索", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 5);
			autoPrint("示例:\"233号商品01\",输入后将会对该key进行检索,会罗列出所有符合的key,并会对key进行颜色强调.", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 5);
			autoPrint("< ESC > 退出该模式.", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 5);
			autoPrint("", ' ', WIDTH, Ed::ALL, 2);
			autoPrint("", '-', WIDTH, Ed::EMPTY);
			break;
		}
		case(Cs::RECORDOPERATION): {
			resetColor(B | I);
			autoPrint(">]  !  [<", '=', WIDTH, Ed::EMPTY);
			autoPrint("", ' ', WIDTH, Ed::ALL, 2);
			autoPrint("是否对检索到的元素进行操作?(PS:当检索到多个元素,将会一起操作.)");
			autoPrint("< 1 > 删除搜索所得历史记录,并同时回退操作.", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 20);
			autoPrint("< 2 > 修改搜索所得历史记录(当目标个数大于1时,会逐个开始修改操作.)", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 20);
			autoPrint("< ESC > 退出修改并继续搜索检索.", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 20);
			autoPrint("", ' ', WIDTH, Ed::ALL, 2);
			autoPrint(">]  ?  [<", '=', WIDTH, Ed::EMPTY);
			break;
		}
		default: {
			std::cout << "调用失败!" << std::endl;
			return;
		}
		}
		setcolor(consoleColor);
		std::cout << "输入对应KEY进入功能>:";
		return;
	}

	/**
	 * @brief 界面展示函数的重载.
	 *
	 * @param mode 实际上好像不需要?只是一个过场.
	 * @param rod 历史记录的一个std::vector.
	 * @param special 是否选择特殊输出--也就是是直接全体打印还是检索后打印
	 * @param commonTokens target类型,用于存入找到的tokens,并在接下来的输出中使用.
	 * @param mapTokens map类图索引,存储了对应的点位信息和染色信息.此变量依赖于前一参数!
	 *
	 * @return 函数的运行状态,是否输出了/找到了元素.
	 *
	 */
	bool ShowMenu(std::vector<record>& rcd, Rft special, std::vector<target>* commonTokens,
		std::map<int, std::vector<int>>* mapTokens) {//B24
		resetColor(YELLOW);
		if (special == Rft::ORGINIAL) {
			std::cout << "开始检索所有操作的历史记录,最上面的为最新." << std::endl;
		}
		else {
			std::cout << "以下为索引找到的对应记录.索引对应内容已被染色." << std::endl;
		}
		//UI打印
		autoPrint("", '-', WIDTH, Ed::EMPTY);
		autoPrint("NUM", ' ', 6, Ed::ALL, 1, true);
		autoPrint("操作名称", ' ', 54, Ed::ALL, 1, true);
		autoPrint("收入", ' ', 20, Ed::ALL, 1, true);
		autoPrint("成本", ' ', 20, Ed::ALL, 1, false);
		//记录界面打印.
		/*
		 * @brief 用于展示特定菜单并进行特项染色.
		 * @param index 传入的最终下标.需要直接指向record.
		 * @param orginialIndex 传入的原始坐标(commonTokens内部索引).当此项不为大于等于0时将会使用对应下标下commonTokens内存储的额外数据进行染色.
		 * @param mapDye 由于map的键值就是index,无需传入额外参数或是限定.此项为真的时候将使用图的数据
		 *
		 *
		 *
		 * @warning { 务必不要传入空容器.
		 *  务必不要在没有传入图或vector的时候进行非Rft::ORGINIAL调用.
		 *  务必不要尝试同时使用commonDye和mapDye.
		 * }
		 *
		 */
		auto dataView = [&](int index, int orginialIndex = -1, bool mapDyeMode = false) {
			//B25使用lambda表达式避免函数碎片化分布,并且避免了DRY问题.使用&是自动捕获局部变量
			bool commonDye = (orginialIndex >= 0 && mapDyeMode == false);
			bool mapDye = (orginialIndex == -1 && mapDyeMode == true);
			if (commonDye && (*commonTokens)[orginialIndex].otherMessage.find("_index_") != std::string::npos) {
				setcolor(HIGHLIGHT);//设置为红色高亮
			}
			autoPrint(std::format("{:03d}", index), ' ', 6, Ed::ALL, 1, true);//---B22
			if (commonDye)resetColor(YELLOW);
			if (mapDye) {//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<功能有误!!!
				outputString tempSave = autoPrint(rcd[index].operateName, ' ', 54, Ed::ALL, 1, true, Ag::MIDDLE, 0, true);
				std::string stringPiece = "";
				int pieceLength = (int)(*commonTokens)[0].otherMessage.length();//捕获目标长度.
				int cutIndex = tempSave.start - 1;//一定注意,autoPrint内start是指**元素位置**而非下标!.
				int dynamicStartIndex = cutIndex;
				int totalSize = 54;/*这个54是autoPrint的宽度,就在上面那个生成tempSave的autoPrint内*/
				int aimStartIndex = 0;
				std::queue<outputString> outputStringPiece;//此处只识别start元素.当start元素>0时染色,当start元素==0时不染色.只是为了复用已有的功能相似的结构体.
				//这里先把map指针解引用为map类型,随后再键入index值来到此元素的特定vector,再对此vector进行size操作.保证对于每一个vector都必定不为空(见recordFind()中mapContainer的存储逻辑.)

				outputStringPiece.push({ tempSave.words.substr(0,dynamicStartIndex),0,0 });//先导片段压队
				for (int i = 0; i < (int)(*mapTokens)[index].size() && dynamicStartIndex < cutIndex + tempSave.length; i++) {
					aimStartIndex = cutIndex + (*mapTokens)[index][i];
					outputStringPiece.push({ tempSave.words.substr(dynamicStartIndex, aimStartIndex - dynamicStartIndex),0,0 });//前置片段压队.
					outputStringPiece.push({ tempSave.words.substr(aimStartIndex, pieceLength),1,0 });//目标片段压队
					dynamicStartIndex = aimStartIndex + pieceLength;
				}
				if (dynamicStartIndex < totalSize) {
					outputStringPiece.push({ tempSave.words.substr(dynamicStartIndex),0,0 });//尾部片段压队
				}
				while (!outputStringPiece.empty()) {
					outputString look = outputStringPiece.front();

					if (outputStringPiece.front().start != 0) {
						resetColor(HIGHLIGHT);
					}
					std::cout << outputStringPiece.front().words;
					outputStringPiece.pop();
					resetColor(YELLOW);
				}
			}
			else {
				autoPrint(rcd[index].operateName, ' ', 54, Ed::ALL, 1, true);
			}
			if (commonDye && (*commonTokens)[orginialIndex].otherMessage.find("_income_") != std::string::npos) {
				setcolor(HIGHLIGHT);
			}
			autoPrint(std::format("${}", rcd[index].income), ' ', 20, Ed::ALL, 1, true);
			if (commonDye) {
				if ((*commonTokens)[orginialIndex].otherMessage.find("_cost_") != std::string::npos) {
					resetColor(HIGHLIGHT);
				}
				else {
					resetColor(YELLOW);
				}
			}
			autoPrint(std::format("${}", rcd[index].cost), ' ', 20, Ed::ALL, 1, false);
			resetColor(YELLOW);
			};

		if (special == Rft::ORGINIAL) {//这里的分类方式非常简洁干净,以后遇到只需要代换一个值的问题可以考虑lambda表达式.
			for (int i = static_cast<int>(rcd.size()) - 1; i >= 0; i--) {
				dataView(i);
			}
		}
		else if (special == Rft::SPECIAL) {
			for (int i = (int)commonTokens->size() - 1; i >= 0; i--) {
				dataView((*commonTokens)[i]._key, i);//完成了对仅数类型的搜索
			}
		}
		else if (special == Rft::FUZZY) {
			for (int i = (int)commonTokens->size() - 1; i >= 0; i--) {
				dataView((*commonTokens)[i]._key, -1, true);
			}
		}
		else {
			return false;
		}
		autoPrint("", '-', WIDTH, Ed::EMPTY);
		resetColor(consoleColor);
		return true;
	}

	/**
	 * @brief 用于检索对应的历史记录
	 * @param rcd 目标历史记录
	 * @param tdPointer 当前的timedata容器.记住timedata可以保存一整天的信息,是一个集合,所以不需要数组之类.
	 *
	 * @return 运行成功状态.
	 */
	bool recordFind(std::vector<record>& rcd, timedata* tdPointer) {
		bool onRecordFind = true;
		std::vector<target> tokenContainer;
		int length_rcd = (int)rcd.size();
		std::map<int, std::vector<int>> mapContainer;//图token.优先级低于tokenContainer,假设该容器有元素,则tokenContainer必有元素.
		std::vector<target> tmpContainer;
		bool toOperating = false;
		while (onRecordFind) {
			system("cls");
			if (toOperating) {
				bool operating = false;
				if (!mapContainer.empty()) {
					ShowMenu(rcd, Rft::FUZZY, &tokenContainer, &mapContainer);
					operating = true;
				}
				else if (!tokenContainer.empty()) {
					ShowMenu(rcd, Rft::SPECIAL, &tokenContainer);
					operating = true;
				}
				if (operating) {
					bool onOperation = true;
					while (onOperation) {
						if (operating != true) system("cls");
						ShowMenu(Cs::RECORDOPERATION);
						if (rcd.empty() || tokenContainer.empty()) {//避免在vector为空的情况下使用erase()!!
							std::cout << "搜索列表为空!按任意键退出..." << std::endl;
							onOperation = false;
							toOperating = false;
							(void)_getch();
							break;
						}
						if (operating != true) {
							if (!mapContainer.empty()) {
								ShowMenu(rcd, Rft::FUZZY, &tokenContainer, &mapContainer);
							}
							else {
								ShowMenu(rcd, Rft::SPECIAL, &tokenContainer);
							}
						}
						Rcm choose = static_cast<Rcm>(_getch() - '0');
						switch (choose) {
						case(Rcm::CHANGE): {
							int changeData = 0;
							int lengthC = (int)tokenContainer.size();
							for (int i = 0; i < lengthC; i++) {
								std::cout << "\n当前正在修改第_" << i + 1 << "_项.进度:: [" << i + 1 << " / " << lengthC << "]." << std::endl;
								std::cout << "编号为" << std::format("< {:03d} >", tokenContainer[i]._key) << ".预览如下." << std::endl;
								tmpContainer.clear();
								tmpContainer.push_back({ tokenContainer[i]._key,"" });
								changeData = 0;
								ShowMenu(rcd, Rft::SPECIAL, &tmpContainer);
								timedataUpdate(tdPointer, rcd[tokenContainer[i]._key].income, rcd[tokenContainer[i]._key].cost, Tdm::ERASE);
								std::cout << "正在修改income! 输入-1可以沿用原值." << std::endl;
								std::cin >> changeData;
								if (changeData != -1) {
									rcd[tokenContainer[i]._key].income = changeData;
								}
								std::cout << "正在修改cost! 输入-1可以沿用原值." << std::endl;
								std::cin >> changeData;
								if (changeData != -1) {
									rcd[tokenContainer[i]._key].cost = changeData;
								}
								timedataUpdate(tdPointer, rcd[tokenContainer[i]._key].income, rcd[tokenContainer[i]._key].cost, Tdm::ADD);
								std::cout << "操作成功!" << std::endl;
								tmpContainer[0].otherMessage = "_income__cost_";//高光
								std::cout << "预览如下,按任意键继续" << std::endl;
								ShowMenu(rcd, Rft::SPECIAL, &tmpContainer);
								(void)_getch();
							}
							std::cout << "修改完成!" << std::endl;
							operating = false;
							break;
						}
						case(Rcm::ERASE): {
							//仅完成了删除操作,并没有撤回加成!待完成!
							//从后面往前删除,不会导致索引修改!!
							int num = (int)tokenContainer.size();
							for (int i = (int)tokenContainer.size() - 1; i >= 0; i--) {
								timedataUpdate(tdPointer, rcd[tokenContainer[i]._key].income, rcd[tokenContainer[i]._key].cost, Tdm::ERASE);//太方便了!还好之前给timdataUpdate写了删除的实现.
								rcd.erase(rcd.begin() + tokenContainer[i]._key);
							}
							tokenContainer.clear();
							std::cout << "删除成功!已成功删除" << num << "个元素.按任意键退出...";
							mapContainer.clear();
							toOperating = false;
							onOperation = false;
							(void)_getch();
							break;
						}
						case(Rcm::EXIT): {
							onOperation = false;
							toOperating = false;
							break;
						}
						default:
							break;
						}
					}
				}
			}
			toOperating = false;
			system("cls");
			ShowMenu(rcd, Rft::ORGINIAL);
			if (rcd.empty()) {
				std::cout << "历史记录为空!按任意键退出." << std::endl;
				(void)_getch();
				return false;
			}
			ShowMenu(Cs::RECORDMENU);
			Rfw choose = static_cast<Rfw>(_getch() - '0');
			switch (choose) {
			case(Rfw::EXIT): {
				onRecordFind = false;
				break;
			}
			case(Rfw::SIZE): {
				std::cout << "\n当前正在进行大小比较搜索!>:";
				std::string aimToken = "";
				mapContainer.clear();
				tokenContainer.clear();
				double aimSize = 0.0;//捕获的比较大小.

				Ssb compareMethod = Ssb::EMPTY;
				std::string coloringTarget = "";
				int progressBar = 0;//最大50;

				while (TRUE) {
					std::cin >> aimToken;
					if (std::regex_match(aimToken, match, pattern_size) && !aimToken.empty()) {
						break;
					}
					else {
						std::cout << "输入格式有误!请确认为比较符号'><='组成的前缀加上纯数字或小数>:" << std::endl;
					}
				}
				//数据捕获和异常检索,需要注意,如若遇到异常,将无法回到重新输入阶段,只能回到recordFind菜单.
				try {//B29<<-----
					aimSize = std::stod(match[2].str());//B28<<------
				}
				catch (const std::invalid_argument&) {//B30<<-----
					std::cerr << "无效的字符格式:" << match.str() << std::endl;
					break;
				}
				for (int i = 0; i < 5; i++) {
					if (match[1].str() == sizeSymbol[i]) {
						compareMethod = static_cast<Ssb>(i);
					}
				}
				if (compareMethod == Ssb::EMPTY) {
					std::cout << "ERROR,比较方式为空." << std::endl;
					break;
				}
				//搜索并存储的阶段.需要注意,当误差在Inf内的时候程序判断为相等,即使有微小的大于或小于也会忽略.
				//示例,输入'>10.0',检索到10.000001,不会把该元素列入检索到的范围,因为此时认为该元素与输入相等,而'>'恰好排除了相等的情况,注意.

				printf("|");
				for (int i = 0; i < length_rcd; i++) {//想要存储对应元素所在的位置并染色....--完成!
					//进度条...--完成!!
					coloringTarget.clear();
					for (; progressBar < ceil((i + 1) * 50.0 / length_rcd); progressBar++) {
						printf("#");
					}
					auto check = [&](double value, std::string aimName) -> bool {//B35<<--
						bool equal = (value > aimSize - Inf) && (value < aimSize + Inf);
						bool larger = value > aimSize + Inf;
						bool less = value < aimSize - Inf;//避免模糊大于/小于.

						if (equal && (compareMethod == Ssb::LARGER_EQUAL
							|| compareMethod == Ssb::EQUAL || compareMethod == Ssb::LESS_EQUAL)) {
							coloringTarget += "_" + aimName + "_";
							return true;
						}
						if (larger && (compareMethod == Ssb::LARGER || compareMethod == Ssb::LARGER_EQUAL)) {
							coloringTarget += "_" + aimName + "_";
							return true;
						}
						if (less && (compareMethod == Ssb::LESS || compareMethod == Ssb::LESS_EQUAL)) {
							coloringTarget += "_" + aimName + "_";
							return true;
						}
						return false;
						};
					bool _co_find = check(rcd[i].income, "income");
					bool _inc_find = check(rcd[i].cost, "cost");
					if (_co_find || _inc_find) {//不能使用 || !!因为在检测到前面的值正确后第二个判断条件不会执行!
						tokenContainer.push_back({ i,coloringTarget });
					}
				}
				for (; progressBar < 50; progressBar++) {
					printf("#");
				}
				printf("|<<已完成!!\n");//这是进度条的一部分...
				//目前仅得到了对应token组,暂时没有展示. --已完成!
				ShowMenu(rcd, Rft::SPECIAL, &tokenContainer);
				std::cout << "按任意键退出...";//这里需要想办法做一个让输出界面能够滞留的逻辑.
				toOperating = true;
				(void)_getch();
				break;
			}
			case(Rfw::NUMBER): {
				std::cout << "\n当前正在进行数字下标搜索!>:";
				int numberAimToken = -1;
				tokenContainer.clear();
				mapContainer.clear();
				while (true) {
					std::cout << "输入一个非负整数(下标)" << std::endl;
					std::cin >> numberAimToken;
					if (numberAimToken < (int)rcd.size() && numberAimToken >= 0) {
						tokenContainer.push_back({ numberAimToken, "_index_" });//这就是token.
						break;
					}
					else if (numberAimToken < (int)rcd.size()) {
						std::cout << "数据不存在!" << std::endl;
						break;
					}
					else {

						std::cout << "无效输入." << std::endl;
					}
				}
				toOperating = true;
				break;
			}
			case(Rfw::NAME): {
				std::cout << "\n当前正在模糊搜索!输入大致名字以搜索对应目标.>:";
				mapContainer.clear();
				tokenContainer.clear();
				std::string aimToken;
				std::cin >> aimToken;
				int length_token = (int)aimToken.length();
				int tokenIndex;
				for (int i = 0; i < length_rcd; i++) {
					//注意,C++里赋值语句赋值后,在布尔表达式内部已经相当于该变量了B37<<------
					tokenIndex = 0;//一定要记得状态还原
					while (rcd[i].operateName.find(aimToken, tokenIndex) != std::string::npos) { //注意条件就是不为npos!这个东西不会返回false之类的东西.
						//当find中的tokenIndex大于长度时,会返回npos!
						tokenIndex = (int)rcd[i].operateName.find(aimToken, tokenIndex);
						std::cout << tokenIndex << "},[" << aimToken << "],<" << rcd[i].operateName << std::endl;
						mapContainer[i].push_back(tokenIndex);//存入图内.注意这里操作的是对于的存储空间!B38<<-----
						tokenIndex += length_token;//直接加,避免中文斩断等问题.B39<<------
					}
					if (!mapContainer[i].empty()) {
						tokenContainer.push_back({ i,aimToken });//统一到tokenContainer内部,简化代码.
					}
				}
				toOperating = true;
				//--dotoken-----
				break;
			}
			default:
				break;
			}
		}
		std::cout << "按任意键退出...";
		(void)_getch();
		return true;
	}

	/**
	 * @brief 返回操作记录的快速函数
	 *
	 * @param std::string oper 操作名称
	 * @param double& income 收入(正向数据)
	 * @param double& cost 成本(负面数据)
	 *
	 * @return 返回整合好的记录类型record
	 *
	 */
	struct record autoRecord(std::string oper, double& income, double& cost) {
		record temp = { oper,income,cost };
		return temp;
	}

	//CALC_TIME_RANGE计算面板,用于统计一天的数据.  在9-18日,我十分骄傲的宣布:---CTR在v1.0的所有功能已完成!
	bool CTR_menu() {
		system("cls");
		std::cout << "当前版本" << Rst::Version << "暂时不支持多日期同时统计请等待更新." << std::endl;
		bool onRunningName = true, onRunningMain = true;
		timedata* tmp = new timedata;
		std::vector<struct record> history_CTR;

		while (onRunningName) {
			int m[13] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 };
			std::cout << "--->统计一段时间的数据!请输入时间范围.\nYYYY-MM-DD\n" << std::endl;
			tmp->days = "NULL";
			std::string input = "";
			std::cin >> input;
			if (std::regex_match(input, match, pattern_date)) {
				int Y = std::stoi(match[1]);
				int M = std::stoi(match[2]);
				int D = std::stoi(match[3]);
				bool leapYear = (Y % 100 != 0 && Y % 4 == 0) || Y % 400 == 0;
				if (Y < 2000 || Y > 2100 || M < 1 || M > 12 || D < 1 || D > 31) {
					std::cout << "日期伪造!\n";
					continue;
				}
				if (leapYear && M == 2) m[2]++;//局部变量每次遍历初始化.
				if (D > m[M]) {
					std::cout << "日期伪造!\n";
					continue;
				}
				tmp->days = std::format("{:04d}-{:02d}-{:02d}", Y, M, D);
			}
			else {
				std::cout << "格式不正确!,注意格式为YYYY-MM-DD.\n";
				continue;
			}
			std::cout << "确定为" << tmp->days << "统计日期?按' '(空格)撤销,按下Esc退出";
			char ret_02n = _getch();
			if (ret_02n == (char)27) {//检索到esc并退出.
				return false;
			}

			ret_02n == ' ' ? onRunningName = true : onRunningName = false;
		}
		while (onRunningMain) {
			system("cls");
			ShowMenu(Cs::CTR, tmp);
			CTR ret_02i = static_cast<CTR>(_getch());
			switch (ret_02i) {
			case(CTR::EXIT): {
				if (history_CTR.empty()) {
					std::cout << "本次没有录入任何数据!将于一秒后直接退出." << std::endl;
					Sleep(1000);
					onRunningMain = false;
					break;
				}
				std::cout << "确定退出?再次点击ESC即可保存并退出\n点击其他键撤销退出." << std::endl;
				char yesORno = _getch();
				if (yesORno != (char)27) {
					break;
				}
				onRunningMain = false;
				timedata saveData = *tmp;
				//所有保存的整天数据.
				CRecord.SaveDaily(daily(*tmp, timeCatch()));
				break;
			}
			case(CTR::CHECK): {
				bool onCheck = true;

				while (onCheck) {
					system("cls");
					ShowMenu(history_CTR);
					if (history_CTR.empty()) {
						std::cout << "历史记录为空!" << std::endl;
						onCheck = false;
						break;
					}
					std::cout << "需要对历史记录进行操作吗?按空格开始操作,按Esc退出." << std::endl;
					bool succesInput = false;
					while (!succesInput) {
						int choose = (int)_getch();
						switch (choose) {
						case(27): {
							succesInput = true;
							onCheck = false;
							break;
						}
						case(32): {
							if (history_CTR.empty()) {
								std::cout << "历史记录为空!" << std::endl;
								onCheck = false;
							}
							recordFind(history_CTR, tmp);
							onCheck = false;
							succesInput = true;
							break;
						}
						default:
							break;
						}
					}
				}
				break;
			}
			case(CTR::NEW_IN): {
				std::cout << "输入大致成本(建议向上取整)" << std::endl;
				double _cost = 0.0;
				std::cin >> _cost;
				std::cout << "输入收入>:";
				double _income = 0.0;
				std::cin >> _income;

				timedataUpdate(tmp, _income, _cost);
				history_CTR.push_back(autoRecord("快捷录入:[-]", _income, _cost));
				std::cout << "<<<录入成功!";
				Sleep(120);
				break;
			}
			case(CTR::OLD_IN): {
				coordInt aim = CDserach();
				if (aim.X == -1) {
					break;
				}
				std::cout << "已自动录入 " << datas[aim.X][aim.Y].name << " 的成本:" << datas[aim.X][aim.Y].cost << std::endl;
				std::cout << "输入收入>:";
				double _income = 0.0;
				std::cin >> _income;

				timedataUpdate(tmp, _income, datas[aim.X][aim.Y].cost);
				history_CTR.push_back(autoRecord("库录入:[" + datas[aim.X][aim.Y].name + "]", _income, datas[aim.X][aim.Y].cost));
				std::cout << "<<<录入成功!";
				Sleep(120);
				break;
			}
			case(CTR::SAVE_IN): {
				coordInt aim = toRegist();
				std::cout << "\n已自动录入并保存 " << datas[aim.X][aim.Y].name << " 的成本:" << datas[aim.X][aim.Y].cost << std::endl;
				std::cout << "输入收入>:";
				double _income = 0.0;
				std::cin >> _income;

				timedataUpdate(tmp, _income, datas[aim.X][aim.Y].cost);
				//文本过长会无法打印,待修复...
				history_CTR.push_back(autoRecord(std::format("录入&保存[{}]", datas[aim.X][aim.Y].name, aim.X, aim.Y), _income, datas[aim.X][aim.Y].cost));
				std::cout << "<<<录入成功!";
				Sleep(120);
				break;
			}
			}
		}
		return true;
	}

	////捕获调用该函数时的时间,并返回由这个时间组成的固定格式字符串,避免传回结构体,但使用的是传统的time.h,已弃用
	//std::string timeCatch() {
	//	time_t timep;
	//	tm* t;
	//	time(&timep);//这是捕获时间的流程,先用time捕获时间,再用localtime转化为当地时间,并且以结构体形式输出.
	//	t = localtime(&timep);
	//	return std::format("{:04d}-{:02d}-{:02d}_{:02d}:{:02d}:{:02d}", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);//此处注意.
	//}

	//捕获调用该函数时的时间,并返回由这个时间组成的固定格式字符串
	std::string timeCatch() {
		auto Nowtime = std::chrono::system_clock::now();
		time_t now_c = std::chrono::system_clock::to_time_t(Nowtime);
		tm* nc = localtime(&now_c);
		return std::format("{:%Y-%m-%d_%H:%M}:{:02d}", Nowtime, nc->tm_sec);//这里!!format和chrono联动,使用特定的格式说明符来捕获时间信息.并且要用now函数B44<<---
		//由于没有找到好的方法来处理chrono秒数的小数点精确,只好暂时吧chrono转化为time_t和tm这种经典数据来输出,这样也方便格式化输出.
	}

	/**
	 * @brief 登记函数,直接执行注册流程
	 *
	 * @return 返回刚刚录入数据所在的坐标,当数据库已满时将返回{-233,-233};
	 */
	coordInt toRegist() {
		coordInt position = { dapx,dapy };
		if (!Rst::Addxy(Am::ADD)) {
			std::cout << ">>>数据库已满!!" << std::endl;
			return { -233,-233 };
		}
		Addxy(Am::SUB);
		Rst::bis tmp;
		std::cout << "输入商品名>:";
		std::cin >> tmp.name;
		std::cout << "输入商品成本>:";
		std::cin >> tmp.cost;
		tmp.occupied = true;
		//Rst::datas[Rst::dapx][Rst::dapy] = tmp;//可能出错,不能直接拷贝.
		datas[dapx][dapy] = std::move(tmp);//直接移动,避免拷贝行为异常.
		std::cout << "录入成功!按任意键返回.";
		Addxy(Am::ADD);
		(void)_getch();
		return position;
	}

	/**
	 * @brief 整合几个计算好的时间数据, 并需要输入整合的名字, 自动保存整合好的数据
	 *
	 * @param ptr 需要统计的时间数据的数组.
	 * @param names 期待的取的名字.
	 *
	 * @return 是否成功保存.
	 *
	 * @warning 需要注意,存入的过程为转移语义,会清空原有数据,不过调用时逻辑也大致需要吧daily内部数据拔出来组成一个新的vector,应此这反而是最好的方式.
	 */
	bool sumCalc(std::vector<timedata>& ptr, std::string& names) {
		if (ptr.empty() || names.empty()) {
			std::cout << "空访问!";
			return false;
		}
		timedata temp = {};
		std::vector<std::string> moveSave = {};
		int lengthV = (int)ptr.size();
		temp.days = names;
		temp.otherdata = std::format("共计{}者数据的汇总", lengthV);//最现代的格式化函数,其中{}为占位符(相当于%s%d之类),{:.2f}中的:相当于转义符号,释义.2f是格式化处理符号.
		for (int i = 0; i < lengthV; i++) {
			temp.total_cost += ptr[i].total_cost;
			temp.total_income += ptr[i].total_income;
			temp.total_number += ptr[i].total_number;
			temp.pure_income += ptr[i].pure_income;
			moveSave.emplace_back(ptr[i].days);//push + move=移动.emplace = 直接创建新变量.
		}
		temp.avg_income = temp.total_cost / temp.total_number;
		temp.avg_pure_income = temp.pure_income / temp.total_number;
		if (CRecord.SaveRange(timeRange(temp, timeCatch(), moveSave)))//注意需要构造为timeRange类型.
			return true;
		else
			return false;
	};

	//整体数据展示,参数为打印的数据类型,默认ALL
	void calcRecord::ShowSavedRecord(Pr mode = Pr::ALL, std::vector<int>* choice = nullptr) {

		const bool _tr = CRecord.Saved_timeRange.empty();
		const bool _d = CRecord.Saved_daily.empty();
		if (choice != nullptr) {
			if (choice->empty()) {
				choice = nullptr;
			}
			else if ((*choice)[0] < 0) {
				std::cerr << "存有异常下标的choice数组." << std::endl;
				choice = nullptr;
			}
		}
		//特殊比例,10::30:30:30,大致视图如下
		//XXX:YY-mm-dd-HH-MM-SS>
		//+------->> |name=====================|
		//          | xxx     | xxx     | xxx  |
		//          | xxx     | xxx     | xxx  |
		//          |        vvvvvvvvvv        |
		//XXX:YY-mm-dd-HH-MM-SS>
		//+------->> |name=====================|
		//          | xxx     | xxx     | xxx  |
		//          | xxx     | xxx     | xxx  |
		//          |        vvvvvvvvvv        |
		auto print = [](timedata& aim, std::string date, int& i) {
			date = "操作日期:" + date + ">";
			std::cout << date << ">_" << std::format("{:03d}_/", i);
			std::cout << "\n+------->> ";
			autoPrint(">" + aim.days, '=', 89, Ed::ALL, 1, false, Ag::LEFT);

			printf("          ");
			autoPrint(std::format("总单数:#{:.0f}", aim.total_number), ' ', 30, Ed::LEFT, 1, true, Ag::LEFT, 10);
			autoPrint(std::format("总收入:${:.1f}", aim.total_income), ' ', 30, Ed::LEFT, 1, true, Ag::LEFT, 10);
			autoPrint(std::format("总成本:${:.1f}", aim.total_cost), ' ', 30, Ed::ALL, 1, false, Ag::LEFT, 10);

			printf("          ");
			autoPrint(std::format("净收入:${:.2f}", aim.pure_income), ' ', 30, Ed::LEFT, 1, true, Ag::LEFT, 10);
			autoPrint(std::format("均收入:${:.2f}", aim.avg_income), ' ', 30, Ed::LEFT, 1, true, Ag::LEFT, 10);
			autoPrint(std::format("均净收入:${:.2f}", aim.avg_pure_income), ' ', 30, Ed::ALL, 1, false, Ag::LEFT, 10);

			printf("          ");
			autoPrint("备注信息:" + aim.otherdata, ' ', 90, Ed::ALL, 1, false, Ag::MIDDLE, 5);
			};
		int highlight = -1;
		if (choice != nullptr) highlight = 0;
		if (_tr && _d) {
			std::cout << "历史记录为空!" << std::endl;
			return;
		}
		if (!_d && (mode == Pr::DAILY || mode == Pr::ALL)) {
			resetColor(G | I);
			int len = (int)CRecord.Saved_daily.size();
			if (len != 0) {
				std::cout << "检索到 " << len << " 条*单日*数据,开始展示:" << std::endl;
				autoPrint("Start", '=', WIDTH, Ed::EMPTY);
				autoPrint("", ' ', WIDTH, Ed::EMPTY);
				for (int i = 0; i < len; i++) {
					if (highlight != -1 && (*choice)[highlight] == i) {
						setcolor(B | R | I);
						if (++highlight == choice->size()) {
							highlight = -1;
						}
					}
					print(CRecord.Saved_daily[i].dailySave, CRecord.Saved_daily[i].operationDate, i);
					resetColor(G | I);
				}
				autoPrint("", ' ', WIDTH, Ed::EMPTY);
				autoPrint("End", '=', WIDTH, Ed::EMPTY);
				setcolor(consoleColor);
			}
		}
		if (!_tr && (mode == Pr::TIMESUM || mode == Pr::ALL)) {
			resetColor(G | I);
			int len = (int)CRecord.Saved_timeRange.size();
			if (len != 0) {
				std::cout << "检索到 " << len << " 条*集合*数据,开始展示:" << std::endl;//有问题,待调试.<<-----------
				autoPrint("Start", '=', WIDTH, Ed::EMPTY);
				autoPrint("", ' ', WIDTH, Ed::EMPTY);
				for (int i = 0; i < len; i++) {
					print(CRecord.Saved_timeRange[i].totalData, CRecord.Saved_timeRange[i].operationDate, i);
				}
				autoPrint("", ' ', WIDTH, Ed::EMPTY);
				autoPrint("End", '=', WIDTH, Ed::EMPTY);
				setcolor(consoleColor);
			}
		}
	}

	/**
	 * @brief 保存传入的数据,需要传入一个完整的daily数据.
	 * @param data daily的右值引用,建议加上std::move().
	 *
	 * @return 是否成功录入元素
	 */
	bool calcRecord::SaveDaily(daily&& data) noexcept {
		if (data.dailySave.total_number <= 0) {
			return false;
		}
		//catch time()
		CRecord.Saved_daily.emplace_back(std::move(data));//构造
		return true;
	}

	/**
	 * @brief 保存传入的数据,需要传入一个完整的timeRange类型数据
	 * @param data timeRange的右值引用,建议加上std::move().
	 *
	 * @return 是否成功录入元素
	 */
	bool calcRecord::SaveRange(timeRange&& data) noexcept {//注意:有名字的就是左值,无名字的才是右值.使用右值引用构造的左值的时候,需要重新用move把左值换成右值.
		if (data.totalData.total_number <= 0 || data.multiSave.empty()) {
			return false;
		}
		CRecord.Saved_timeRange.emplace_back(std::move(data));//直接重新构造
		return true;
	}

	//数据初始化.
	void calcRecord::datainit() {
		Saved_daily.clear();
		Saved_timeRange.clear();
	}

	/*
	 * @brief 使用二分在一个有序数组种寻找某一值,允许多种类型.
	 * @param vec vector数组,需要寻找的数组.
	 * @param aim 需要寻找的数据.
	 *
	 * @note 会在操作前检查数组是否有序,若无需将会自动sort.
	 */
	template<typename T>
	int dichotomyFind(std::vector<T>& vec, const T& aim) {
		if (vec.empty()) {
			return -1;
		}
		if (!std::is_sorted(vec.begin(), vec.end())) {
			sort(vec.begin(), vec.end());
		}
		int l = 0, r = (int)vec.size() - 1;
		while (l <= r) {
			int flag = l + (r - l) / 2;
			if (vec[flag] == aim) {
				return flag;
			}
			else if (vec[flag] > aim) {
				r = flag - 1;
			}
			else {
				l = flag + 1;
			}
		}
		return -1;
	}

	//sumCalc的前置方法.处理初始数据
	void CS_menu() {
		std::vector<timedata> range;//抽象组合得出的数组.
		std::string nam;//集合名字
		std::vector<int> choice;
		int total = (int)CRecord.Saved_daily.size() - 1;
		std::string orginIn = "";
		int input = 0;
		bool onChoose = true;

		while (onChoose) {
			system("cls");
			if (!choice.empty()) {
				if (!std::is_sorted(choice.begin(), choice.end()))
					std::sort(choice.begin(), choice.end());//默认升序排列.
				CRecord.ShowSavedRecord(Pr::DAILY, &choice);
			}
			else {
				CRecord.ShowSavedRecord(Pr::DAILY);
			}
			std::cout << "选择你需要整合的时间数据.格式:以三种前缀来使用不同的功能." << std::endl;
			std::cout << "a,使用该前缀加上一个数来加上该数据." << std::endl;
			std::cout << "d,使用该前缀加上一个数来删除该数据,但是前提是你选择了该元素" << std::endl;
			std::cout << "x,使用该前缀加上任意整数来退出." << std::endl;
			if (!(std::cin >> orginIn)) {
				std::cerr << "读取失败!" << std::endl;
				Sleep(300);
				continue;
			}
			else {
				if (std::regex_match(orginIn, match, pattern_sumCalc)) {
					input = std::stoi(match[2]);
					if (match[1] == 'x') {
						std::cout << "录入结束!当前已录入:" << (int)choice.size() << "个元素!" << std::endl;
						onChoose = false;
					}
					else if (match[1] == 'a') {
						if (input < -1 || input > total) {
							std::cout << "超出数据范围!" << std::endl;
							Sleep(300);
							continue;
						}
						else if ((!choice.empty()) && dichotomyFind(choice, input) != -1) {
							std::cout << "重复录入!已录入过该数值." << std::endl;
							Sleep(300);
							continue;
						}
						else {
							choice.push_back(input);
							std::cout << "录入成功!" << std::endl;
						}
					}
					else {
						int Index = 0;
						if (input < -1 || input > total) {
							std::cout << "超出数据范围!" << std::endl;
							Sleep(300);
							continue;
						}//注意,下面此处的运算优先级不同,!=的优先级先于=,这导致后面的表达式先运算,并变成了true.
						else if ((!choice.empty()) && ((Index = dichotomyFind(choice, input)) != -1)) {
							choice.erase(choice.begin() + Index);
							std::cout << "删除成功!" << std::endl;
							Sleep(300);
						}
						else {
							std::cout << "找不到元素!" << std::endl;
						}
					}
				}
				else {
					std::cout << "格式不正确!" << std::endl;
					continue;
				}
			}
		}
		if (choice.empty()) {
			std::cout << "元素数为空!按任意键退出..." << std::endl;
			(void)_getch();
			return;
		}
		else {
			std::cout << "请为该集合起个名字!" << std::endl;
			std::cin >> nam;
			int len = static_cast<int>(choice.size());
			range.reserve(len);
			for (int i = 0; i < len; ++i) {
				range.emplace_back(CRecord.Saved_daily[choice[i]].dailySave);
			}
			if (sumCalc(range, nam)) {
				std::cout << "计算成功!" << std::endl;
			}
			else {
				std::cout << "计算失败!" << std::endl;
			}
		}
	}

	//注册模式
	void Register() {//注册模式
		bool RegMode = true;
		while (RegMode) {
			system("cls");
			Rst::ShowNowDataBase();
			std::cout << "\n";
			ShowMenu(Cs::REGISTERMENU);
			Re ret01 = static_cast<Re>(_getch() - '0');
			switch (ret01) {
			case(Re::REGIST): {
				toRegist();
				break;
			}
			case(Re::ERASE): {
				Rst::Addxy(Am::SUB);
				std::cout << "\n删除最后一次输入的数据\n";
				std::cout << "找到如下数据";
				std::cout << std::format("#<{}-{}>: {} $:{:.3f}\n", Rst::dapx + 1, Rst::dapy + 1, Rst::datas[Rst::dapx][Rst::dapy].name, Rst::datas[Rst::dapx][Rst::dapy].cost);//用于格式化输出数据文件.
				std::cout << "是否确定要删除该元素?按下Y确认,按其他键跳过.>:" << std::endl;
				char ret02_01 = _getch();
				if (ret02_01 == 'y' || ret02_01 == 'Y') {
					Rst::datas[Rst::dapx][Rst::dapy].occupied = false;
					std::cout << "\n已成功删除该项!";
				}
				else {
					Rst::Addxy();
					std::cout << "\n已撤销删除操作!";
				}
				std::cout << "\n按任意键退出...\n";
				(void)_getch();
				break;
			}
			case(Re::CHANGE): {
				std::cout << "\n使用数字快捷键搜索修改目标,将在0.5秒后开始检索模式";
				Sleep(500);
				coordInt aim = Rst::CDserach();
				if (aim.X != -1 && aim.Y != -1) {
					bool alter = true;
					while (alter) {
						std::cout << "\n以下是目标项当前状态!\n";
						Rst::ShowNowDataBase(aim.X, aim.Y);
						std::cout << "\n-----1.修改名称--2.修改金额--0.EXIT-----\n";
						char ret03_01 = _getch();
						switch (ret03_01) {
						case('0'): {
							alter = false;
							break;
						}
						case('1'): {
							std::cout << "\n请输入名字>:";
							std::cin >> Rst::datas[aim.X][aim.Y].name;
							std::cout << "修改成功!\n";
							Sleep(200);
							break;
						}
						case('2'): {
							std::cout << "\n请输入金额(成本)>:";
							std::cin >> Rst::datas[aim.X][aim.Y].cost;
							std::cout << "修改成功!\n";
							Sleep(200);
							break;
						}
						default:
							break;
						}
					}
				}
				std::cout << "\n按任意键退出...\n";
				(void)_getch();
				break;
			}
			case(Re::SHOWALL): {
				std::cout << "全部数据如下,按任意键退出." << std::endl;
				Rst::ShowNowDataBase();
				(void)_getch();
				break;
			}
			case(Re::EXIT):
				RegMode = false;
				break;

			default:
				std::cout << "请正确输入...";
				Sleep(300);
				break;
			}
		}
		return;
	}

	//计算模式
	void Calcrlate() {
		bool CaMode = true;
		while (CaMode) {
			system("cls");
			Rst::ShowMenu(Cs::CALCRLATEMENU);
			Ca ret02 = static_cast<Ca>(_getch() - '0');
			switch (ret02) {
			case(Ca::CALC_TIME_RANGE): { //ok
				if (!CTR_menu()) {
					break;
				}
				break;
			}
			case(Ca::CALC_SUM): {//整合01
				CS_menu();
				break;
			}
			case(Ca::CALC_HISTORY): {//展示历史记录02???
				//要不换成其他功能吧?
				break;
			}
			case(Ca::SHOW_EVERY_DATA): {//展示当前所有数据
				CRecord.ShowSavedRecord();
				std::cout << "按任意键退出..." << std::endl;
				(void)_getch();
				break;
			}
			case(Ca::COMPARE): {//比较
				std::cout << "进入比较模式!你可以比较任意两个(天数信息)已存储数据." << std::endl;
				int d_len, s_len;
				d_len = (int)CRecord.Saved_daily.size() - 1;
				s_len = (int)CRecord.Saved_timeRange.size() - 1;
				CRecord.ShowSavedRecord();
				std::cout << "输入两个数字下标,将会展示比较结果.注意,正数为天数数据的下标,而负数是集合数据的下标加一再取反.\n建议把新的数据放在右边.>:" << std::endl;
				int left = 0, right = 0;
				while (true) {
					std::cin >> left;
					std::cout << "已选择左项:" << left << "." << std::endl;
					std::cin >> right;
					std::cout << "已选择右项:" << right << "." << std::endl;
					if (left > d_len || left < -s_len - 1) {
						std::cout << "左值输入有误!!重新输入...";
						continue;
					}
					else if (right > d_len || right < -s_len - 1) {
						std::cout << "右值输入有误!!重新输入...";
						continue;
					}
					else {
						break;
					}
				}
				std::cout << "开始比较" << left << "和" << right << std::endl;
				timedata* le = nullptr, * ri = nullptr;
				if (left < 0) {
					left = (-left) - 1;
					le = &CRecord.Saved_timeRange[left].totalData;
				}
				else {
					le = &CRecord.Saved_daily[left].dailySave;
				}
				if (right < 0) {
					right = (-right) - 1;
					ri = &CRecord.Saved_timeRange[right].totalData;
				}
				else {
					ri = &CRecord.Saved_daily[right].dailySave;
				}
				Compare(*le, *ri);//该操作没有发生多余的副本复制,*ptr相当于对数据的引用,不会发生多余的复制,是很好的写法.
				std::cout << "按任意键退出..." << std::endl;
				(void)_getch();
				//输入编号比较,待完成...
				break;
			}
			case(Ca::EXIT): {
				CaMode = false;
				break;
			}
			default:
				break;
			}
		}
		return;
	}

	void debug_ShowMenu() {
		bool re_debug = true;
		while (re_debug) {
			std::cout << "当前测试ShowMenu函数...\n";
			std::cout << "请输入目标类型为calcrlateMode 的数以对应功能,按0退出." << std::endl;
			std::cout << "==1.MAINMENU" << std::endl;
			std::cout << "==2.REGISTERMENU" << std::endl;
			std::cout << "==3.CALCRLATEMENU" << std::endl;
			char de_aim = ' ';
			bool re_in = true;
			while (re_in) {
				de_aim = _getch() - '0';
				if (de_aim >= 1 && de_aim <= 3) {
					re_in = false;
				}
				else if (de_aim == 0) {
					re_in = false, re_debug = false;
				}
			}
			ShowMenu(static_cast<Cs>(de_aim));
		}
		return;
	}

	void debug_autoPrint() {
		bool re_debug = true;
		while (re_debug) {
			std::cout << "当前测试autoPrint函数...\n";
			std::cout << "请输入目标类型为AlignmentMode的数以对应功能,按0退出." << std::endl;
			std::cout << "==1.文本左对齐" << std::endl;
			std::cout << "==2.文本居中" << std::endl;
			std::cout << "==3.文本右对齐" << std::endl;
			int de_aim = 0;
			bool re_in = true;
			while (re_in) {
				de_aim = (int)(_getch() - '0');
				if (de_aim >= 1 && de_aim <= 3) {
					re_in = false;
				}
				else if (de_aim == 0) {
					return;
				}
			}
			int perc = 0;
			std::cout << "输入想要显示的话语" << std::endl;
			std::string input = "";
			std::cin >> input;
			std::cout << "输入空格百分比(整数)";
			std::cin >> perc;
			autoPrint(input, '^', 10, Ed::ALL, 2, false, static_cast<Ag>(de_aim), perc);
		}
	}

	void debug_recordFind() {

		while (true) {
			std::cout << "当前测试recordFind函数...,按1继续,按0退出.此模式下禁止对数据进行删改操作.\n";
			if (_getch() == '0') {
				return;
			}
			std::vector<record> tmp;
			tmp.push_back({ "测试操作,测试23336777]" ,520,999 });
			tmp.push_back({ "萝莉赛高" ,520,999 });
			tmp.push_back({ "哈哈哈哈是你吗!]" ,520,999 });
			tmp.push_back({ "GetThrughTheDarkness]" ,520,999 });
			tmp.push_back({ "你好世界,你好萝莉]" ,520,999 });
			tmp.push_back({ "你好你好你好萝莉]" ,520,999 });
			tmp.push_back({ "萝莉萝莉萝莉你好]" ,520,999 });
			tmp.push_back({ "2424242424242424" ,520,999 });
			recordFind(tmp, nullptr);

		}
	}

	void Debug() {
		while (true) {
			system("cls");
			std::cout << "欢迎来到Debug模式!按0退出该模式" << std::endl;
			std::cout << "1>.测试ShowMenu函数" << std::endl;
			std::cout << "2>.测试autoPrint" << std::endl;
			std::cout << "3>.测试recordFind" << std::endl;
			std::cout << "4>." << std::endl;
			std::cout << "5>." << std::endl;

			Db token = static_cast<Db>(_getch() - '0');
			switch (token) {
			case(Db::EXIT): {
				std::cout << "按任意键安全退出Debug模式!" << std::endl;
				(void)_getch();
				return;
			}
			case(Db::SHOWMENU): {
				debug_ShowMenu();
				break;
			}
			case(Db::AUTOPRINT): {
				debug_autoPrint();
				break;
			}
			case(Db::RECORDFIND): {
				debug_recordFind();
				break;
			}
			}
		}
	}
}