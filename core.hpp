#pragma once

/**
 * @file core.hpp
 * @brief ��������ϵͳ���� - ��������
 * @author Artcrk
 */

#include <iostream>
#include <algorithm>
#include <windows.h>//ʹ��win_API
#include <string>
#include <format>//format��ʹ��
#include <vector>//����
#include <map>//ͼ
#include <queue>//����
#include <conio.h>//_getch()��ʹ��
 //#include <variant>//����AnyEnum��ö����������(������)
  //#include <sstream>//isstringstream���͵�ʹ��(����)
#include <regex>//������ʽ
#include <time.h>//ʹ��ʱ�䲶����(��)
#include <chrono>//��ȷʱ�����
#include <utility>//ʹ��std::move(),std::pair();
#include <fstream>//ʹ���ļ���д��.



//����autoPrint������Ĭ�Ͽ��ֵ
#define WIDTH 100

 //B09&10//����������.
enum class mainMode : int {
	EXIT = -21,//esc�ļ�ֵ����'0'��ֵ.��������esc�˳��ļ��.
	REGISTER = 1,
	CALCULATE,
	DEBUG = '`' - '0',
	MAINMENU = 5//Ĭ�ϴ�0��ʼ��ֵ.
};
using Rm = mainMode;
//����ע��ģʽ(Rģʽ)������
enum class registerMode : int {
	EXIT = -21,
	REGIST = 1,
	ERASE,
	CHANGE,
	SHOWALL
};
using Re = registerMode;
//���ڼ���ģʽ(Cģʽ)������.
enum class calcrlateMode : int {
	EXIT = -21,
	CALC_TIME_RANGE = 1,
	CALC_SUM,
	CALC_HISTORY,
	SHOW_EVERY_DATA,
	COMPARE
};
using Ca = calcrlateMode;

//�����ʼ��ö��,����ShowMenu����
enum class chooseSystem : int {
	MAINMENU = 1,//ע����㲻һ��.Ҫ����ʵ��ʹ����������ֵ.
	REGISTERMENU,
	CALCRLATEMENU,
	CTR,//'calc time renge' �����˵� 
	RECORDMENU,//��ʷ��¼�����˵�
	RECORDOPERATION
};
using Cs = chooseSystem;

//����ģʽ
enum class debugMode : int {
	EXIT = -21,
	SHOWMENU = 1,
	AUTOPRINT,
	RECORDFIND
};
using Db = debugMode;

//����ʱ�䷶Χ���㺯��,�������ֶ�Ӧʹ�õĹ���
enum class CTR_Mode : char {
	EXIT = (char)27,
	OLD_IN = ' ',
	NEW_IN = 'z',
	SAVE_IN = 's',
	CHECK = 'c'
};
using CTR = CTR_Mode;

//����Addxy����.
enum class AddxyMode : int {
	ADD,
	SUB
};
using Am = AddxyMode;

//����timedataUpdate����,�����������������ݻ���ɾ������
enum class timeDataMode : int {
	ADD = 1,
	ERASE = -1
};
using Tdm = timeDataMode;

//����autoPrint����.���ұ߿�Ĵ�ӡ���.
enum class LRedge : int {
	EMPTY,
	LEFT,
	RIGHT,
	ALL
};
using Ed = LRedge;

//����autoPrint����,���뷽ʽ.
enum class Alignment :int {
	LEFT = 1,
	MIDDLE,
	RIGHT
};
using Ag = Alignment;

//����ShowMenu�Ĺ���recordFind�����غ���.�Ƿ���ʾȫ������
enum class recordFindToken : int {
	ORGINIAL,//��ʾȫ������ʱʹ��
	SPECIAL,//��ʾ�ֲ�����ʱʹ��,������ֱ��ʹ��target���ʹ���ʱʹ��.
	FUZZY//��������ģ������,ʹ��mapͼ����ʱʹ��.
};
using Rft = recordFindToken;

//����recordFind����.�������ּ����ķ�ʽ
enum class recordFindWay : int {
	EXIT = -21,
	SIZE = 1,
	NUMBER,
	NAME
};
using Rfw = recordFindWay;

//����recordFind,���ֹ��ڴ�С�Ƚϵ�����.
enum class sizeSymbolBehavior : int {
	LARGER,
	LESS,
	LARGER_EQUAL,
	LESS_EQUAL,
	EQUAL,
	EMPTY
};
using Ssb = sizeSymbolBehavior;

//����recordFind,���ֶ���ʷ��¼�Ĳ�����ʽ.
enum class recordChangeMethod : int {
	EXIT = -21,
	ERASE = 1,
	CHANGE
};
using Rcm = recordChangeMethod;

//��ɫ����.
enum colorful : WORD {//������int��ʽת��
	R = FOREGROUND_RED,
	G = FOREGROUND_GREEN,
	B = FOREGROUND_BLUE,
	I = FOREGROUND_INTENSITY,
	YELLOW = R | G | I,//������ɫ����.
	PINK = R | B | I,
	CYAN = B | G | I,
	HIGHLIGHT = R | I
};

//���ݴ�ӡ����,����calcRecord�����ʷ���ݴ�ӡ����.
enum class printRule : int {
	DAILY,
	TIMESUM,
	ALL
};
using Pr = printRule;

namespace Rst {//B07�����ռ��װ���г�Ա

	struct business {//��Ʒ����
		business() : name(""), cost(0.0), occupied(false) {}//B00
		std::string name;
		double cost;
		bool occupied;
	};
	using bis = business;//B01�����Ͼ�typedef

	struct coordInt {//ʹ��int���͵�COORD,����Ҫת��.
		int X;
		int Y;
	};

	struct timedata {//һ��ʱ�������,ͬʱ����������ͳ�ƺͶ�����ʱ��ͳ��. -- ����ĵ�����
		timedata() : days(""), total_cost(0.0), total_income(0), total_number(0),
			pure_income(0), avg_income(0), avg_pure_income(0), otherdata("") {
		}
		std::string days;//����,���ڸ�ʽĬ��ΪYYYY-MM-DD
		double total_cost,//�ܳɱ�,¼�벢����
			total_income,//������,¼�벢����
			pure_income,//������,�Լ���
			avg_income,//��ƽ������,�Լ���
			avg_pure_income,//��ƽ��������,�Լ���
			total_number;//����,�Լ���
		std::string otherdata;
	};

	//������¼�Ĵ洢�ṹ��
	struct record {
		std::string operateName;
		double income;
		double cost;
	};

	//autoPrint�����ݴ洢�ṹ��
	struct outputString {
		outputString(std::string str, int s, int l) : words(str), start(s), length(l) {}
		outputString() : words(""), start(0), length(0) {}
		std::string words;
		int start;//��ʼ������.
		int length;//Ŀ��ĳ���.
	};

	//���ڴ洢�ҵ�������,�Լ�������Ϣ(�����ĸ�������Ҫ�߹�).Ŀǰ����recordFind()����
	struct target {
		//���캯��Ҳ������,����Ĺ��캯����Ҫ��������.���Դ���ֻ��Ҫһ����������û�в����Ĺ��캯��.
		target(int k, std::string oth) : _key(k), otherMessage(oth) {};//��ʼ������B31<<<--------
		int _key;
		std::string otherMessage;//���ڴ洢������Ϣ.
	};

	//��ϴ洢һ�������Ϣ,��������Ϊ"�����ڲ���"
	struct daily {
		//�ò����Ὣ���������Ϳ�.
		daily(timedata& td, const std::string& s) noexcept :
			dailySave(std::move(td)),
			operationDate(s) {
		}
		timedata dailySave;
		std::string operationDate;//YYYY-MM-DD_hh:mm:ss
	};

	//��϶�����ڵ���Ϣ,��������Ϊ"�����ںϲ�"
	//&&��ʾҪ��������,&��ʾҪ�޸�����,const��ʾֻ��.
	struct timeRange {
		//�ò����Ὣ���������Ϳ�.//const�����ƶ�.
		timeRange(timedata& td, const std::string& s, std::vector<std::string>& Vstr) noexcept //ʹ��noexcept,���������֤���Բ������,�ڴ˴����ڲ�,����vector����������ִ�и��Ӹ�Ч�ʵĲ���--�ƶ�����.
			: totalData(std::move(td)),
			  operationDate(s) {
			//multiSave(std::move(Vtd))vector���ƶ�������Ҫ���Ч��,��������������д:
			multiSave.reserve(Vstr.size());
			for (auto& i : Vstr) {
				multiSave.push_back(std::move(i));//����ƶ�.���������Ч�ʸߺܶ�
			}
		}
		timedata totalData;//���Ϻ������.
		std::string operationDate;
		std::vector<std::string> multiSave;//�漰�ĳ�ʼ�������ݺ����ݷ�Χ,��ѡ?
	};

	//���ڴ洢ʱ����Ϣ.
	struct dateContainer {
		//��������ʱ��Ϊ˳���������.�����¼���Լ�������Ĳ���.¼��-1��ʾ���Ը�ʱ��λ��.
		dateContainer(std::initializer_list<int> dateInformation) : Y(-1), M(-1), D(-1), h(-1), m(-1) {
			auto it = dateInformation.begin();
			if (it != dateInformation.end()) Y = *it++;//�����Ƿ�Ϊ�յ�����,����¼��Ԫ��
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
	
		void ShowSavedRecord(Pr mode,std::vector<int>* choice);//����1:չʾ�洢����ʷ��¼,��չʾ���еĵ����ڲ����Ͷ����ڲ����Լ����е�����.���Դ洢���ĵ�,�϶�Ϊlog

		void UpdateData();//����2:��ȫ�����ݽ��� ɾ,��,�� �Ĳ���;

		bool SaveRange(timeRange&& data) noexcept;//����3:�������ݲ��Զ�¼��ϵͳʱ��,֧���������ݵĴ洢.
		bool SaveDaily(daily&& data) noexcept;//ע��,noexcept��һ���쳣�淶,��������: �������� + ������ + �����б� + �쳣�淶   .���.
		
		void datainit();
	};

	extern double Version;//�汾
	extern int dapx, dapy;//�洢��¼ָ��
	extern bis datas[10][10];//�洢��Ʒ����
	extern HANDLE set_consoleColor;//һ����ɫȨ�޾��.
	extern std::regex pattern_size;//�Ƿ���ϴ�С�Ƚ��﷨.
	extern std::regex pattern_date;//�Ƿ��������
	extern std::regex pattern_Time;//�Ƿ��������,�ϸ��ʽ,����ƥ��chrono���ص�ʱ���ַ���.
	extern std::smatch match;//ƥ��
	extern std::string sizeSymbol[];//�ȽϷ�ʽ
	extern WORD consoleColor;//�洢��ʼ״̬�Ŀ���̨��ɫ
	extern calcRecord CRecord;//ȫ�ּ����¼�洢����
	extern double Inf;

	bool Addxy(Am x = Am::ADD);//B03�ڴ˴�ʹ��=0��ʼ�����ڶ���Ͳ���Ҫ�ٳ�ʼ����,������ΪΨһ��
	bool ShowNowDataBase();
	bool ShowNowDataBase(int index_x);
	bool ShowNowDataBase(int index_x, int index_y);
	bool timedataUpdate(timedata* t_P, double& income, double& cost, Tdm mode = Tdm::ADD);
	void ShowMenu(Cs mode, timedata* tmp = nullptr);//�˵�չʾ����
	bool ShowMenu(std::vector<record>& rcd,
		Rft special = Rft::ORGINIAL,
		std::vector<target>* commonTokens = nullptr,
		std::map<int, std::vector<int>>* mapTokens = nullptr);//��һ������.
	bool sumCalc(std::vector<timedata>& ptr, std::string& names);//���ݺͼ�����
	bool recordFind(std::vector<record>& rcd, timedata* tdPointer);//���ڼ�¼������;
	void setcolor(WORD COLOR);//���ÿ���̨������ɫ,ֱ��¼��WORD��,�����ʹ��colorfulö�����������ٲ���.
	WORD getcolor();//��ȡ��ǰ����̨��ɫ.
	void resetColor(WORD AIMCOLOR);
	outputString autoPrint(const std::string& text, const char& fills = ' ', int width = WIDTH, Ed L_R_edge = Ed::ALL, int repeat = 1, bool Right_API = false, Ag alignment = Ag::MIDDLE, int percent = 0, bool saveOutput = false);
	std::string timeCatch();//ʱ�䲶����,����һ����ʽ����ʱ����Ϣ:YYYY-mm-dd_HH:MM:SS
	//���ֲ���,�������ҵ���ֵ,û�ҵ�ʱ����-1.
	template<typename T>
	int dichotomyFind(std::vector<T>& vec, const T& aim);
	void CS_menu();//�����ݸ�sumCalc�ĳ�ʼ����.
	bool Compare(timedata& left, timedata& right);//�ȽϺ���

	// @todo
	std::string toDate(dateContainer d);//����ɢ��intʱ����������Ϊһ����,��Ӧ1~5��ʱ�����ݵĴ洢.ʹ������ƥ��.
	dateContainer toNum(std::string s);//��һ����ʱ���ɢΪint������,��Ӧ1~5��ʱ�����ݵĴ洢.

	coordInt toRegist();//ע����Ʒ,������ע��ĵ�ַ.
	void Register();//ע��ģʽ
	void Calcrlate();//ע��ģʽ
	void Debug();

	//template<typename T>//B11&B17,ģ�����ֻ��ʵ����ʱ�Ż����ɴ���,ģ������﷨
	//inline auto toReal = [](AnyEnum mode) -> T {//B12&16,ע��inline���Ա�֤��ģ�庯���Ķ���Ψһ��,lambda���ʽ���Ը���ݷ���Ĵ���һ��������,���ҿ���ʹ��Ԫ�ؽ��з���.ʹ��inline�Ǹ��߱����������ļ�����ͬһ�ݶ���.
	//	return std::visit([](auto m) -> T {//B14&15//��Ƕ��lambda����,��������C++�﷨��ֻ��std::visit�е�һ���÷�,visit����mode���ڲ����Ͳ����䶨��Ϊlambda���ʽ����������.
	//		return static_cast<T>(m);
	//		}
	//	, mode);
	//	};
	coordInt CDserach();
}