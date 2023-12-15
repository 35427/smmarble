//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include <time.h>
#include <string.h>
#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"



//board configuration parameters
static int board_nr;
static int food_nr;
static int festival_nr;

static int player_nr;

float generateRandomGrade() {
    // 난수 발생기 초기화
    srand(time(NULL));

    // A+부터 C-까지의 학점을 난수로 생성 (1.7 ~ 4.3)
    float random_grade = ((float)rand() / RAND_MAX) * (4.3 - 1.7) + 1.7;

    return random_grade;
}


char *getGradeLetter(float grade) {
    // 학점 범위에 따라 학점 문자열 반환
    if (grade >= 4.3) {
        return "A+";
    } else if (grade >= 4.0) {
        return "A";
    } else if (grade >= 3.7) {
        return "A-";
    } else if (grade >= 3.3) {
        return "B+";
    } else if (grade >= 3.0) {
        return "B";
    } else if (grade >= 2.7) {
        return "B-";
    } else if (grade >= 2.3) {
        return "C+";
    } else if (grade >= 2.0) {
        return "C";
    } else {
        return "C-";
    }
}

/*int main() {
    // 랜덤 학점 생성 및 출력
    float grade = generateRandomGrade();
    printf("Randomly generated grade: %.1f (%s)\n", grade, getGradeLetter(grade));

    return 0;
}*/

typedef struct player {
        int energy;
        int position;
        char name[MAX_CHARNAME];
        int accumCredit;
        int flag_graduate;
        //int nodeNum; 
} player_t;

//static player_t *cur_player;
//player_t *cur_player[MAX_PLAYER];
player_t *cur_player;


//static player_t cur_player[MAX_PLAYER];

#if 0
static int player_energy[MAX_PLAYER];
static int player_position[MAX_PLAYER];
static char player_name[MAX_PLAYER][MAX_CHARNAME];
#endif

//function prototypes
#if 0
int isGraduated(void); //check if any player is graduated
 //print grade history of the player
void goForward(int player, int step); //make player go "step" steps on the board (check if player is graduated)
void printPlayerStatus(void); //print all player status at the beginning of each turn
float calcAverageGrade(int player); //calculate average grade of the player
smmGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)
void* findGrade(int player, char *lectureName); //find the grade from the player's grade history
void printGrades(int player); //print all the grade history of the player
#endif

int endflag = 0; 

void goForward(int player, int step) {
	//player_t *cur_player[MAX_PLAYER];
    //int nextNode = PlayerList[player].nodeNum + step;
    int nextNode = cur_player[player].accumCredit + step;
    int totalNodes = 7;/* 전체 노드 수를 여기에 입력하세요 */;
    
    if (nextNode >= totalNodes) {
        nextNode %= totalNodes;
        // 플레이어가 졸업 조건을 만족하면서 맵을 한 바퀴 이상 돌았을 때 추가적인 동작을 여기에 넣어주세요.
        // 예를 들어, 특정 보상을 주거나 추가 이벤트를 발생시키는 등의 동작을 넣을 수 있습니다.
    }
    
    // 플레이어 위치 업데이트
    cur_player[player].accumCredit = nextNode;
    
    // 플레이어가 졸업 조건을 만족하는지 확인
    if (cur_player[player].accumCredit >= GRADUATE_CREDIT) {
        printf("Congratulations! You have enough credits to graduate!\n");
        // 졸업 조건을 만족하면 endflag를 설정하여 게임 종료
        endflag = 1;
    }
}


void printGrades(int player)
{
     int i;
     void *gradePtr;
     for (i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++)
     {
         gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
         //printf("%s : %i\n", smmObj_getNodeName(gradePtr), smmObj_getNodeGrade(gradePtr));
     }
}

void printPlayerStatus(void)
{
     int i;
     
     for (i=0;i<player_nr;i++)
     {
         printf("%s : credit %i, energy %i, position %i\n", 
                      cur_player[i].name,
                      cur_player[i].accumCredit,
                      cur_player[i].energy,
                      cur_player[i].position);
     }
}

/*void generatePlayers(int n, int initEnergy) //generate a new player
{
     int i;
     //n time loop
     for (i=0;i<n;i++)
     {
         //input name
         printf("Input player %i's name:", i); //??? ???? 
         scanf("%s", cur_player[i].name);
         fflush(stdin);
         
         //set position
         //player_position[i] = 0;
         cur_player[i].position = 0;
         
         //set energy
         //player_energy[i] = initEnergy;
         cur_player[i].energy = initEnergy;
         cur_player[i].accumCredit = 0;
         cur_player[i].flag_graduate = 0;
     }
}
*/
void generatePlayers(int n, int initEnergy) {
    int i;
    cur_player = malloc(n * sizeof(player_t)); // 플레이어 수에 맞게 메모리를 할당합니다.

    //n time loop
    for (i = 0; i < n; i++) {
        //input name
        printf("Input player %i's name:", i); //??? ????
       // scanf("%s", cur_player[i].name);
       scanf("%s", cur_player[i].name);
        fflush(stdin);

        //set position
        cur_player[i].position = 0;

        //set energy
        cur_player[i].energy = initEnergy;
        cur_player[i].accumCredit = 0;
        cur_player[i].flag_graduate = 0;
    }
}


int rolldie(int player)
{
    char c;
    printf(" Press any key to roll a die (press g to see grade): ");
    c = getchar();
    fflush(stdin);
    
#if 1
    if (c == 'g')
        printGrades(player);
#endif
    
    return (rand()%MAX_DIE + 1);
}

//action code when a player stays at a node
void actionNode(int player)
{
    void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position );
    //int type = smmObj_getNodeType( cur_player[player].position );
    int type = smmObj_getNodeType( boardPtr );
    char *name = smmObj_getNodeName( boardPtr );
    void *gradePtr;
    
  /*  switch(type)
    {
        //case lecture:
        case SMMNODE_TYPE_LECTURE:
             if (
            cur_player[player].accumCredit += smmObj_getNodeCredit( boardPtr );
            cur_player[player].energy -= smmObj_getNodeEnergy( boardPtr );
            
            //grade generation
            gradePtr = smmObj_genObject(name, smmObjType_grade, 0, smmObj_getNodeCredit( boardPtr ), 0, ??);
            smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
            
            break;
            
        default: ) 
            break;
    }
}
*/

void actionNode(int player) {
    void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
    int type = smmObj_getNodeType(boardPtr);
    char *name = smmObj_getNodeName(boardPtr);
    void *gradePtr;

       switch(type) {
        case SMMNODE_TYPE_LECTURE:
            if (cur_player[player].accumCredit >= smmObj_getNodeCredit(boardPtr)) {
                cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
                cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);

                // Grade generation
                cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
				cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);

                //gradePtr = smmObj_genObject(name, smmObjType_grade, 0, smmObj_getNodeCredit(boardPtr), 0, (int)(generateRandomGrade() * 10));
                smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
            } else {
                printf("Not enough credit to attend this lecture.\n");
            }
            break;

        default:
            break;
    }
}




void goForward(int player, int step)
{
     void *boardPtr;
     cur_player[player].position += step;
     boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position );
     
     printf("%s go to node %i (name: %s)\n"), 
                cur_player[player].name, cur_player[player].position,
                smmObj_getNodeName(boardPtr);
}


int main(int argc, const char * argv[]) {
    
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    int i;
    int initEnergy;
    int turn=0;
    
    board_nr = 0;
    food_nr = 0;
    festival_nr = 0;
    
    srand(time(NULL));
    
    
    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig 
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    
    printf("Reading board component......\n");
    while ( fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4 ) //read a node parameter set
    {
        //store the parameter set
        //(char* name, smmObjType_e objType, int type, int credit, int energy, smmObjGrade_e grade);
       // void*boardObj = smmObj_genObject(name, smmObjType_board, type, credit, energy, 0);
        (void)smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);

        //smmdb_addTail(LISTNO_NODE, boardObj);
        
        if (type == SMMNODE_TYPE_HOME)
           initEnergy = energy;
        board_nr++;
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);
    
    
    for (i = 0;i<board_nr;i++)
    {
        void *boardObj = smmdb_getData(LISTNO_NODE, i);
        
        printf("node %i : %s, %i(%s), credit %i, energy %i\n", 
                     i, smmObj_getNodeName(boardObj), 
                     smmObj_getNodeType(boardObj), smmObj_getTypeName(smmObj_getNodeType(boardObj)),
                     smmObj_getNodeCredit(boardObj), smmObj_getNodeEnergy(boardObj));
    }
    //printf("(%s)", smmObj_getTypeName(SMMNODE_TYPE_LECTURE));
    
    #if 0
    //2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1;
    }
    
    printf("\n\nReading food card component......\n");
    while () //read a food parameter set
    {
        //store the parameter set
    }
    fclose(fp);
    printf("Total number of food cards : %i\n", food_nr);
    
    
    
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    while () //read a festival card string
    {
        //store the parameter set
    }
    fclose(fp);
    printf("Total number of festival cards : %i\n", festival_nr);
    #endif
    
    
    //2. Player configuration ---------------------------------------------------------------------------------
    
    do
    {
        //input player number to player_nr
        printf("input player no.:");
        scanf("%d", &player_nr);
        fflush(stdin);
    }
    while (player_nr < 0 || player_nr >  MAX_PLAYER);
    
    cur_player = (player_t*)malloc(player_nr*sizeof(player_t));
    generatePlayers(player_nr, initEnergy);
    
    
    
    
    
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (1) //is anybody graduated?
    {
        int die_result;
        
        
        //4-1. initial printing
        printPlayerStatus();
        
        //4-2. die rolling (if not in experiment)        
        die_result = rolldie(turn);
        
        //4-3. go forward
        goForward(turn, die_result);

		//4-4. take action at the destination node of the board
        actionNode(turn);
        
        //4-5. next turn
        turn = (turn + 1)%player_nr;
    }
    
    
    free(cur_player);
    system("PAUSE");
    return 0;
} }
