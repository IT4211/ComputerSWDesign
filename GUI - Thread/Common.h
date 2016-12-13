//============================================================
//업데이트 2016-12-05 노광준 c++
//TCPClient 프로젝트 관련 파일
//헤더파일 모음
//상수 선언 모음
//구조체 선언
//============================================================

#ifndef _COM_H
#define _COM_H

#include <stdio.h>	
#include <iostream>
#include <cstdlib>
#include <stdlib.h>    
#include <string.h>     
#include <winsock2.h>  
#include <vector>		

#define MAX_MENU_LENGTH 1000
#define MAX_NUM 20
#define MAX_COM 30 //AVL트리 명령어
#define TCP_PORT 9000	//포트번호
using namespace std;
using Tokens = vector<string>;

#endif // !_COM_H