#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <conio.h>
#include <time.h>

#define MAX_THEATER 6       // ���� �ִ� ����
#define MAX_TIME 6          // �ð����� �ִ밳��
#define MAX_MOVIE 7         // ��ȭ ������ �ִ밳��

typedef struct {
    int status; // �¼� ���� (0: �� �¼�, 1: ����� �¼�)
} Seat;

typedef struct {
    Seat** seats;
    int remainseat;
} TimeTable;

typedef struct {
    TimeTable timeTable[MAX_TIME]; // �� ���忡�� ���ϴ� ��ȭ�� �ش��ϴ� �� �ð��� �����ϴ� �迭
    int rows;   //�ش� ���� ��
    int cols;   //�ش� ���� ��
} Theater;

// ��ȭ  �� ����
typedef struct Movie{
    char movieName[30]; // ��ȭ��
    int start_time; // ��ȭ���� �ð�
    int running_time; // ���� Ÿ��
    int box; // �󿵰�
    int movieId; // ��ȭ ������ȣ;
    struct Movie *prev;
    struct Movie *next;
}Movie;

// ��ȭ���� ����Ʈ
typedef struct MovieList{
    int date;   // ���� ��¥ (YYYYMMDD �������� ����)
    int movies; // ��ȭ ��
    int LastId; // ������ ��ȭ
    Movie *head, *tail;
}MovieList;

typedef struct People{
    int adult;          //������ ��
    int children;       //��� ��
    int teenager;       //û�ҳ� ��
    int total_people; // ������ �� �ο� ��
}People;

typedef struct Pay{
    int amount;         //�ݾ�
    int disamount;      //���αݾ�
    int totalpayment;   //�����ݾ�
}Pay;

//�ָ��� ����
typedef struct Member {
    int personId; //������ ������ȣ
    People pn; // �ο���
    char name[30]; // �̸�
    Movie m; // ��ȭ��, ��ȭ���۽ð�, ����Ÿ��, �󿵰�
    char reserve_seat[25]; //������ �¼�
    char phone[30];
    Pay pay;
    struct Member* prev, * next;
}Member;

//�ָ��� ����Ʈ
typedef struct MemberList {
    int members;
    int LastperosonId; // ������ �ָ��� ��ȣ
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

MovieList m_ML; // ��ȭ ����
MemberList a_ML; // ������ ����
Theater theaters[MAX_THEATER]; //��
People mvpeople[MAX_MOVIE];

int screenCount[MAX_THEATER] = { 0 };
char screenTimes[MAX_THEATER][MAX_TIME][5];
int currentStep = 1;  // ���� �ܰ�
int previousStep = 0; // ���� �ܰ�
int flag=1;


void color(int color, int bgcolor);
void gotoxy(int x, int y);
void init(void);            // List �ʱ�ȭ
void readandprocess(void);
void show_smu(void);        // smu ���� ��ũ��(�ָ� ��忡��)
void Theater_init(void);    //���� ũ�� ����
int showMangerMode(void);   // ������ �޴� ���
int showMainMenu(void);     //������ �޴� ���
void mangerMode(void);      // ������ ���
void guestMode(void);       // �ָ��� ���
void push(Member* member);  // ���� ����
void pop(void);             // ���� ����
Movie* create_node(void);   // ��ȭ ��� ����
Member* create_node1(void); // �ָ��� ��� ����

void display_movieName(void);   // �ʱ⿡ ��ȭ�� �����ֱ�
void input_movie(void);     // ������ ���� ��ȭ ���� �ֱ�
void search_movie(void);    // ��ȭ ��ȸ
void remove_movie(void);    // ��ȭ ���� ����
void display_movie(void);   // ��ȭ��� ���(��ȭ����)
void total_money(void);
void print_movie_people(void);
void display_total(void);

void input_member(Member *tmp); // ������ �ֱ�
void refund_member(int id); // ���� ���
void display_movie_by_screen(char* movieName); // �󿵰� �� ��ȭ �ð���
void print_screen(void); // screen���� ��ũ��
void showSeatMap(int theaternum, int timeIndex,Member *tmp);         //�¼� ��ġ���� ����ϴ� �Լ�
int reserveSeat(int theaternum, int timeIndex, int cursorX, int CursorY,Member* tmp); //�¼��� �����ϴ� �Լ�
int num_adult();    // ���� ��
int num_children(); // ��� ��
int num_teenager(); // û�ҳ� ��
void MovieReservation(void); // �ܰ躰 ����
void calculatePayment(Member* member, int isSMU); // �� ��� ���
void display_reservation(Member *tmp);
void display_member_by_phone(void); // ���� ���� ��ȸ(��)

void free_seat(void); // ���� �Ҵ� ����
void read_movie(void);      // ��ȭ���� �ҷ�����
void read_member(char *Filename);     // �ָ��� ���� �ҷ�����
void read_smu(Member *member);        //smu �л� �ҷ�����
void write_movie(void);     // ��ȭ ���� ���Ͽ� ����
void write_member(char* Filename);    // �ָ��� ���� ���Ͽ� ����
void write_smu(void);       //smu �л� ���� ���Ͽ� ����

int main(void)
{
    time_t t;
    struct tm* today;

    t = time(NULL); // ���� �ð� ��������

    today = localtime(&t);
    // ���� ��¥ ����
    char todayFilemembername[20];
    //todayFilename = 2023.��.��
    sprintf(todayFilemembername, "member-%04d-%02d-%02d.bin", today->tm_year + 1900, today->tm_mon + 1, today->tm_mday);

    // ���� ��¥ ����

    init(); // �ʱ�ȭ
    Theater_init();
    read_movie();

    read_member(todayFilemembername);
    readandprocess();

    mangerMode(); // ������ ���
    guestMode();

    write_movie();
    write_member(todayFilemembername);

    free_seat(); // �¼� �Ҵ� ����
    printf("\n���α׷��� ����˴ϴ�.");
    system("pause");
    return 0;
}
// ����
void color(int color, int bgcolor)
{
    color &= 0xf;
    bgcolor &= 0xf;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (bgcolor << 4) | color);
}
// ��ǥ
void gotoxy(int x, int y)
{
    COORD Cur;
    Cur.X = x;
    Cur.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
}
// List �ʱ�ȭ
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
        int box = temp->box - 1; // �󿵰� ��ȣ�� �迭 �ε����� ��ȯ

        // �ð��� char �������� ��ȯ�Ͽ� ����
        char timeChar[5]; // �ð��� ������ char �迭
        sprintf(timeChar, "%d", temp->start_time);

        // �ð��� char �������� ����
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
            // �¼� �������� ��� �� ���� �����մϴ�
            int row = seat_token[0] - 'A';
            int col = atoi(&seat_token[1])-1;
            // �ش� �¼��� ó���ϴ� �ڵ带 �ۼ��մϴ�
            theaters[temp->m.box - 1].timeTable[memtimeIndex].seats[row][col].status = 2;
            theaters[temp->m.box - 1].timeTable[memtimeIndex].remainseat--;
            // ���� �¼� ������ �����ɴϴ�
            seat_token = strtok(NULL, " ");
        }

    }

}

// ������ ���
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
            printf("===========================< ��ȭ ���� >=======================\n");
            color(WHITE,BLACK);
            remove_movie();
            break;
        case 4:
            display_movie();
            color(RED, BLACK);
            printf("\n\nó�� ȭ������ ���ư����� �ƹ� Ű�� ��������.");
            color(WHITE, BLACK);
            getch();
            break;
        case 5:
            total_money();
            color(RED, BLACK);
            printf("\n\nó�� ȭ������ ���ư����� �ƹ�Ű�� ��������.");
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
// ������ ���
void guestMode(void)
{
    int choice,id;
    while (flag) { // ���� ����
        system("cls");
        show_smu();
        printf("\t\t\t <����Ϸ��� �ƹ�Ű�� ��������>");
        getch();  // �ƹ� Ű�� ���� ������ ���
        system("cls");
        choice = showMainMenu();
        system("cls");
        switch(choice){
        case 1: // �� �ð�ǥ
            display_total();
            color(RED, BLACK);
            printf("\n\nó�� ȭ������ ���ư����� �ƹ� Ű�� ��������.");
            color(WHITE, BLACK);
            getch();
            break;
        case 2: // �ָ�
            MovieReservation();
            break;
        case 3: // ���ų��� ��ȸ
            display_member_by_phone();
            break;
        case 4: //���� ���
            fflush(stdin);
            color(RED,BLACK);
            printf("\n���� ������ȣ: ");
            color(WHITE,BLACK);
            scanf("%d", &id);
            refund_member(id);
            color(RED, BLACK);
            printf("\n\nó�� ȭ������ ���ư����� �ƹ�Ű�� ��������.");
            color(WHITE, BLACK);
            getch();
            break;

        }
    }
}
// ���ÿ� ����
void push(Member* member)
{
    StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
    newNode->member = member;
    newNode->next = top;
    top = newNode;
}
// ���ÿ� ����
void pop(void)
{
    if (top == NULL) {
        printf("������ ����ֽ��ϴ�.\n");
        return;
    }

    StackNode* temp = top;
    top = top->next;
    free(temp);
}

// ��ȭ ��� ����
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
// ������ ��� ����
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
// ������ �޴� ���
int showMangerMode(void)
{

    int selector, cursorx = 1;
    color(RED,BLACK);
    printf("                                                < �޴� >                            \n");
    color(WHITE,BLACK);
    gotoxy(10,7);
    printf("1.��ȭ��� 2.��ȭ��ȸ 3.��ȭ���� 4.��ü��ȸ 5.������Ȳ 6.������ ��� ���� 7.���α׷� ����");
    gotoxy(0, 10);
    printf("                        < �¿� ����Ű�� �̿��Ͽ� ���� Ŀ���� ������ �� �ֽ��ϴ�. > \n");
    printf("                                     < EnterŰ�� �޴��� ���ϼ���. >              \n");
    gotoxy(10,7);
    color(SKY_BLUE,BLACK);
    printf("1.��ȭ���");
    color(WHITE, BLACK);
    while (1) {
        if (kbhit())
        {
            color(WHITE, BLACK);
            gotoxy(10, 7);
            printf("1.��ȭ��� 2.��ȭ��ȸ 3.��ȭ���� 4.��ü��ȸ 5.������Ȳ 6.������ ��� ���� 7.���α׷� ����");
            selector = getch();
            if (selector == 224) { // ȭ��ǥ Ű�� ���
                selector = getch(); // ���� Ű �Է� �ޱ�
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
                printf("1.��ȭ���");
                color(WHITE, BLACK);
                break;
            case 2:
                gotoxy(21, 7);
                color(SKY_BLUE, BLACK);
                printf("2.��ȭ��ȸ");
                color(WHITE, BLACK);
                break;
            case 3:
                gotoxy(32, 7);
                color(SKY_BLUE, BLACK);
                printf("3.��ȭ����");
                color(WHITE, BLACK);
                break;
            case 4:
                gotoxy(43, 7);
                color(SKY_BLUE, BLACK);
                printf("4.��ü��ȸ");
                color(WHITE, BLACK);
                break;
            case 5:
                gotoxy(54, 7);
                color(SKY_BLUE, BLACK);
                printf("5.������Ȳ");
                color(WHITE, BLACK);
                break;
            case 6:
                gotoxy(65, 7);
                color(SKY_BLUE, BLACK);
                printf("6.������ ��� ����");
                color(WHITE, BLACK);
                break;
            case 7:
                gotoxy(84, 7);
                color(SKY_BLUE, BLACK);
                printf("7.���α׷� ����");
                color(WHITE, BLACK);
                break;
            }
        }
    }
    return cursorx;
}
// �� �޴� ���
int showMainMenu(void) {

    char enteredPassword[30];
    char password[30] = "password";
    color(RED,BLACK);
    int selector, cursorx = 1;
    printf("                                         < �޴� >                 \n");
    color(WHITE,BLACK);
    printf("\n                       1.�󿵽ð�ǥ 2.���� 3.���ų��� ��ȸ 4.������� ");
    gotoxy(0,4);
    printf("                 < �¿� ����Ű�� �̿��Ͽ� ���� Ŀ���� ������ �� �ֽ��ϴ�. > \n");
    printf("                                 < EnterŰ�� �޴��� ���ϼ���. >              \n");
    gotoxy(23, 2);
    color(SKY_BLUE, BLACK);
    printf("1.�󿵽ð�ǥ");
    color(WHITE, BLACK);
    while (1) {
        if (kbhit())
        {
            color(WHITE, BLACK);
            gotoxy(23, 2);
            printf("1.�󿵽ð�ǥ 2.���� 3.���ų��� ��ȸ 4.������� ");
            selector = getch();
            if (selector == 224) { // ȭ��ǥ Ű�� ���
                selector = getch(); // ���� Ű �Է� �ޱ�
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
                printf("������ ��й�ȣ �Է�: ");
                int i = 0;
                char ch;
                while ((ch = getch()) != '\r') { // Enter(���� ����) Ű�� ���� ������ ���ڸ� �н��ϴ�
                    if (ch == '\b') { // �齺���̽� ó��
                        if (i > 0) {
                            i--;
                            printf("\b \b"); // Ŀ���� �ڷ� �̵��Ͽ� ������ ����� �� �ٽ� Ŀ���� �ڷ� �̵��մϴ�
                        }
                    } else {
                        enteredPassword[i] = ch;
                        i++;
                        printf(" "); // ���� ���� ��� ��ǥ(*)�� ����մϴ�
                        }
                }
                enteredPassword[i] = '\0'; // �Էµ� ��й�ȣ�� �� ���ڷ� �����մϴ�

                printf("\n");
                if (strcmp(enteredPassword, password) == 0) {
                    mangerMode();
                } else {
                    printf("�߸��� ��й�ȣ�Դϴ�.\n");
                    getch(); // ����ϱ� ���� �ƹ� Ű�� ���� ������ ���
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
                printf("1.�󿵽ð�ǥ");
                color(WHITE, BLACK);
                break;
            case 2:
                gotoxy(36, 2);
                color(SKY_BLUE, BLACK);
                printf("2.����");
                color(WHITE, BLACK);
                break;
            case 3:
                gotoxy(43, 2);
                color(SKY_BLUE, BLACK);
                printf("3.���ų��� ��ȸ");
                color(WHITE, BLACK);
                break;
            case 4:
                gotoxy(59, 2);
                color(SKY_BLUE, BLACK);
                printf("4.�������");
                color(WHITE, BLACK);
                break;
            }
        }
    }
    return cursorx;
}
// ���� ���(�ָ��� ��忡��)
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

// �ʱ⿡ ��ȭ�� ���
void display_movieName(void)
{
    Movie* tmp;
    char prevMovieName[30] = "";
    int uniqueMovies = 0;
    int isFirstMovie = 1;

    printf("=================================================================================================================\n");
    color(DARK_SKY_BLUE,BLACK);
    printf("���� �� ��ȭ: ");
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
    printf("�� %d���� ��ȭ�� �����Դϴ�.\n\n", uniqueMovies);
}
void input_movie(void) {
    Movie* tmp;
    tmp = create_node();

    printf("<< �̹��޿� �����ϴ� ��ȭ ������ �Է��ϼ��� >> \n");
    tmp->movieId = ++(m_ML.movies);
    printf("��ȭ ���� ��ȣ: %d\n", tmp->movieId);
    printf("��ȭ��: ");
    scanf("%s", tmp->movieName);
    printf("�󿵰�: ");
    scanf("%d", &(tmp->box));

    while (tmp->box > 8 || tmp->box < 1) {
        printf("1������ 8������ �ֽ��ϴ�.\n");
        printf("�ٽ� �Է����ּ���:\n");
        scanf("%d", &(tmp->box));
    }

    // ��ȭ�� ���� �ð� �Է�
    color(RED, BLACK);
    printf("         < :�� �����ϰ�  �� �ð��� �Է����ּ��� >                      \n");
    printf("                  ex) 06:00 -> 600                         \n");
    color(WHITE, BLACK);
    printf("�Է�:");
    scanf("%d", &(tmp->start_time));

    // ��ȭ�� ���� Ÿ��(�� �ð�) �Է�
    printf("���� Ÿ��: ");
    color(RED, BLACK);
    printf("         < ������ �Է����ּ��� >                      \n");
    printf("                  ex) 120�� -> 120                         \n");
    color(WHITE, BLACK);
    printf("�Է�: ");
    scanf("%d", &(tmp->running_time));

    tmp->prev = tmp->next = NULL;

    while (1) {
        // ������ ��ȭ, �ð�, �󿵰��� ���� ��ȭ�� �̹� �����ϴ��� Ȯ��
        Movie* sameMovie = NULL;

        for (Movie* node = m_ML.head; node != NULL; node = node->next) {
            if (strcmp(node->movieName, tmp->movieName) == 0 && node->box == tmp->box && node->start_time == tmp->start_time) {
                sameMovie = node;
                break;
            }
        }

        if (sameMovie != NULL) {
            printf("�̹� ���� ��ȭ(�󿵰�, �ð���)�� ��ϵǾ� �ֽ��ϴ�.\n");
            printf("�󿵰�: ");
            scanf("%d", &(tmp->box));

            while (tmp->box > 8 || tmp->box < 1) {
                printf("1������ 8������ �ֽ��ϴ�.\n");
                printf("�ٽ� �Է����ּ���:\n");
                scanf("%d", &(tmp->box));
            }

            color(RED, BLACK);
            printf("         < :�� �����ϰ�  �� �ð��� �Է����ּ��� >                      \n");
            printf("                  ex) 06:00 -> 600                         \n");
            color(WHITE, BLACK);
            printf("�Է�:");
            scanf("%d", &(tmp->start_time));



            printf("���� Ÿ��: ");
            color(RED, BLACK);
            printf("         < ������ �Է����ּ��� >                      \n");
            printf("                  ex) 120�� -> 120                         \n");
            color(WHITE, BLACK);
            printf("�Է�: ");
            scanf("%d", &(tmp->running_time));

        }else{
            break;
        }
    }

    if (m_ML.tail == NULL) {
        m_ML.head = m_ML.tail = tmp;
    } else {
    // ���� ��ȭ�� ã�Ƽ� �ش� ��ȭ�� �� �ڿ� ���ο� ��ȭ�� �߰�

    Movie* lastSameMovie = NULL;
    for (Movie* node = m_ML.head; node != NULL; node = node->next) {
        if (strcmp(node->movieName, tmp->movieName) == 0) {
            lastSameMovie = node;
        }
    }

    if (lastSameMovie != NULL) {
        // ���� ��ȭ�� ã�� ���, �ش� ��ȭ�� �� �ڿ� ���ο� ��ȭ�� �߰�
        tmp->prev = lastSameMovie;
        tmp->next = lastSameMovie->next;
        if (lastSameMovie->next != NULL)
            lastSameMovie->next->prev = tmp;
        lastSameMovie->next = tmp;
    } else {
        // ���� ��ȭ�� ã�� ���� ���, ���� ����Ʈ�� �� �ڿ� ���ο� ��ȭ�� �߰�
        tmp->prev = m_ML.tail;
        m_ML.tail->next = tmp;
        m_ML.tail = tmp;
        }
    }
    int box = tmp->box - 1; // �󿵰� ��ȣ�� �迭 �ε����� ��ȯ

            // �ð��� char �������� ��ȯ�Ͽ� ����
            char timeChar[5]; // �ð��� ������ char �迭
            sprintf(timeChar, "%d", tmp->start_time);

            // �ð��� char �������� ����
            strcpy(screenTimes[box][screenCount[box]], timeChar);
            screenCount[box]++;

    printf("��ȭ ������ ��ϵǾ����ϴ�.\n");

    color(RED, BLACK);
    printf("\n\nó�� ȭ������ ���ư����� �ƹ�Ű�� ��������.");
    color(WHITE, BLACK);
    getch();
}

// ��ȭ ��ȸ
void search_movie(void)
{
    char name[30];
    Movie* tmp;
    int flag;
    display_movieName();
    fflush(stdin);
    printf("\n<��ȭ ��ȸ>");
    printf("��ȭ��: ");
    gets(name);
    flag = 0;
    for (tmp = m_ML.head; tmp != NULL; tmp = tmp->next)
        if (strstr(tmp->movieName, name)) {
            printf("���� ������ ��ȭ ����Դϴ�.\n");
             printf("=================================================================================================================\n");
            printf("�� ��ȭ ��ȣ: %d        ��ȭ��: %s           �󿵰�: %d��", tmp->movieId, tmp->movieName, tmp->box);
            int hour = tmp->start_time / 100;
            int minute = tmp->start_time % 100;
            printf("        ���۽ð�: %02d��:%02d��", hour, minute);
            printf("        ����Ÿ��: %d��\n", tmp->running_time);
            printf("\n=================================================================================================================\n\n");;
            flag = 1;
        }
    if (flag == 0)
        printf("��ϵ� ��ȭ�� �ƴմϴ�.\n\n");
    color(RED, BLACK);
    printf("\n\nó�� ȭ������ ���ư����� �ƹ�Ű�� ��������.");
    color(WHITE, BLACK);
    getch();
}
// ��ȭ ����
void remove_movie(void)
{
    int id, flag;
    Movie *tmp;
    int cnt = 0;
    display_movie();
    fflush(stdin);
    color(RED, BLACK);
    printf("\n���� ������ȣ: ");
    color(WHITE, BLACK);
    scanf("%d", &id);

    flag = 0;
    for (tmp = m_ML.head; tmp != NULL; tmp = tmp->next) {
        if (tmp->movieId == id) {

            // ��ȭ�� ������ ȸ������ Ȯ���Ͽ� ���Ű� �����ϴ� ��� ���� ����
            int reservationExist = 0;
            for (Member* member = a_ML.head; member != NULL; member = member->next) {
                if (strcmp(member->m.movieName, tmp->movieName) == 0 && member->m.start_time == tmp->start_time && member->m.box == tmp->box) {
                    reservationExist = 1;
                    break;
                }
            }

            if (reservationExist) {
                color(RED, BLACK);
                printf("�̹� �����ڰ� �־ ������ �� �����ϴ�.\n\n");
                printf("\n\nó�� ȭ������ ���ư����� �ƹ�Ű�� ��������.");
                color(WHITE, BLACK);
                getch();
                return;
            }

            if (tmp->prev == NULL) { // ù ��ȭ�� ���
                m_ML.head = tmp->next;

                if (tmp->next != NULL)
                    tmp->next->prev = NULL;
            } else {
                tmp->prev->next = tmp->next;
                if (tmp->next != NULL) // ������ ��ȭ�� ������ ���
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
        printf("�ش� ��ȭ�� �����Ͽ����ϴ�.\n\n");

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
        int box = tmp->box - 1; // �󿵰� ��ȣ�� �迭 �ε����� ��ȯ
        // �ð��� char �������� ����
        strcpy(screenTimes[box][screenCount[box]], "");
        screenCount[box]--;
        free(tmp);
    } else {
        printf("��ϵ� ��ȭ�� ��ȣ�� �ƴմϴ�.\n\n");
    }

    printf("\n\nó�� ȭ������ ���ư����� �ƹ� Ű�� ��������.");
    color(WHITE, BLACK);
    getch();
}
// ��ȭ��� ��� (������)
void display_movie(void)
{
    Movie* tmp;
    int prevBox = 0; // �ʱⰪ 0���� ����

    printf("\n�� ��ȭ ��: %d\n", m_ML.movies);
    for (int box = 1; box <= 8; box++) { // 1������ 8������ ������� ���
        for (tmp = m_ML.head; tmp != NULL; tmp = tmp->next) {
            if (tmp->box == box) { // ���� �󿵰��� ��ġ�ϴ� ��ȭ�� ���
                if (tmp->box != prevBox) {
                    printf("=====================( �󿵰� %d )=====================\n", tmp->box);
                    prevBox = tmp->box;
                }

                printf("<<������ȣ>>: %-10d\n", tmp->movieId);
                printf("��ȭ��: %-30s\n", tmp->movieName);
                int hour = tmp->start_time / 100;
                int minute = tmp->start_time % 100;
                printf("���۽ð�: %02d��%02d��\n", hour, minute);
                printf("����Ÿ��: %d��\n", tmp->running_time);
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
    printf("\n\n�� %d���� ��ȭ�� �����Դϴ�.\n", uniqueMovies);
    color(WHITE,BLACK);

}
void total_money(void){
    printf("�� �����: %d\n",a_ML.managepay.amount);
    printf("�� ���αݾ�: %d\n",a_ML.managepay.disamount);
    printf("�Ǹ����: %d\n",a_ML.managepay.totalpayment);

}
//���� ũ�� ����
void Theater_init(void)
{

    int Row[MAX_THEATER]={8,7,9,5,9,6};
    int Col[MAX_THEATER]={9,8,9,7,7,8};
    for (int i = 0; i < MAX_THEATER; i++) {
        theaters[i].rows = Row[i]; // ���� �� �� ����
        theaters[i].cols = Col[i]; // ���� �� �� ����

        for (int j = 0; j < MAX_TIME; j++) {
            theaters[i].timeTable[j].seats = (Seat**)malloc(Row[i] * sizeof(Seat*)); // �ð��뺰�� �¼� �迭 ���� �Ҵ�
            theaters[i].timeTable[j].remainseat=theaters[i].rows*theaters[i].cols;
            for (int k = 0; k < Row[i]; k++) {
                theaters[i].timeTable[j].seats[k] = (Seat*)malloc(Col[i] * sizeof(Seat)); // �� �ະ�� �¼� �迭 ���� �Ҵ�

                for (int l = 0; l < Col[i]; l++) {
                    theaters[i].timeTable[j].seats[k][l].status = 0; // �¼� ���� �ʱ�ȭ (0: �� �¼�)
                }
            }
        }
    }
}
void display_movie_by_screen(char* movieName)
{
    color(DARK_SKY_BLUE,BLACK);
    printf("\n\"%s\" ��ȭ�� �󿵰��� ����:\n", movieName);
    color(WHITE,BLACK);
    char tempScreenTimes[MAX_THEATER][MAX_TIME][6]; // �ӽ� �迭�� �����Ͽ� ���ڿ��� ��ȯ�� �ð� ������ ����
    int tempScreenCount[MAX_THEATER] = {0}; // �ӽ� �迭�� �����Ͽ� ������ �ð� ������ ������ ����
    // ��ȭ�� ��ġ�ϴ� �� ������ ã�Ƽ� �ӽ� �迭�� ����
    for (Movie* tmp = m_ML.head; tmp != NULL; tmp = tmp->next) {
        if (strcmp(tmp->movieName, movieName) == 0) {
            int box = tmp->box - 1; // �󿵰� ��ȣ�� �迭 �ε����� ��ȯ
            int timeIndex = tempScreenCount[box]; // ���� �󿵰������� �ð��� �ε���
            int time = tmp->start_time;
            int hour = time / 100;
            int minute = time % 100;
            sprintf(tempScreenTimes[box][timeIndex], "%02d:%02d", hour, minute);

            tempScreenCount[box]++;
        }
    }

    // ������ �ð��븦 ����
    for (int box = 0; box < MAX_THEATER; box++) {
        if (tempScreenCount[box] > 0) {
            char (*timeArray)[6] = tempScreenTimes[box];
            int count = tempScreenCount[box];

            // ���� ������ ����Ͽ� �ð��븦 �����մϴ�.
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

    // ���ĵ� �ð��븦 ���
    for (int box = 0; box < MAX_THEATER; box++) {
        if (tempScreenCount[box] > 0) {
            printf("===================================================( �󿵰�: %d )=================================================\n", box + 1);
            printf("�󿵽ð�: ");
            for (int i = 0; i < tempScreenCount[box]; i++) {
                int hour,minute,index;
                sscanf(tempScreenTimes[box][i], "%d:%d", &hour, &minute); // ���ڿ��� int�� ��ȯ�Ͽ� hour�� minute ������ ��
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
    printf("===================================================( �󿵰�: %d )=================================================\n", box); // �󿵰� ��ȣ ��� �� +1�� ���ݴϴ�.

    int found = 0;

    // �� ������ ���õ� ��ȭ�� ã�Ƽ� �ð��븦 ����
    char tempScreenTimes[MAX_TIME][6]; // �ӽ� �迭�� �����Ͽ� ���ڿ��� ��ȯ�� �ð� ������ ����
    int tempScreenCount = 0; // ������ �ð� ������ ������ ����

    for (Movie* tmp = m_ML.head; tmp != NULL; tmp = tmp->next) {
        if (strcmp(tmp->movieName, movieName) == 0 && tmp->box == box) { // box ���� +1�� ���ݴϴ�.
            found = 1;
            int time = tmp->start_time;
            int hour = time / 100;
            int minute = time % 100;
            sprintf(tempScreenTimes[tempScreenCount], "%02d:%02d", hour, minute);
            tempScreenCount++;
        }
    }

    if (found) {
        // �ð��븦 ����
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

        // ���ĵ� �ð��븦 ���
        printf("�󿵽ð�: ");
        for (int i = 0; i < tempScreenCount; i++) {
                int hour,minute,index;
                sscanf(tempScreenTimes[i], "%d:%d", &hour, &minute); // ���ڿ��� int�� ��ȯ�Ͽ� hour�� minute ������ ��
                int inttime = hour * 100 + minute;
                index=findTimeIndex(box-1,inttime);
             printf("%s ", tempScreenTimes[i]);
            color(RED,BLACK);
            printf("(%d/%d)  ",theaters[box-1].timeTable[index].remainseat,theaters[box-1].rows*theaters[box-1].cols);
            color(WHITE,BLACK);
        }
        printf("\n");
    } else {
        printf("�ش� ��ȭ�� �󿵵��� �ʰų� �󿵰��� �ùٸ��� �ʽ��ϴ�.\n");
    }

    printf("================================================================================================================\n\n");
}


// SCREEN ���� ���
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

// �¼� ���� �ϱ�(Ȯ��, �ߺ� üũ)
int reserveSeat(int theaternum, int timeIndex, int cursorX, int cursorY,Member* tmp)
{
    char seat[3];

    if(theaters[theaternum].timeTable[timeIndex].seats[cursorY][cursorX].status == 0)
    {
        theaters[theaternum].timeTable[timeIndex].seats[cursorY][cursorX].status = 1;
        // �¼� ������ �������� reserve_seat�� �߰�
        sprintf(seat, "%c%d", 'A' + cursorY, cursorX + 1);
        strcat(tmp->reserve_seat, seat);
        strcat(tmp->reserve_seat, " ");
        printf("%c%d �¼��� ����Ǿ����ϴ�.\n",'A'+cursorY,cursorX+1);
        return 1;
    }
    else {
        printf("�̹� ����� �¼��Դϴ�\n");
        return 0;
    }
}

//�¼� ��ġ���� ����ϴ� �Լ�
void showSeatMap(int theaternum, int timeIndex, Member *tmp)
{
    system("cls");
    print_screen();
    int row = theaters[theaternum].rows;
    int col = theaters[theaternum].cols;
    gotoxy(60, 13);
    color(BLACK,SKY_BLUE);
    printf("��");
    color(WHITE, BLACK);
   printf(" : ����");
   gotoxy(60, 14);
    color(BLACK,BLUE);
    printf("��");
    color(WHITE, BLACK);
   printf(" : ���ÿϷ�");
   gotoxy(60, 15);
   color(BLACK,WHITE);
   printf("��");
    color(WHITE, BLACK);
   printf(" : ���Ű���");
   gotoxy(60, 16);
   color(BLACK,RED);
   printf("��");
   color(WHITE, BLACK);
   printf(" : ���źҰ�");
   gotoxy(30,7);
   printf("\n");

    int cursorX = 0; // ���� Ŀ���� ���� ��ġ
    int cursorY = 0; // ���� Ŀ���� ���� ��ġ
    int currentnum = 0;

    while (currentnum < tmp->pn.total_people) {
        gotoxy(0, 9); // �¼� ��ġ�� ���� ��ġ�� Ŀ�� �̵�

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

        // Ű �Է� ó��
        int key = getch(); // Ű �Է� �ޱ�

        if (key == 224) { // ȭ��ǥ Ű�� ���
            key = getch(); // ���� Ű �Է� �ޱ�

            switch (key) {
                case 72: // ���� ȭ��ǥ Ű
                    if (cursorY > 0)
                        cursorY--;
                    break;
                case 80: // �Ʒ��� ȭ��ǥ Ű
                    if (cursorY < row - 1)
                        cursorY++;
                    break;
                case 75: // ���� ȭ��ǥ Ű
                    if (cursorX > 0)
                        cursorX--;
                    break;
                case 77: // ������ ȭ��ǥ Ű
                    if (cursorX < col - 1)
                        cursorX++;
                    break;
            }
        } else if (key == 13) { // ���� Ű�� ��� (�¼� ����)
             currentnum+=reserveSeat(theaternum, timeIndex, cursorX, cursorY,tmp);
        }
    }

     printf("�� ������ �¼�: %s\n", tmp->reserve_seat);
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
    return -1; // �ð��� ���� ��� -1 ��ȯ
}

//���� ��
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
            if (selector == 224) { // ȭ��ǥ Ű�� ���
                selector = _getch(); // ���� Ű �Է� �ޱ�
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
                printf("��");
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

//û�ҳ� ��
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
            if (selector == 224) { // ȭ��ǥ Ű�� ���
                selector = _getch(); // ���� Ű �Է� �ޱ�
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
                printf("��");
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

//��� ��
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
             if (selector == 224) { // ȭ��ǥ Ű�� ���
                 selector = getch(); // ���� Ű �Է� �ޱ�
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
                 printf("��");
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


// ������ ���
void input_member(Member *tmp)
{
    int inputflag=1;
    printf("\n<����ȸ�� ���>\n");
    tmp->personId = ++(a_ML.LastperosonId);
    printf("���Ź�ȣ(�ڵ��ο�): %d\n", tmp->personId);
    tmp->pn.total_people = 0;
    printf("�̸� �Է�: ");
    scanf("%s", tmp->name);
    while(inputflag){
        gotoxy(20,8);
        printf("����");
        gotoxy(28, 8);
        printf("0 1 2 3 4 5 6 7 8 ");
        gotoxy(60,8);
        color(BLACK,WHITE);
        printf("  ����: 13,000�� ");
        color(WHITE, BLACK);
        gotoxy(20,10);
        printf("û�ҳ�");
        gotoxy(28, 10);
        printf("0 1 2 3 4 5 6 7 8 ");
        gotoxy(60,10);
        color(BLACK,WHITE);
        printf("û�ҳ�: 10,000�� ");
        color(WHITE, BLACK);
        gotoxy(20, 12);
        printf("���");
        gotoxy(28, 12);
        printf("0 1 2 3 4 5 6 7 8 ");
        gotoxy(60,12);
        color(BLACK,WHITE);
        printf("���: 7,000��  ");
        color(WHITE, BLACK);
        gotoxy(5, 5);
        printf("            < �¿� ����Ű�� �̿��Ͽ� ���� Ŀ���� ������ �� �ֽ��ϴ�. > \n");
        gotoxy(28, 7);
        printf("��");
        gotoxy(45, 7);
        printf("��");
        gotoxy(0, 15);
        printf("                          < EnterŰ�� �ο��� ���ϼ���. >              \n");
        printf("               < �ð��� �ٽ� �����Ϸ��� BackspaceŰ�� �����ּ���. >   \n");

        tmp->pn.adult=num_adult();
        tmp->pn.teenager = num_teenager();
        tmp->pn.children=num_children();
        tmp->pn.total_people =  tmp->pn.adult + tmp->pn.children + tmp->pn.teenager;
        int index=findTimeIndex(tmp->m.box-1,tmp->m.start_time);
        if(tmp->pn.total_people>theaters[tmp->m.box-1].timeTable[index].remainseat){
                gotoxy(0, 14);
            color(RED,BLACK);
            printf("                                ���� �¼��� %d�Դϴ�.",theaters[tmp->m.box-1].timeTable[index].remainseat);
            color(WHITE,BLACK);
            getchar();
        }
        else if(tmp->pn.total_people == 0){
            gotoxy(0, 14);
            color(RED,BLACK);
            printf("                          <�ּ� 1���� �����ϼž��մϴ�.>                ");
            color(WHITE,BLACK);
            getchar();
        }
        else if(tmp->pn.total_people>10){
            gotoxy(0, 14);
            color(RED,BLACK);
            printf("                             <�ִ� 10����� �����մϴ�.>                ");
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

// ���� ���
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
            // �¼� �������� ��� �� ���� �����մϴ�
            int row = seat_token[0] - 'A';
            int col = atoi(&seat_token[1])-1;
            // �ش� �¼��� ó���ϴ� �ڵ带 �ۼ��մϴ�
            theaters[tmp->m.box - 1].timeTable[memtimeIndex].seats[row][col].status = 0;
            theaters[tmp->m.box - 1].timeTable[memtimeIndex].remainseat++;
            // ���� �¼� ������ �����ɴϴ�
            seat_token = strtok(NULL, " ");
        }

            if (tmp->prev == NULL){ // ùȸ���� ���
                a_ML.head = tmp->next;

                if(tmp->next != NULL)
                    tmp->next->prev = NULL;

            } else {
                tmp->prev->next = tmp->next;
                if (tmp->next != NULL) // ������ ȸ���� ������ ���
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
        printf("���Ű� ��� �Ǿ����ϴ�.\n\n");
    }
    else
        printf("��ϵ� ���� ��ȣ�� �ƴմϴ�.\n\n");

}
// �� ���� �ݾ�
void calculatePayment(Member* member, int isSMU)
{
    // ���� ���� (SMU ȸ���� ���)
    if (isSMU) {

        member->pay.disamount = member->pay.amount * 0.2;     //20%�����ϴ� �ݾ�
        member->pay.totalpayment = member->pay.amount * 0.8;  //20%���ε� �ݾ�

    }else{
        member->pay.totalpayment=member->pay.amount;
    }

}

//smu �л� �ҷ��ͼ� Ȯ��(����)
void read_smu(Member *tmp)
{
    FILE* fp = fopen("member.txt", "r"); // smu ȸ�� ���ϸ�

    if (fp == NULL) {
        printf("���� ������ �����ϴ�.\n");
        return;
    }

    char choice;
    char number[10];
    char name[30];
    char smuNumber[10];
    int smuFound = 0; // SMU ȸ�� ���θ� �����ϴ� ����
    printf("smuȸ���̽ø� y �ƴϽø� n�� �����ּ���\n");

    while(choice!='y'&&choice!='n'){
        choice=getch();
    }

    if(choice=='y')
    {
        printf("smuȮ���� ���� �й��� �Է��ϼ���: ");

        scanf("%s", smuNumber);
        while (fscanf(fp, "%s %s", number, name) != EOF) {
            if (strcmp(smuNumber, number) == 0) {
                fclose(fp);
                printf("smu ȸ������ Ȯ���Ͽ����ϴ�.\n");
                smuFound = 1;
                break;
            }
        }
    }
    else
        printf("smuȸ���� �ƴϽø� ������ �Ұ����մϴ�.");
    if (smuFound==0&&choice=='y')
                printf("��ϵ� smuȸ���� �ƴմϴ�.\n");
    calculatePayment(tmp, smuFound); // SMU ȸ���̹Ƿ� ���� ����
    fclose(fp);

}
// ��ȭ ���� �ܰ�(�ڷΰ���)
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
                printf("��ȭ���� �Է����ּ���: ");
                scanf("%s", tmp->m.movieName);

                // ��ȭ�� ���Ͽ� ��ϵ� ��ȭ���� Ȯ��
                int movieRegistered = 0;  // ��ȭ ��� ���� �÷���
                for (Movie* temp = m_ML.head; temp != NULL; temp = temp->next) {
                    if (strcmp(temp->movieName, tmp->m.movieName) == 0) {
                        movieRegistered = 1;
                        break;
                    }
                }

                if (!movieRegistered) {
                    color(RED, BLACK);
                    printf("\n\n ��ϵ��� ���� ��ȭ�Դϴ�. �ʱ�ȭ������ ���ư��ϴ�");
                    color(WHITE, BLACK);
                    getch();
                    return;
                }
                break;

            case 2:
               display_movie_by_screen(tmp->m.movieName);

                    printf("\n��ȭ �ð�ǥ�� Ȯ���ϰ� �󿵰��� �Է����ּ���: ");
                    scanf("%d", &tmp->m.box);

                    int boxRegistered = 0;  // �󿵰� ��� ���� �÷���
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
                        printf("\n\n��ϵ� �󿵰��� �ƴմϴ�. �ʱ�ȭ������ ���ư��ϴ�\n");
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
                    printf("         < :�� �����ϰ�  �ð��� �Է����ּ��� >                      \n");
                    printf("                  ex) 06:00 -> 600                         \n");
                    color(WHITE, BLACK);

                    printf("�Է�: ");
                    scanf("%d", &tmp->m.start_time);

                    timeIndex = findTimeIndex(tmp->m.box - 1, tmp->m.start_time);
                    if (timeIndex == -1) {
                        color(RED, BLACK);
                        printf("\n\n������ �ð��� ��ȭ �ð�ǥ�� �����ϴ�. �ʱ�ȭ������ ���ư��ϴ�\n");
                        color(WHITE, BLACK);
                        getch();
                        return;
                    } else {
                        row = theaters[tmp->m.box - 1].rows;
                        col = theaters[tmp->m.box - 1].cols;
                    }
                    if(theaters[tmp->m.box-1].timeTable[timeIndex].remainseat==0){
                        color(RED, BLACK);
                        printf("\n\n���� �¼��� �����ϴ�. �ʱ�ȭ������ ���ư��ϴ�\n");
                        color(WHITE, BLACK);
                        getch();
                        return;
                    }
                break;

            case 4:
                printf("�޴��� ��ȣ�� �Է��ϼ���: ");
                scanf("%s", tmp->phone);
                break;
            case 5:
                input_member(tmp);
                break;
            case 6:

                showSeatMap(tmp->m.box - 1, timeIndex, tmp);
                break;
            case 7:
                read_smu(tmp); // smuȸ������ Ȯ���ϸ� 10%����
                break;
        }
        gotoxy(75,14);
        color(BLUE, BLACK);
        printf("<<���� �ܰ�:   Enter   >>");
        gotoxy(75,15);
        color(RED, BLACK);
        printf("<<���� �ܰ�: BackSpace >>");
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
                    pop(); // ���� �ܰ�� ���ư��鼭 ���ÿ��� pop
                    for (int i = 0; i < row; i++) {
                        for (int j = 0; j < col; j++) {
                            if (theaters[tmp->m.box-1].timeTable[timeIndex].seats[i][j].status == 1) {
                                theaters[tmp->m.box-1].timeTable[timeIndex].seats[i][j].status = 0;  // ������ ������ �¼��� �ٽ� ���� �����ϵ��� ����
                            }
                        }
                    }
                    tmp->pay.amount=0;
                    tmp->pay.disamount=0;
                    tmp->pay.totalpayment=0;
                    strcpy(tmp->reserve_seat, ""); // reserve_seat�� �� ���ڿ��� �ʱ�ȭ
                    break;
                } else {
                    printf("�� �̻� �ڷ� �� �� �����ϴ�.\n");
                }
            }
        }

        if (currentStep <= 7 && currentStep > previousStep) {
            Member* newNode = (Member*)malloc(sizeof(Member));
            memcpy(newNode, tmp, sizeof(Member));
            push(newNode); // ���� �ܰ迡�� ���ÿ� push
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
    int boxWidth = 40; // �׸�ĭ�� �⺻ ���� ����
    printf("                       �������������������������������������������������������������������������������������������� \n");
            printf("                       �� %*s�� \n",boxWidth+3,"");

            // ��ȭ�� ���


            // ��ȭ�� ��� - �߾� ����
        int movieNameLength = strlen(tmp->m.movieName);
        printf("                       �� ��ȭ��: %s%*s�� \n", tmp->m.movieName, boxWidth - movieNameLength -5, "");
        printf("                       ��%*s�� \n",boxWidth+4,"");
        // ���� ��ȣ ���
        char personid[20];
        sprintf(personid, "%d", tmp->personId);
        int personidLength = strlen(personid);
        printf("                       �� ���� ��ȣ: %d%*s�� \n",tmp->personId, boxWidth - personidLength - 8, "");

        // �󿵰� ���
        printf("                       �� �󿵰�: %d��%*s�� \n",tmp->m.box,boxWidth - 8, "");

        // �� �ð� ���
        int hours = tmp->m.start_time / 100;
        int minutes = tmp->m.start_time % 100;
        printf("                       �� �� �ð�: %02d:%02d%*s�� \n",  hours,minutes, boxWidth - 13, "");

        // ��� ���
        printf("                       �� ���: SMU ��ȭ��%*s�� \n", boxWidth - 13, "");

        // �ο� ���
        char people[10];
        sprintf(people, "%d", tmp->pn.total_people);
        int peopleLength = strlen(people);
        printf("                       �� �ο�: %d��%*s�� \n",tmp->pn.total_people, boxWidth-5-peopleLength, "");
        if(tmp->pn.adult!=0){
            printf("                       ��       ����: %d��%*s�� \n", tmp->pn.adult,boxWidth - 12, "");
        }
        if(tmp->pn.teenager!=0){
            printf("                       ��       û�ҳ�: %d��%*s�� \n", tmp->pn.teenager,boxWidth -14, "");
        }
        if(tmp->pn.children!=0){
            printf("                       ��       ���: %d��%*s�� \n", tmp->pn.children,boxWidth -14, "");

        }
        // ���� ���
        char amount[10];
        sprintf(amount, "%d", tmp->pay.amount);
        int amountLength = strlen(amount);
        char disamount[10];
        sprintf(disamount, "%d", tmp->pay.disamount);
        int disamountLength = strlen(disamount);
        char total[10];
        sprintf(total, "%d", tmp->pay.totalpayment);
        int totalLength = strlen(total);
        printf("                       �� �ݾ�: %d��%*s�� \n", tmp->pay.amount,boxWidth - 5-amountLength, "");
        printf("                       ��       ���� �ݾ�: %d��%*s�� \n", tmp->pay.disamount,boxWidth - 16-disamountLength, "");
        printf("                       ��       �� ���� �ݾ�: %d��%*s�� \n", tmp->pay.totalpayment,boxWidth -19-totalLength, "");

        printf("                       ��%*s�� \n",boxWidth+4,"");
        int reserveSeatLength = strlen(tmp->reserve_seat);
        printf("                       �� ������ �¼�: %s%*s�� \n", tmp->reserve_seat, boxWidth - reserveSeatLength - 10, "");
        printf("                       ��%*s�� \n",boxWidth+4,"");
        printf("                       ��%*s�� \n",boxWidth+4,"");
        printf("                       ��%*s�� \n",boxWidth+4,"");
        int nameLength = strlen(tmp->name);
        printf("                       �� %s��!%*s�� \n", tmp->name, boxWidth - nameLength , "");
        printf("                       �� �̿��� �ּż� �����մϴ�.%*s�� \n", boxWidth - 22, "");
        printf("                       �� ���� �Ϸ� �ູ�ϼ���.%*s�� \n", boxWidth - 18, "");
        printf("                       ��%*s�� \n",boxWidth+4,"");
        printf("                       ��%*s - SMU�ó׸��� \n",boxWidth-8,"");
        printf("                       ��%*s�� \n",boxWidth+4,"");
        printf("                       �������������������������������������������������������������������������������������������� \n");


        color(WHITE, BLACK);
        color(WHITE, BLACK);
        gotoxy(45, 26);
        for (int i = 0; i < 10; i++)
         {
            gotoxy(45, 26);
            printf("%d���Ŀ� ó�� ȭ������ ���ư��ϴ�.", 10 - i);
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
// ���� ���� ���
void display_member_by_phone(void)
{
    Member* node;
    int found = 0;
    // int boxWidth = 40; // �׸�ĭ�� �⺻ ���� ����

    char phone_number[30];
    fflush(stdin);
    printf("�޴��� ��ȣ�� �Է��ϼ���: ");
    scanf("%s", phone_number);

    printf("\n");

    for (Member* tmp = a_ML.head; tmp != NULL; tmp = tmp->next) {
        if (strcmp(tmp->phone, phone_number) == 0) {
            found = 1;
            node = tmp;
            display_reservation(node);
            break; // �����ڸ� ã�����Ƿ� �ݺ��� ����
        }
    }

        if (!found) {
            color(WHITE,BLACK);
            printf("��ġ�ϴ� �����ڸ� ã�� �� �����ϴ�.\n");
            gotoxy(45, 15);
            for (int i = 0; i < 10; i++)
            {
                gotoxy(45, 15);
                printf("%d���Ŀ� ó�� ȭ������ ���ư��ϴ�.", 10 - i);
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

// ��ȭ���� �ҷ�����
void read_movie(void)
{
    FILE* fp;
    Movie* tmp;
    int i;

    if ((fp = fopen("movie.bin", "rb")) == NULL) {
        fprintf(stderr, "���� ��ȭ ���� ������ �����ϴ�.\n\n");
        return;
    }

    fread(&m_ML, sizeof(MovieList), 1, fp); // List ������ ����
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
        fprintf(stderr, "���� ������ ���� ������ �����ϴ�.\n\n");
        return;
    }
    fread(&a_ML, sizeof(MemberList), 1, fp); // List ������ ����
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

// ��ȭ ���� ���Ͽ� ����
void write_movie(void)
{
    FILE *fp;
    Movie *tmp;

    if ((fp = fopen("movie.bin", "wb")) == NULL) { // �������ϸ� : movie.bin
        fprintf(stderr, "���Ͽ��� ������ ��ȭ������ �������� ���Ͽ����ϴ�.\n");
        return;
    }

    fwrite(&m_ML, sizeof(MovieList), 1, fp); // List ������ ����
    for (tmp = m_ML.head; tmp != NULL; tmp = tmp->next) { // �� ȸ�������� ����
        fwrite(tmp, sizeof(Movie), 1, fp);
    }
    printf("��ȭ ����� ����Ǿ����ϴ�.\n");
    fclose(fp);
}

// �ָ���  ���� ���Ͽ� ����
void write_member(char* Filename)
{
    FILE *fp;
    Member *tmp;

    if ((fp = fopen(Filename, "wb")) == NULL) { // �������ϸ� : movie.bin
        fprintf(stderr, "���Ͽ��� ������ ������ ������ �������� ���Ͽ����ϴ�.\n");
        return;
    }
    fwrite(&a_ML, sizeof(MemberList), 1, fp); // List ������ ����
    for (tmp = a_ML.head; tmp != NULL; tmp = tmp->next) { // �� ȸ�������� ����
        fwrite(tmp, sizeof(Member), 1, fp);
    }
    fclose(fp);

    printf("������ ����(%s)�� ����Ǿ����ϴ�.\n",Filename);
}

// ���� �Ҵ�� �޸� ����
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
