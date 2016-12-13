//===========================================================
//2016 12 08 노광준 c++
//레코드 생성 및 리턴 함수
//===========================================================
#ifndef MENUC

#define MENUC

#include "Menu_con.h"




map<string, tuple<int, int> > menu;            // 메뉴정보를 담기위한 자료구조로 map선택


//메뉴에 따른 가격 및 file save 용 암호화 값
void insert_map()
{
	menu["와퍼세트"] = make_tuple(0, 7500);
	menu["와퍼"] = make_tuple(1, 5400);
	menu["갈릭스테이크버거세트"] = make_tuple(2, 8500);
	menu["갈릭스테이크버거"] = make_tuple(3, 6400);
	menu["콰트로치즈와퍼세트"] = make_tuple(4, 8300);
	menu["콰트로치즈와퍼"] = make_tuple(5, 6300);
	menu["해쉬치즈와퍼세트"] = make_tuple(6, 8300);
	menu["해쉬치즈와퍼"] = make_tuple(7, 6300);
	menu["와퍼주니어세트"] = make_tuple(8, 5900);
	menu["와퍼주니어"] = make_tuple(9, 3900);
	menu["불고기버거세트"] = make_tuple(10, 5000);
	menu["불고기버거"] = make_tuple(11, 2900);

	menu["치킨크리스피버거세트"] = make_tuple(12, 7500);
	menu["치킨크리스피버거"] = make_tuple(13, 4500);
	menu["프렌치프라이"] = make_tuple(14, 2300);
	menu["어니언링"] = make_tuple(15, 2400);
	menu["코카콜라"] = make_tuple(16, 1700);
	menu["코카콜라제로"] = make_tuple(17, 2000);
	menu["스프라이트"] = make_tuple(18, 1700);
	menu["아메리카노"] = make_tuple(19, 1500);
	menu["현금"] = make_tuple(20, 0);
	menu["카드"] = make_tuple(21, 0);
}

//i=0일 경우 save용 메뉴 int값
//i=1일 경우 메뉴에 해당하는 가격
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
