#include "BASIC.h"
/*
실행 트레이스 파일 분석:

Nexus5_Kernel_BIOTracer_traces.tar내 WorkSpace_nexus5/Trace_files/

열 번호	설명
0	접근 시작 주소 (섹터 단위)
1	접근 크기 (섹터 단위) — 보통 4KB = 8 섹터이며, MMC 드라이버가 일부 요청에 섹터를 추가하므로 정제(cleaning) 필요
2	접근 크기 (바이트 단위)
3	접근 유형 및 대기 여부 — 하위 비트는 읽기/쓰기(0: 읽기, 1: 쓰기), 3번째 비트는 대기 여부(4: 대기 없음, 0: 대기함). 예: 5는 "쓰기 요청" + "대기 없음"을 의미
4	요청 생성 시간 (요청 큐에 삽입된 시점)
5	요청 처리 시작 시간 (MMC 드라이버가 큐에서 요청을 꺼내 처리 시작)
6	하드웨어에 요청이 제출된 시간
7	요청 완료 시간 (콜백 함수 호출 시점)
*/

int main()
{
	//전역변수 동적할당
	max_strct_size = (int*)calloc(sizeof(int), file_count);
	max_command_size_count = (int*)calloc(sizeof(int), file_count);//전역변수 - 업데이트 기록 비교 사이즈

	if (max_strct_size == NULL || max_command_size_count ==NULL)
	{
		printf("calloc 할당 실패");
		exit(0);
	}

	FILE** fp = all_fp_set();
	FILE_INFOR* f_infor = file_information_set();
	COMMAND*** f_command = file_commandt_set();
	for (int i = 0; i < file_count;i++)
	{
		f_infor_data(fp[i], i, f_infor, f_command);
	}
	file_information_print(f_infor); // 파일별 기본 정보 출력
	//저장된 내용 초기화
	f_command = file_command_free(f_command);
	f_infor = f_infor_free(f_infor);
	for (int i = 0; i < file_count; i++)
	{
		//파일 포인터 초기화
		rewind(fp[i]);
		segmented(fp[i], i, 16385);
	}

	fp = all_fp_free(fp);



	return 0;
}