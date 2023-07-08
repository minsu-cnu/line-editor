#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Page{
    int count; // 저장된 문자열 개수
    char lines[10][100]; // 문자열 10개 저장 가능 (문자열은 최대 80자 입력됨)
    struct Page * nextPage; // 다음 페이지 구조체 포인터
    struct Page * prevPage; // 이전 페이지 구조체 포인터
} Page;

// page 구조체 동적 메모리 생성
Page * newPage(Page * prev) {
    Page * newPage = (Page *)malloc(sizeof(Page));
    newPage->count = 0;
    newPage->nextPage = NULL;
    newPage->prevPage = prev;
    return newPage;
}

//append, insert, remove 함수 구현
void appendLine(Page * page, char * str, int cntGlobal, int * ptrCntGlobal) {
    int cntCurPage = page->count;

    // 뒷 페이지에 원소가 더 있거나, 뒷 페이지에 원소가 더 없고 현재 페이지가 꽉 찬 경우
    if (cntGlobal > cntCurPage || cntCurPage == 10) {
        if (page->nextPage == NULL) {
            page->nextPage = newPage(page);
        }
        appendLine(page->nextPage, str, cntGlobal - cntCurPage, ptrCntGlobal);
        return;
    }
    
    strcpy(page->lines[cntCurPage], str);
    (page->count)++;
    (*ptrCntGlobal)++;
    return;
};

void insertLine(Page* page, int idx, char* str, int cntGlobal, int *ptrCntGlobal) {
    int cntCurPage = page->count;

    // 삽입 위치가 뒷 페이지에 있는 경우
    if (idx > cntCurPage) {
        insertLine(page->nextPage, idx-cntCurPage, str, cntGlobal, ptrCntGlobal);
        return;
    }

    // 페이지가 꽉 찬 경우, 뒷 페이지 하나 생성 후, 마지막 원소를 거기에 삽입
    if (cntCurPage == 10) {
        if (page->nextPage == NULL) {
            page->nextPage = newPage(page);
        } else {
            Page * nextTmpPtr = page->nextPage;
            page->nextPage = newPage(page);
            Page * nextCurPtr = page->nextPage;
            nextCurPtr->nextPage = nextTmpPtr;
            nextTmpPtr->prevPage = nextCurPtr;
        }

        strcpy(page->nextPage->lines[0], page->lines[9]);
        page->count--;
        cntCurPage--;
        page->nextPage->count++;
    }

    // 한 칸씩 뒤로 밀기
    for (int i=cntCurPage; i>=idx; i--) {
        strcpy(page->lines[i], page->lines[i-1]);
    }
    
    // 확보한 자리에 원소 삽입
    strcpy(page->lines[idx-1], str);
    page->count++;
    cntCurPage++;
    (*ptrCntGlobal)++;

    return;
}

void removeLine(Page* page, int idx, int cntGlobal, int * ptrCntGlobal) {
    int cntCurPage = page->count;

    // 지울 위치가 뒷 페이지에 있는 경우
    if (idx > cntCurPage) {
        removeLine(page->nextPage, idx - cntCurPage, cntGlobal, ptrCntGlobal);
        return;
    }

    // 지울 위치가 페이지의 마지막 라인인 경우
    if (idx == 10) {
        page->lines[idx-1][0] = '\0';
        page->count--;
        (*ptrCntGlobal)--;
        return;
    }


    // 한 칸씩 앞으로 당기기
    for (int i=idx; i<cntCurPage; i++) {
        strcpy(page->lines[i-1], page->lines[i]);
    }

    // 마지막으로 당긴 원소가 원래 있던 칸을 빈 문자열로 갱신해주기
    page->lines[cntCurPage-1][0] = '\0';
    page->count--;
    (*ptrCntGlobal)--;
    return;
}

int main(int argc, char * argv[]){
    //argv에는 파일 실행 커맨드와 인자들이 저장.
    //argc에는 인수의 총 개수가 저장
    //ex) ./a.out -a -b
    //argv[0] == ./a.out, argv[1] == -a, argv[2] == -b
    //argc == 3

    //본 실습의 경우 3번째 인자에 파일명, 4번째 인자에 명령어 존재

    // 명령어 최초 입력
    char inputTmp[100];
    char args[3][100] = {"\0", "\0", "\0"};
    int idxArgs;

    idxArgs = 0;
    gets(inputTmp);
    char *ptrSplit = strtok(inputTmp, " ");
    while (ptrSplit != NULL) {
        strcpy(args[idxArgs++], ptrSplit);
        ptrSplit = strtok(NULL, " ");
    }


    // 전체 원소 카운트, 메인 페이지 초기화
    int cntGlobal = 0;
    Page mainPage = { 0, NULL, NULL, NULL };

    // 명령어 종류 정의
    char a[] = "-a";
    char i[] = "-i";
    char r[] = "-r";

    // 기존 텍스트 파일 로드해서 구조체에 집어 넣기
    FILE * fileOrigin = fopen(argv[2], "r");
    Page * curPagePtr = &mainPage;
    int iter_cnt = 0;
    int move_cnt = 0;

    if (fileOrigin != NULL) {
        char strOrigin[100];

        while (fgets(strOrigin, 100, fileOrigin) != NULL) {
            if (strOrigin[strlen(strOrigin)-1] == '\n') {
                strOrigin[strlen(strOrigin)-1] = '\0';
            }
            if (iter_cnt > 0 && iter_cnt % 5 == 0) {
                curPagePtr->nextPage = newPage(curPagePtr);
                curPagePtr = curPagePtr->nextPage;
                move_cnt++;
            }
            appendLine(curPagePtr, strOrigin, cntGlobal - move_cnt * 5, &cntGlobal);
            iter_cnt++;
        }

        fclose(fileOrigin);
    }

    char * cmd;
    char str[100];
    int idx;

    // 명령어 실행 (exit이 들어올 때까지 반복)
    while (strcmp(args[0], "exit") != 0) {
        cmd = args[0];

        if (strcmp(cmd, a) == 0) { // 명령어가 -a 일 때
            strcpy(str, args[1]);
            if (str[strlen(str)-1] == '\n') { // 개행 문자 제거
                str[strlen(str)-1] = '\0';
            }
            if (strlen(str) == 0) { // 빈 문자열이 입력된 경우
                printf("input text data invalid!! (null error)\n");
            } else {
                appendLine(&mainPage, str, cntGlobal, &cntGlobal);
            }
        } else if (strcmp(cmd, i) == 0) { // 명령어가 -i 일 때
            idx = atoi(args[1]);
            strcpy(str, args[2]);
            if (str[strlen(str)-1] == '\n') { // 개행 문자 제거
                str[strlen(str)-1] = '\0';
            }
            if (idx < 1 || idx > cntGlobal) { // 삽입 위치 인덱스가 유효한 값이 아닌 경우
                printf("insert invalid!!\n");
            } else if (strlen(str) == 0) { // 빈 문자열이 입력된 경우
                printf("input text data invalid!! (null error)\n");
            } else {
                insertLine(&mainPage, idx, str, cntGlobal, &cntGlobal);
            }
        } else if (strcmp(cmd, r) == 0) { // 명령어가 -r 일 때
            idx = atoi(args[1]);
            if (idx < 1 || idx > cntGlobal) { // 삭제 위치 인덱스가 유효한 값이 아닌 경우
                printf("remove invalid!!\n");
            } else {
                removeLine(&mainPage, idx, cntGlobal, &cntGlobal);
            }
        } else { // 입력된 명령어 종류가 유효한 값이 아닌 경우
            printf("command invalid!!\n");
        }
        

        // 디버깅 및 구조 확인을 위한, 구조체 원소 출력 코드
        int idx_debug = 0; // 구조체 lines에 접근할 index
        int cntPut_debug = 0; // 전체적으로 원소 출력해낸 개수
        int itemCntPage_debug = 0; // 현재 구조체에서 출력해낸 원소 개수
        int pageNum_debug = 1; // 구조체 번호
        Page * ptrPage_debug = &mainPage;
        char * strItem_debug = ptrPage_debug->lines[0];
        
        printf("===========================================\n");
        while (cntPut_debug < cntGlobal) {
            if (idx_debug == 0) {
                printf("<page %d>\n", pageNum_debug++);
            }

            if (strlen(strItem_debug) != 0) { // 특정 자리에 원소가 존재한다면 출력해줌
                printf("%s\n", strItem_debug);
                cntPut_debug++;
                itemCntPage_debug++;
            }

            idx_debug++;

            // 현재 구조체의 원소를 모두 출력했다면 다음 구조체로 포인터 옮기기
            if (itemCntPage_debug == ptrPage_debug->count) {
                ptrPage_debug = ptrPage_debug->nextPage;
                idx_debug = 0;
                itemCntPage_debug = 0;
            }

            strItem_debug = ptrPage_debug->lines[idx_debug];
        }
        printf("===========================================\n");

        // 명령어와 인자들을 담은 args 배열 초기화해주기
        for (int k=0; k<3; k++) {
            args[k][0] = '\0';
        }

        // 명령어와 인자들 다시 입력 받기
        idxArgs = 0;
        gets(inputTmp);
        char *ptrSplit = strtok(inputTmp, " ");
        while (ptrSplit != NULL) {
            strcpy(args[idxArgs++], ptrSplit);
            ptrSplit = strtok(NULL, " ");
        }
    }

    // 동작이 끝난 파일을 write 하는 코드
    FILE * file = fopen(argv[2], "w");
    idx = 0; // 구조체 문자열 조회 인덱스
    int cntPut = 0; // 파일에 넣은 개수
    int itemCntPage = 0; // 현재 구조체에서 파일에 넣은 원소 개수
    int pageNum = 1; // 구조체 번호
    Page * ptrPage = &mainPage;
    char * strItem = ptrPage->lines[0];
    
    while (cntPut < cntGlobal) {
        if (strlen(strItem) != 0) {
            fputs(strItem, file);
            fputs("\n", file);
            cntPut++;
            itemCntPage++;
        }

        idx++;

        // 현재 구조체에서 원소 다 파일에 넣었으면 다음 구조체로 포인터 이동
        if (itemCntPage == ptrPage->count) {
            ptrPage = ptrPage->nextPage;
            idx = 0;
            itemCntPage = 0;
        }

        strItem = ptrPage->lines[idx];
    }
    
    fclose(file);

    Page * tmpPtr = &mainPage;
    int ii = 1;

    while (tmpPtr != NULL) {
        printf("%d번째 구조체 원소 개수 : %d\n", ii++, tmpPtr->count);
        tmpPtr = tmpPtr->nextPage;
    }

    // 동적 메모리 free 하는 코드 작성
    Page * iterPagePtr = mainPage.nextPage;
    Page * tmpPagePtr;
    while (iterPagePtr != NULL) {
        tmpPagePtr = iterPagePtr->nextPage;
        free(iterPagePtr);
        iterPagePtr = tmpPagePtr;
    }

    return 0;

}

