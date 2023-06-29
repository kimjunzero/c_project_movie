#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <conio.h>
#include <time.h>

#define MAX_THEATER 6       // 관의 최대 개수
#define MAX_TIME 6          // 시간대의 최대개수
#define MAX_MOVIE 7         // 영화 종류의 최대개수

typedef struct {
    int status; // 좌석 상태 (0: 빈 좌석, 1: 예약된 좌석)
} Seat;

typedef struct {
    Seat** seats;
    int remainseat;
} TimeTable;

typedef struct {
    TimeTable timeTable[MAX_TIME]; // 각 극장에서 상영하는 영화와 해당하는 상영 시간을 저장하는 배열
    int rows;   //해당 관의 열
    int cols;   //해당 관의 열
} Theater;

// 영화  총 정보
typedef struct Movie{
    char movieName[30]; // 영화명
    int start_time; // 영화시작 시간
    int running_time; // 러닝 타임
    int box; // 상영관
    int movieId; // 영화 고유번호;
    struct Movie *prev;
    struct Movie *next;
}Movie;

// 영화정보 리스트
typedef struct MovieList{
    int date;   // 예매 날짜 (YYYYMMDD 형식으로 저장)
    int movies; // 영화 수
    int LastId; // 마지막 영화
    Movie *head, *tail;
}MovieList;

typedef struct People{
    int adult;          //성인의 수
    int children;       //어린이 수
    int teenager;       //청소년 수
    int total_people; // 예매한 총 인원 수
}People;

typedef struct Pay{
    int amount;         //금액
    int disamount;      //할인금액
    int totalpayment;   //결제금액
}Pay;

//애매자 정보
typedef struct Member {
    int personId; //예매자 고유번호
    People pn; // 인원수
    char name[30]; // 이름
    Movie m; // 영화명, 영화시작시간, 러닝타임, 상영관
    char reserve_seat[25]; //예매한 좌석
    char phone[30];
    Pay pay;
    struct Member* prev, * next;
}Member;

//애매자 리스트
typedef struct MemberList {
    int members;
    int LastperosonId; // 마지막 애매자 번호
    Pay managepay;
    Member* head, * tail;
}MemberList;

typedef struct StackNode {
    Member* member;
    struct StackNode* next;
} StackNode;

StackNode* top = NULL;

enum {
    BLACK,
    DARK_BLUE,
    DARK_GREEN,
    DARK_SKY_BLUE,
    DARK_RED,
    DARK_VIOLET,
    DARK_YELLOW,
    GRAY,
    DARK_GRAY,
    BLUE,
    GREEN,
    SKY_BLUE,
    RED,
    VIOLET,
    YELLOW,
    WHITE,
};

MovieList m_ML; // 영화 정보
MemberList a_ML; // 예매자 정보
Theater theaters[MAX_THEATER]; //관
People mvpeople[MAX_MOVIE];

int screenCount[MAX_THEATER] = { 0 };
char screenTimes[MAX_THEATER][MAX_TIME][5];
int currentStep = 1;  // 현재 단계
int previousStep = 0; // 이전 단계
int flag=1;


void color(int color, int bgcolor);
void gotoxy(int x, int y);
void init(void);            // List 초기화
void readandprocess(void);
void show_smu(void);        // smu 글자 스크린(애매 모드에서)
void Theater_init(void);    //관의 크기 지정
int showMangerMode(void);   // 관리자 메뉴 모드
int showMainMenu(void);     //예매자 메뉴 모드
void mangerMode(void);      // 관리자 모드
void guestMode(void);       // 애매자 모드
void push(Member* member);  // 스택 삽입
void pop(void);             // 스택 삭제
Movie* create_node(void);   // 영화 노드 생성
Member* create_node1(void); // 애매자 노드 생성

void display_movieName(void);   // 초기에 영화명 보여주기
void input_movie(void);     // 관리자 모드로 영화 정보 넣기
void search_movie(void);    // 영화 조회
void remove_movie(void);    // 영화 정보 삭제
void display_movie(void);   // 영화목록 출력(영화별로)
void total_money(void);
void print_movie_people(void);
void display_total(void);

void input_member(Member *tmp); // 예매자 넣기
void refund_member(int id); // 예매 취소
void display_movie_by_screen(char* movieName); // 상영관 별 영화 시간대
void print_screen(void); // screen글자 스크린
void showSeatMap(int theaternum, int timeIndex,Member *tmp);         //좌석 배치도를 출력하는 함수
int reserveSeat(int theaternum, int timeIndex, int cursorX, int CursorY,Member* tmp); //좌석을 예약하는 함수
int num_adult();    // 성인 수
int num_children(); // 어린이 수
int num_teenager(); // 청소년 수
void MovieReservation(void); // 단계별 예매
void calculatePayment(Member* member, int isSMU); // 총 비용 계산
void display_reservation(Member *tmp);
void display_member_by_phone(void); // 예매 내역 조회(콜)

void free_seat(void); // 동적 할당 해제
void read_movie(void);      // 영화정보 불러오기
void read_member(char *Filename);     // 애매자 정보 불러오기
void read_smu(Member *member);        //smu 학생 불러오기
void write_movie(void);     // 영화 정보 파일에 쓰기
void write_member(char* Filename);    // 애매자 정보 파일에 쓰기
void write_smu(void);       //smu 학생 정보 파일에 쓰기

int main(void)
{
    time_t t;
    struct tm* today;

    t = time(NULL); // 현재 시간 가져오기

    today = localtime(&t);
    // 오늘 날짜 파일
    char todayFilemembername[20];
    //todayFilename = 2023.월.일
    sprintf(todayFilemembername, "member-%04d-%02d-%02d.bin", today->tm_year + 1900, today->tm_mon + 1, today->tm_mday);

    // 내일 날짜 파일

    init(); // 초기화
    Theater_init();
    read_movie();

    read_member(todayFilemembername);
    readandprocess();

    mangerMode(); // 관리자 모드
    guestMode();

    write_movie();
    write_member(todayFilemembername);

    free_seat(); // 좌석 할당 해제
    printf("\n프로그램이 종료됩니다.");
    system("pause");
    return 0;
}
// 색깔
void color(int color, int bgcolor)
{
    color &= 0xf;
    bgcolor &= 0xf;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (bgcolor << 4) | color);
}
// 좌표
void gotoxy(int x, int y)
{
    COORD Cur;
    Cur.X = x;
    Cur.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
}
// List 초기화
void init(void)
{
    m_ML.movies = 0;
    m_ML.head = m_ML.tail = NULL;
    m_ML.LastId=0;
    a_ML.managepay.amount=0;
    a_ML.managepay.disamount=0;
    a_ML.managepay.totalpayment=0;
    a_ML.members = 0;
    a_ML.LastperosonId = 0;
    a_ML.head = a_ML.tail = NULL;
}
void readandprocess(void) {

    for (Movie* temp = m_ML.head; temp != NULL; temp = temp->next) {
        int box = temp->box - 1; // 상영관 번호를 배열 인덱스로 변환

        // 시간을 char 형식으로 변환하여 저장
        char timeChar[5]; // 시간을 저장할 char 배열
        sprintf(timeChar, "%d", temp->start_time);

        // 시간을 char 형식으로 저장
        strcpy(screenTimes[box][screenCount[box]], timeChar);

        screenCount[box]++;
    }
    getch();


    for (Member* temp = a_ML.head; temp != NULL; temp = temp->next) {
        char seats_copy[25];
        strcpy(seats_copy, temp->reserve_seat);
        char* seat_token = strtok(seats_copy, " ");

        int memtimeIndex = findTimeIndex(temp->m.box - 1, temp->m.start_time);

        while (seat_token != NULL) {
            // 좌석 정보에서 행과 열 값을 추출합니다
            int row = seat_token[0] - 'A';
            int col = atoi(&seat_token[1])-1;
            // 해당 좌석을 처리하는 코드를 작성합니다
            theaters[temp->m.box - 1].timeTable[memtimeIndex].seats[row][col].status = 2;
            theaters[temp->m.box - 1].timeTable[memtimeIndex].remainseat--;
            // 다음 좌석 정보를 가져옵니다
            seat_token = strtok(NULL, " ");
        }

    }

}

// 관리자 모드
void mangerMode(void)
{
    int menu;

    while (1) {
        system("cls");
        display_movieName();
        menu = showMangerMode();
        system("cls");
        switch (menu) {
        case 1:
            fflush(stdin);
            input_movie();
            break;
        case 2:
            search_movie();
            break;
        case 3:
            color(RED,BLACK);
            printf("===========================< 영화 삭제 >=======================\n");
            color(WHITE,BLACK);
            remove_movie();
            break;
        case 4:
            display_movie();
            color(RED, BLACK);
            printf("\n\n처음 화면으로 돌아가려면 아무 키나 누르세요.");
            color(WHITE, BLACK);
            getch();
            break;
        case 5:
            total_money();
            color(RED, BLACK);
            printf("\n\n처음 화면으로 돌아가려면 아무키나 누르세요.");
            color(WHITE, BLACK);
            getch();
            break;
        case 6:
            return 0;
            break;
        case 7:
            flag=0;
            return 0;
            break;
        }
    }
}
// 예매자 모드
void guestMode(void)
{
    int choice,id;
    while (flag) { // 무한 루프
        system("cls");
        show_smu();
        printf("\t\t\t <계속하려면 아무키나 누르세요>");
        getch();  // 아무 키나 누를 때까지 대기
        system("cls");
        choice = showMainMenu();
        system("cls");
        switch(choice){
        case 1: // 상영 시간표
            display_total();
            color(RED, BLACK);
            printf("\n\n처음 화면으로 돌아가려면 아무 키나 누르세요.");
            color(WHITE, BLACK);
            getch();
            break;
        case 2: // 애매
            MovieReservation();
            break;
        case 3: // 예매내역 조회
            display_member_by_phone();
            break;
        case 4: //예매 취소
            fflush(stdin);
            color(RED,BLACK);
            printf("\n예매 고유번호: ");
            color(WHITE,BLACK);
            scanf("%d", &id);
            refund_member(id);
            color(RED, BLACK);
            printf("\n\n처음 화면으로 돌아가려면 아무키나 누르세요.");
            color(WHITE, BLACK);
            getch();
            break;

        }
    }
}
// 스택에 삽입
void push(Member* member)
{
    StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
    newNode->member = member;
    newNode->next = top;
    top = newNode;
}
// 스택에 삭제
void pop(void)
{
    if (top == NULL) {
        printf("스택이 비어있습니다.\n");
        return;
    }

    StackNode* temp = top;
    top = top->next;
    free(temp);
}

// 영화 노드 생성
Movie* create_node(void)
{
    Movie* tmp;

    tmp = (Movie*)malloc(sizeof(Movie));
    if (tmp == NULL) {
        fprintf(stderr, "Memory Allocation Error\n");
        exit(1);
    }

    return tmp;
}
// 예매자 노드 생성
Member* create_node1(void)
{
    Member* tmp;

    tmp = (Member*)malloc(sizeof(Member));
    if (tmp == NULL) {
        fprintf(stderr, "Memory Allocation Error\n");
        exit(1);
    }
    return tmp;
}
// 관리자 메뉴 모드
int showMangerMode(void)
{

    int selector, cursorx = 1;
    color(RED,BLACK);
    printf("                                                < 메뉴 >                            \n");
    color(WHITE,BLACK);
    gotoxy(10,7);
    printf("1.영화등록 2.영화조회 3.영화삭제 4.전체조회 5.매출현황 6.관리자 모드 종료 7.프로그램 종료");
    gotoxy(0, 10);
    printf("                        < 좌우 방향키를 이용하여 선택 커서를 움직일 수 있습니다. > \n");
    printf("                                     < Enter키로 메뉴를 정하세요. >              \n");
    gotoxy(10,7);
    color(SKY_BLUE,BLACK);
    printf("1.영화등록");
    color(WHITE, BLACK);
    while (1) {
        if (kbhit())
        {
            color(WHITE, BLACK);
            gotoxy(10, 7);
            printf("1.영화등록 2.영화조회 3.영화삭제 4.전체조회 5.매출현황 6.관리자 모드 종료 7.프로그램 종료");
            selector = getch();
            if (selector == 224) { // 화살표 키일 경우
                selector = getch(); // 다음 키 입력 받기
                switch (selector)
                {
                case 77:
                    cursorx++;
                    break;
                case 75:
                    cursorx--;
                    break;
                }
            }
            else if (selector == 13)
                break;
            if (cursorx < 1)
                cursorx = 1;
            if (cursorx > 7)
                cursorx = 7;
            switch (cursorx)
            {
            case 1:
                gotoxy(10, 7);
                color(SKY_BLUE, BLACK);
                printf("1.영화등록");
                color(WHITE, BLACK);
                break;
            case 2:
                gotoxy(21, 7);
                color(SKY_BLUE, BLACK);
                printf("2.영화조회");
                color(WHITE, BLACK);
                break;
            case 3:
                gotoxy(32, 7);
                color(SKY_BLUE, BLACK);
                printf("3.영화삭제");
                color(WHITE, BLACK);
                break;
            case 4:
                gotoxy(43, 7);
                color(SKY_BLUE, BLACK);
                printf("4.전체조회");
                color(WHITE, BLACK);
                break;
            case 5:
                gotoxy(54, 7);
                color(SKY_BLUE, BLACK);
                printf("5.매출현황");
                color(WHITE, BLACK);
                break;
            case 6:
                gotoxy(65, 7);
                color(SKY_BLUE, BLACK);
                printf("6.관리자 모드 종료");
                color(WHITE, BLACK);
                break;
            case 7:
                gotoxy(84, 7);
                color(SKY_BLUE, BLACK);
                printf("7.프로그램 종료");
                color(WHITE, BLACK);
                break;
            }
        }
    }
    return cursorx;
}
// 고객 메뉴 모드
int showMainMenu(void) {

    char enteredPassword[30];
    char password[30] = "password";
    color(RED,BLACK);
    int selector, cursorx = 1;
    printf("                                         < 메뉴 >                 \n");
    color(WHITE,BLACK);
    printf("\n                       1.상영시간표 2.예매 3.예매내역 조회 4.예매취소 ");
    gotoxy(0,4);
    printf("                 < 좌우 방향키를 이용하여 선택 커서를 움직일 수 있습니다. > \n");
    printf("                                 < Enter키로 메뉴를 정하세요. >              \n");
    gotoxy(23, 2);
    color(SKY_BLUE, BLACK);
    printf("1.상영시간표");
    color(WHITE, BLACK);
    while (1) {
        if (kbhit())
        {
            color(WHITE, BLACK);
            gotoxy(23, 2);
            printf("1.상영시간표 2.예매 3.예매내역 조회 4.예매취소 ");
            selector = getch();
            if (selector == 224) { // 화살표 키일 경우
                selector = getch(); // 다음 키 입력 받기
                switch (selector)
                {
                case 77:
                    cursorx++;
                    break;
                case 75:
                    cursorx--;
                    break;
                }
            }
            else if (selector == 13)
                break;
            else if (selector == 113){
                gotoxy(23,7);
                printf("관리자 비밀번호 입력: ");
                int i = 0;
                char ch;
                while ((ch = getch()) != '\r') { // Enter(개행 문자) 키가 눌릴 때까지 문자를 읽습니다
                    if (ch == '\b') { // 백스페이스 처리
                        if (i > 0) {
                            i--;
                            printf("\b \b"); // 커서를 뒤로 이동하여 공백을 출력한 후 다시 커서를 뒤로 이동합니다
                        }
                    } else {
                        enteredPassword[i] = ch;
                        i++;
                        printf(" "); // 실제 문자 대신 별표(*)를 출력합니다
                        }
                }
                enteredPassword[i] = '\0'; // 입력된 비밀번호를 널 문자로 종료합니다

                printf("\n");
                if (strcmp(enteredPassword, password) == 0) {
                    mangerMode();
                } else {
                    printf("잘못된 비밀번호입니다.\n");
                    getch(); // 대기하기 위해 아무 키나 누를 때까지 대기
                }
                return 0;
                break;
            }
            if (cursorx < 1)
                    cursorx = 1;
                if (cursorx >= 4)
                    cursorx = 4;
            switch (cursorx)
            {
            case 1:
                gotoxy(23, 2);
                color(SKY_BLUE, BLACK);
                printf("1.상영시간표");
                color(WHITE, BLACK);
                break;
            case 2:
                gotoxy(36, 2);
                color(SKY_BLUE, BLACK);
                printf("2.예매");
                color(WHITE, BLACK);
                break;
            case 3:
                gotoxy(43, 2);
                color(SKY_BLUE, BLACK);
                printf("3.예매내역 조회");
                color(WHITE, BLACK);
                break;
            case 4:
                gotoxy(59, 2);
                color(SKY_BLUE, BLACK);
                printf("4.예매취소");
                color(WHITE, BLACK);
                break;
            }
        }
    }
    return cursorx;
}
// 글자 출력(애매자 모드에서)
void show_smu(void)
{
    printf("              #############   #                  #  ##              ##    \n");
    printf("             ##############   ##                ##  ##              ##    \n");
    printf("            ##                ###              ###  ##              ##    \n");
   printf("            ##                ## ##           ## #  ##              ##    \n");
   printf("            ##                ##  ##         ## ##  ##              ##    \n");
   printf("             ##############   ##   ##       ##  ##  ##              ##    \n");
   printf("             ##############   ##    ##     ##   ##  ##              ##    \n");
   printf("                          ##  ##     ##   ##    ##  ##              ##    \n");
   printf("                          ##  ##       ###      ##  ##              ##    \n");
   printf("                          ##  ##                ##  ##              ##    \n");
   printf("             ##############   ##                ##  ##              ##    \n");
   printf("             #############    ##                ##   ################     \n");
   printf("\n");
   printf("                  ##      ##  ##            ##  ##  ##############  ##    \n");
   printf("                 ##       ##  ##            ##  ##  ##          ##  ##    \n");
   printf("                ##        ##  ##            ##  ##  ##          ##  ##    \n");
   printf("               ##         ##  ##            ##  ##  ##          ##  ##    \n");
   printf("              ####        ##  ##      ########  ##  ##          ##  ##### \n");
   printf("             ##  ##       ##  ##            ##  ##  ##          ##  ##    \n");
   printf("            ##    ##      ##  ##            ##  ##  ##          ##  ##    \n");
   printf("           ##      ##     ##  ############  ##  ##  ##############  ##    \n");
   printf("          ##        ##    ##                ##  ##                  ##    \n");
   printf("                          ##                ##  ##                  ##    \n");
   printf("\n");
}

// 초기에 영화명 출력
void display_movieName(void)
{
    Movie* tmp;
    char prevMovieName[30] = "";
    int uniqueMovies = 0;
    int isFirstMovie = 1;

    printf("=================================================================================================================\n");
    color(DARK_SKY_BLUE,BLACK);
    printf("오늘 상영 영화: ");
    for (tmp = m_ML.head; tmp != NULL; tmp = tmp->next) {
        if (strcmp(tmp->movieName, prevMovieName) != 0) {
            if (isFirstMovie) {
                printf("%s", tmp->movieName);
                isFirstMovie = 0;
            }
            else {
                printf(", %s", tmp->movieName);
            }
            strcpy(prevMovieName, tmp->movieName);
            uniqueMovies++;
        }
    }
    color(WHITE,BLACK);
    printf("\n=================================================================================================================\n");
    printf("총 %d개의 영화가 상영중입니다.\n\n", uniqueMovies);
}
void input_movie(void) {
    Movie* tmp;
    tmp = create_node();

    printf("<< 이번달에 개봉하는 영화 정보를 입력하세요 >> \n");
    tmp->movieId = ++(m_ML.movies);
    printf("영화 고유 번호: %d\n", tmp->movieId);
    printf("영화명: ");
    scanf("%s", tmp->movieName);
    printf("상영관: ");
    scanf("%d", &(tmp->box));

    while (tmp->box > 8 || tmp->box < 1) {
        printf("1관부터 8관까지 있습니다.\n");
        printf("다시 입력해주세요:\n");
        scanf("%d", &(tmp->box));
    }

    // 영화의 시작 시간 입력
    color(RED, BLACK);
    printf("         < :을 제외하고  상영 시간을 입력해주세요 >                      \n");
    printf("                  ex) 06:00 -> 600                         \n");
    color(WHITE, BLACK);
    printf("입력:");
    scanf("%d", &(tmp->start_time));

    // 영화의 러닝 타임(상영 시간) 입력
    printf("러닝 타임: ");
    color(RED, BLACK);
    printf("         < 분으로 입력해주세요 >                      \n");
    printf("                  ex) 120분 -> 120                         \n");
    color(WHITE, BLACK);
    printf("입력: ");
    scanf("%d", &(tmp->running_time));

    tmp->prev = tmp->next = NULL;

    while (1) {
        // 동일한 영화, 시간, 상영관을 가진 영화가 이미 존재하는지 확인
        Movie* sameMovie = NULL;

        for (Movie* node = m_ML.head; node != NULL; node = node->next) {
            if (strcmp(node->movieName, tmp->movieName) == 0 && node->box == tmp->box && node->start_time == tmp->start_time) {
                sameMovie = node;
                break;
            }
        }

        if (sameMovie != NULL) {
            printf("이미 같은 영화(상영관, 시간대)가 등록되어 있습니다.\n");
            printf("상영관: ");
            scanf("%d", &(tmp->box));

            while (tmp->box > 8 || tmp->box < 1) {
                printf("1관부터 8관까지 있습니다.\n");
                printf("다시 입력해주세요:\n");
                scanf("%d", &(tmp->box));
            }

            color(RED, BLACK);
            printf("         < :을 제외하고  상영 시간을 입력해주세요 >                      \n");
            printf("                  ex) 06:00 -> 600                         \n");
            color(WHITE, BLACK);
            printf("입력:");
            scanf("%d", &(tmp->start_time));



            printf("러닝 타임: ");
            color(RED, BLACK);
            printf("         < 분으로 입력해주세요 >                      \n");
            printf("                  ex) 120분 -> 120                         \n");
            color(WHITE, BLACK);
            printf("입력: ");
            scanf("%d", &(tmp->running_time));

        }else{
            break;
        }
    }

    if (m_ML.tail == NULL) {
        m_ML.head = m_ML.tail = tmp;
    } else {
    // 같은 영화를 찾아서 해당 영화의 맨 뒤에 새로운 영화를 추가

    Movie* lastSameMovie = NULL;
    for (Movie* node = m_ML.head; node != NULL; node = node->next) {
        if (strcmp(node->movieName, tmp->movieName) == 0) {
            lastSameMovie = node;
        }
    }

    if (lastSameMovie != NULL) {
        // 같은 영화를 찾은 경우, 해당 영화의 맨 뒤에 새로운 영화를 추가
        tmp->prev = lastSameMovie;
        tmp->next = lastSameMovie->next;
        if (lastSameMovie->next != NULL)
            lastSameMovie->next->prev = tmp;
        lastSameMovie->next = tmp;
    } else {
        // 같은 영화를 찾지 못한 경우, 연결 리스트의 맨 뒤에 새로운 영화를 추가
        tmp->prev = m_ML.tail;
        m_ML.tail->next = tmp;
        m_ML.tail = tmp;
        }
    }
    int box = tmp->box - 1; // 상영관 번호를 배열 인덱스로 변환

            // 시간을 char 형식으로 변환하여 저장
            char timeChar[5]; // 시간을 저장할 char 배열
            sprintf(timeChar, "%d", tmp->start_time);

            // 시간을 char 형식으로 저장
            strcpy(screenTimes[box][screenCount[box]], timeChar);
            screenCount[box]++;

    printf("영화 정보가 등록되었습니다.\n");

    color(RED, BLACK);
    printf("\n\n처음 화면으로 돌아가려면 아무키나 누르세요.");
    color(WHITE, BLACK);
    getch();
}

// 영화 조회
void search_movie(void)
{
    char name[30];
    Movie* tmp;
    int flag;
    display_movieName();
    fflush(stdin);
    printf("\n<영화 조회>");
    printf("영화명: ");
    gets(name);
    flag = 0;
    for (tmp = m_ML.head; tmp != NULL; tmp = tmp->next)
        if (strstr(tmp->movieName, name)) {
            printf("현재 상영중인 영화 목록입니다.\n");
             printf("=================================================================================================================\n");
            printf("☞ 영화 번호: %d        영화명: %s           상영관: %d관", tmp->movieId, tmp->movieName, tmp->box);
            int hour = tmp->start_time / 100;
            int minute = tmp->start_time % 100;
            printf("        시작시간: %02d시:%02d분", hour, minute);
            printf("        러닝타임: %d분\n", tmp->running_time);
            printf("\n=================================================================================================================\n\n");;
            flag = 1;
        }
    if (flag == 0)
        printf("등록된 영화가 아닙니다.\n\n");
    color(RED, BLACK);
    printf("\n\n처음 화면으로 돌아가려면 아무키나 누르세요.");
    color(WHITE, BLACK);
    getch();
}
// 영화 삭제
void remove_movie(void)
{
    int id, flag;
    Movie *tmp;
    int cnt = 0;
    display_movie();
    fflush(stdin);
    color(RED, BLACK);
    printf("\n삭제 고유번호: ");
    color(WHITE, BLACK);
    scanf("%d", &id);

    flag = 0;
    for (tmp = m_ML.head; tmp != NULL; tmp = tmp->next) {
        if (tmp->movieId == id) {

            // 영화를 예매한 회원들을 확인하여 예매가 존재하는 경우 삭제 중지
            int reservationExist = 0;
            for (Member* member = a_ML.head; member != NULL; member = member->next) {
                if (strcmp(member->m.movieName, tmp->movieName) == 0 && member->m.start_time == tmp->start_time && member->m.box == tmp->box) {
                    reservationExist = 1;
                    break;
                }
            }

            if (reservationExist) {
                color(RED, BLACK);
                printf("이미 예매자가 있어서 삭제할 수 없습니다.\n\n");
                printf("\n\n처음 화면으로 돌아가려면 아무키나 누르세요.");
                color(WHITE, BLACK);
                getch();
                return;
            }

            if (tmp->prev == NULL) { // 첫 영화인 경우
                m_ML.head = tmp->next;

                if (tmp->next != NULL)
                    tmp->next->prev = NULL;
            } else {
                tmp->prev->next = tmp->next;
                if (tmp->next != NULL) // 마지막 영화를 삭제할 경우
                    tmp->next->prev = tmp->prev;
            }
            if (tmp == m_ML.tail)
                m_ML.tail = tmp->prev;
            m_ML.movies--;
            flag = 1;
            break;
        }
        cnt++;
    }
    if (flag == 1) {
        printf("해당 영화를 삭제하였습니다.\n\n");

        int i;
        for (i = cnt; i < MAX_THEATER; i++) {
            if (cnt == MAX_THEATER - 1) {
                mvpeople[i].adult = 0;
                mvpeople[i].children = 0;
                mvpeople[i].teenager = 0;
                mvpeople[i].total_people = 0;
            } else
                mvpeople[i] = mvpeople[i + 1];
        }
        int box = tmp->box - 1; // 상영관 번호를 배열 인덱스로 변환
        // 시간을 char 형식으로 저장
        strcpy(screenTimes[box][screenCount[box]], "");
        screenCount[box]--;
        free(tmp);
    } else {
        printf("등록된 영화의 번호가 아닙니다.\n\n");
    }

    printf("\n\n처음 화면으로 돌아가려면 아무 키나 누르세요.");
    color(WHITE, BLACK);
    getch();
}
// 영화목록 출력 (관별로)
void display_movie(void)
{
    Movie* tmp;
    int prevBox = 0; // 초기값 0으로 설정

    printf("\n총 영화 수: %d\n", m_ML.movies);
    for (int box = 1; box <= 8; box++) { // 1관부터 8관까지 순서대로 출력
        for (tmp = m_ML.head; tmp != NULL; tmp = tmp->next) {
            if (tmp->box == box) { // 현재 상영관과 일치하는 영화만 출력
                if (tmp->box != prevBox) {
                    printf("=====================( 상영관 %d )=====================\n", tmp->box);
                    prevBox = tmp->box;
                }

                printf("<<고유번호>>: %-10d\n", tmp->movieId);
                printf("영화명: %-30s\n", tmp->movieName);
                int hour = tmp->start_time / 100;
                int minute = tmp->start_time % 100;
                printf("시작시간: %02d시%02d분\n", hour, minute);
                printf("러닝타임: %d분\n", tmp->running_time);
            }
        }
    }

}
void display_total(void)
{
    Movie* tmp;
    char prevMovieName[30] = "";
    int uniqueMovies = 0;

    for (tmp = m_ML.head; tmp != NULL; tmp = tmp->next) {
        if (strcmp(tmp->movieName, prevMovieName) != 0) {
            display_movie_by_screen(tmp->movieName);
            strcpy(prevMovieName, tmp->movieName);
            uniqueMovies++;
        }
    }
     color(DARK_SKY_BLUE,BLACK);
    printf("\n\n총 %d개의 영화가 상영중입니다.\n", uniqueMovies);
    color(WHITE,BLACK);

}
void total_money(void){
    printf("총 매출액: %d\n",a_ML.managepay.amount);
    printf("총 할인금액: %d\n",a_ML.managepay.disamount);
    printf("실매출액: %d\n",a_ML.managepay.totalpayment);

}
//관의 크기 지정
void Theater_init(void)
{

    int Row[MAX_THEATER]={8,7,9,5,9,6};
    int Col[MAX_THEATER]={9,8,9,7,7,8};
    for (int i = 0; i < MAX_THEATER; i++) {
        theaters[i].rows = Row[i]; // 관의 행 수 설정
        theaters[i].cols = Col[i]; // 관의 열 수 설정

        for (int j = 0; j < MAX_TIME; j++) {
            theaters[i].timeTable[j].seats = (Seat**)malloc(Row[i] * sizeof(Seat*)); // 시간대별로 좌석 배열 동적 할당
            theaters[i].timeTable[j].remainseat=theaters[i].rows*theaters[i].cols;
            for (int k = 0; k < Row[i]; k++) {
                theaters[i].timeTable[j].seats[k] = (Seat*)malloc(Col[i] * sizeof(Seat)); // 각 행별로 좌석 배열 동적 할당

                for (int l = 0; l < Col[i]; l++) {
                    theaters[i].timeTable[j].seats[k][l].status = 0; // 좌석 상태 초기화 (0: 빈 좌석)
                }
            }
        }
    }
}
void display_movie_by_screen(char* movieName)
{
    color(DARK_SKY_BLUE,BLACK);
    printf("\n\"%s\" 영화의 상영관별 정보:\n", movieName);
    color(WHITE,BLACK);
    char tempScreenTimes[MAX_THEATER][MAX_TIME][6]; // 임시 배열을 생성하여 문자열로 변환된 시간 정보를 저장
    int tempScreenCount[MAX_THEATER] = {0}; // 임시 배열을 생성하여 복사할 시간 정보의 개수를 저장
    // 영화와 일치하는 상영 정보를 찾아서 임시 배열에 저장
    for (Movie* tmp = m_ML.head; tmp != NULL; tmp = tmp->next) {
        if (strcmp(tmp->movieName, movieName) == 0) {
            int box = tmp->box - 1; // 상영관 번호를 배열 인덱스로 변환
            int timeIndex = tempScreenCount[box]; // 현재 상영관에서의 시간대 인덱스
            int time = tmp->start_time;
            int hour = time / 100;
            int minute = time % 100;
            sprintf(tempScreenTimes[box][timeIndex], "%02d:%02d", hour, minute);

            tempScreenCount[box]++;
        }
    }

    // 관별로 시간대를 정렬
    for (int box = 0; box < MAX_THEATER; box++) {
        if (tempScreenCount[box] > 0) {
            char (*timeArray)[6] = tempScreenTimes[box];
            int count = tempScreenCount[box];

            // 삽입 정렬을 사용하여 시간대를 정렬합니다.
            for (int i = 1; i < count; i++) {
                char key[6];
                strcpy(key, timeArray[i]);
                int j = i - 1;
                while (j >= 0 && strcmp(timeArray[j], key) > 0) {
                    strcpy(timeArray[j + 1], timeArray[j]);
                    j--;
                }
                strcpy(timeArray[j + 1], key);
            }
        }
    }

    // 정렬된 시간대를 출력
    for (int box = 0; box < MAX_THEATER; box++) {
        if (tempScreenCount[box] > 0) {
            printf("===================================================( 상영관: %d )=================================================\n", box + 1);
            printf("상영시간: ");
            for (int i = 0; i < tempScreenCount[box]; i++) {
                int hour,minute,index;
                sscanf(tempScreenTimes[box][i], "%d:%d", &hour, &minute); // 문자열을 int로 변환하여 hour와 minute 변수에 저
                int inttime = hour * 100 + minute;
                index=findTimeIndex(box,inttime);
                printf("%s ", tempScreenTimes[box][i]);
                color(RED,BLACK);
                printf("(%d/%d)  ",theaters[box].timeTable[index].remainseat,theaters[box].rows*theaters[box].cols);
                color(WHITE,BLACK);
            }
            printf("\n");
        }
    }
}

void display_movie_theater(char* movieName, int box) {
    printf("===================================================( 상영관: %d )=================================================\n", box); // 상영관 번호 출력 시 +1을 해줍니다.

    int found = 0;

    // 상영 정보와 관련된 영화를 찾아서 시간대를 저장
    char tempScreenTimes[MAX_TIME][6]; // 임시 배열을 생성하여 문자열로 변환된 시간 정보를 저장
    int tempScreenCount = 0; // 복사할 시간 정보의 개수를 저장

    for (Movie* tmp = m_ML.head; tmp != NULL; tmp = tmp->next) {
        if (strcmp(tmp->movieName, movieName) == 0 && tmp->box == box) { // box 값에 +1을 해줍니다.
            found = 1;
            int time = tmp->start_time;
            int hour = time / 100;
            int minute = time % 100;
            sprintf(tempScreenTimes[tempScreenCount], "%02d:%02d", hour, minute);
            tempScreenCount++;
        }
    }

    if (found) {
        // 시간대를 정렬
        for (int i = 1; i < tempScreenCount; i++) {
            char key[6];
            strcpy(key, tempScreenTimes[i]);
            int j = i - 1;
            while (j >= 0 && strcmp(tempScreenTimes[j], key) > 0) {
                strcpy(tempScreenTimes[j + 1], tempScreenTimes[j]);
                j--;
            }
            strcpy(tempScreenTimes[j + 1], key);
        }

        // 정렬된 시간대를 출력
        printf("상영시간: ");
        for (int i = 0; i < tempScreenCount; i++) {
                int hour,minute,index;
                sscanf(tempScreenTimes[i], "%d:%d", &hour, &minute); // 문자열을 int로 변환하여 hour와 minute 변수에 저
                int inttime = hour * 100 + minute;
                index=findTimeIndex(box-1,inttime);
             printf("%s ", tempScreenTimes[i]);
            color(RED,BLACK);
            printf("(%d/%d)  ",theaters[box-1].timeTable[index].remainseat,theaters[box-1].rows*theaters[box-1].cols);
            color(WHITE,BLACK);
        }
        printf("\n");
    } else {
        printf("해당 영화는 상영되지 않거나 상영관이 올바르지 않습니다.\n");
    }

    printf("================================================================================================================\n\n");
}


// SCREEN 글자 출력
void print_screen(void)
{
    printf("              #####   #####  #####  ###### ###### #     #   \n");
   printf("             #       #     # #    # #      #      ##    #    \n");
   printf("             #       #       #    # #      #      # #   #    \n");
   printf("              #####  #       #####  ###### ###### #  #  #    \n");
   printf("                   # #       #    # #      #      #   # #    \n");
   printf("                   # #     # #    # #      #      #    ##    \n");
   printf("              #####   #####  #    # ###### ###### #     #    \n");
}

// 좌석 예약 하기(확인, 중복 체크)
int reserveSeat(int theaternum, int timeIndex, int cursorX, int cursorY,Member* tmp)
{
    char seat[3];

    if(theaters[theaternum].timeTable[timeIndex].seats[cursorY][cursorX].status == 0)
    {
        theaters[theaternum].timeTable[timeIndex].seats[cursorY][cursorX].status = 1;
        // 좌석 정보를 예매자의 reserve_seat에 추가
        sprintf(seat, "%c%d", 'A' + cursorY, cursorX + 1);
        strcat(tmp->reserve_seat, seat);
        strcat(tmp->reserve_seat, " ");
        printf("%c%d 좌석이 예약되었습니다.\n",'A'+cursorY,cursorX+1);
        return 1;
    }
    else {
        printf("이미 예약된 좌석입니다\n");
        return 0;
    }
}

//좌석 배치도를 출력하는 함수
void showSeatMap(int theaternum, int timeIndex, Member *tmp)
{
    system("cls");
    print_screen();
    int row = theaters[theaternum].rows;
    int col = theaters[theaternum].cols;
    gotoxy(60, 13);
    color(BLACK,SKY_BLUE);
    printf("■");
    color(WHITE, BLACK);
   printf(" : 선택");
   gotoxy(60, 14);
    color(BLACK,BLUE);
    printf("■");
    color(WHITE, BLACK);
   printf(" : 선택완료");
   gotoxy(60, 15);
   color(BLACK,WHITE);
   printf("■");
    color(WHITE, BLACK);
   printf(" : 예매가능");
   gotoxy(60, 16);
   color(BLACK,RED);
   printf("■");
   color(WHITE, BLACK);
   printf(" : 예매불가");
   gotoxy(30,7);
   printf("\n");

    int cursorX = 0; // 현재 커서의 가로 위치
    int cursorY = 0; // 현재 커서의 세로 위치
    int currentnum = 0;

    while (currentnum < tmp->pn.total_people) {
        gotoxy(0, 9); // 좌석 배치도 시작 위치로 커서 이동

        for (int i = 0; i < row; i++) {
            printf("\t\t%c  ", 'A' + i);

            for (int j = 0; j < col; j++) {
                printf(" ");

                if (cursorX == j && cursorY == i) {
                    color(BLACK, SKY_BLUE);
                    printf("%c%d", 'A' + i, j + 1);
                    color(WHITE, BLACK);
                } else if (theaters[theaternum].timeTable[timeIndex].seats[i][j].status == 0) {
                    color(BLACK, WHITE);
                    printf("%c%d", 'A' + i, j + 1);
                    color(WHITE, BLACK);
                } else if (theaters[theaternum].timeTable[timeIndex].seats[i][j].status == 1){
                    color(BLACK, DARK_BLUE);
                    printf("%c%d", 'A' + i, j + 1);
                    color(WHITE, BLACK);
                } else {
                    color(BLACK, RED);
                    printf("%c%d", 'A' + i, j + 1);
                    color(WHITE, BLACK);
                }
            }

            printf("\n\n");
        }

        // 키 입력 처리
        int key = getch(); // 키 입력 받기

        if (key == 224) { // 화살표 키일 경우
            key = getch(); // 다음 키 입력 받기

            switch (key) {
                case 72: // 위쪽 화살표 키
                    if (cursorY > 0)
                        cursorY--;
                    break;
                case 80: // 아래쪽 화살표 키
                    if (cursorY < row - 1)
                        cursorY++;
                    break;
                case 75: // 왼쪽 화살표 키
                    if (cursorX > 0)
                        cursorX--;
                    break;
                case 77: // 오른쪽 화살표 키
                    if (cursorX < col - 1)
                        cursorX++;
                    break;
            }
        } else if (key == 13) { // 엔터 키일 경우 (좌석 선택)
             currentnum+=reserveSeat(theaternum, timeIndex, cursorX, cursorY,tmp);
        }
    }

     printf("총 예매한 좌석: %s\n", tmp->reserve_seat);
}
int findTimeIndex(int box, int time)
{
    char time_str[5];
    sprintf(time_str, "%d", time);
    for (int i = 0; i < screenCount[box ]; i++) {

            if (strcmp(screenTimes[box ][i], time_str) == 0) {
                return i;
            }
    }
    return -1; // 시간이 없을 경우 -1 반환
}

//성인 수
 int num_adult(void)
{
    int selector;
    int cursorx = 0;
    gotoxy(28, 8);
    color(SKY_BLUE, BLACK);
    printf("0");
    color(WHITE, BLACK);
    while (1) {
        if (kbhit())
        {
            color(WHITE, BLACK);
            gotoxy(28, 8);
            printf("0 1 2 3 4 5 6 7 8 ");
            selector = getch();
            if (selector == 224) { // 화살표 키일 경우
                selector = _getch(); // 다음 키 입력 받기
                switch (selector)
                {
                case 77:
                    cursorx++;
                    break;
                case 75:
                    cursorx--;
                    break;
                }
            }
            else if (selector == 13)
                break;
            if (cursorx <= 0)
                cursorx = 0;
            if (cursorx >= 8)
                cursorx = 8;
            switch (cursorx)
            {
            case 0:
                 gotoxy(28, 8);
                 color(SKY_BLUE, BLACK);
                 printf("0");
                 color(WHITE, BLACK);
                 break;
            case 1:
                gotoxy(30, 8);
                color(SKY_BLUE, BLACK);
                printf("１");
                color(WHITE, BLACK);
                break;
            case 2:
                gotoxy(32, 8);
                color(SKY_BLUE, BLACK);
                printf("2");
                color(WHITE, BLACK);
                break;
            case 3:
                gotoxy(34, 8);
                color(SKY_BLUE, BLACK);
                printf("3");
                color(WHITE, BLACK);
                break;
            case 4:
                gotoxy(36, 8);
                color(SKY_BLUE, BLACK);
                printf("4");
                color(WHITE, BLACK);
                break;
            case 5:
                gotoxy(38, 8);
                color(SKY_BLUE, BLACK);
                printf("5");
                color(WHITE, BLACK);
                break;
            case 6:
                gotoxy(40, 8);
                color(SKY_BLUE, BLACK);
                printf("6");
                color(WHITE, BLACK);
                break;
            case 7:
                gotoxy(42, 8);
                color(SKY_BLUE, BLACK);
                printf("7");
                color(WHITE, BLACK);
                break;
            case 8:
                gotoxy(44, 8);
                color(SKY_BLUE, BLACK);
                printf("8");
                color(WHITE, BLACK);
                break;
            }
        }
    }
    gotoxy(30 + cursorx * 2 - 2, 8);
    color(RED, BLACK);
    printf("%d", cursorx);
    color(WHITE, BLACK);
    return cursorx;
}

//청소년 수
int num_teenager(void)
{
    int selector;
    int cursorx = 0;
    gotoxy(28, 10);
    color(SKY_BLUE, BLACK);
    printf("0");
    color(WHITE, BLACK);
    while (1) {
        if (kbhit())
        {
            color(WHITE, BLACK);
            gotoxy(28, 10);
            printf("0 1 2 3 4 5 6 7 8 ");
            selector = getch();
            if (selector == 224) { // 화살표 키일 경우
                selector = _getch(); // 다음 키 입력 받기
                switch (selector)
                {
                case 77:
                    cursorx++;
                    break;
                case 75:
                    cursorx--;
                    break;
                }
            }
            else if (selector == 13)
                break;
            if (cursorx <= 0)
                cursorx = 0;
            if (cursorx >= 8)
                cursorx = 8;
            switch (cursorx)
            {
            case 0:
                 gotoxy(28, 10);
                 color(SKY_BLUE, BLACK);
                 printf("0");
                 color(WHITE, BLACK);
                 break;
            case 1:
                gotoxy(30, 10);
                color(SKY_BLUE, BLACK);
                printf("１");
                color(WHITE, BLACK);
                break;
            case 2:
                gotoxy(32, 10);
                color(SKY_BLUE, BLACK);
                printf("2");
                color(WHITE, BLACK);
                break;
            case 3:
                gotoxy(34, 10);
                color(SKY_BLUE, BLACK);
                printf("3");
                color(WHITE, BLACK);
                break;
            case 4:
                gotoxy(36, 10);
                color(SKY_BLUE, BLACK);
                printf("4");
                color(WHITE, BLACK);
                break;
            case 5:
                gotoxy(38, 10);
                color(SKY_BLUE, BLACK);
                printf("5");
                color(WHITE, BLACK);
                break;
            case 6:
                gotoxy(40, 10);
                color(SKY_BLUE, BLACK);
                printf("6");
                color(WHITE, BLACK);
                break;
            case 7:
                gotoxy(42, 10);
                color(SKY_BLUE, BLACK);
                printf("7");
                color(WHITE, BLACK);
                break;
            case 8:
                gotoxy(44, 10);
                color(SKY_BLUE, BLACK);
                printf("8");
                color(WHITE, BLACK);
                break;
            }
        }
    }
    gotoxy(30 + cursorx * 2 - 2, 10);
    color(RED, BLACK);
    printf("%d", cursorx);
    color(WHITE, BLACK);
    return cursorx;
}

//어린이 수
int num_children(void)
 {
     int selector;
     int cursorx = 0;
     gotoxy(28, 12);
     color(SKY_BLUE, BLACK);
     printf("0");
     color(WHITE, BLACK);;

     while (1) {
         if (kbhit())
         {
             color(WHITE, BLACK);
             gotoxy(28, 12);
             printf("0 1 2 3 4 5 6 7 8 ");
             selector = getch();
             if (selector == 224) { // 화살표 키일 경우
                 selector = getch(); // 다음 키 입력 받기
                 switch (selector)
                 {
                 case 77:
                     cursorx++;
                     break;
                 case 75:
                     cursorx--;
                     break;
                 }
             }
             else if (selector == 13)
                 break;
             if (cursorx <= 0)
                 cursorx = 0;
             if (cursorx >= 8)
                 cursorx = 8;
             switch (cursorx)
             {
             case 0:
                 gotoxy(28, 12);
                 color(SKY_BLUE, BLACK);
                 printf("0");
                 color(WHITE, BLACK);
                 break;
             case 1:
                 gotoxy(30, 12);
                 color(SKY_BLUE, BLACK);
                 printf("１");
                 color(WHITE, BLACK);
                 break;
             case 2:
                 gotoxy(32, 12);
                 color(SKY_BLUE, BLACK);
                 printf("2");
                 color(WHITE, BLACK);
                 break;
             case 3:
                 gotoxy(34, 12);
                 color(SKY_BLUE, BLACK);
                 printf("3");
                 color(WHITE, BLACK);
                 break;
             case 4:
                 gotoxy(36, 12);
                 color(SKY_BLUE, BLACK);
                 printf("4");
                 color(WHITE, BLACK);
                 break;
             case 5:
                 gotoxy(38, 12);
                 color(SKY_BLUE, BLACK);
                 printf("5");
                 color(WHITE, BLACK);
                 break;
             case 6:
                 gotoxy(40, 12);
                 color(SKY_BLUE, BLACK);
                 printf("6");
                 color(WHITE, BLACK);
                 break;
             case 7:
                 gotoxy(42, 12);
                 color(SKY_BLUE, BLACK);
                 printf("7");
                 color(WHITE, BLACK);
                 break;
             case 8:
                 gotoxy(44, 12);
                 color(SKY_BLUE, BLACK);
                 printf("8");
                 color(WHITE, BLACK);
                 break;
             }
         }
     }
     gotoxy(30 + cursorx * 2 - 2, 12);
     color(RED, BLACK);
     printf("%d", cursorx);
     color(WHITE, BLACK);
     return cursorx;
 }


// 예매자 등록
void input_member(Member *tmp)
{
    int inputflag=1;
    printf("\n<예매회원 등록>\n");
    tmp->personId = ++(a_ML.LastperosonId);
    printf("예매번호(자동부여): %d\n", tmp->personId);
    tmp->pn.total_people = 0;
    printf("이름 입력: ");
    scanf("%s", tmp->name);
    while(inputflag){
        gotoxy(20,8);
        printf("성인");
        gotoxy(28, 8);
        printf("0 1 2 3 4 5 6 7 8 ");
        gotoxy(60,8);
        color(BLACK,WHITE);
        printf("  성인: 13,000원 ");
        color(WHITE, BLACK);
        gotoxy(20,10);
        printf("청소년");
        gotoxy(28, 10);
        printf("0 1 2 3 4 5 6 7 8 ");
        gotoxy(60,10);
        color(BLACK,WHITE);
        printf("청소년: 10,000원 ");
        color(WHITE, BLACK);
        gotoxy(20, 12);
        printf("어린이");
        gotoxy(28, 12);
        printf("0 1 2 3 4 5 6 7 8 ");
        gotoxy(60,12);
        color(BLACK,WHITE);
        printf("어린이: 7,000원  ");
        color(WHITE, BLACK);
        gotoxy(5, 5);
        printf("            < 좌우 방향키를 이용하여 선택 커서를 움직일 수 있습니다. > \n");
        gotoxy(28, 7);
        printf("←");
        gotoxy(45, 7);
        printf("→");
        gotoxy(0, 15);
        printf("                          < Enter키로 인원을 정하세요. >              \n");
        printf("               < 시간을 다시 선택하려면 Backspace키를 눌러주세요. >   \n");

        tmp->pn.adult=num_adult();
        tmp->pn.teenager = num_teenager();
        tmp->pn.children=num_children();
        tmp->pn.total_people =  tmp->pn.adult + tmp->pn.children + tmp->pn.teenager;
        int index=findTimeIndex(tmp->m.box-1,tmp->m.start_time);
        if(tmp->pn.total_people>theaters[tmp->m.box-1].timeTable[index].remainseat){
                gotoxy(0, 14);
            color(RED,BLACK);
            printf("                                남은 좌석은 %d입니다.",theaters[tmp->m.box-1].timeTable[index].remainseat);
            color(WHITE,BLACK);
            getchar();
        }
        else if(tmp->pn.total_people == 0){
            gotoxy(0, 14);
            color(RED,BLACK);
            printf("                          <최소 1명은 예매하셔야합니다.>                ");
            color(WHITE,BLACK);
            getchar();
        }
        else if(tmp->pn.total_people>10){
            gotoxy(0, 14);
            color(RED,BLACK);
            printf("                             <최대 10명까지 가능합니다.>                ");
            color(WHITE,BLACK);
            getchar();
        }
        else
            inputflag=0;
    }


    tmp->prev = tmp->next = NULL;

    if (a_ML.tail == NULL) {
        a_ML.head = a_ML.tail = tmp;
    } else {
        tmp->prev = a_ML.tail;
        a_ML.tail->next =tmp;
        a_ML.tail = tmp;
    }

    a_ML.members++;

}

// 예매 취소
void refund_member(int id)
{
    Member *tmp;
    int flag;


    flag = 0;

    for (tmp = a_ML.head; tmp != NULL; tmp = tmp->next) {


        if (tmp->personId == id) {
        char seats_copy[25];
        strcpy(seats_copy, tmp->reserve_seat);
        char* seat_token = strtok(seats_copy, " ");

        int memtimeIndex = findTimeIndex(tmp->m.box - 1, tmp->m.start_time);

        while (seat_token != NULL) {
            // 좌석 정보에서 행과 열 값을 추출합니다
            int row = seat_token[0] - 'A';
            int col = atoi(&seat_token[1])-1;
            // 해당 좌석을 처리하는 코드를 작성합니다
            theaters[tmp->m.box - 1].timeTable[memtimeIndex].seats[row][col].status = 0;
            theaters[tmp->m.box - 1].timeTable[memtimeIndex].remainseat++;
            // 다음 좌석 정보를 가져옵니다
            seat_token = strtok(NULL, " ");
        }

            if (tmp->prev == NULL){ // 첫회원인 경우
                a_ML.head = tmp->next;

                if(tmp->next != NULL)
                    tmp->next->prev = NULL;

            } else {
                tmp->prev->next = tmp->next;
                if (tmp->next != NULL) // 마지막 회원을 삭제할 경우
                    tmp->next->prev = tmp->prev;
            }
            if(tmp == a_ML.tail)
                a_ML.tail = tmp->prev;
            a_ML.managepay.amount-=tmp->pay.amount;
            a_ML.managepay.disamount-=tmp->pay.disamount;
            a_ML.managepay.totalpayment-=tmp->pay.totalpayment;

            int cnt = 0;
            for (Movie* temp = m_ML.head; temp != NULL; temp = temp->next) {
                if (strcmp(tmp->m.movieName, temp->movieName) == 0) {
                    mvpeople[cnt].adult -= tmp->pn.adult;
                    mvpeople[cnt].teenager -= tmp->pn.teenager;
                    mvpeople[cnt].children -= tmp->pn.children;
                    mvpeople[cnt].total_people -= tmp->pn.total_people;
            break;
        }
        cnt++;
    }
            free(tmp);
            a_ML.members--;
            flag = 1;
            break;

        }
    }
    if (flag == 1){
        printf("예매가 취소 되었습니다.\n\n");
    }
    else
        printf("등록된 예매 번호가 아닙니다.\n\n");

}
// 총 결제 금액
void calculatePayment(Member* member, int isSMU)
{
    // 할인 적용 (SMU 회원인 경우)
    if (isSMU) {

        member->pay.disamount = member->pay.amount * 0.2;     //20%할인하는 금액
        member->pay.totalpayment = member->pay.amount * 0.8;  //20%할인된 금액

    }else{
        member->pay.totalpayment=member->pay.amount;
    }

}

//smu 학생 불러와서 확인(할인)
void read_smu(Member *tmp)
{
    FILE* fp = fopen("member.txt", "r"); // smu 회원 파일명

    if (fp == NULL) {
        printf("기존 파일이 없습니다.\n");
        return;
    }

    char choice;
    char number[10];
    char name[30];
    char smuNumber[10];
    int smuFound = 0; // SMU 회원 여부를 저장하는 변수
    printf("smu회원이시면 y 아니시면 n을 눌러주세요\n");

    while(choice!='y'&&choice!='n'){
        choice=getch();
    }

    if(choice=='y')
    {
        printf("smu확인을 위해 학번을 입력하세요: ");

        scanf("%s", smuNumber);
        while (fscanf(fp, "%s %s", number, name) != EOF) {
            if (strcmp(smuNumber, number) == 0) {
                fclose(fp);
                printf("smu 회원임을 확인하였습니다.\n");
                smuFound = 1;
                break;
            }
        }
    }
    else
        printf("smu회원이 아니시면 할인은 불가능합니다.");
    if (smuFound==0&&choice=='y')
                printf("등록된 smu회원이 아닙니다.\n");
    calculatePayment(tmp, smuFound); // SMU 회원이므로 할인 적용
    fclose(fp);

}
// 영화 예매 단계(뒤로가기)
void MovieReservation(void)
{
    int choice;
    int currentStep = 1;
    int previousStep = 0;
    int timeIndex = 0;
    int row = 0;
    int col = 0;
    int date;
    Member* tmp;
    tmp = create_node1();
    tmp->pay.amount=0;
    tmp->pay.disamount=0;
    tmp->pay.totalpayment=0;
    memset(tmp->reserve_seat, 0, sizeof(tmp->reserve_seat));
    while (currentStep <= 7) {
        switch (currentStep) {
            case 1:
                fflush(stdin);
                display_movieName();
                printf("영화명을 입력해주세요: ");
                scanf("%s", tmp->m.movieName);

                // 영화명 비교하여 등록된 영화인지 확인
                int movieRegistered = 0;  // 영화 등록 여부 플래그
                for (Movie* temp = m_ML.head; temp != NULL; temp = temp->next) {
                    if (strcmp(temp->movieName, tmp->m.movieName) == 0) {
                        movieRegistered = 1;
                        break;
                    }
                }

                if (!movieRegistered) {
                    color(RED, BLACK);
                    printf("\n\n 등록되지 않은 영화입니다. 초기화면으로 돌아갑니다");
                    color(WHITE, BLACK);
                    getch();
                    return;
                }
                break;

            case 2:
               display_movie_by_screen(tmp->m.movieName);

                    printf("\n영화 시간표를 확인하고 상영관을 입력해주세요: ");
                    scanf("%d", &tmp->m.box);

                    int boxRegistered = 0;  // 상영관 등록 여부 플래그
                    for (Movie* temp = m_ML.head; temp != NULL; temp = temp->next) {
                        if (strcmp(temp->movieName, tmp->m.movieName) == 0) {
                            if (temp->box == tmp->m.box) {
                            boxRegistered = 1;
                            break;
                            }
                        }
                    }

                    if (!boxRegistered) {
                        color(RED, BLACK);
                        printf("\n\n등록된 상영관이 아닙니다. 초기화면으로 돌아갑니다\n");
                        color(WHITE, BLACK);
                        getch();
                        return;
                    } else {
                        break;
                    }

                break;
            case 3:
                display_movie_theater(tmp->m.movieName, tmp->m.box);


                    color(RED, BLACK);
                    printf("         < :을 제외하고  시간을 입력해주세요 >                      \n");
                    printf("                  ex) 06:00 -> 600                         \n");
                    color(WHITE, BLACK);

                    printf("입력: ");
                    scanf("%d", &tmp->m.start_time);

                    timeIndex = findTimeIndex(tmp->m.box - 1, tmp->m.start_time);
                    if (timeIndex == -1) {
                        color(RED, BLACK);
                        printf("\n\n선택한 시간이 영화 시간표에 없습니다. 초기화면으로 돌아갑니다\n");
                        color(WHITE, BLACK);
                        getch();
                        return;
                    } else {
                        row = theaters[tmp->m.box - 1].rows;
                        col = theaters[tmp->m.box - 1].cols;
                    }
                    if(theaters[tmp->m.box-1].timeTable[timeIndex].remainseat==0){
                        color(RED, BLACK);
                        printf("\n\n남은 좌석이 없습니다. 초기화면으로 돌아갑니다\n");
                        color(WHITE, BLACK);
                        getch();
                        return;
                    }
                break;

            case 4:
                printf("휴대폰 번호를 입력하세요: ");
                scanf("%s", tmp->phone);
                break;
            case 5:
                input_member(tmp);
                break;
            case 6:

                showSeatMap(tmp->m.box - 1, timeIndex, tmp);
                break;
            case 7:
                read_smu(tmp); // smu회원임을 확인하면 10%할인
                break;
        }
        gotoxy(75,14);
        color(BLUE, BLACK);
        printf("<<다음 단계:   Enter   >>");
        gotoxy(75,15);
        color(RED, BLACK);
        printf("<<이전 단계: BackSpace >>");
        color(WHITE, BLACK);

        while(1){
            choice = getch();

            if (choice == 13) {
                previousStep = currentStep;
                currentStep++;
                break;
            } else if (choice == 8) {
                if (currentStep > 1) {
                    currentStep = previousStep;
                    pop(); // 이전 단계로 돌아가면서 스택에서 pop
                    for (int i = 0; i < row; i++) {
                        for (int j = 0; j < col; j++) {
                            if (theaters[tmp->m.box-1].timeTable[timeIndex].seats[i][j].status == 1) {
                                theaters[tmp->m.box-1].timeTable[timeIndex].seats[i][j].status = 0;  // 이전에 선택한 좌석을 다시 선택 가능하도록 변경
                            }
                        }
                    }
                    tmp->pay.amount=0;
                    tmp->pay.disamount=0;
                    tmp->pay.totalpayment=0;
                    strcpy(tmp->reserve_seat, ""); // reserve_seat를 빈 문자열로 초기화
                    break;
                } else {
                    printf("더 이상 뒤로 갈 수 없습니다.\n");
                }
            }
        }

        if (currentStep <= 7 && currentStep > previousStep) {
            Member* newNode = (Member*)malloc(sizeof(Member));
            memcpy(newNode, tmp, sizeof(Member));
            push(newNode); // 현재 단계에서 스택에 push
        }
        tmp->prev = tmp->next = NULL;

        system("cls");
    }
     for (int i = 0; i < row; i++) {
                    for (int j = 0; j < col; j++) {
                        if ( theaters[tmp->m.box - 1].timeTable[timeIndex].seats[i][j].status == 1) {
                            theaters[tmp->m.box - 1].timeTable[timeIndex].seats[i][j].status = 2;
                            theaters[tmp->m.box - 1].timeTable[timeIndex].remainseat--;
                        }
                    }
            }

    a_ML.managepay.amount+=tmp->pay.amount;
    a_ML.managepay.disamount+=tmp->pay.disamount;
    a_ML.managepay.totalpayment+=tmp->pay.totalpayment;
    display_reservation(tmp);
}

void display_reservation(Member *tmp)
{
    color(SKY_BLUE, BLACK);
    int boxWidth = 40; // 네모칸의 기본 가로 길이
    printf("                       ┌────────────────────────────────────────────┐ \n");
            printf("                       │ %*s│ \n",boxWidth+3,"");

            // 영화명 출력


            // 영화명 출력 - 중앙 정렬
        int movieNameLength = strlen(tmp->m.movieName);
        printf("                       │ 영화명: %s%*s│ \n", tmp->m.movieName, boxWidth - movieNameLength -5, "");
        printf("                       │%*s│ \n",boxWidth+4,"");
        // 예매 번호 출력
        char personid[20];
        sprintf(personid, "%d", tmp->personId);
        int personidLength = strlen(personid);
        printf("                       │ 예매 번호: %d%*s│ \n",tmp->personId, boxWidth - personidLength - 8, "");

        // 상영관 출력
        printf("                       │ 상영관: %d관%*s│ \n",tmp->m.box,boxWidth - 8, "");

        // 상영 시간 출력
        int hours = tmp->m.start_time / 100;
        int minutes = tmp->m.start_time % 100;
        printf("                       │ 상영 시간: %02d:%02d%*s│ \n",  hours,minutes, boxWidth - 13, "");

        // 장소 출력
        printf("                       │ 장소: SMU 영화관%*s│ \n", boxWidth - 13, "");

        // 인원 출력
        char people[10];
        sprintf(people, "%d", tmp->pn.total_people);
        int peopleLength = strlen(people);
        printf("                       │ 인원: %d명%*s│ \n",tmp->pn.total_people, boxWidth-5-peopleLength, "");
        if(tmp->pn.adult!=0){
            printf("                       │       성인: %d명%*s│ \n", tmp->pn.adult,boxWidth - 12, "");
        }
        if(tmp->pn.teenager!=0){
            printf("                       │       청소년: %d명%*s│ \n", tmp->pn.teenager,boxWidth -14, "");
        }
        if(tmp->pn.children!=0){
            printf("                       │       어린이: %d명%*s│ \n", tmp->pn.children,boxWidth -14, "");

        }
        // 가격 출력
        char amount[10];
        sprintf(amount, "%d", tmp->pay.amount);
        int amountLength = strlen(amount);
        char disamount[10];
        sprintf(disamount, "%d", tmp->pay.disamount);
        int disamountLength = strlen(disamount);
        char total[10];
        sprintf(total, "%d", tmp->pay.totalpayment);
        int totalLength = strlen(total);
        printf("                       │ 금액: %d원%*s│ \n", tmp->pay.amount,boxWidth - 5-amountLength, "");
        printf("                       │       할인 금액: %d원%*s│ \n", tmp->pay.disamount,boxWidth - 16-disamountLength, "");
        printf("                       │       총 결제 금액: %d원%*s│ \n", tmp->pay.totalpayment,boxWidth -19-totalLength, "");

        printf("                       │%*s│ \n",boxWidth+4,"");
        int reserveSeatLength = strlen(tmp->reserve_seat);
        printf("                       │ 예매한 좌석: %s%*s│ \n", tmp->reserve_seat, boxWidth - reserveSeatLength - 10, "");
        printf("                       │%*s│ \n",boxWidth+4,"");
        printf("                       │%*s│ \n",boxWidth+4,"");
        printf("                       │%*s│ \n",boxWidth+4,"");
        int nameLength = strlen(tmp->name);
        printf("                       │ %s님!%*s│ \n", tmp->name, boxWidth - nameLength , "");
        printf("                       │ 이용해 주셔서 감사합니다.%*s│ \n", boxWidth - 22, "");
        printf("                       │ 오늘 하루 행복하세요.%*s│ \n", boxWidth - 18, "");
        printf("                       │%*s│ \n",boxWidth+4,"");
        printf("                       │%*s - SMU시네마│ \n",boxWidth-8,"");
        printf("                       │%*s│ \n",boxWidth+4,"");
        printf("                       └────────────────────────────────────────────┘ \n");


        color(WHITE, BLACK);
        color(WHITE, BLACK);
        gotoxy(45, 26);
        for (int i = 0; i < 10; i++)
         {
            gotoxy(45, 26);
            printf("%d초후에 처음 화면으로 돌아갑니다.", 10 - i);
            if (_kbhit()) { _getch(); break; }
            Sleep(200);
            if (_kbhit()) { _getch(); break; }
            Sleep(200);
            if (_kbhit()) { _getch(); break; }
            Sleep(200);
            if (_kbhit()) { _getch(); break; }
            Sleep(200);
            if (_kbhit()) { _getch(); break; }
            Sleep(200);
         }

}
// 예매 내역 출력
void display_member_by_phone(void)
{
    Member* node;
    int found = 0;
    // int boxWidth = 40; // 네모칸의 기본 가로 길이

    char phone_number[30];
    fflush(stdin);
    printf("휴대폰 번호를 입력하세요: ");
    scanf("%s", phone_number);

    printf("\n");

    for (Member* tmp = a_ML.head; tmp != NULL; tmp = tmp->next) {
        if (strcmp(tmp->phone, phone_number) == 0) {
            found = 1;
            node = tmp;
            display_reservation(node);
            break; // 예매자를 찾았으므로 반복문 종료
        }
    }

        if (!found) {
            color(WHITE,BLACK);
            printf("일치하는 예매자를 찾을 수 없습니다.\n");
            gotoxy(45, 15);
            for (int i = 0; i < 10; i++)
            {
                gotoxy(45, 15);
                printf("%d초후에 처음 화면으로 돌아갑니다.", 10 - i);
                if (_kbhit()) { _getch(); break; }
                Sleep(200);
                if (_kbhit()) { _getch(); break; }
                Sleep(200);
                if (_kbhit()) { _getch(); break; }
                Sleep(200);
                if (_kbhit()) { _getch(); break; }
                Sleep(200);
                if (_kbhit()) { _getch(); break; }
                Sleep(200);
            }

        }
}

// 영화정보 불러오기
void read_movie(void)
{
    FILE* fp;
    Movie* tmp;
    int i;

    if ((fp = fopen("movie.bin", "rb")) == NULL) {
        fprintf(stderr, "기존 영화 파일 정보는 없습니다.\n\n");
        return;
    }

    fread(&m_ML, sizeof(MovieList), 1, fp); // List 정보를 저장
    m_ML.head =m_ML.tail = NULL;

    for (i = 0; i < m_ML.movies; i++) {

        tmp = create_node();

        fread(tmp, sizeof(Movie), 1, fp);
        tmp->prev = m_ML.tail;
        tmp->next = NULL;
        if (m_ML.tail == NULL) {
            m_ML.head = m_ML.tail = tmp;
        }
        else {
            m_ML.tail->next = tmp;
            m_ML.tail = tmp;
        }


    }
    fclose(fp);
}

void read_member(char*Filename)
{
    FILE* fp;
    Member* tmp;
    int i;

    if ((fp = fopen(Filename, "rb")) == NULL) {
        fprintf(stderr, "기존 예매자 파일 정보는 없습니다.\n\n");
        return;
    }
    fread(&a_ML, sizeof(MemberList), 1, fp); // List 정보를 저장
    a_ML.head = a_ML.tail = NULL;

    for (i = 0; i < a_ML.members; i++) {

        tmp = create_node1();

        fread(tmp, sizeof(Member), 1, fp);
        tmp->prev = a_ML.tail;
        tmp->next = NULL;
        if (a_ML.tail == NULL) {
            a_ML.head = a_ML.tail = tmp;
        }
        else {
            a_ML.tail->next = tmp;
            a_ML.tail = tmp;

        }
    }

    fclose(fp);
}

// 영화 정보 파일에 쓰기
void write_movie(void)
{
    FILE *fp;
    Movie *tmp;

    if ((fp = fopen("movie.bin", "wb")) == NULL) { // 이진파일명 : movie.bin
        fprintf(stderr, "파일열기 오류로 영화정보를 저장하지 못하였습니다.\n");
        return;
    }

    fwrite(&m_ML, sizeof(MovieList), 1, fp); // List 정보를 저장
    for (tmp = m_ML.head; tmp != NULL; tmp = tmp->next) { // 각 회원정보를 저장
        fwrite(tmp, sizeof(Movie), 1, fp);
    }
    printf("영화 목록이 저장되었습니다.\n");
    fclose(fp);
}

// 애매자  정보 파일에 쓰기
void write_member(char* Filename)
{
    FILE *fp;
    Member *tmp;

    if ((fp = fopen(Filename, "wb")) == NULL) { // 이진파일명 : movie.bin
        fprintf(stderr, "파일열기 오류로 예매자 정보를 저장하지 못하였습니다.\n");
        return;
    }
    fwrite(&a_ML, sizeof(MemberList), 1, fp); // List 정보를 저장
    for (tmp = a_ML.head; tmp != NULL; tmp = tmp->next) { // 각 회원정보를 저장
        fwrite(tmp, sizeof(Member), 1, fp);
    }
    fclose(fp);

    printf("예매자 파일(%s)이 저장되었습니다.\n",Filename);
}

// 동적 할당된 메모리 해제
void free_seat(void)
{
    for (int i = 0; i < MAX_THEATER; i++) {
        for (int j = 0; j < MAX_TIME; j++) {
            for (int k = 0; k < theaters[i].rows; k++) {
                free(theaters[i].timeTable[j].seats[k]);
            }
            free(theaters[i].timeTable[j].seats);
        }
    }
}
