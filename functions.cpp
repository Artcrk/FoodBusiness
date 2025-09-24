#include "core.hpp"

/**
 * @file functions.cpp
 * @brief ��������ϵͳ������תģ�� - �߼����������
 * @author Artcrk
 *
 */

namespace Rst { // B04
	bis datas[10][10];//��Ʒ���ݴ洢,
	calcRecord CRecord;//ȫ�ּ������ݴ洢����.

	int dapx, dapy;
	double Inf = 1e-4;//�������.

	HANDLE set_consoleColor = GetStdHandle(STD_OUTPUT_HANDLE);//��ɫ����Ȩ��.
	std::regex pattern_size(R"((>|<|>=|<=|==)(-?\d+(\.?\d+)?))");//b26<<-----
	std::regex pattern_date(R"((\d{4})-(\d{1,2})-(\d{1,2}))");//������.
	std::regex pattern_Time(R"((\d{4})-(\d{2})-(\d{2})_(\d{2}):(\d{2}):(\d{2}))");
	std::regex pattern_sumCalc(R"(([axd]{1})(\d+))");//���ڲ���suncalcʹ�õĲ�������.
	std::string sizeSymbol[5] = { ">","<",">=","<=","==" };
	std::smatch match;//B27<<------B36<<-----�ٶ�ʹ�û��Զ����Ŷ!
	WORD consoleColor = R | G | B;
	//*���º�������,��ʹ����һ������,��������enum colorful��Ƶ�������,�Լ���һ�����������Ը���.������������,�һ�ûд�ʼ���
	// 
	//void setcolor(std::initializer_list<colorful> colors) {//ʹ��initializer_list������ɱ䳤�Ȳ����б�,��Ҫע���ɴ˷�ʽ��װ�Ĳ�������ͨ��[]������,���ǿ���ʹ�����Դ��ĵ�����������.B39<<----
	//	WORD ColorMix = 0;//������ɫ�Ķ�����λ�����߼�,��ʼ��Ϊһ��0��û�����;B38<<-----
	//	for (const auto& i : colors) {//�Զ������б�,ԭ�����Ƶ�����,�պÿ�������init_list�����B37<<-----
	//		ColorMix |= i;//�����B36<<-------
	//	}
	//	SetConsoleTextAttribute(set_consoleColor, ColorMix);
	//}

	/* @brief ���ڿ������ÿ���̨��ɫ;
	 * @param colors ��Ҫ�������ɫ(R,G,B,I,YELLOW,PINK,CYAN}Ҳ���Ǻ�����,��ɫ,��ɫ�����ɫ,��������λ����..
	 *
	 * @note ʾ��:
	 *   setcolor(R|G|I);//��ɫ��ɫ.
	 *   setcolor(YELLOW);//��ɫ��ɫ.
	 */
	void setcolor(WORD COLOR) {//��ֵ���ò�����ʽת��B42<<-------------
		SetConsoleTextAttribute(set_consoleColor, COLOR);
	}

	/*
	 * @brief ��ȡ��ǰ��Ļ����ɫ
	 *
	 * @return ��ǰ��Ļ��ɫ�ĵ���λWORDֵ.
	 */
	WORD getcolor() {//B40<<------��ȡ��ǰ��Ļ��ɫ������!
		CONSOLE_SCREEN_BUFFER_INFO csbi;//<<step1
		GetConsoleScreenBufferInfo(set_consoleColor, &csbi);//step2
		return csbi.wAttributes & 0x0F;//step3,���Ⲷ���λ(����ɫ)
	}

	//����ǰ����̨��ɫ����xx,������Ϊxx.
	void resetColor(WORD AIMCOLOR) {
		if (getcolor() != AIMCOLOR) {
			setcolor(AIMCOLOR);//�м���Ԫ������ĺ���λ�������ͱ�����ͬ!!
		}
	}

	/**
	 * @brief ���ڸ�������Ϊtimedata*��Ŀ������
	 *
	 * @param t_p ָ��timedata���͵�ָ��
	 * @param income ����
	 * @param cost �ɱ�
	 * @param mode ����״̬ ADD/ERASE.(default = Tdm::ADD)
	 *
	 * @return ���гɹ�״̬
	 */
	bool timedataUpdate(timedata* t_p, double& income, double& cost, Tdm mode) {
		//�����쳣����
		if (t_p == nullptr || income < 0.0 || cost < 0.0) {
			return false;
		}
		if (mode == Tdm::ERASE && t_p->total_number < 1) {
			return false;
		}
		const int changeWay = static_cast<int>(mode);
		//�����ݵ����ϼ���
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
	 * @brief ��Ʒ��ȱλָ����������
	 *
	 * @param x ���ڵ���Addxy����Ϊ,x��ΪADD,SUB.(default = ADD)
	 * @return ���غ���������״̬
	 *
	 * @throw ����ʽ�쳣,���ڼӼ�����ʧ��ʱ����false
	 *
	 *
	 */
	bool Addxy(Am x) {//ʵ�ֶ����ݿ�ָ��xy�ļӼ�����.//B03
		int sx = dapx, sy = dapy;
		bool st = true;
		int mode = (x == Am::ADD) ? 1 : -1;
		dapx += (dapy + mode) < 0 ? -1 : 0;
		dapx += (dapy + mode) > 9 ? 1 : 0;
		dapy = (dapy + mode + 10) % 10;
		(dapx < 0 || dapx>9) ? dapx = sx, dapy = sy, st = false : 0;//�Ͻ�ͻ����������½�ͻ�����
		return st;
	}

	//���ڱȽ���������timedata����.�����Ҳ�Ϊ���µ�����
	bool Compare(timedata& left, timedata& right) {
		setcolor(YELLOW);
		//��ӡ����Ϊ21:26:26:26:1
		//Ĭ�����һ��'|'Ҫ������ӡ.
		//r:�ұ�~    l:�����Ҫչʾ����ֵ
		auto print = [](double& r, double& l, const std::string& type) {//ע��,�������const�������ó���.constexpr�Ǳ���ʱ����,�����������ڼ��޸�,��const��ͬ.
			double _sub = r - l;
			autoPrint(type, ' ', 21, Ed::LEFT, 1, true, Ag::LEFT, 10);
			autoPrint(std::format("{:.1f}", l), ' ', 26, Ed::LEFT, 1, true);
			autoPrint(std::format("{:.1f}", r), ' ', 26, Ed::ALL, 1, true);
			if (_sub > 0 + Inf) {
				resetColor(G | I);
				autoPrint(std::format("+{:.2f}", _sub), ' ', 26, Ed::EMPTY, 1, true);//����ʱ��ɫ,����ʱ��ɫ.����ʱ��ɫ.
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
		autoPrint("%�ȽϽ��%", '=', WIDTH, Ed::EMPTY);

		autoPrint("", ' ', 21, Ed::LEFT, 1, true, Ag::LEFT, 10);
		autoPrint("��ֵ:" + left.days, ' ', 26, Ed::LEFT, 1, true);
		autoPrint("��ֵ:" + right.days, ' ', 26, Ed::ALL, 1, true);
		autoPrint("�Ƚϲ�ֵ", ' ', 26, Ed::EMPTY, 1, true);//����ʱ��ɫ,����ʱ��ɫ.����ʱ��ɫ.
		printf("|\n");

		print(right.total_number, left.total_number, "�ܵ���");
		print(right.total_income, left.total_income, "������");
		print(right.pure_income, left.pure_income, "�ܾ�����");
		print(right.total_cost, left.total_cost, "�ܳɱ�");
		print(right.avg_income, left.avg_income, "������");
		print(right.avg_pure_income, left.avg_pure_income, "��������");
		autoPrint("", '=', WIDTH, Ed::EMPTY);
		return true;
	}

	//@brief ��ӡ��Ӧ���ݿ�洢��Ԫ��
	bool ShowNowDataBase() {//����չʾ,��������������.

		int i = 0, j = 0;
		if (!datas[i][j].occupied) {

			std::cout << "\n#���ݿ���û�д洢����!" << std::endl;
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
				std::cout << std::format("#<{}-{}>: {} $:{:.3f}\n", i + 1, j + 1, datas[i][j].name, datas[i][j].cost);//���ڸ�ʽ����������ļ�.
			}
			std::cout << "\n";
		}
		return true;
	}

	//չʾ��index_x�е�����
	bool ShowNowDataBase(int index_x) {
		if (!datas[index_x][0].occupied) {
			std::cout << std::format("\n#<{}-*>��û�д洢����!", index_x + 1) << std::endl;
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

	//չʾindex_x,index_y�������.
	bool ShowNowDataBase(int index_x, int index_y) {
		if (!datas[index_x][index_y].occupied) {
			std::cout << std::format("\n#<{}-{}>��û�д洢����!", index_x + 1, index_y + 1) << std::endl;
			return false;
		}
		std::cout << std::format("#<{}-{}>: {} $:{:.3f}\n", index_x + 1, index_y + 1, datas[index_x][index_y].name, datas[index_x][index_y].cost);
		return true;
	}

	/**
	 * @brief ���̿�ݼ�����
	 *
	 * @return ���ҵ����������
	 *
	 * @note Ŀǰֻ������[10][10]�������С
	 *
	 */
	coordInt CDserach() {//����Ѱ�Ҷ�Ӧ����,�ȽϿ���,�൱�ڿ�ݼ�����.
	ser_a1:
		system("cls");
		std::cout << "\nSerachģʽ��, �����ͨ������ESC�������˳���ģʽ.\n";
		bool able01 = ShowNowDataBase();
		coordInt cod = { 0,0 };
		if (able01 == false) {
			std::cout << "\n���ݿ������ݴ洢,��������˳�����ģʽ";
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
		std::cout << "Ѱ�ҵ���������\n";
		bool able02 = ShowNowDataBase((int)(a1 - '0') - 1);
		if (able02 == false) { std::cout << "\n�ô������ݴ洢,�������������һ��"; }
		char a2 = _getch();
		if (a2 == 27) {
			return { -1,-1 };
		}
		if (a2 == (int)' ' || !able02 || ((int)(a2 - '0') - 1 < 0 || (int)(a2 - '0') - 1 > 9)) {
			goto ser_a1;
		}
		system("cls");
		std::cout << "Ѱ�ҵ���\n";
		bool able03 = ShowNowDataBase((int)(a1 - '0') - 1, (int)(a2 - '0') - 1);
		if (able03 == false) { std::cout << "\n�ô������ݴ洢,�������������һ��"; }
		else { std::cout << "ȷ��ѡ�������?"; }
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
	 * @brief �Զ������ӡ����
	 *
	 * @param text ����ӡ�ַ���,֧����Ӣ��
	 * @param fills ��϶������(default = ' ')
	 * @param width ��ӡ���(default = 100)
	 * @param L_R_edge �Ƿ��ӡ���ұ߿�'|'.ʹ��LRedgeö�ٳ���(EMPTY,LEFT,RIGHT,ALL).(default = ALL)
	 * @param repeat �ظ���ӡ����(default = 1);
	 * @param Right_API �Ƿ�������չ(default = false)
	 * @param alignment ���뷽ʽ,ʹ��AlignmentModeö�ٳ���MIDDLE,LEFT,RIGHT.(default = MIDDLE);
	 * @param percent ��������Ŀո�İٷֱ�(default = 0),�ò���������MIDDLEģʽ�±�ʹ��.
	 * @param saveOutput �Ƿ񱣴�����������.(default = false)
	 *
	 * @note �㾭���ܹ����������ü���autoPrint�����,��������������ӡ�������б��.
	 *
	 * @return ���β���������ַ���,�Լ�start��lengthS������.�������쳣ʱ���ؿ��ַ���.
	 *
	 * @throw ����ʽ�쳣,��������Ϊ �ı����ȹ��� /�ظ�����С��0 /���С��0 ������false
	 *
	 */
	outputString autoPrint(const std::string& text, const char& fills, int width, Ed L_R_edge, int repeat, bool Right_API, Ag alignment, int percent, bool saveOutput) {
		std::string totalOutput = "";

		int lengthS = static_cast<int>(text.length());//��ʵ֤��,�����ַ���ʵ�ʿ��ռ�þ���2�ֽ�B23
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
			std::cout << "Error!��ӡ�ַ�����/�ظ������쳣/����쳣." << std::endl;
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
					i < width ? totalOutput += fills : "";//��Ϊi++��Ե��,��֤ÿһ��++���ܹ����һ������,���򽫻���ĩβ��һ���ո�.
				}
				L_R_edge == Ed::ALL || L_R_edge == Ed::RIGHT ? totalOutput += "|" : totalOutput += fills;
				Right_API ? "" : totalOutput += "\n";
			}
		}
		if (saveOutput == false) {
			std::cout << totalOutput;//ʵ�����Ż������������,�����ε��������
		}
		return { totalOutput, start, lengthS };
	}

	/**
	 * @brief ʹ��autoPrint���Ԥ����Ĳ˵������ӡ
	 *
	 * @param mode Cs���͵Ĳ���,ѡ��Ҫ��ӡ�Ĳ˵�����
	 * @param tmp ��CTR(calculate time range)ģʽ��ʹ��,�ǵ�ǰ������.
	 *
	 * @details �ڿ�ͷ���Զ���ӡһ������x
	 *
	 */
	void ShowMenu(Cs mode, timedata* tmp) {

		printf("\n");
		switch (mode) {
		case(Cs::CALCRLATEMENU): {
			resetColor(R | G);
			autoPrint("", '-', WIDTH, Ed::EMPTY);
			autoPrint("{++++>����ģʽ�˵�<++++}");
			autoPrint("", ' ', WIDTH, Ed::ALL, 2);

			autoPrint("< 1 > ����ĳ��ʱ�������", ' ', 50, Ed::LEFT, 1, true, Ag::LEFT, 20);
			autoPrint("< 2 > ���ϼ���TD������", ' ', 50, Ed::RIGHT, 1, false, Ag::LEFT, 10);

			autoPrint("< 3 > ���в�����ʷ��¼(����)", ' ', 50, Ed::LEFT, 1, true, Ag::LEFT, 20);
			autoPrint("< 4 > ����������չʾ", ' ', 50, Ed::RIGHT, 1, false, Ag::LEFT, 10);

			autoPrint("< 5 > �������ڱȽ�", ' ', 50, Ed::LEFT, 1, true, Ag::LEFT, 20);
			autoPrint("< 6 > �������ݱȽ�", ' ', 50, Ed::RIGHT, 1, false, Ag::LEFT, 10);

			autoPrint("< ESC > �˳�");
			autoPrint("", ' ', WIDTH, Ed::ALL, 2);
			autoPrint("", '-', WIDTH, Ed::EMPTY);
			break;
		}
		case(Cs::MAINMENU): {
			resetColor(CYAN);
			autoPrint("", '-', WIDTH, Ed::EMPTY);
			autoPrint("{[  _>���˵�<_  ]}");
			autoPrint("", ' ', WIDTH, Ed::ALL, 2);

			autoPrint("< 1 > �Ǽ�&ע��&�鿴ģʽ", ' ', 50, Ed::LEFT, 1, true, Ag::LEFT, 20);
			autoPrint("< 2 > ����ģʽ", ' ', 50, Ed::RIGHT, 1, false, Ag::LEFT, 10);

			autoPrint("< ESC > �˳�");
			autoPrint("", ' ', WIDTH, Ed::ALL, 2);
			autoPrint("", '-', WIDTH, Ed::EMPTY);
			break;
		}
		case(Cs::REGISTERMENU): {
			resetColor(PINK);
			autoPrint("", '-', WIDTH, Ed::EMPTY);
			autoPrint("{---->ע��ģʽ�˵�<----}");
			autoPrint("", ' ', WIDTH, Ed::ALL, 2);

			autoPrint("< 1 > ��¼��", ' ', 50, Ed::LEFT, 1, true, Ag::LEFT, 20);
			autoPrint("< 2 > ɾ��", ' ', 50, Ed::RIGHT, 1, false, Ag::LEFT, 10);

			autoPrint("< 3 > �޸�", ' ', 50, Ed::LEFT, 1, true, Ag::LEFT, 20);
			autoPrint("< 4 > �鿴ȫ��", ' ', 50, Ed::RIGHT, 1, false, Ag::LEFT, 10);

			autoPrint("< ESC > �˳�");
			autoPrint("", ' ', WIDTH, Ed::ALL, 2);
			autoPrint("", '-', WIDTH, Ed::EMPTY);
			break;
		}
		case(Cs::CTR): {
			if (tmp == nullptr) {
				std::cout << "����CTR���ʱ����ָ��Ϊ��!";
				break;
			}
			resetColor(B | I);
			autoPrint("����|< " + tmp->days + " >|����", '-', WIDTH, Ed::EMPTY, 1);
			autoPrint("", ' ', WIDTH, Ed::ALL, 2);
			autoPrint("<' '>ʹ����¼�����ݽ�������,�ò�����ֱ��������������.", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 20);
			autoPrint("<'z'>���¼��һ����ʱ����,����¼������.", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 20);
			autoPrint("<'s'>��ݴ浵���������,��Ҫ��������,���ݻ��Զ��洢.", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 20);
			autoPrint("<'c'>�鿴��ʷ¼���¼,��������ɾ�����޸Ĳ���.", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 20);
			autoPrint("< ESC >���沢�˳�(������ͳ������,�����������ʷ).", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 20);
			autoPrint("", ' ', WIDTH, Ed::ALL, 2);
			autoPrint("", '-', WIDTH, Ed::EMPTY);
			std::cout << "��ǰ�����������:" << std::endl;
			setcolor(YELLOW);
			//��һ��
			autoPrint(std::format("[������>: {:.2f}]", tmp->total_income), '=', 40, Ed::ALL, 1, true, Ag::LEFT, 5);
			autoPrint(std::format("[�ܳɱ�>: {:.2f}]", tmp->total_cost), '=', 30, Ed::RIGHT, 1, true, Ag::LEFT, 10);
			autoPrint(std::format("[�ܵ���>: {:.0f}]", tmp->total_number), '=', 30, Ed::RIGHT, 1, false, Ag::LEFT, 10);
			//�ڶ���
			autoPrint(std::format("[�ܾ�����>: {:.1f}]", tmp->pure_income), '=', 40, Ed::ALL, 1, true, Ag::LEFT, 5);
			autoPrint(std::format("[��ƽ������>: {:.2f}]", tmp->avg_income), '=', 30, Ed::RIGHT, 1, true, Ag::LEFT, 10);
			autoPrint(std::format("[��ƽ��������>: {:.2f}]", tmp->avg_pure_income), '=', 30, Ed::RIGHT, 1, false, Ag::LEFT, 10);
			break;
		}
		case(Cs::RECORDMENU): {
			resetColor(B | I);
			autoPrint("#>>>��ǰ���ڼ���ģʽ!<<<#", '-', WIDTH, Ed::EMPTY);
			autoPrint("", ' ', WIDTH, Ed::ALL, 2);
			autoPrint("_�ڼ���ģʽ��,����������²���:", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 5);
			autoPrint("< 1 > ʹ��'>' '<' '<=' '>=' '=='��Ѱ�Ҷ�Ӧ*���*������.(����ͳɱ��������)", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 5);
			autoPrint("ʾ��:\">=200.0\",�������õ�����income/cost����200.0����200.0���С��1e-5������.", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 5);
			autoPrint("< 2 > ֱ��ʹ����������������Ӧ��ŵ�����.", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 5);
			autoPrint("ʾ��:\"12\",����󽫻�õ����Ϊ12������.", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 5);
			autoPrint("< 3 > ʹ������ģ������", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 5);
			autoPrint("ʾ��:\"233����Ʒ01\",����󽫻�Ը�key���м���,�����г����з��ϵ�key,�����key������ɫǿ��.", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 5);
			autoPrint("< ESC > �˳���ģʽ.", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 5);
			autoPrint("", ' ', WIDTH, Ed::ALL, 2);
			autoPrint("", '-', WIDTH, Ed::EMPTY);
			break;
		}
		case(Cs::RECORDOPERATION): {
			resetColor(B | I);
			autoPrint(">]  !  [<", '=', WIDTH, Ed::EMPTY);
			autoPrint("", ' ', WIDTH, Ed::ALL, 2);
			autoPrint("�Ƿ�Լ�������Ԫ�ؽ��в���?(PS:�����������Ԫ��,����һ�����.)");
			autoPrint("< 1 > ɾ������������ʷ��¼,��ͬʱ���˲���.", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 20);
			autoPrint("< 2 > �޸�����������ʷ��¼(��Ŀ���������1ʱ,�������ʼ�޸Ĳ���.)", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 20);
			autoPrint("< ESC > �˳��޸Ĳ�������������.", ' ', WIDTH, Ed::ALL, 1, false, Ag::LEFT, 20);
			autoPrint("", ' ', WIDTH, Ed::ALL, 2);
			autoPrint(">]  ?  [<", '=', WIDTH, Ed::EMPTY);
			break;
		}
		default: {
			std::cout << "����ʧ��!" << std::endl;
			return;
		}
		}
		setcolor(consoleColor);
		std::cout << "�����ӦKEY���빦��>:";
		return;
	}

	/**
	 * @brief ����չʾ����������.
	 *
	 * @param mode ʵ���Ϻ�����Ҫ?ֻ��һ������.
	 * @param rod ��ʷ��¼��һ��std::vector.
	 * @param special �Ƿ�ѡ���������--Ҳ������ֱ��ȫ���ӡ���Ǽ������ӡ
	 * @param commonTokens target����,���ڴ����ҵ���tokens,���ڽ������������ʹ��.
	 * @param mapTokens map��ͼ����,�洢�˶�Ӧ�ĵ�λ��Ϣ��Ⱦɫ��Ϣ.�˱���������ǰһ����!
	 *
	 * @return ����������״̬,�Ƿ������/�ҵ���Ԫ��.
	 *
	 */
	bool ShowMenu(std::vector<record>& rcd, Rft special, std::vector<target>* commonTokens,
		std::map<int, std::vector<int>>* mapTokens) {//B24
		resetColor(YELLOW);
		if (special == Rft::ORGINIAL) {
			std::cout << "��ʼ�������в�������ʷ��¼,�������Ϊ����." << std::endl;
		}
		else {
			std::cout << "����Ϊ�����ҵ��Ķ�Ӧ��¼.������Ӧ�����ѱ�Ⱦɫ." << std::endl;
		}
		//UI��ӡ
		autoPrint("", '-', WIDTH, Ed::EMPTY);
		autoPrint("NUM", ' ', 6, Ed::ALL, 1, true);
		autoPrint("��������", ' ', 54, Ed::ALL, 1, true);
		autoPrint("����", ' ', 20, Ed::ALL, 1, true);
		autoPrint("�ɱ�", ' ', 20, Ed::ALL, 1, false);
		//��¼�����ӡ.
		/*
		 * @brief ����չʾ�ض��˵�����������Ⱦɫ.
		 * @param index ����������±�.��Ҫֱ��ָ��record.
		 * @param orginialIndex �����ԭʼ����(commonTokens�ڲ�����).�����Ϊ���ڵ���0ʱ����ʹ�ö�Ӧ�±���commonTokens�ڴ洢�Ķ������ݽ���Ⱦɫ.
		 * @param mapDye ����map�ļ�ֵ����index,���贫�������������޶�.����Ϊ���ʱ��ʹ��ͼ������
		 *
		 *
		 *
		 * @warning { ��ز�Ҫ���������.
		 *  ��ز�Ҫ��û�д���ͼ��vector��ʱ����з�Rft::ORGINIAL����.
		 *  ��ز�Ҫ����ͬʱʹ��commonDye��mapDye.
		 * }
		 *
		 */
		auto dataView = [&](int index, int orginialIndex = -1, bool mapDyeMode = false) {
			//B25ʹ��lambda���ʽ���⺯����Ƭ���ֲ�,���ұ�����DRY����.ʹ��&���Զ�����ֲ�����
			bool commonDye = (orginialIndex >= 0 && mapDyeMode == false);
			bool mapDye = (orginialIndex == -1 && mapDyeMode == true);
			if (commonDye && (*commonTokens)[orginialIndex].otherMessage.find("_index_") != std::string::npos) {
				setcolor(HIGHLIGHT);//����Ϊ��ɫ����
			}
			autoPrint(std::format("{:03d}", index), ' ', 6, Ed::ALL, 1, true);//---B22
			if (commonDye)resetColor(YELLOW);
			if (mapDye) {//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<��������!!!
				outputString tempSave = autoPrint(rcd[index].operateName, ' ', 54, Ed::ALL, 1, true, Ag::MIDDLE, 0, true);
				std::string stringPiece = "";
				int pieceLength = (int)(*commonTokens)[0].otherMessage.length();//����Ŀ�곤��.
				int cutIndex = tempSave.start - 1;//һ��ע��,autoPrint��start��ָ**Ԫ��λ��**�����±�!.
				int dynamicStartIndex = cutIndex;
				int totalSize = 54;/*���54��autoPrint�Ŀ��,���������Ǹ�����tempSave��autoPrint��*/
				int aimStartIndex = 0;
				std::queue<outputString> outputStringPiece;//�˴�ֻʶ��startԪ��.��startԪ��>0ʱȾɫ,��startԪ��==0ʱ��Ⱦɫ.ֻ��Ϊ�˸������еĹ������ƵĽṹ��.
				//�����Ȱ�mapָ�������Ϊmap����,����ټ���indexֵ������Ԫ�ص��ض�vector,�ٶԴ�vector����size����.��֤����ÿһ��vector���ض���Ϊ��(��recordFind()��mapContainer�Ĵ洢�߼�.)

				outputStringPiece.push({ tempSave.words.substr(0,dynamicStartIndex),0,0 });//�ȵ�Ƭ��ѹ��
				for (int i = 0; i < (int)(*mapTokens)[index].size() && dynamicStartIndex < cutIndex + tempSave.length; i++) {
					aimStartIndex = cutIndex + (*mapTokens)[index][i];
					outputStringPiece.push({ tempSave.words.substr(dynamicStartIndex, aimStartIndex - dynamicStartIndex),0,0 });//ǰ��Ƭ��ѹ��.
					outputStringPiece.push({ tempSave.words.substr(aimStartIndex, pieceLength),1,0 });//Ŀ��Ƭ��ѹ��
					dynamicStartIndex = aimStartIndex + pieceLength;
				}
				if (dynamicStartIndex < totalSize) {
					outputStringPiece.push({ tempSave.words.substr(dynamicStartIndex),0,0 });//β��Ƭ��ѹ��
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

		if (special == Rft::ORGINIAL) {//����ķ��෽ʽ�ǳ����ɾ�,�Ժ�����ֻ��Ҫ����һ��ֵ��������Կ���lambda���ʽ.
			for (int i = static_cast<int>(rcd.size()) - 1; i >= 0; i--) {
				dataView(i);
			}
		}
		else if (special == Rft::SPECIAL) {
			for (int i = (int)commonTokens->size() - 1; i >= 0; i--) {
				dataView((*commonTokens)[i]._key, i);//����˶Խ������͵�����
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
	 * @brief ���ڼ�����Ӧ����ʷ��¼
	 * @param rcd Ŀ����ʷ��¼
	 * @param tdPointer ��ǰ��timedata����.��סtimedata���Ա���һ�������Ϣ,��һ������,���Բ���Ҫ����֮��.
	 *
	 * @return ���гɹ�״̬.
	 */
	bool recordFind(std::vector<record>& rcd, timedata* tdPointer) {
		bool onRecordFind = true;
		std::vector<target> tokenContainer;
		int length_rcd = (int)rcd.size();
		std::map<int, std::vector<int>> mapContainer;//ͼtoken.���ȼ�����tokenContainer,�����������Ԫ��,��tokenContainer����Ԫ��.
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
						if (rcd.empty() || tokenContainer.empty()) {//������vectorΪ�յ������ʹ��erase()!!
							std::cout << "�����б�Ϊ��!��������˳�..." << std::endl;
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
								std::cout << "\n��ǰ�����޸ĵ�_" << i + 1 << "_��.����:: [" << i + 1 << " / " << lengthC << "]." << std::endl;
								std::cout << "���Ϊ" << std::format("< {:03d} >", tokenContainer[i]._key) << ".Ԥ������." << std::endl;
								tmpContainer.clear();
								tmpContainer.push_back({ tokenContainer[i]._key,"" });
								changeData = 0;
								ShowMenu(rcd, Rft::SPECIAL, &tmpContainer);
								timedataUpdate(tdPointer, rcd[tokenContainer[i]._key].income, rcd[tokenContainer[i]._key].cost, Tdm::ERASE);
								std::cout << "�����޸�income! ����-1��������ԭֵ." << std::endl;
								std::cin >> changeData;
								if (changeData != -1) {
									rcd[tokenContainer[i]._key].income = changeData;
								}
								std::cout << "�����޸�cost! ����-1��������ԭֵ." << std::endl;
								std::cin >> changeData;
								if (changeData != -1) {
									rcd[tokenContainer[i]._key].cost = changeData;
								}
								timedataUpdate(tdPointer, rcd[tokenContainer[i]._key].income, rcd[tokenContainer[i]._key].cost, Tdm::ADD);
								std::cout << "�����ɹ�!" << std::endl;
								tmpContainer[0].otherMessage = "_income__cost_";//�߹�
								std::cout << "Ԥ������,�����������" << std::endl;
								ShowMenu(rcd, Rft::SPECIAL, &tmpContainer);
								(void)_getch();
							}
							std::cout << "�޸����!" << std::endl;
							operating = false;
							break;
						}
						case(Rcm::ERASE): {
							//�������ɾ������,��û�г��ؼӳ�!�����!
							//�Ӻ�����ǰɾ��,���ᵼ�������޸�!!
							int num = (int)tokenContainer.size();
							for (int i = (int)tokenContainer.size() - 1; i >= 0; i--) {
								timedataUpdate(tdPointer, rcd[tokenContainer[i]._key].income, rcd[tokenContainer[i]._key].cost, Tdm::ERASE);//̫������!����֮ǰ��timdataUpdateд��ɾ����ʵ��.
								rcd.erase(rcd.begin() + tokenContainer[i]._key);
							}
							tokenContainer.clear();
							std::cout << "ɾ���ɹ�!�ѳɹ�ɾ��" << num << "��Ԫ��.��������˳�...";
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
				std::cout << "��ʷ��¼Ϊ��!��������˳�." << std::endl;
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
				std::cout << "\n��ǰ���ڽ��д�С�Ƚ�����!>:";
				std::string aimToken = "";
				mapContainer.clear();
				tokenContainer.clear();
				double aimSize = 0.0;//����ıȽϴ�С.

				Ssb compareMethod = Ssb::EMPTY;
				std::string coloringTarget = "";
				int progressBar = 0;//���50;

				while (TRUE) {
					std::cin >> aimToken;
					if (std::regex_match(aimToken, match, pattern_size) && !aimToken.empty()) {
						break;
					}
					else {
						std::cout << "�����ʽ����!��ȷ��Ϊ�ȽϷ���'><='��ɵ�ǰ׺���ϴ����ֻ�С��>:" << std::endl;
					}
				}
				//���ݲ�����쳣����,��Ҫע��,���������쳣,���޷��ص���������׶�,ֻ�ܻص�recordFind�˵�.
				try {//B29<<-----
					aimSize = std::stod(match[2].str());//B28<<------
				}
				catch (const std::invalid_argument&) {//B30<<-----
					std::cerr << "��Ч���ַ���ʽ:" << match.str() << std::endl;
					break;
				}
				for (int i = 0; i < 5; i++) {
					if (match[1].str() == sizeSymbol[i]) {
						compareMethod = static_cast<Ssb>(i);
					}
				}
				if (compareMethod == Ssb::EMPTY) {
					std::cout << "ERROR,�ȽϷ�ʽΪ��." << std::endl;
					break;
				}
				//�������洢�Ľ׶�.��Ҫע��,�������Inf�ڵ�ʱ������ж�Ϊ���,��ʹ��΢С�Ĵ��ڻ�С��Ҳ�����.
				//ʾ��,����'>10.0',������10.000001,����Ѹ�Ԫ������������ķ�Χ,��Ϊ��ʱ��Ϊ��Ԫ�����������,��'>'ǡ���ų�����ȵ����,ע��.

				printf("|");
				for (int i = 0; i < length_rcd; i++) {//��Ҫ�洢��ӦԪ�����ڵ�λ�ò�Ⱦɫ....--���!
					//������...--���!!
					coloringTarget.clear();
					for (; progressBar < ceil((i + 1) * 50.0 / length_rcd); progressBar++) {
						printf("#");
					}
					auto check = [&](double value, std::string aimName) -> bool {//B35<<--
						bool equal = (value > aimSize - Inf) && (value < aimSize + Inf);
						bool larger = value > aimSize + Inf;
						bool less = value < aimSize - Inf;//����ģ������/С��.

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
					if (_co_find || _inc_find) {//����ʹ�� || !!��Ϊ�ڼ�⵽ǰ���ֵ��ȷ��ڶ����ж���������ִ��!
						tokenContainer.push_back({ i,coloringTarget });
					}
				}
				for (; progressBar < 50; progressBar++) {
					printf("#");
				}
				printf("|<<�����!!\n");//���ǽ�������һ����...
				//Ŀǰ���õ��˶�Ӧtoken��,��ʱû��չʾ. --�����!
				ShowMenu(rcd, Rft::SPECIAL, &tokenContainer);
				std::cout << "��������˳�...";//������Ҫ��취��һ������������ܹ��������߼�.
				toOperating = true;
				(void)_getch();
				break;
			}
			case(Rfw::NUMBER): {
				std::cout << "\n��ǰ���ڽ��������±�����!>:";
				int numberAimToken = -1;
				tokenContainer.clear();
				mapContainer.clear();
				while (true) {
					std::cout << "����һ���Ǹ�����(�±�)" << std::endl;
					std::cin >> numberAimToken;
					if (numberAimToken < (int)rcd.size() && numberAimToken >= 0) {
						tokenContainer.push_back({ numberAimToken, "_index_" });//�����token.
						break;
					}
					else if (numberAimToken < (int)rcd.size()) {
						std::cout << "���ݲ�����!" << std::endl;
						break;
					}
					else {

						std::cout << "��Ч����." << std::endl;
					}
				}
				toOperating = true;
				break;
			}
			case(Rfw::NAME): {
				std::cout << "\n��ǰ����ģ������!�������������������ӦĿ��.>:";
				mapContainer.clear();
				tokenContainer.clear();
				std::string aimToken;
				std::cin >> aimToken;
				int length_token = (int)aimToken.length();
				int tokenIndex;
				for (int i = 0; i < length_rcd; i++) {
					//ע��,C++�︳ֵ��丳ֵ��,�ڲ������ʽ�ڲ��Ѿ��൱�ڸñ�����B37<<------
					tokenIndex = 0;//һ��Ҫ�ǵ�״̬��ԭ
					while (rcd[i].operateName.find(aimToken, tokenIndex) != std::string::npos) { //ע���������ǲ�Ϊnpos!����������᷵��false֮��Ķ���.
						//��find�е�tokenIndex���ڳ���ʱ,�᷵��npos!
						tokenIndex = (int)rcd[i].operateName.find(aimToken, tokenIndex);
						std::cout << tokenIndex << "},[" << aimToken << "],<" << rcd[i].operateName << std::endl;
						mapContainer[i].push_back(tokenIndex);//����ͼ��.ע������������Ƕ��ڵĴ洢�ռ�!B38<<-----
						tokenIndex += length_token;//ֱ�Ӽ�,��������ն�ϵ�����.B39<<------
					}
					if (!mapContainer[i].empty()) {
						tokenContainer.push_back({ i,aimToken });//ͳһ��tokenContainer�ڲ�,�򻯴���.
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
		std::cout << "��������˳�...";
		(void)_getch();
		return true;
	}

	/**
	 * @brief ���ز�����¼�Ŀ��ٺ���
	 *
	 * @param std::string oper ��������
	 * @param double& income ����(��������)
	 * @param double& cost �ɱ�(��������)
	 *
	 * @return �������Ϻõļ�¼����record
	 *
	 */
	struct record autoRecord(std::string oper, double& income, double& cost) {
		record temp = { oper,income,cost };
		return temp;
	}

	//CALC_TIME_RANGE�������,����ͳ��һ�������.  ��9-18��,��ʮ�ֽ���������:---CTR��v1.0�����й��������!
	bool CTR_menu() {
		system("cls");
		std::cout << "��ǰ�汾" << Rst::Version << "��ʱ��֧�ֶ�����ͬʱͳ����ȴ�����." << std::endl;
		bool onRunningName = true, onRunningMain = true;
		timedata* tmp = new timedata;
		std::vector<struct record> history_CTR;

		while (onRunningName) {
			int m[13] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 };
			std::cout << "--->ͳ��һ��ʱ�������!������ʱ�䷶Χ.\nYYYY-MM-DD\n" << std::endl;
			tmp->days = "NULL";
			std::string input = "";
			std::cin >> input;
			if (std::regex_match(input, match, pattern_date)) {
				int Y = std::stoi(match[1]);
				int M = std::stoi(match[2]);
				int D = std::stoi(match[3]);
				bool leapYear = (Y % 100 != 0 && Y % 4 == 0) || Y % 400 == 0;
				if (Y < 2000 || Y > 2100 || M < 1 || M > 12 || D < 1 || D > 31) {
					std::cout << "����α��!\n";
					continue;
				}
				if (leapYear && M == 2) m[2]++;//�ֲ�����ÿ�α�����ʼ��.
				if (D > m[M]) {
					std::cout << "����α��!\n";
					continue;
				}
				tmp->days = std::format("{:04d}-{:02d}-{:02d}", Y, M, D);
			}
			else {
				std::cout << "��ʽ����ȷ!,ע���ʽΪYYYY-MM-DD.\n";
				continue;
			}
			std::cout << "ȷ��Ϊ" << tmp->days << "ͳ������?��' '(�ո�)����,����Esc�˳�";
			char ret_02n = _getch();
			if (ret_02n == (char)27) {//������esc���˳�.
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
					std::cout << "����û��¼���κ�����!����һ���ֱ���˳�." << std::endl;
					Sleep(1000);
					onRunningMain = false;
					break;
				}
				std::cout << "ȷ���˳�?�ٴε��ESC���ɱ��沢�˳�\n��������������˳�." << std::endl;
				char yesORno = _getch();
				if (yesORno != (char)27) {
					break;
				}
				onRunningMain = false;
				timedata saveData = *tmp;
				//���б������������.
				CRecord.SaveDaily(daily(*tmp, timeCatch()));
				break;
			}
			case(CTR::CHECK): {
				bool onCheck = true;

				while (onCheck) {
					system("cls");
					ShowMenu(history_CTR);
					if (history_CTR.empty()) {
						std::cout << "��ʷ��¼Ϊ��!" << std::endl;
						onCheck = false;
						break;
					}
					std::cout << "��Ҫ����ʷ��¼���в�����?���ո�ʼ����,��Esc�˳�." << std::endl;
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
								std::cout << "��ʷ��¼Ϊ��!" << std::endl;
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
				std::cout << "������³ɱ�(��������ȡ��)" << std::endl;
				double _cost = 0.0;
				std::cin >> _cost;
				std::cout << "��������>:";
				double _income = 0.0;
				std::cin >> _income;

				timedataUpdate(tmp, _income, _cost);
				history_CTR.push_back(autoRecord("���¼��:[-]", _income, _cost));
				std::cout << "<<<¼��ɹ�!";
				Sleep(120);
				break;
			}
			case(CTR::OLD_IN): {
				coordInt aim = CDserach();
				if (aim.X == -1) {
					break;
				}
				std::cout << "���Զ�¼�� " << datas[aim.X][aim.Y].name << " �ĳɱ�:" << datas[aim.X][aim.Y].cost << std::endl;
				std::cout << "��������>:";
				double _income = 0.0;
				std::cin >> _income;

				timedataUpdate(tmp, _income, datas[aim.X][aim.Y].cost);
				history_CTR.push_back(autoRecord("��¼��:[" + datas[aim.X][aim.Y].name + "]", _income, datas[aim.X][aim.Y].cost));
				std::cout << "<<<¼��ɹ�!";
				Sleep(120);
				break;
			}
			case(CTR::SAVE_IN): {
				coordInt aim = toRegist();
				std::cout << "\n���Զ�¼�벢���� " << datas[aim.X][aim.Y].name << " �ĳɱ�:" << datas[aim.X][aim.Y].cost << std::endl;
				std::cout << "��������>:";
				double _income = 0.0;
				std::cin >> _income;

				timedataUpdate(tmp, _income, datas[aim.X][aim.Y].cost);
				//�ı��������޷���ӡ,���޸�...
				history_CTR.push_back(autoRecord(std::format("¼��&����[{}]", datas[aim.X][aim.Y].name, aim.X, aim.Y), _income, datas[aim.X][aim.Y].cost));
				std::cout << "<<<¼��ɹ�!";
				Sleep(120);
				break;
			}
			}
		}
		return true;
	}

	////������øú���ʱ��ʱ��,�����������ʱ����ɵĹ̶���ʽ�ַ���,���⴫�ؽṹ��,��ʹ�õ��Ǵ�ͳ��time.h,������
	//std::string timeCatch() {
	//	time_t timep;
	//	tm* t;
	//	time(&timep);//���ǲ���ʱ�������,����time����ʱ��,����localtimeת��Ϊ����ʱ��,�����Խṹ����ʽ���.
	//	t = localtime(&timep);
	//	return std::format("{:04d}-{:02d}-{:02d}_{:02d}:{:02d}:{:02d}", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);//�˴�ע��.
	//}

	//������øú���ʱ��ʱ��,�����������ʱ����ɵĹ̶���ʽ�ַ���
	std::string timeCatch() {
		auto Nowtime = std::chrono::system_clock::now();
		time_t now_c = std::chrono::system_clock::to_time_t(Nowtime);
		tm* nc = localtime(&now_c);
		return std::format("{:%Y-%m-%d_%H:%M}:{:02d}", Nowtime, nc->tm_sec);//����!!format��chrono����,ʹ���ض��ĸ�ʽ˵����������ʱ����Ϣ.����Ҫ��now����B44<<---
		//����û���ҵ��õķ���������chrono������С���㾫ȷ,ֻ����ʱ��chronoת��Ϊtime_t��tm���־������������,����Ҳ�����ʽ�����.
	}

	/**
	 * @brief �ǼǺ���,ֱ��ִ��ע������
	 *
	 * @return ���ظո�¼���������ڵ�����,�����ݿ�����ʱ������{-233,-233};
	 */
	coordInt toRegist() {
		coordInt position = { dapx,dapy };
		if (!Rst::Addxy(Am::ADD)) {
			std::cout << ">>>���ݿ�����!!" << std::endl;
			return { -233,-233 };
		}
		Addxy(Am::SUB);
		Rst::bis tmp;
		std::cout << "������Ʒ��>:";
		std::cin >> tmp.name;
		std::cout << "������Ʒ�ɱ�>:";
		std::cin >> tmp.cost;
		tmp.occupied = true;
		//Rst::datas[Rst::dapx][Rst::dapy] = tmp;//���ܳ���,����ֱ�ӿ���.
		datas[dapx][dapy] = std::move(tmp);//ֱ���ƶ�,���⿽����Ϊ�쳣.
		std::cout << "¼��ɹ�!�����������.";
		Addxy(Am::ADD);
		(void)_getch();
		return position;
	}

	/**
	 * @brief ���ϼ�������õ�ʱ������, ����Ҫ�������ϵ�����, �Զ��������Ϻõ�����
	 *
	 * @param ptr ��Ҫͳ�Ƶ�ʱ�����ݵ�����.
	 * @param names �ڴ���ȡ������.
	 *
	 * @return �Ƿ�ɹ�����.
	 *
	 * @warning ��Ҫע��,����Ĺ���Ϊת������,�����ԭ������,��������ʱ�߼�Ҳ������Ҫ��daily�ڲ����ݰγ������һ���µ�vector,Ӧ���ⷴ������õķ�ʽ.
	 */
	bool sumCalc(std::vector<timedata>& ptr, std::string& names) {
		if (ptr.empty() || names.empty()) {
			std::cout << "�շ���!";
			return false;
		}
		timedata temp = {};
		std::vector<std::string> moveSave = {};
		int lengthV = (int)ptr.size();
		temp.days = names;
		temp.otherdata = std::format("����{}�����ݵĻ���", lengthV);//���ִ��ĸ�ʽ������,����{}Ϊռλ��(�൱��%s%d֮��),{:.2f}�е�:�൱��ת�����,����.2f�Ǹ�ʽ���������.
		for (int i = 0; i < lengthV; i++) {
			temp.total_cost += ptr[i].total_cost;
			temp.total_income += ptr[i].total_income;
			temp.total_number += ptr[i].total_number;
			temp.pure_income += ptr[i].pure_income;
			moveSave.emplace_back(ptr[i].days);//push + move=�ƶ�.emplace = ֱ�Ӵ����±���.
		}
		temp.avg_income = temp.total_cost / temp.total_number;
		temp.avg_pure_income = temp.pure_income / temp.total_number;
		if (CRecord.SaveRange(timeRange(temp, timeCatch(), moveSave)))//ע����Ҫ����ΪtimeRange����.
			return true;
		else
			return false;
	};

	//��������չʾ,����Ϊ��ӡ����������,Ĭ��ALL
	void calcRecord::ShowSavedRecord(Pr mode = Pr::ALL, std::vector<int>* choice = nullptr) {

		const bool _tr = CRecord.Saved_timeRange.empty();
		const bool _d = CRecord.Saved_daily.empty();
		if (choice != nullptr) {
			if (choice->empty()) {
				choice = nullptr;
			}
			else if ((*choice)[0] < 0) {
				std::cerr << "�����쳣�±��choice����." << std::endl;
				choice = nullptr;
			}
		}
		//�������,10::30:30:30,������ͼ����
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
			date = "��������:" + date + ">";
			std::cout << date << ">_" << std::format("{:03d}_/", i);
			std::cout << "\n+------->> ";
			autoPrint(">" + aim.days, '=', 89, Ed::ALL, 1, false, Ag::LEFT);

			printf("          ");
			autoPrint(std::format("�ܵ���:#{:.0f}", aim.total_number), ' ', 30, Ed::LEFT, 1, true, Ag::LEFT, 10);
			autoPrint(std::format("������:${:.1f}", aim.total_income), ' ', 30, Ed::LEFT, 1, true, Ag::LEFT, 10);
			autoPrint(std::format("�ܳɱ�:${:.1f}", aim.total_cost), ' ', 30, Ed::ALL, 1, false, Ag::LEFT, 10);

			printf("          ");
			autoPrint(std::format("������:${:.2f}", aim.pure_income), ' ', 30, Ed::LEFT, 1, true, Ag::LEFT, 10);
			autoPrint(std::format("������:${:.2f}", aim.avg_income), ' ', 30, Ed::LEFT, 1, true, Ag::LEFT, 10);
			autoPrint(std::format("��������:${:.2f}", aim.avg_pure_income), ' ', 30, Ed::ALL, 1, false, Ag::LEFT, 10);

			printf("          ");
			autoPrint("��ע��Ϣ:" + aim.otherdata, ' ', 90, Ed::ALL, 1, false, Ag::MIDDLE, 5);
			};
		int highlight = -1;
		if (choice != nullptr) highlight = 0;
		if (_tr && _d) {
			std::cout << "��ʷ��¼Ϊ��!" << std::endl;
			return;
		}
		if (!_d && (mode == Pr::DAILY || mode == Pr::ALL)) {
			resetColor(G | I);
			int len = (int)CRecord.Saved_daily.size();
			if (len != 0) {
				std::cout << "������ " << len << " ��*����*����,��ʼչʾ:" << std::endl;
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
				std::cout << "������ " << len << " ��*����*����,��ʼչʾ:" << std::endl;//������,������.<<-----------
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
	 * @brief ���洫�������,��Ҫ����һ��������daily����.
	 * @param data daily����ֵ����,�������std::move().
	 *
	 * @return �Ƿ�ɹ�¼��Ԫ��
	 */
	bool calcRecord::SaveDaily(daily&& data) noexcept {
		if (data.dailySave.total_number <= 0) {
			return false;
		}
		//catch time()
		CRecord.Saved_daily.emplace_back(std::move(data));//����
		return true;
	}

	/**
	 * @brief ���洫�������,��Ҫ����һ��������timeRange��������
	 * @param data timeRange����ֵ����,�������std::move().
	 *
	 * @return �Ƿ�ɹ�¼��Ԫ��
	 */
	bool calcRecord::SaveRange(timeRange&& data) noexcept {//ע��:�����ֵľ�����ֵ,�����ֵĲ�����ֵ.ʹ����ֵ���ù������ֵ��ʱ��,��Ҫ������move����ֵ������ֵ.
		if (data.totalData.total_number <= 0 || data.multiSave.empty()) {
			return false;
		}
		CRecord.Saved_timeRange.emplace_back(std::move(data));//ֱ�����¹���
		return true;
	}

	//���ݳ�ʼ��.
	void calcRecord::datainit() {
		Saved_daily.clear();
		Saved_timeRange.clear();
	}

	/*
	 * @brief ʹ�ö�����һ������������Ѱ��ĳһֵ,�����������.
	 * @param vec vector����,��ҪѰ�ҵ�����.
	 * @param aim ��ҪѰ�ҵ�����.
	 *
	 * @note ���ڲ���ǰ��������Ƿ�����,�����轫���Զ�sort.
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

	//sumCalc��ǰ�÷���.�����ʼ����
	void CS_menu() {
		std::vector<timedata> range;//������ϵó�������.
		std::string nam;//��������
		std::vector<int> choice;
		int total = (int)CRecord.Saved_daily.size() - 1;
		std::string orginIn = "";
		int input = 0;
		bool onChoose = true;

		while (onChoose) {
			system("cls");
			if (!choice.empty()) {
				if (!std::is_sorted(choice.begin(), choice.end()))
					std::sort(choice.begin(), choice.end());//Ĭ����������.
				CRecord.ShowSavedRecord(Pr::DAILY, &choice);
			}
			else {
				CRecord.ShowSavedRecord(Pr::DAILY);
			}
			std::cout << "ѡ������Ҫ���ϵ�ʱ������.��ʽ:������ǰ׺��ʹ�ò�ͬ�Ĺ���." << std::endl;
			std::cout << "a,ʹ�ø�ǰ׺����һ���������ϸ�����." << std::endl;
			std::cout << "d,ʹ�ø�ǰ׺����һ������ɾ��������,����ǰ������ѡ���˸�Ԫ��" << std::endl;
			std::cout << "x,ʹ�ø�ǰ׺���������������˳�." << std::endl;
			if (!(std::cin >> orginIn)) {
				std::cerr << "��ȡʧ��!" << std::endl;
				Sleep(300);
				continue;
			}
			else {
				if (std::regex_match(orginIn, match, pattern_sumCalc)) {
					input = std::stoi(match[2]);
					if (match[1] == 'x') {
						std::cout << "¼�����!��ǰ��¼��:" << (int)choice.size() << "��Ԫ��!" << std::endl;
						onChoose = false;
					}
					else if (match[1] == 'a') {
						if (input < -1 || input > total) {
							std::cout << "�������ݷ�Χ!" << std::endl;
							Sleep(300);
							continue;
						}
						else if ((!choice.empty()) && dichotomyFind(choice, input) != -1) {
							std::cout << "�ظ�¼��!��¼�������ֵ." << std::endl;
							Sleep(300);
							continue;
						}
						else {
							choice.push_back(input);
							std::cout << "¼��ɹ�!" << std::endl;
						}
					}
					else {
						int Index = 0;
						if (input < -1 || input > total) {
							std::cout << "�������ݷ�Χ!" << std::endl;
							Sleep(300);
							continue;
						}//ע��,����˴����������ȼ���ͬ,!=�����ȼ�����=,�⵼�º���ı��ʽ������,�������true.
						else if ((!choice.empty()) && ((Index = dichotomyFind(choice, input)) != -1)) {
							choice.erase(choice.begin() + Index);
							std::cout << "ɾ���ɹ�!" << std::endl;
							Sleep(300);
						}
						else {
							std::cout << "�Ҳ���Ԫ��!" << std::endl;
						}
					}
				}
				else {
					std::cout << "��ʽ����ȷ!" << std::endl;
					continue;
				}
			}
		}
		if (choice.empty()) {
			std::cout << "Ԫ����Ϊ��!��������˳�..." << std::endl;
			(void)_getch();
			return;
		}
		else {
			std::cout << "��Ϊ�ü����������!" << std::endl;
			std::cin >> nam;
			int len = static_cast<int>(choice.size());
			range.reserve(len);
			for (int i = 0; i < len; ++i) {
				range.emplace_back(CRecord.Saved_daily[choice[i]].dailySave);
			}
			if (sumCalc(range, nam)) {
				std::cout << "����ɹ�!" << std::endl;
			}
			else {
				std::cout << "����ʧ��!" << std::endl;
			}
		}
	}

	//ע��ģʽ
	void Register() {//ע��ģʽ
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
				std::cout << "\nɾ�����һ�����������\n";
				std::cout << "�ҵ���������";
				std::cout << std::format("#<{}-{}>: {} $:{:.3f}\n", Rst::dapx + 1, Rst::dapy + 1, Rst::datas[Rst::dapx][Rst::dapy].name, Rst::datas[Rst::dapx][Rst::dapy].cost);//���ڸ�ʽ����������ļ�.
				std::cout << "�Ƿ�ȷ��Ҫɾ����Ԫ��?����Yȷ��,������������.>:" << std::endl;
				char ret02_01 = _getch();
				if (ret02_01 == 'y' || ret02_01 == 'Y') {
					Rst::datas[Rst::dapx][Rst::dapy].occupied = false;
					std::cout << "\n�ѳɹ�ɾ������!";
				}
				else {
					Rst::Addxy();
					std::cout << "\n�ѳ���ɾ������!";
				}
				std::cout << "\n��������˳�...\n";
				(void)_getch();
				break;
			}
			case(Re::CHANGE): {
				std::cout << "\nʹ�����ֿ�ݼ������޸�Ŀ��,����0.5���ʼ����ģʽ";
				Sleep(500);
				coordInt aim = Rst::CDserach();
				if (aim.X != -1 && aim.Y != -1) {
					bool alter = true;
					while (alter) {
						std::cout << "\n������Ŀ���ǰ״̬!\n";
						Rst::ShowNowDataBase(aim.X, aim.Y);
						std::cout << "\n-----1.�޸�����--2.�޸Ľ��--0.EXIT-----\n";
						char ret03_01 = _getch();
						switch (ret03_01) {
						case('0'): {
							alter = false;
							break;
						}
						case('1'): {
							std::cout << "\n����������>:";
							std::cin >> Rst::datas[aim.X][aim.Y].name;
							std::cout << "�޸ĳɹ�!\n";
							Sleep(200);
							break;
						}
						case('2'): {
							std::cout << "\n��������(�ɱ�)>:";
							std::cin >> Rst::datas[aim.X][aim.Y].cost;
							std::cout << "�޸ĳɹ�!\n";
							Sleep(200);
							break;
						}
						default:
							break;
						}
					}
				}
				std::cout << "\n��������˳�...\n";
				(void)_getch();
				break;
			}
			case(Re::SHOWALL): {
				std::cout << "ȫ����������,��������˳�." << std::endl;
				Rst::ShowNowDataBase();
				(void)_getch();
				break;
			}
			case(Re::EXIT):
				RegMode = false;
				break;

			default:
				std::cout << "����ȷ����...";
				Sleep(300);
				break;
			}
		}
		return;
	}

	//����ģʽ
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
			case(Ca::CALC_SUM): {//����01
				CS_menu();
				break;
			}
			case(Ca::CALC_HISTORY): {//չʾ��ʷ��¼02???
				//Ҫ�������������ܰ�?
				break;
			}
			case(Ca::SHOW_EVERY_DATA): {//չʾ��ǰ��������
				CRecord.ShowSavedRecord();
				std::cout << "��������˳�..." << std::endl;
				(void)_getch();
				break;
			}
			case(Ca::COMPARE): {//�Ƚ�
				std::cout << "����Ƚ�ģʽ!����ԱȽ���������(������Ϣ)�Ѵ洢����." << std::endl;
				int d_len, s_len;
				d_len = (int)CRecord.Saved_daily.size() - 1;
				s_len = (int)CRecord.Saved_timeRange.size() - 1;
				CRecord.ShowSavedRecord();
				std::cout << "�������������±�,����չʾ�ȽϽ��.ע��,����Ϊ�������ݵ��±�,�������Ǽ������ݵ��±��һ��ȡ��.\n������µ����ݷ����ұ�.>:" << std::endl;
				int left = 0, right = 0;
				while (true) {
					std::cin >> left;
					std::cout << "��ѡ������:" << left << "." << std::endl;
					std::cin >> right;
					std::cout << "��ѡ������:" << right << "." << std::endl;
					if (left > d_len || left < -s_len - 1) {
						std::cout << "��ֵ��������!!��������...";
						continue;
					}
					else if (right > d_len || right < -s_len - 1) {
						std::cout << "��ֵ��������!!��������...";
						continue;
					}
					else {
						break;
					}
				}
				std::cout << "��ʼ�Ƚ�" << left << "��" << right << std::endl;
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
				Compare(*le, *ri);//�ò���û�з�������ĸ�������,*ptr�൱�ڶ����ݵ�����,���ᷢ������ĸ���,�Ǻܺõ�д��.
				std::cout << "��������˳�..." << std::endl;
				(void)_getch();
				//�����űȽ�,�����...
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
			std::cout << "��ǰ����ShowMenu����...\n";
			std::cout << "������Ŀ������ΪcalcrlateMode �����Զ�Ӧ����,��0�˳�." << std::endl;
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
			std::cout << "��ǰ����autoPrint����...\n";
			std::cout << "������Ŀ������ΪAlignmentMode�����Զ�Ӧ����,��0�˳�." << std::endl;
			std::cout << "==1.�ı������" << std::endl;
			std::cout << "==2.�ı�����" << std::endl;
			std::cout << "==3.�ı��Ҷ���" << std::endl;
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
			std::cout << "������Ҫ��ʾ�Ļ���" << std::endl;
			std::string input = "";
			std::cin >> input;
			std::cout << "����ո�ٷֱ�(����)";
			std::cin >> perc;
			autoPrint(input, '^', 10, Ed::ALL, 2, false, static_cast<Ag>(de_aim), perc);
		}
	}

	void debug_recordFind() {

		while (true) {
			std::cout << "��ǰ����recordFind����...,��1����,��0�˳�.��ģʽ�½�ֹ�����ݽ���ɾ�Ĳ���.\n";
			if (_getch() == '0') {
				return;
			}
			std::vector<record> tmp;
			tmp.push_back({ "���Բ���,����23336777]" ,520,999 });
			tmp.push_back({ "��������" ,520,999 });
			tmp.push_back({ "��������������!]" ,520,999 });
			tmp.push_back({ "GetThrughTheDarkness]" ,520,999 });
			tmp.push_back({ "�������,�������]" ,520,999 });
			tmp.push_back({ "�������������]" ,520,999 });
			tmp.push_back({ "���������������]" ,520,999 });
			tmp.push_back({ "2424242424242424" ,520,999 });
			recordFind(tmp, nullptr);

		}
	}

	void Debug() {
		while (true) {
			system("cls");
			std::cout << "��ӭ����Debugģʽ!��0�˳���ģʽ" << std::endl;
			std::cout << "1>.����ShowMenu����" << std::endl;
			std::cout << "2>.����autoPrint" << std::endl;
			std::cout << "3>.����recordFind" << std::endl;
			std::cout << "4>." << std::endl;
			std::cout << "5>." << std::endl;

			Db token = static_cast<Db>(_getch() - '0');
			switch (token) {
			case(Db::EXIT): {
				std::cout << "���������ȫ�˳�Debugģʽ!" << std::endl;
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