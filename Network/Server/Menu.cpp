//===========================================================
//2016 12 08 �뱤�� c++
//���ڵ� ���� �� ���� �Լ�
//===========================================================
#ifndef MENUC

#define MENUC

#include "Menu_con.h"




map<string, tuple<int, int> > menu;            // �޴������� ������� �ڷᱸ���� map����


//�޴��� ���� ���� �� file save �� ��ȣȭ ��
void insert_map()
{
	menu["���ۼ�Ʈ"] = make_tuple(0, 7500);
	menu["����"] = make_tuple(1, 5400);
	menu["����������ũ���ż�Ʈ"] = make_tuple(2, 8500);
	menu["����������ũ����"] = make_tuple(3, 6400);
	menu["��Ʈ��ġ����ۼ�Ʈ"] = make_tuple(4, 8300);
	menu["��Ʈ��ġ�����"] = make_tuple(5, 6300);
	menu["�ؽ�ġ����ۼ�Ʈ"] = make_tuple(6, 8300);
	menu["�ؽ�ġ�����"] = make_tuple(7, 6300);
	menu["�����ִϾƮ"] = make_tuple(8, 5900);
	menu["�����ִϾ�"] = make_tuple(9, 3900);
	menu["�Ұ����ż�Ʈ"] = make_tuple(10, 5000);
	menu["�Ұ�����"] = make_tuple(11, 2900);

	menu["ġŲũ�����ǹ��ż�Ʈ"] = make_tuple(12, 7500);
	menu["ġŲũ�����ǹ���"] = make_tuple(13, 4500);
	menu["����ġ������"] = make_tuple(14, 2300);
	menu["��Ͼ�"] = make_tuple(15, 2400);
	menu["��ī�ݶ�"] = make_tuple(16, 1700);
	menu["��ī�ݶ�����"] = make_tuple(17, 2000);
	menu["��������Ʈ"] = make_tuple(18, 1700);
	menu["�Ƹ޸�ī��"] = make_tuple(19, 1500);
	menu["����"] = make_tuple(20, 0);
	menu["ī��"] = make_tuple(21, 0);
}

//i=0�� ��� save�� �޴� int��
//i=1�� ��� �޴��� �ش��ϴ� ����
int map_at(char* keymenu, bool i)
{
	try {
		get<1>(menu[keymenu]);
	}
	catch (const out_of_range& oor) {
		cerr << "Out of Range error: " << oor.what() << '\n';
		return 0;
	}
	if (i)
		return get<1>(menu[keymenu]);
	else
		return get<0>(menu[keymenu]);
}

#endif // !MENUC
