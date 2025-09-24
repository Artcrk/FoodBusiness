#pragma once

/**
 * @file core.hpp
 * @brief 餐饮管理系统核心 - 数据整合
 * @author Artcrk
 */

#include <iostream>
#include <algorithm>
#include <windows.h>//使用win_API
#include <string>
#include <format>//format的使用
#include <vector>//数组
#include <map>//图
#include <queue>//队列
#include <conio.h>//_getch()的使用
 //#include <variant>//用于AnyEnum的枚举类型整合(可弃用)
  //#include <sstream>//isstringstream类型的使用(弃用)
#include <regex>//正则表达式
#include <time.h>//使用时间捕获函数(旧)
#include <chrono>//精确时间管理
#include <utility>//使用std::move(),std::pair();
#include <fstream>//使用文件读写流.



//用于autoPrint函数的默认宽度值
#define WIDTH 100

 //B09&10//用于主界面.
enum class mainMode : int {
	EXIT = -21,//esc的键值加上'0'的值.这是用于esc退出的检测.
	REGISTER = 1,
	CALCULATE,
	DEBUG = '`' - '0',
	MAINMENU = 5//默认从0开始赋值.
};
using Rm = mainMode;
//用于注册模式(R模式)主界面
enum class registerMode : int {
	EXIT = -21,
	REGIST = 1,
	ERASE,
	CHANGE,
	SHOWALL
};
using Re = registerMode;
//用于计算模式(C模式)主界面.
enum class calcrlateMode : int {
	EXIT = -21,
	CALC_TIME_RANGE = 1,
	CALC_SUM,
	CALC_HISTORY,
	SHOW_EVERY_DATA,
	COMPARE
};
using Ca = calcrlateMode;

//界面初始化枚举,用于ShowMenu函数
enum class chooseSystem : int {
	MAINMENU = 1,//注意起点不一样.要根据实际使用来调整数值.
	REGISTERMENU,
	CALCRLATEMENU,
	CTR,//'calc time renge' 函数菜单 
	RECORDMENU,//历史记录操作菜单
	RECORDOPERATION
};
using Cs = chooseSystem;

//调试模式
enum class debugMode : int {
	EXIT = -21,
	SHOWMENU = 1,
	AUTOPRINT,
	RECORDFIND
};
using Db = debugMode;

//用于时间范围计算函数,用于区分对应使用的功能
enum class CTR_Mode : char {
	EXIT = (char)27,
	OLD_IN = ' ',
	NEW_IN = 'z',
	SAVE_IN = 's',
	CHECK = 'c'
};
using CTR = CTR_Mode;

//用于Addxy函数.
enum class AddxyMode : int {
	ADD,
	SUB
};
using Am = AddxyMode;

//用于timedataUpdate函数,用于区分是增加数据还是删除数据
enum class timeDataMode : int {
	ADD = 1,
	ERASE = -1
};
using Tdm = timeDataMode;

//用于autoPrint函数.左右边框的打印情况.
enum class LRedge : int {
	EMPTY,
	LEFT,
	RIGHT,
	ALL
};
using Ed = LRedge;

//用于autoPrint函数,对齐方式.
enum class Alignment :int {
	LEFT = 1,
	MIDDLE,
	RIGHT
};
using Ag = Alignment;

//用于ShowMenu的关于recordFind的重载函数.是否显示全部数据
enum class recordFindToken : int {
	ORGINIAL,//显示全部数据时使用
	SPECIAL,//显示局部数据时使用,并且是直接使用target类型传参时使用.
	FUZZY//用于名称模糊搜索,使用map图传参时使用.
};
using Rft = recordFindToken;

//用于recordFind函数.用于区分检索的方式
enum class recordFindWay : int {
	EXIT = -21,
	SIZE = 1,
	NUMBER,
	NAME
};
using Rfw = recordFindWay;

//用于recordFind,区分关于大小比较的类型.
enum class sizeSymbolBehavior : int {
	LARGER,
	LESS,
	LARGER_EQUAL,
	LESS_EQUAL,
	EQUAL,
	EMPTY
};
using Ssb = sizeSymbolBehavior;

//用于recordFind,区分对历史记录的操作方式.
enum class recordChangeMethod : int {
	EXIT = -21,
	ERASE = 1,
	CHANGE
};
using Rcm = recordChangeMethod;

//颜色调用.
enum colorful : WORD {//避免与int隐式转换
	R = FOREGROUND_RED,
	G = FOREGROUND_GREEN,
	B = FOREGROUND_BLUE,
	I = FOREGROUND_INTENSITY,
	YELLOW = R | G | I,//常用颜色集合.
	PINK = R | B | I,
	CYAN = B | G | I,
	HIGHLIGHT = R | I
};

//数据打印规则,用于calcRecord类的历史数据打印函数.
enum class printRule : int {
	DAILY,
	TIMESUM,
	ALL
};
using Pr = printRule;

namespace Rst {//B07命名空间包装所有成员

	struct business {//餐品数据
		business() : name(""), cost(0.0), occupied(false) {}//B00
		std::string name;
		double cost;
		bool occupied;
	};
	using bis = business;//B01代替老旧typedef

	struct coordInt {//使用int类型的COORD,不需要转换.
		int X;
		int Y;
	};

	struct timedata {//一定时间的数据,同时用于日数据统计和多日期时间统计. -- 最核心的数据
		timedata() : days(""), total_cost(0.0), total_income(0), total_number(0),
			pure_income(0), avg_income(0), avg_pure_income(0), otherdata("") {
		}
		std::string days;//日期,日期格式默认为YYYY-MM-DD
		double total_cost,//总成本,录入并累增
			total_income,//总收入,录入并累增
			pure_income,//净收入,自计数
			avg_income,//单平均收入,自计数
			avg_pure_income,//单平均净收入,自计数
			total_number;//总数,自计数
		std::string otherdata;
	};

	//操作记录的存储结构体
	struct record {
		std::string operateName;
		double income;
		double cost;
	};

	//autoPrint的数据存储结构体
	struct outputString {
		outputString(std::string str, int s, int l) : words(str), start(s), length(l) {}
		outputString() : words(""), start(0), length(0) {}
		std::string words;
		int start;//开始的坐标.
		int length;//目标的长度.
	};

	//用于存储找到的数据,以及其他信息(比如哪个部分需要高光).目前用于recordFind()函数
	struct target {
		//构造函数也有重载,下面的构造函数需要两个参数.可以创建只需要一个参数或着没有参数的构造函数.
		target(int k, std::string oth) : _key(k), otherMessage(oth) {};//初始化函数B31<<<--------
		int _key;
		std::string otherMessage;//用于存储格外信息.
	};

	//组合存储一整天的信息,操作类型为"单日期操作"
	struct daily {
		//该操作会将传入数据掏空.
		daily(timedata& td, const std::string& s) noexcept :
			dailySave(std::move(td)),
			operationDate(s) {
		}
		timedata dailySave;
		std::string operationDate;//YYYY-MM-DD_hh:mm:ss
	};

	//组合多个日期的信息,操作类型为"多日期合并"
	//&&表示要夺走数据,&表示要修改数据,const表示只读.
	struct timeRange {
		//该操作会将传入数据掏空.//const不可移动.
		timeRange(timedata& td, const std::string& s, std::vector<std::string>& Vstr) noexcept //使用noexcept,向编译器保证绝对不会出错,在此代码内部,类似vector等容器就能执行更加高效率的操作--移动语义.
			: totalData(std::move(td)),
			  operationDate(s) {
			//multiSave(std::move(Vtd))vector的移动语义想要最高效率,必须像下面这样写:
			multiSave.reserve(Vstr.size());
			for (auto& i : Vstr) {
				multiSave.push_back(std::move(i));//逐个移动.比逐个拷贝效率高很多
			}
		}
		timedata totalData;//整合后的数据.
		std::string operationDate;
		std::vector<std::string> multiSave;//涉及的初始日期数据和数据范围,可选?
	};

	//用于存储时间信息.
	struct dateContainer {
		//以年月日时分为顺序填入参数.不会记录秒以及更靠后的参数.录入-1表示忽略该时间位置.
		dateContainer(std::initializer_list<int> dateInformation) : Y(-1), M(-1), D(-1), h(-1), m(-1) {
			auto it = dateInformation.begin();
			if (it != dateInformation.end()) Y = *it++;//检索是否为空迭代器,否则录入元素
			if (it != dateInformation.end()) M = *it++;
			if (it != dateInformation.end()) D = *it++;
			if (it != dateInformation.end()) h = *it++;
			if (it != dateInformation.end()) m = *it++;
		}
		int Y;//4,year
		int M;//2,month
		int D;//2,day
		int h;//2,hour
		int m;//2,minute
	};

	class calcRecord {
	public:
		

		std::vector<daily> Saved_daily;
		std::vector<timeRange> Saved_timeRange;
	
		void ShowSavedRecord(Pr mode,std::vector<int>* choice);//功能1:展示存储的历史记录,会展示所有的单日期操作和多日期操作以及所有的数据.可以存储进文档,认定为log

		void UpdateData();//功能2:对全体数据进行 删,查,改 的操作;

		bool SaveRange(timeRange&& data) noexcept;//功能3:存入数据并自动录入系统时间,支持两种数据的存储.
		bool SaveDaily(daily&& data) noexcept;//注意,noexcept是一个异常规范,而函数由: 返回类型 + 函数名 + 参数列表 + 异常规范   .组成.
		
		void datainit();
	};

	extern double Version;//版本
	extern int dapx, dapy;//存储记录指针
	extern bis datas[10][10];//存储餐品数据
	extern HANDLE set_consoleColor;//一个颜色权限句柄.
	extern std::regex pattern_size;//是否符合大小比较语法.
	extern std::regex pattern_date;//是否符合日期
	extern std::regex pattern_Time;//是否符合日期,严格格式,用于匹配chrono返回的时间字符串.
	extern std::smatch match;//匹配
	extern std::string sizeSymbol[];//比较方式
	extern WORD consoleColor;//存储初始状态的控制台颜色
	extern calcRecord CRecord;//全局计算记录存储容器
	extern double Inf;

	bool Addxy(Am x = Am::ADD);//B03在此处使用=0初始化后在定义就不需要再初始化了,这是因为唯一性
	bool ShowNowDataBase();
	bool ShowNowDataBase(int index_x);
	bool ShowNowDataBase(int index_x, int index_y);
	bool timedataUpdate(timedata* t_P, double& income, double& cost, Tdm mode = Tdm::ADD);
	void ShowMenu(Cs mode, timedata* tmp = nullptr);//菜单展示函数
	bool ShowMenu(std::vector<record>& rcd,
		Rft special = Rft::ORGINIAL,
		std::vector<target>* commonTokens = nullptr,
		std::map<int, std::vector<int>>* mapTokens = nullptr);//另一个重载.
	bool sumCalc(std::vector<timedata>& ptr, std::string& names);//数据和计算器
	bool recordFind(std::vector<record>& rcd, timedata* tdPointer);//用于记录内搜索;
	void setcolor(WORD COLOR);//设置控制台字体颜色,直接录入WORD码,你可以使用colorful枚举类型来快速操作.
	WORD getcolor();//获取当前控制台颜色.
	void resetColor(WORD AIMCOLOR);
	outputString autoPrint(const std::string& text, const char& fills = ' ', int width = WIDTH, Ed L_R_edge = Ed::ALL, int repeat = 1, bool Right_API = false, Ag alignment = Ag::MIDDLE, int percent = 0, bool saveOutput = false);
	std::string timeCatch();//时间捕获函数,返回一个格式化的时间信息:YYYY-mm-dd_HH:MM:SS
	//二分查找,并返回找到的值,没找到时返回-1.
	template<typename T>
	int dichotomyFind(std::vector<T>& vec, const T& aim);
	void CS_menu();//处理传递给sumCalc的初始数据.
	bool Compare(timedata& left, timedata& right);//比较函数

	// @todo
	std::string toDate(dateContainer d);//将分散的int时间数据整合为一整条,适应1~5个时间数据的存储.使用正则匹配.
	dateContainer toNum(std::string s);//将一整条时间分散为int数据组,适应1~5个时间数据的存储.

	coordInt toRegist();//注册商品,并返回注册的地址.
	void Register();//注册模式
	void Calcrlate();//注册模式
	void Debug();

	//template<typename T>//B11&B17,模板代码只有实例化时才会生成代码,模板类的语法
	//inline auto toReal = [](AnyEnum mode) -> T {//B12&16,注意inline可以保证该模板函数的定义唯一性,lambda表达式可以更快捷方便的创建一个匿名类,并且快速使用元素进行反馈.使用inline是告诉编译器所有文件共用同一份定义.
	//	return std::visit([](auto m) -> T {//B14&15//内嵌套lambda函数,并不属于C++语法而只是std::visit中的一种用法,visit检索mode的内部类型并将其定义为lambda表达式参数的类型.
	//		return static_cast<T>(m);
	//		}
	//	, mode);
	//	};
	coordInt CDserach();
}