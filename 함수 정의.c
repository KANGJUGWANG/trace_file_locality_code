#include "BASIC.h"
/*
log106_Messaging : 최대 문자 길이 : 113, 라인수 : 5702
log121_Movie : 최대 문자 길이 : 113, 라인수 : 4781
log202_GoogleMap : 최대 문자 길이 : 116, 라인수 : 6266
log260_FaceBookHandOuts : 최대 문자 길이 : 114, 라인수 : 15601
*/
const char* log_file_name[] = LOG_FILE_NAME;// 불러올 파일 이름 리스트 정의
const int file_count = sizeof(log_file_name) / sizeof(log_file_name[0]);// 불러올 파일 총 수량 정의
int* max_strct_size = NULL;
int* max_command_size_count = NULL;
FILE** all_fp_set() 
{
	//이중 파일 포인터로 모든 파일포인터를 리턴
	FILE** fp = (FILE**)malloc(sizeof(FILE*) * file_count);
	if (fp == NULL) 
	{
		printf("파일포인터 배열 메모리 할당 실패");
		exit(0);
	}
	for (int i = 0; i < file_count;i++)
	{
		fp[i] = fopen(log_file_name[i], "r");
		if (fp[i] == NULL) {// 파일 포인터 호출이 실패할경우 프로그램 종료
			for (int j=0;j<i;j++) 
			{
				fclose(fp[j]);
			}
			free(fp);
			printf("\n%s 파일 열기 실패\n", log_file_name[i]);
			exit(0);
		}
	}
	return fp;
}
FILE** all_fp_free(FILE** fp) //파일 포인터 해제
{
	for (int i=0;i<file_count;i++) 
	{
		fclose(fp[i]);
	}
	free(fp);
	return NULL;
}
FILE_INFOR* file_information_set() // 모든 파일의 정보기록 구조체 생성
{
	FILE_INFOR* f_infor = (FILE_INFOR*)malloc(sizeof(FILE_INFOR) * file_count);
	if (f_infor == NULL)
	{
		printf("파일 정보 메모리 할당 실패");
		exit(0);
	}
	
	for (int i=0; i<file_count;i++) 
	{
		f_infor[i].file_name = (char*)malloc(sizeof(char) * (strlen(log_file_name[i]) + 1));
		if (f_infor[i].file_name == NULL)
		{
			printf("파일 정보(이름) 메모리 할당 실패");
			exit(0);
		}
		strcpy(f_infor[i].file_name, log_file_name[i]);
		f_infor[i].writ = 0;
		f_infor[i].command_line = 0;
		f_infor[i].min_sector = 0;
		f_infor[i].max_sector = 0;
		f_infor[i].read = 0;
		f_infor[i].writ = 0;
		f_infor[i].update = 0;
		f_infor[i].update_percent = 0;
		f_infor[i].update_size_count = (int**)malloc(sizeof(int*)*100);
		f_infor[i].sequential_locality_count = 0;
		f_infor[i].temporal_locality_count = 0;
		if (f_infor[i].update_size_count == NULL)
		{
			printf("메모리 할당 실패");
			exit(0);
		}
		for (int j=0;j<100;j++) 
		{
			f_infor[i].update_size_count[j] = (int*)calloc(sizeof(int), 2);
			if (f_infor[i].update_size_count[j] == NULL)
			{
				printf("메모리 할당 실패");
				exit(0);
			}
		}
		max_command_size_count[i] = 100;
	}
	return f_infor;
}
void update_size_count_reset(FILE_INFOR f_infor,int index)//업데이트 커맨드의 사이즈 기록배열 크기 재할당
{
	f_infor.update_size_count = (int**)realloc(f_infor.update_size_count,sizeof(int*) * max_command_size_count[index]);
	if (f_infor.update_size_count == NULL)
	{
		printf("메모리 할당 실패");
		exit(0);
	}
	for (int j = max_command_size_count[index] -100; j < max_command_size_count[index]; j++)
	{
		f_infor.update_size_count[j] = (int*)calloc(sizeof(int), 2);
		if (f_infor.update_size_count[j] == NULL)
		{
			printf("메모리 할당 실패");
			exit(0);
		}
	}
}

FILE_INFOR* f_infor_free(FILE_INFOR* f_infor)//모든 파일의 정보기록 구조체 제거
{
	for (int i = 0;i < file_count;i++)
	{
		free(f_infor[i].file_name);
		for (int j =0;j< max_command_size_count[i];j++)
		{
			free(f_infor[i].update_size_count[j]);
		}
	}
	free(f_infor); 
	return NULL;
}

COMMAND*** file_commandt_set() //로그의 내용을 저장할 구조체 셋
{
	int size = 100;
	COMMAND*** file_command = (COMMAND***)malloc(sizeof(COMMAND**) * file_count);
	if (file_command==NULL)
	{
		printf("임시저장할 구조체 할당 실패");
		exit(0);
	}
	for (int i = 0; i < file_count; i++)
	{
		file_command[i] = (COMMAND**)malloc(sizeof(COMMAND*) * size);//초기 사이즈 100
		if (file_command[i] == NULL)
		{
			printf("임시저장할 구조체 할당 실패");
			exit(0);
		}
		for (int j=0;j<size;j++) 
		{
			file_command[i][j] = (COMMAND*)malloc(sizeof(COMMAND));
			if (file_command[i][j] == NULL)
			{
				printf("임시저장할 구조체 할당 실패");
				exit(0);
			}
			file_command[i][j]->start_sector = 0;
			file_command[i][j]->end_sector = 0;
			file_command[i][j]->update_count = 0;
		}
		max_strct_size[i] = size;
	}
	return file_command;
}

void file_commandt_re_pointer_set(COMMAND*** file_command,int index) //로그의 내용을 저장할 구조체 셋_2
{
	
	file_command[index] = (COMMAND**)realloc(file_command[index],sizeof(COMMAND*)* max_strct_size[index]);
	if (file_command[index] == NULL)
	{
		printf("임시저장할 구조체 추가할당 실패");
		exit(0);
	}
	for (int j = max_strct_size[index] -100; j < max_strct_size[index]; j++)
	{
		file_command[index][j] = (COMMAND*)malloc(sizeof(COMMAND));
		if (file_command[index][j] == NULL)
		{
			printf("임시저장할 구조체 추가할당 실패");
			exit(0);
		}
		file_command[index][j]->start_sector = 0;
		file_command[index][j]->end_sector = 0;
		file_command[index][j]->update_count = 0;
	}
}

COMMAND*** file_command_free(COMMAND*** file_command)//로그 내용을 저정한 구조체를 제거
{
	for (int i = 0; i < file_count; i++)
	{
		for (int j = 0; j < max_strct_size[i]; j++)
		{
			free(file_command[i][j]);
		}
		free(file_command[i]);
	}
	free(file_command);
	return NULL;
}
void f_infor_data(FILE* fp, int index, FILE_INFOR* f_infor, COMMAND*** file_command)//파일 정보를 구조체에 기록
{
	int count = 0;
	int command_count = 0;
	char line[120];
	int token_start;
	int token_size;
	int action = 0;
	int no_update = 0;//고유값 수
	int update = 0;//중복 수==업데이트 수

	int sector_max_size = 0;
	while (fgets(line, sizeof(line), fp)) {
		if (count == max_strct_size[index])
		{
			max_strct_size[index] += 100;
			file_commandt_re_pointer_set(file_command, index);
		}
		if (command_count == max_command_size_count[index])
		{
			max_command_size_count[index] += 100;
			update_size_count_reset(f_infor[index], index);
		}
		token_start = atoi(strtok(line, "	"));//접근 시작 섹터
		if (f_infor[index].min_sector==0)
		{
			f_infor[index].min_sector = token_start;
		}
		else if(f_infor[index].min_sector> token_start)
		{
			f_infor[index].min_sector = token_start;
		}
		token_size = atoi(strtok(NULL, "	"));// 접근 섹터 크기
		if (sector_max_size < token_size) 
		{
			sector_max_size = token_size;
		}
		if (f_infor[index].max_sector == 0)//접근 섹터수는 시작 섹터 포함 이기 때문에 -1
		{
			f_infor[index].max_sector = (token_start +token_size-1);
		}
		else if (f_infor[index].max_sector < (token_start +token_size-1))
		{
			f_infor[index].max_sector = (token_start +token_size-1);
		}

		strtok(NULL, "	");// 접근 바이트
		action = atoi(strtok(NULL, "	"));
		switch (action)// 접근 유형별 분류
		{
			case 0://읽기
			{
				f_infor[index].read++;
				break;
			}
			case 1://쓰기
			{
				f_infor[index].writ++;
				break;
			}
			case 4://읽기
			{
				f_infor[index].read++;
				break;
			}
			case 5://쓰기
			{
				f_infor[index].writ++;
				break;
			}
		}
		if (temporal_locality_function(file_command[index], count, token_start, token_start + token_size - 1))
		{
			f_infor[index].temporal_locality_count++;
			if (action == 1 || action == 5)//
			{
				command_count += updatea_size_count_control(f_infor, index, command_count, token_size);
				update++;
			}
			else 
			{
				no_update++;
			}
		}
		else 
		{
			no_update++;
		}


		file_command[index][count]->start_sector = token_start;
		file_command[index][count]->end_sector = (token_start + token_size - 1);

		count++;
	}
	f_infor[index].sequential_locality_count = sequential_locality_function(file_command[index], count);
	printf("----------------------------------------------------------\n");
	printf("%s\n", log_file_name[index]);
	printf("no_update : %d\nup_date : %d\n", no_update, update);
	printf("all no update : %d - all update : %d - all data %d\n", no_update, update, no_update + update);
	printf("max size %d\n",sector_max_size);
	qsort(f_infor[index].update_size_count, command_count,sizeof(sizeof(int*)), compar);
	f_infor[index].command_line = count;
	f_infor[index].update = update;//쓰기 유형의 모든 업데이트
	f_infor[index].update_percent = (float)f_infor[index].update / (float)f_infor[index].command_line * 100;
}
void file_information_print(FILE_INFOR* f_infor) // 파일별 기본 정보 출력
{
	printf("=================================================================================\n");
	for (int i=0;i< file_count;i++)
	{
		printf("파일 이름 : %s파일 단순 분석결과\n", f_infor[i].file_name);
		printf("총 라인수 : %d\n", f_infor[i].command_line);
		printf("읽기 횟수 : %d, 쓰기 횟수 : %d, 읽기쓰기 총 합 : %d\n", f_infor[i].read, f_infor[i].writ, (f_infor[i].read + f_infor[i].writ));
		printf("총 업데이트 횟수 %d, 전체 커맨드 중 업데이트 커맨드 비율 : %0.3f%%\n", f_infor[i].update, f_infor[i].update_percent);
		printf("temporal_locality(시간 지역성)을 가지는 커맨드 수 : %d\n", f_infor[i].temporal_locality_count);
		printf("temporal_locality(시간 지역성)을 가지는 커맨드 비율 : %0.3f%%\n", (float)f_infor[i].temporal_locality_count / (float)f_infor[i].command_line * 100);
		printf("sequential_locality(순차 지역성)을 가지는 커맨드 수 : %d\n", f_infor[i].sequential_locality_count);
		printf("sequential_locality(순차 지역성)을 가지는 커맨드 비율 : %0.3f%%\n", (float)f_infor[i].sequential_locality_count / (float)f_infor[i].command_line * 100);
		printf("		----------		----------		----------			\n");
		printf("업데이트되는 커맨드의 데이터 섹터 크기(내림차순)\n");
		for (int j = 0; f_infor[i].update_size_count[j][0]!=0 &&f_infor[i].update_size_count[j][1]!=0; j++)
		{
			printf("섹터 사이즈 : %d, 횟수 : %d\n", f_infor[i].update_size_count[j][0], f_infor[i].update_size_count[j][1]);
		}
		printf("=================================================================================\n");
	}
}

int temporal_locality_function(COMMAND** file_command,int size, int start_sector, int end_sector)//시간 지역성 확인함수(업데이트 x - 유형 판단 x)
{
	int update = 0;
	for (int i=0;i<size;i++) 
	{
		if (file_command[i]->start_sector>=start_sector && end_sector>=file_command[i]->end_sector)//기존 작성된 섹터를 포함해 추가섹터 접근 == 섹터 겹침
		{
			update = 1;
		}
		else if (file_command[i]->start_sector <= start_sector && start_sector <= file_command[i]->end_sector)//시작섹터가 기존 작성된 섹터에 겹침발생 == 섹터 겹침
		{
			update = 1;
		}
		else if (file_command[i]->start_sector <= end_sector && end_sector <= file_command[i]->end_sector)//종료 섹터가 기존 작성된 섹터에 겹침 발생 == 섹터 겹침
		{
			update = 1;
		}
	}
	return update;
}

int updatea_size_count_control(FILE_INFOR* f_infor,int index,int command_count,int sector_size)
{
	for (int i =0;i<command_count;i++) 
	{
		if (f_infor[index].update_size_count[i][0] == sector_size)
		{
			f_infor[index].update_size_count[i][1]++;
			return 0;
		}
	}
	f_infor[index].update_size_count[command_count][0] = sector_size;
	f_infor[index].update_size_count[command_count][1]++;
	return 1;
}
int compar(const void* a, const void* b)// 내림차순 정렬함수 
{
	int* rowA = *(int**)a;
	int* rowB = *(int**)b;
	 return rowB[1] - rowA[1]; // 내림차순
}
int sequential_locality_function(COMMAND** file_command, int size) // 순차 지역성 분류 함수
{
	int count = 0;
	int check = 0;
	int count_plus = 0;

	for (int i=2;i<size;i++) 
	{	// 커맨드 3개가 같은 거리를 두고 발생할 경우(일부 겹침도 동일한 간격이면 순차 지역성, 완전 겹침은 순차지역성 x)
		if (file_command[i-1]->start_sector - file_command[i - 2]->end_sector == file_command[i]->start_sector - file_command[i - 1]->end_sector && file_command[i]->start_sector - file_command[i - 1]->end_sector !=0) //순차 접근
		{
			count_plus++;
			check = 1;
		}
		else if (file_command[i - 1]->end_sector - file_command[i - 2]->start_sector == file_command[i]->end_sector - file_command[i - 1]->start_sector && file_command[i]->end_sector - file_command[i - 1]->start_sector!=0) // 역접근
		{
			count_plus++;
			check = 1;
		}
		else if (check==1)//섹터의 지역성을 찾았는데 더이상 없을때 
		{
			count = count_plus + 2;
			check = 0;
		}
	}
	if (check == 1)// 반복문이 종료되었을떄
	{
		count = count_plus + 2;
	}
	return count;
}
void segmented(FILE* fp, int index, int segmented_max_size)
{

	FILE_INFOR* f_infor = file_information_set();
	COMMAND*** file_command = file_commandt_set();
	// 파일 이름 만들기
	char csv_filename[256];
	snprintf(csv_filename, sizeof(csv_filename), "%s.csv", f_infor[index].file_name);

	// CSV 파일 열기
	FILE* csv_fp = fopen(csv_filename, "w");
	if (csv_fp == NULL) {
		perror("CSV 파일 열기 실패");
		exit(1);
	}

	// CSV 헤더 작성 (1번만)
	fprintf(csv_fp,
		"파일 이름,구간 번호,라인 수,읽기 횟수,쓰기 횟수,총 합,"
		"업데이트 수,업데이트 비율,temporal_locality(시간 지역성),temporal_locality(시간 지역성) 비율,"
		"sequential_locality(순차 지역성) 지역성 수,sequential_locality(순차 지역성) 지역성 비율\n");

	int count = 0;
	int command_count = 0;
	char line[220];
	char temp_line[200];
	int token_start;
	int token_size;
	int action = 0;
	int no_update = 0;//고유값 수
	int update = 0;//중복 수==업데이트 수
	int segmented_size = 0;
	int try_count = 1;//분할 구간 카운팅
	long long save = 0;
	long long temp_save = 0;
	while (1) {

		if (!fgets(line, sizeof(line), fp))break;
		temp_save = strlen(line);// 완료라인 파일 포인터 세이브
		strcpy(temp_line, line);
		if (count == max_strct_size[index])
		{
			max_strct_size[index] += 100;
			file_commandt_re_pointer_set(file_command, index);
		}
		if (command_count == max_command_size_count[index])
		{
			max_command_size_count[index] += 100;
			update_size_count_reset(f_infor[index], index);
		}
		//printf("||||%s||||\n",line);
		token_start = atoi(strtok(line, "	 "));//접근 시작 섹터
		token_size = atoi(strtok(NULL, "	 "));// 접근 섹터 크기
		strtok(NULL, "	");// 접근 바이트
		action = atoi(strtok(NULL, "	 "));
		segmented_size += token_size;

		if (save!=0 && segmented_size>segmented_max_size)
		{
			f_infor[index].sequential_locality_count = sequential_locality_function(file_command[index], count);
			f_infor[index].command_line = count;
			f_infor[index].update = update;//쓰기 유형의 모든 업데이트
			f_infor[index].update_percent = (float)f_infor[index].update / (float)f_infor[index].command_line * 100;
			printf("=================================================================================\n");
			printf("파일 이름 : %s파일 분할 분석결과\n", f_infor[index].file_name);
			printf("%d구간 라인수 : %d\n", try_count,f_infor[index].command_line);
			printf("읽기 횟수 : %d, 쓰기 횟수 : %d, 읽기쓰기 총 합 : %d\n", f_infor[index].read, f_infor[index].writ, (f_infor[index].read + f_infor[index].writ));
			printf("총 업데이트 횟수 %d, 전체 커맨드 중 업데이트 커맨드 비율 : %0.3f%%\n", f_infor[index].update, f_infor[index].update_percent);
			printf("temporal_locality(시간 지역성)을 가지는 커맨드 수 : %d\n", f_infor[index].temporal_locality_count);
			printf("temporal_locality(시간 지역성)을 가지는 커맨드 비율 : %0.3f%%\n", (float)f_infor[index].temporal_locality_count / (float)f_infor[index].command_line * 100);
			printf("sequential_locality(순차 지역성)을 가지는 커맨드 수 : %d\n", f_infor[index].sequential_locality_count);
			printf("sequential_locality(순차 지역성)을 가지는 커맨드 비율 : %0.3f%%\n", (float)f_infor[index].sequential_locality_count / (float)f_infor[index].command_line * 100);
			printf("=================================================================================\n");
			///////////////////////////
			fprintf(csv_fp,
				"%s,%d,%d,%d,%d,%d,%d,%.3f,%d,%.3f%%,%d,%.3f%%\n",
				f_infor[index].file_name,
				try_count,
				f_infor[index].command_line,
				f_infor[index].read,
				f_infor[index].writ,
				f_infor[index].read + f_infor[index].writ,
				f_infor[index].update,
				f_infor[index].update_percent,
				f_infor[index].temporal_locality_count,
				(f_infor[index].command_line > 0)
				? (float)f_infor[index].temporal_locality_count / f_infor[index].command_line * 100
				: 0.0,
				f_infor[index].sequential_locality_count,
				(f_infor[index].command_line > 0)
				? (float)f_infor[index].sequential_locality_count / f_infor[index].command_line * 100
				: 0.0
			);
			///////////////////////////////
			if (f_infor[index].command_line == 0) {
				printf("분석 제외됨\n", index);
				printf("%s\n", temp_line);
				return;
			}
			file_command = file_command_free(file_command);
			f_infor = f_infor_free(f_infor);
			file_command = file_commandt_set();
			f_infor = file_information_set();
			max_strct_size[index]=100;
			max_command_size_count[index]=100;
			fseek(fp, save, SEEK_SET);
			count = 0;
			command_count = 0;
			token_start=0;
			token_size=0;
			action = 0;
			no_update = 0;
			update = 0;
			segmented_size = 0;
			try_count++;
			continue;
		}
		

		if (f_infor[index].min_sector == 0)
		{
			f_infor[index].min_sector = token_start;
		}
		else if (f_infor[index].min_sector > token_start)
		{
			f_infor[index].min_sector = token_start;
		}
		if (f_infor[index].max_sector == 0)//접근 섹터수는 시작 섹터 포함 이기 때문에 -1
		{
			f_infor[index].max_sector = (token_start + token_size - 1);
		}
		else if (f_infor[index].max_sector < (token_start + token_size - 1))
		{
			f_infor[index].max_sector = (token_start + token_size - 1);
		}

		switch (action)// 접근 유형별 분류
		{
		case 0://읽기
		{
			f_infor[index].read++;
			break;
		}
		case 1://쓰기
		{
			f_infor[index].writ++;
			break;
		}
		case 4://읽기
		{
			f_infor[index].read++;
			break;
		}
		case 5://쓰기
		{
			f_infor[index].writ++;
			break;
		}
		}
		if (temporal_locality_function(file_command[index], count, token_start, token_start + token_size - 1))
		{
			f_infor[index].temporal_locality_count++;
			if (action == 1 || action == 5)//
			{
				command_count += updatea_size_count_control(f_infor, index, command_count, token_size);
				update++;
			}
			else
			{
				no_update++;
			}
		}
		else
		{
			no_update++;
		}
		save +=temp_save;
		file_command[index][count]->start_sector = token_start;
		file_command[index][count]->end_sector = (token_start + token_size - 1);
		count++;
	}
	f_infor[index].sequential_locality_count = sequential_locality_function(file_command[index], count);
	f_infor[index].command_line = count;
	f_infor[index].update = update;//쓰기 유형의 모든 업데이트
	f_infor[index].update_percent = (float)f_infor[index].update / (float)f_infor[index].command_line * 100;

	printf("=================================================================================\n");
	printf("파일 이름 : %s파일 분할 분석결과\n", f_infor[index].file_name);
	printf("%d구간(마지막) 라인수 : %d\n", try_count, f_infor[index].command_line);
	printf("읽기 횟수 : %d, 쓰기 횟수 : %d, 읽기쓰기 총 합 : %d\n", f_infor[index].read, f_infor[index].writ, (f_infor[index].read + f_infor[index].writ));
	printf("총 업데이트 횟수 %d, 전체 커맨드 중 업데이트 커맨드 비율 : %0.3f%%\n", f_infor[index].update, f_infor[index].update_percent);
	printf("temporal_locality(시간 지역성)을 가지는 커맨드 수 : %d\n", f_infor[index].temporal_locality_count);
	printf("temporal_locality(시간 지역성)을 가지는 커맨드 비율 : %0.3f%%\n", (float)f_infor[index].temporal_locality_count / (float)f_infor[index].command_line * 100);
	printf("sequential_locality(순차 지역성)을 가지는 커맨드 수 : %d\n", f_infor[index].sequential_locality_count);
	printf("sequential_locality(순차 지역성)을 가지는 커맨드 비율 : %0.3f%%\n", (float)f_infor[index].sequential_locality_count / (float)f_infor[index].command_line * 100);
	printf("=================================================================================\n");
	///////////////////////////////
	fprintf(csv_fp,
		"%s,%d,%d,%d,%d,%d,%d,%.3f,%d,%.3f%%,%d,%.3f%%\n",
		f_infor[index].file_name,
		try_count,
		f_infor[index].command_line,
		f_infor[index].read,
		f_infor[index].writ,
		f_infor[index].read + f_infor[index].writ,
		f_infor[index].update,
		f_infor[index].update_percent,
		f_infor[index].temporal_locality_count,
		(f_infor[index].command_line > 0)
		? (float)f_infor[index].temporal_locality_count / f_infor[index].command_line * 100
		: 0.0,
		f_infor[index].sequential_locality_count,
		(f_infor[index].command_line > 0)
		? (float)f_infor[index].sequential_locality_count / f_infor[index].command_line * 100
		: 0.0
	);
	fclose(csv_fp);
	file_command = file_command_free(file_command);
	f_infor = f_infor_free(f_infor);
	return;

	///////////////////////////////
}