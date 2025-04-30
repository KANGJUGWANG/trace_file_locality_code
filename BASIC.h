#ifndef BASIC
#define BASIC
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOG_FILE_NAME {"log106_Messaging.txt", "log121_Movie.txt", "log202_GoogleMap.txt","log260_FaceBookHandOuts.txt"}

typedef struct COMMAND 
{
	//커맨드별 정보?
	int start_sector;//접근 시작 섹터
	int end_sector;// 접근 마지막 섹터
	int update_count;//업데이트 횟수
}COMMAND;
typedef struct FILE_INFOR
{
	// 각 파일별 정보 
	char* file_name;//실행 파일 이름
	int command_line;//총 라인수 
	int min_sector;//접근한 최소 섹터
	int max_sector;//접근한 최대 섹터
	int read;//총 읽기 횟수 0 or 4
	int writ;//총 쓰기 횟수 1 or 5
	int update;//업데이트 횟수 작성된 섹터에 다시 작성한 횟수
	float update_percent;//업데이트 퍼센트 비율
	int** update_size_count;//업데이이트 커멘드의 섹터 크기와 크기별 횟수
	int temporal_locality_count;//시간 지역성 커맨드 수
	int sequential_locality_count;//순차 지역성 커맨드 수
}FILE_INFOR;

extern const char* log_file_name[];// 불러올 파일 이름 리스트
extern const int file_count;// 불러올 파일 총 수량
extern int* max_strct_size; //할당된 커맨드 저장 구조체량
extern int* max_command_size_count;// 할당된 커맨드별 사이즈 저장량
FILE** all_fp_set();//모든 파일 포인터 생성
FILE** all_fp_free(FILE** fp);// 모든 파일 포인터 제거 
FILE_INFOR* file_information_set();// 모든 파일의 정보기록 구조체 생성
FILE_INFOR* f_infor_free(FILE_INFOR* f_infor);//모든 파일의 정보기록 구조체 제거
COMMAND*** file_commandt_set();//로그의 내용을 저장할 구조체 셋
void file_commandt_re_pointer_set(COMMAND*** file_command, int index); //로그의 내용을 저장할 구조체 셋_2
COMMAND*** file_command_free(COMMAND*** file_command);//로그 내용을 저정한 구조체를 제거
void f_infor_data(FILE* fp, int index, FILE_INFOR* f_infor, COMMAND*** file_command);//파일 정보를 구조체에 기록
void file_information_print(FILE_INFOR* f_infor); // 파일별 기본 정보 출력
int updatea_size_count_control(FILE_INFOR* f_infor, int index, int command_count, int sector_size);//업데이트 커맨드의 크기 확인 or 저장 함수
int compar(const void* a, const void* b);// 내림차순 정렬함수
int temporal_locality_function(COMMAND** file_command, int size, int start_sector, int end_sector);//시간 지역성 분류함수(지역성 분류함수)
int sequential_locality_function(COMMAND** file_command, int size);// 순차 지역성 분류 함수(지역성 분류함수)
void segmented(FILE* fp, int index, int segmented_max_size);//분할 계산 함수

#endif