//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"

#define MAX_PLAYER   100
#define MAX_CHARNAME                200
#define MAX_DIE                     6

//board configuration parameters
static int board_nr;
static int food_nr;
static int festival_nr;

static int player_nr;
static int home_energy;
static int success;
static int endflag = 0;

typedef struct foood_chance {
       int menuId; //메뉴의 이름
	   char menuName[15]; //메뉴의 개수 
} foood_chance_e;

typedef struct player {
        int energy;
        int position;
        char name[MAX_CHARNAME];
        int accumCredit; // 누적된 학점 
        int flag_graduate; // 졸업 여부  
        int experience;  
} player_t;

static player_t *cur_player;
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


void printGrades(int player)
{
     int i;
     void *gradePtr;
     for (i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++)
     {
         gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
         printf("%s : %i\n", smmObj_getNodeName(gradePtr), smmObj_getNodeGrade(gradePtr));
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

void generatePlayers(int n, int initEnergy) //generate a new player
{
     int i;
     //n time loop
     for (i=0;i<n;i++)
     {
         //input name
         printf("Input player %i's name:", i); 
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
void actionNode(int player) {
    void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
    int type = smmObj_getNodeType(boardPtr);
    char *name = smmObj_getNodeName(boardPtr);
    void *gradePtr;

    switch(type) {
        case SMMNODE_TYPE_LECTURE:
            cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
            cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);

            // Grade generation
            gradePtr = smmObj_genObject(name, smmObjType_grade, 0, smmObj_getNodeCredit(boardPtr), 0, 0);
            smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
            break;

        case SMMNODE_TYPE_RESTAURANT:
            cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
            break;

        case SMMNODE_TYPE_LABORATORY:
            if (cur_player[player].experience) {
                printf("화이팅! 행운을 빌어요.\n");
            } else {
                printf("그렇지만 실험실로 가지 않아도 됩니다! 다행이네요. \n");
            }
            break;

        case SMMNODE_TYPE_HOME:
            cur_player[player].energy += home_energy;
            printf("집에서 푹 쉬어서 에너지가 회복되었습니다!\n");
            break;

        case SMMNODE_TYPE_GOTOLAB:
            printf("실험실로 가야합니다. 행운을 빌어요!\n");
            cur_player[player].experience = 1;
            cur_player[player].position = success;
            break;

        case SMMNODE_TYPE_FOODCHANCE:
        	{
        		int random_food = rand()%food_nr; 
        		foood_chance_e*random_food_data = smmdb_getData(LISTNO_FOODCARD, random_food);
        		printf("음식 카드 찬스!\n");
                printf("당신이 먹을 음식은 %s\n 입니다.", random_food_data->menuName);

    // 음식에 따른 행동 구현
    switch(random_food_data->menuId) {
        case 1: // 음식이 치킨일 때
            printf("에너지 10을 회복했습니다!\n");
            cur_player[player].energy += 10;
            break;
        case 2: // 음식이 맥주일 때
            printf("에너지 2를 잃었습니다!\n");
            cur_player[player].energy -= 2;
            break;
        case 3: // 음식이 탕후루일 때
            printf("에너지 3을 회복했습니다!\n");
            cur_player[player].energy += 3;
            break;
        case 4: // 음식이 컵라면일 때
            printf("에너지 2를 회복했습니다!\n");
            cur_player[player].energy += 3;
            break;
        case 5: // 음식이 감자탕일 때
            printf("에너지 15를 회복했습니다!\n");
            cur_player[player].energy += 15;
            break;
        case 6: // 음식이 컵밥일 때
            printf("에너지 5를 회복했습니다!\n");
            cur_player[player].energy += 5;
            break;
        case 7: // 음식이 짜장면일 때
            printf("에너지 12를 회복했습니다!\n");
            cur_player[player].energy += 12;
            break;
        case 8: // 음식이 학식일 때
            printf("에너지 11을 회복했습니다!\n");
            cur_player[player].energy += 11;
            break;
        case 9: // 음식이 파스타일 때
            printf("에너지 13을 회복했습니다!\n");
            cur_player[player].energy += 13;
            break;
        case 10: // 음식이 피자일 때
            printf("에너지 7을 회복했습니다!\n");
            cur_player[player].energy += 7;
            break;
        case 11: // 음식이 햄버거일 때
            printf("에너지 8을 회복했습니다!\n");
            cur_player[player].energy += 8;
            break;
        case 12: // 음식이 샌드위치일 때
            printf("에너지 8을 회복했습니다!\n");
            cur_player[player].energy += 8;
            break;
        case 13: // 음식이 요거트일 때
            printf("에너지 4를 회복했습니다!\n");
            cur_player[player].energy += 4;
            break;
        case 14: // 음식이 비빔밥일 때
            printf("에너지 10을 회복했습니다!\n");
            cur_player[player].energy += 10;
            break;
        
        default:
            
            break;
    
}
			}
            
            break;

        case SMMNODE_TYPE_FESTIVAL:
            
            break;

        default:
            
            break;
    }
}



/*void goForward(int player, int step)
{
     void *boardPtr;
     cur_player[player].position += step;
     boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position );
     
     printf("%s go to node %i (name: %s)\n", 
                cur_player[player].name, cur_player[player].position,
                smmObj_getNodeName(boardPtr));
}*/

void goForward(int player, int step) {
    int nextNode = cur_player[player].position + step;
    cur_player[player].position = nextNode % board_nr;

    if (cur_player[player].experience) {
        int randEscape = rand() % MAX_DIE + 1;
        printf("%d\n", randEscape);

        if (step != randEscape) {
            printf("한 번 더!\n");
            return;
        }

        cur_player[player].experience = 0;
    }

    if (nextNode >= board_nr) {
        printf("집에서 쉬었습니다! 당신은 에너지를 회복했습니다.\n", home_energy);
        cur_player[player].energy += home_energy; // Go through house
    }

    
    if (cur_player[player].accumCredit >= GRADUATE_CREDIT) {
        endflag = 1;
        printf("축하해요! 졸업 학점을 달성했습니다.\n", home_energy);
    }
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
        //(char* name, smmObjType_e objType, int type, int credit, int energy, smmObjGrade_e grade)
        void *boardObj = smmObj_genObject(name, smmObjType_board, type, credit, energy, 0);
        smmdb_addTail(LISTNO_NODE, boardObj);
        
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
    printf("(%s)", smmObj_getTypeName(SMMNODE_TYPE_LECTURE));
    
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
}
