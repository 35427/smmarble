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
static int home_energy = 10;
//static int success;
static int endflag = 0;


typedef struct player {
        int energy;
        int position;
        char name[MAX_CHARNAME];
        int accumCredit; // 누적된 학점 
        int flag_graduate; // 졸업 여부  
        int experience;  
        char currentLecture[MAX_CHARNAME]; // 현재 듣는 강의 이름
        int success;
        
        // 수강한 강의 정보를 저장할 배열
    	char takenCourses[100][MAX_CHARNAME];
    	int numCoursesTaken;
        
} player_t;



static player_t *cur_player;
//static player_t cur_player[MAX_PLAYER];


// 함수 선언
int checkCourseTaken(int player, char *lectureName);

// 수강한 강의 중복 체크 함수
int checkCourseTaken(int player, char *lectureName) {
    int i;
    for (i = 0; i < cur_player[player].numCoursesTaken; ++i) {
        if (strcmp(cur_player[player].takenCourses[i], lectureName) == 0) {
            // 이미 수강한 강의인 경우
            return 1; // 중복 수강
        }
    }
    return 0; // 수강하지 않은 강의
}
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


void printGrades(int player) // 플레이어의 전체 성적을 출력함 
{
     int i;
     void *gradePtr;
     for (i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++)
     {
         gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
         printf("%s : %s\n", smmObj_getNodeName(gradePtr), smmObj_getGradeName(smmObj_getNodeGrade(gradePtr)));
     }
}

void printPlayerStatus(void) // 플레이어의 상태를 출력함 
{
     int i;
     printf("\n\n====================================== PLAYER STATUS ======================================\n");
     for (i=0;i<player_nr;i++)
     {
         printf("%s | 학점 %i, 에너지 %i, 위치 %i\n", 
                      cur_player[i].name,
                      cur_player[i].accumCredit,
                      cur_player[i].energy,
                      cur_player[i].position);
     }
     printf("===========================================================================================\n\n");
}

void generatePlayers(int n, int initEnergy) //새 플레이어를 생성 
{
     int i;
     //n time loop
     for (i=0;i<n;i++)
     {
         //input name
         printf("플레이어 %i의 이름을 입력하세요.: ", i); 
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


int rolldie(int player) // 주사위를 굴림 
{
    char c;
    if (cur_player[player].position == SMMNODE_TYPE_LECTURE){
	 
    printf("아무 키나 눌러 주사위를 굴리세요.(학점을 보려면 g를 누르세요.): ");
    c = getchar();
    fflush(stdin);
    }

    if (c == 'g')
        printGrades(player);

    
    return (rand()%MAX_DIE + 1);
}


//action code when a player stays at a node
void actionNode(int player) {
    void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
    int type = smmObj_getNodeType(boardPtr);
    char *name = smmObj_getNodeName(boardPtr);
    void *gradePtr;
    int grade;

    switch(type) {
        case SMMNODE_TYPE_LECTURE: // 강의 칸에 도착 
            {
            	int lec;
				printf("강의를 수강하려면 1을, 수강하지 않으려면 2를 입력하세요. :\n");
				scanf("%d", &lec);
				fflush(stdin);
				
				if (lec == 1){
					if (cur_player[player].energy >= smmObj_getNodeEnergy(boardPtr)){
						// 해당 강의가 이미 수강한 강의인지 체크
               		 if (checkCourseTaken(player, smmObj_getNodeName(boardPtr)) == 1) {
                    printf("이미 수강한 강의입니다.\n");}
					
					else{
					cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
            		cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
            		
					grade = rand()%smmObjGrade_all;
					gradePtr = smmObj_genObject(name, smmObjType_grade, 0, smmObj_getNodeCredit(boardPtr), 0, (smmObjGrade_e)grade);
                    smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
                    
                    
            		strcpy(cur_player[player].currentLecture, smmObj_getNodeName(boardPtr));
					printf("들은 학점은 %d, 남은 에너지는 %d, 수강한 강의는 %s입니다.\n",smmObj_getNodeCredit(boardPtr), cur_player[player].energy,cur_player[player].currentLecture);
					// 수강한 강의를 추가
                    strcpy(cur_player[player].takenCourses[cur_player[player].numCoursesTaken], smmObj_getNodeName(boardPtr));
                    cur_player[player].numCoursesTaken++;
				}	}
				else{
					printf("에너지가 부족해 강의를 수강할 수 없습니다.\n");
				}
            }
        }
       	  break;
			
          

        case SMMNODE_TYPE_RESTAURANT: //식당 칸에 도착 
            cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
            printf("음식을 먹고 에너지를 보충했습니다. 남은 에너지는 %d입니다.\n", cur_player[player].energy);
            break;

        case SMMNODE_TYPE_LABORATORY: //실험실 칸에 도착 
            if (cur_player[player].experience) {
                printf("화이팅! 행운을 빌어요.\n");
				cur_player[player].position = 8; // 실험실 칸으로 이동 
            }
			else {
                printf("그렇지만 실험시간이 아니라 실험실로 가지 않아도 됩니다! 다행이네요. \n");
            }
            break;


    	case SMMNODE_TYPE_GOTOLAB: { // 실험실로 가야 하는 칸에 도착 
    		printf("실험실로 가야합니다. 행운을 빌어요!\n");
   			int success = rand() % MAX_DIE + 1;
   			
    		printf("\n%d 이상의 수가 나오면 실험 성공입니다.\n", success);
 			printf("아무 키나 눌러 주사위를 한 번 더 굴리세요.\n");
    		getchar(); // 아무 키나 눌러서 대기
    		int attempt = rolldie(player); // 주사위 굴리기
    		printf("주사위 결과: %d\n", attempt);
    		
        	if (attempt < success) { // 실패했을 경우 
           		printf("실험에 실패하여 다음 턴에 다시 도전해야합니다.\n");
            	cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
        		
    		} 
			else if (attempt >= success) { // 성공했을 경우 
            	printf("실험에 성공해 실험실에서 탈출합니다!\n");
            	cur_player[player].flag_graduate = 0;
        	}
        break;
		}

        case SMMNODE_TYPE_FOODCHANCE: { //음식 찬스 칸에 도착 
           if (food_nr>0){
            
           int random_food = rand()%food_nr; // 음식카드 개수에 맞게 난수 생성 
           
           void* randomFoodCard = smmdb_getData(LISTNO_FOODCARD, random_food); // 음식카드 데이터 가져오기 
           printf("음식 카드 찬스!\n");
          cur_player[player].energy += smmObj_getNodeEnergy(randomFoodCard);
          printf("당신은 %s을(를) 먹고 에너지를 얻어 총 에너지가 %d가 되었습니다.\n", smmObj_getNodeName(randomFoodCard), cur_player[player].energy);
          }
      break;
      }
        
		case SMMNODE_TYPE_FESTIVAL: {// 축제 카드 칸에 도착
			if (festival_nr>0){
			
			int random_festival = rand() % festival_nr; // 축제카드의 개수에 맞게 난수 생성
    		void* festivalCard = smmdb_getData(LISTNO_FESTCARD, random_festival); // 축제 데이터 가져오기

    		printf("축제 카드 찬스!\n");
    		printf("당신이 즐길 축제 카드는 %s입니다.\n", smmObj_getNodeName(festivalCard));
    	}
    	break;
		}
		
	}
}


void goForward(int player, int step) {
    int nextNode = cur_player[player].position + step;
    cur_player[player].position = nextNode % board_nr;


    if (nextNode >= board_nr) { // 플레이어가 집을 지나치거나 집에 도착했을 경우 
        printf("집에서 쉬었습니다! 당신은 에너지를 회복했습니다.\n", home_energy);
        cur_player[player].energy += home_energy; 
    }
    
    if (cur_player[player].accumCredit >= GRADUATE_CREDIT) { // 플레이어가 졸업 학점을 다 채웠을 경우 
        cur_player[player].position = 0; // 집으로 이동 
            }
            
    if((cur_player[player].flag_graduate == 1 && cur_player[player].position!=0)||cur_player[player].flag_graduate==0)
    {
    	int i;
        for (i = 0; i < step; i++) // 플레이어가 지나온 칸들을 차례대로 출력 
		{
            int pos = (cur_player[player].position - step + i + smmdb_len(LISTNO_NODE)) % smmdb_len(LISTNO_NODE);
            printf("-> %s\n", smmObj_getNodeName(smmdb_getData(LISTNO_NODE, pos)));
        }
        
        // 플레이어가 최종 도착한 위치와 노드 출력 
        int pos = (cur_player[player].position - step + i + smmdb_len(LISTNO_NODE)) % smmdb_len(LISTNO_NODE);
		printf("\n%s 플레이어는 %s(node %i)로 이동합니다.\n", 
	    cur_player[player].name, smmObj_getNodeName(smmdb_getData(LISTNO_NODE, pos)), cur_player[player].position);
} }






int main(int argc, const char * argv[]) {
    
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    int i;
    int initEnergy = 20;
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
    
    
    //2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1;
    }
    
    printf("\n\nReading food card component......\n");
    while (fscanf(fp, "%s %i", name, &energy) == 2) //read a food parameter set
    {
    	
        //store the parameter set
        void *boardObj = smmObj_genObject(name, 0, 0, 0, energy, 0);
        smmdb_addTail(LISTNO_FOODCARD, boardObj);
        food_nr++;
    }
    fclose(fp);
    printf("Total number of food cards : %i\n", food_nr);
    
    for (i = 0; i < food_nr; i++)
	{
		void *boardObj=smmdb_getData(LISTNO_FOODCARD, i);
    	printf("card %i: %s, energy %i\n", i, smmObj_getNodeName(boardObj), smmObj_getNodeEnergy(boardObj));
	}
    
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    while (fscanf(fp, "%s", name) == 1) //read a festival card string
    {
    	void *boardObj = smmObj_genObject(name, 0, 0, 0, 0, 0);
        smmdb_addTail(LISTNO_FESTCARD, boardObj);
        festival_nr++;
        //store the parameter set
    }
    fclose(fp);
    printf("Total number of festival cards : %i\n", festival_nr);
    for(i=0; i< festival_nr; i++){
    	void *boardObj = smmdb_getData(LISTNO_FESTCARD,i);
    	printf("card %i : %s\n", i, smmObj_getNodeName(boardObj));
	}
    
    
    //2. Player configuration ---------------------------------------------------------------------------------
    
    do
    {
        //input player number to player_nr
        printf("\n플레이어의 수를 입력하세요.: ");
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
        printf("주사위 결과 : %i\n", die_result);
        
        //4-3. go forward
        goForward(turn, die_result);

    // 플레이어가 졸업 학점을 달성하고 집에 도착했을 경우 
    if (cur_player[turn].accumCredit >= GRADUATE_CREDIT && cur_player[turn].position == 0) {
        
		printf("%s 플레이어가 졸업 학점을 달성하고 집에 도착하여 게임이 종료됩니다.\n", cur_player[turn].name);
		printf("\n\n-------------------------------------------------------------------------------------------\n");
        printf("---------------------------- 승리한 플레이어가 수강한 강의와 학점---------------------------");
        printf("\n-------------------------------------------------------------------------------------------\n\n");
        printGrades(turn); // 수강한 전체 강의, 학점 출력 
        break; // 게임 종료 
    }

    //4-4. take action at the destination node of the board
        actionNode(turn);
        
    //4-5. next turn
    turn = (turn + 1)%player_nr;
    }
    
    
    free(cur_player);
    system("PAUSE");
    return 0;
} 

