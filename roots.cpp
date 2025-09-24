#include "core.hpp"

/**
 * @file roots.cpp
 * @brief ��������ϵͳ����ģ�� - �������
 * @author Artcrk
 * @date 2025-09-24
 *
 * @todo ����Cģʽ�Ĺ���,ָ����ShowMenu��.
 * 
 * @todo ��ɶ��������ܵ��Ż�,�����ϣ�洢�ͱ�ǩ��˼��.(wish in v0.5)
 *
 * @todo ʹ��Qt��װ�������̨����,�����һ�����������ִ�������.(wish in v2.0)
 */

double Rst::Version = 1.0;
std::pair<int, int> hourRange[6] = { {0,4},{4,9},{9,12},{12,15},{15,19},{19,23} };//����ʹ��Բ����,�ᱻ����Ϊ���ű��ʽ!
std::string RangeHello[6] = {"��ҹ","�糿","����","����","����","ҹ��"};//ʱ��.

//��ȡdatabase.dll�е�����,��Ҫ��ʼ��datas��calcRecord
//ǰ���洢һ��int����(4�ֽ�),���ڴ洢datas�����Ĵ�С.
//�ڶ�ȡ���datas֮��,����һ��int�洢daily,֮�����daily.daily֮����һ��int�洢timeRange,�Դ�����.
bool datainit() {
	std::ifstream FileIn("database.dll");
	if (!FileIn.is_open()) {
		std::cout << ">:database�ļ���ʧ��!����һ��ʹ�ø���������." << std::endl;
		FileIn.close();
		return false;
	}
	FileIn.seekg(0, std::ios::end);
	auto length = FileIn.tellg();
	FileIn.seekg(0, std::ios::beg);
	if (length == 0) {
		std::cout << ">:�ļ�Ϊ��" << std::endl;
		FileIn.close();
		return false;
	}

	//------datas

	int datas_col,datas_row;
	FileIn >> datas_col;//xx xx str dou str dou ...
	FileIn >> datas_row;
	if (datas_col != 0 || datas_row != 0) {
		if (datas_col > 9 || datas_row > 9) {
			std::cout << ">:��ǰ���ݴ洢�쳣,��ȡ��ֹ." << std::endl;
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
				//��ʽ:" string double";

				std::string s = "";
				double d = 0.0;

				FileIn >> s;
				//Inportant information:����δ֪ԭ��,�ڶ�ȡ��ĸ�ֵ�����ᵼ������,
				// ���ó�����������ȡstring����,���Ǽ���std::move���Խ��������<<------����,Ϊʲô?
				Rst::datas[i][j].name = std::move(s);//��ȡstring
				//Rst::datas[i][j].name = s;
				FileIn >> d;
				Rst::datas[i][j].cost = d;//��ȡdouble.

				Rst::datas[i][j].occupied = true;
			}
		}
		std::cout << ">>+:�ɹ���ȡ" << 10 * datas_col + datas_row << "����Ʒ����." << std::endl;
	}
	else {
		std::cout << ">:δ��⵽��Ʒ�洢����." << std::endl;
	}
	//------
	 
	//���ڻ�ȡһ����timedata����.���뱣֤��ʽ��ȷ.
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
		da.reserve(daily_size);//����realloc,����Ҫ����sizeof.
		for (int i = 0; i < daily_size; i++) {
			Rst::timedata td;
			date.clear();
			FileIn >> date;
			Gettimedata(td);
			Rst::CRecord.SaveDaily(Rst::daily(td, date));
		}
		std::cout << ">>+:�ɹ���ȡ" << daily_size << "������������!" << std::endl;
	}
	else {
		std::cout << ">:δ��⵽������." << std::endl;
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
				name.emplace_back(std::move(tmp));//tmp��ָ��ָ���ַ�ı�,�Ѿ��൱��һ���µĿձ���.
			}
			Rst::CRecord.SaveRange(Rst::timeRange(td, date, name));
		}
		std::cout << ">>+:�ɹ���ȡ" << timeRange_size << "��ʱ�伯��ͳ������!" << std::endl;
	}
	else {
		std::cout << ">:δ��⵽ʱ�伯��ͳ������." << std::endl;
	}
	FileIn.close();
	return true;
}

void init() {
	Rst::consoleColor = Rst::getcolor();
	Rst::setcolor(B | I);
	//std::fill(std::begin(Rst::datas), std::end(Rst::datas), 0);//������һά����
	memset(Rst::datas, 0, sizeof(Rst::datas));//�﷨:Ŀ�� + ��Ҫ��ʼ����Ϊ����ֵ + Ŀ���С . ��Ҫ�Ǵ�!
	Rst::CRecord.datainit();
	Rst::dapx = 0, Rst::dapy = 0;
	std::string TimeNow = Rst::timeCatch();
	std::cout << "��ȡϵͳʱ��ɹ�!"+TimeNow << std::endl;
	std::regex_match(TimeNow, Rst::match, Rst::pattern_Time);
	int H = std::stoi(Rst::match[4]);//ע��stoi����string�ڲ�����
	for (int i = 0; i < 6; i++) {
		if (H >= hourRange[i].first && H < hourRange[i].second) {
			std::cout << "�ɹ����ʱ��:" << TimeNow << std::endl;
			Rst::setcolor(G | B | I);
			std::cout << RangeHello[i] << "��,Aethryrn!" << std::endl;
			break;
		}
	}
	Rst::setcolor(B | I);
	printf("\n\n{--->�����������ݶ�ȡ��ʼ��...\n");
	if (datainit()) {
		std::cout << ">+<��ȡ���ݳɹ�!" << std::endl;
	}
	else {
		std::cout << ">-<���ݳ�ʼ��ʧ��,�����ļ�database.dll" << std::endl;
	}
	Rst::setcolor(Rst::consoleColor);
	return;
}

bool save() {//�����.

	std::ofstream FileOut("database.dll");
	if (!FileOut.is_open()) {
		std::cout << "���ļ�ʧ��!" << std::endl;
		FileOut.close();
		return false;
	}
	int col_datas = Rst::dapx;
	int row_datas = Rst::dapy;
	
	FileOut << col_datas << " " << row_datas;


	for (int i = 0; i <= col_datas; i++) {
		for (int j = 0;j <10; j++) {//�˴���10��ʾ��Ʒ�е����ֵ.
			//if (i < col_datas && j >= 10) {
			//	break;//ǰ�������Ҫ�������.������ж�������������ص�,��ɾȥ.
			//}
			if (i == col_datas && j == row_datas) {
				break;//���һ�е���ֹ�˳�,������ָ��ĵص�,��Ϊdapx,dapyָ�����Զ����Ч��ַ��ĵ�һ���յ�ַ.
			}
			FileOut << "\n" << Rst::datas[i][j].name << " " << Rst::datas[i][j].cost;//��ʽ:" string double";
		}
	}
	//�����ʽ:��֤���һλ�ǿո�.
	FileOut << "\n";//����ȫ�����һ��,�ʻ���.


	//�������һ����timedata����.���뱣֤��ʽ��ȷ.
	//-->>>> days total_cost total_income pure_income avg_income avg_pure_income total_number otherdata
	auto Puttimedata = [&](const Rst::timedata& td) {//����
		FileOut << " " << td.days << " ";
		FileOut << td.total_cost << " ";
		FileOut << td.total_income << " ";
		FileOut << td.pure_income << " ";
		FileOut << td.avg_income << " ";
		FileOut << td.avg_pure_income << " ";
		FileOut << td.total_number << " ";
		if ((int)td.otherdata.size() == 0)//������һ���ո��¶�ȡʧ��.
			FileOut << "NULL";
		else 
		FileOut << td.otherdata;
		};

	const auto& da = Rst::CRecord.Saved_daily;
	const auto& tr = Rst::CRecord.Saved_timeRange;
	int da_len = (int)da.size();
	int tr_len = (int)tr.size();
	
	//�����С
	FileOut << da_len;//ͳһ�淶,ĩβ�޿ո����
	for (int i = 0; i < da_len; i++) {
		FileOut << "\n" << da[i].operationDate;
		Puttimedata(da[i].dailySave);
	}

	//�����ʽ:�˴�ռ��һ������λ.
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

	//�����ʽ:�˴�ռ��һ������λ.
	FileOut.close();
	return true;
}

void logUpdate() {
	std::ofstream FileOut("log.txt",std::ios::app);//ֻ׷��
	if (!FileOut.is_open()) {
		std::cout << "������־ʧ��" << std::endl;
		return;
	}
	else {
		std::cout << ">>_��־��¼�ɹ�!" << std::endl;
	}
	std::string t = Rst::timeCatch();
	FileOut << "Opened at " << t << ",in Version:v" <<std::format("{:.1f}", Rst::Version) << std::endl;
}

int main() {
	//ȡ
	init();
	logUpdate();
	(void)_getch();

	//�߼�����
	//
	Rm ongoing = Rm::MAINMENU;//��������ѡ��,����������.
	int debugMode = 0;
	while (ongoing != Rm::EXIT) {
		system("cls");
		Rst::ShowMenu(Cs::MAINMENU);
		Rm ret = static_cast<Rm>(_getch() - '0');
		switch (ret) {
		case(Rm::EXIT):
			if (save()) {
				std::cout << "����ɹ�!" << std::endl;
			}
			else {
				std::cout << "����ʧ��!" << std::endl;
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
			std::cout << std::endl << "�ٰ�" << 4 - debugMode << "�ν���DEBUGģʽ" << std::endl;
			if (debugMode > 3) {
				std::cout << "\n*����DEBUGģʽ!*" << std::endl;
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
