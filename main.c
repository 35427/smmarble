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
        int accumCredit; // ������ ���� 
        int flag_graduate; // ���� ����  
        int experience;  
        char currentLecture[MAX_CHARNAME]; // ���� ��� ���� �̸�
        int success;
        
        // ������ ���� ������ ������ �迭
    	char takenCourses[100][MAX_CHARNAME];
    	int numCoursesTaken;
        
} player_t;



static player_t *cur_player;
//static player_t cur_player[MAX_PLAYER];


// �Լ� ����
int checkCourseTaken(int player, char *lectureName);

// ������ ���� �ߺ� üũ �Լ�
int checkCourseTaken(int player, char *lectureName) {
    int i;
    for (i = 0; i < cur_player[player].numCoursesTaken; ++i) {
        if (strcmp(cur_player[player].takenCourses[i], lectureName) == 0) {
            // �̹� ������ ������ ���
            return 1; // �ߺ� ����
        }
    }
    return 0; // �������� ���� ����
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


void printGrades(int player) // �÷��̾��� ��ü ������ ����� 
{
     int i;
     void *gradePtr;
     for (i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++)
     {
         gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
         printf("%s : %s\n", smmObj_getNodeName(gradePtr), smmObj_getGradeName(smmObj_getNodeGrade(gradePtr)));
     }
}

void printPlayerStatus(void) // �÷��̾��� ���¸� ����� 
{
     int i;
     printf("\n\n====================================== PLAYER STATUS ======================================\n");
     for (i=0;i<player_nr;i++)
     {
         printf("%s | ���� %i, ������ %i, ��ġ %i\n", 
                      cur_player[i].name,
                      cur_player[i].accumCredit,
                      cur_player[i].energy,
                      cur_player[i].position);
     }
     printf("===========================================================================================\n\n");
}

void generatePlayers(int n, int initEnergy) //�� �÷��̾ ���� 
{
     int i;
     //n time loop
     for (i=0;i<n;i++)
     {
         //input name
         printf("�÷��̾� %i�� �̸��� �Է��ϼ���.: ", i); 
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


int rolldie(int player) // �ֻ����� ���� 
{
    char c;
    if (cur_player[player].position == SMMNODE_TYPE_LECTURE){
	 
    printf("�ƹ� Ű�� ���� �ֻ����� ��������.(������ ������ g�� ��������.): ");
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
        case SMMNODE_TYPE_LECTURE: // ���� ĭ�� ���� 
            {
            	int lec;
				printf("���Ǹ� �����Ϸ��� 1��, �������� �������� 2�� �Է��ϼ���. :\n");
				scanf("%d", &lec);
				fflush(stdin);
				
				if (lec == 1){
					if (cur_player[player].energy >= smmObj_getNodeEnergy(boardPtr)){
						// �ش� ���ǰ� �̹� ������ �������� üũ
               		 if (checkCourseTaken(player, smmObj_getNodeName(boardPtr)) == 1) {
                    printf("�̹� ������ �����Դϴ�.\n");}
					
					else{
					cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
            		cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
            		
					grade = rand()%smmObjGrade_all;
					gradePtr = smmObj_genObject(name, smmObjType_grade, 0, smmObj_getNodeCredit(boardPtr), 0, (smmObjGrade_e)grade);
                    smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
                    
                    
            		strcpy(cur_player[player].currentLecture, smmObj_getNodeName(boardPtr));
					printf("���� ������ %d, ���� �������� %d, ������ ���Ǵ� %s�Դϴ�.\n",smmObj_getNodeCredit(boardPtr), cur_player[player].energy,cur_player[player].currentLecture);
					// ������ ���Ǹ� �߰�
                    strcpy(cur_player[player].takenCourses[cur_player[player].numCoursesTaken], smmObj_getNodeName(boardPtr));
                    cur_player[player].numCoursesTaken++;
				}	}
				else{
					printf("�������� ������ ���Ǹ� ������ �� �����ϴ�.\n");
				}
            }
        }
       	  break;
			
          

        case SMMNODE_TYPE_RESTAURANT: //�Ĵ� ĭ�� ���� 
            cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
            printf("������ �԰� �������� �����߽��ϴ�. ���� �������� %d�Դϴ�.\n", cur_player[player].energy);
            break;

        case SMMNODE_TYPE_LABORATORY: //����� ĭ�� ���� 
            if (cur_player[player].experience) {
                printf("ȭ����! ����� �����.\n");
				cur_player[player].position = 8; // ����� ĭ���� �̵� 
            }
			else {
                printf("�׷����� ����ð��� �ƴ϶� ����Ƿ� ���� �ʾƵ� �˴ϴ�! �����̳׿�. \n");
            }
            break;


    	case SMMNODE_TYPE_GOTOLAB: { // ����Ƿ� ���� �ϴ� ĭ�� ���� 
    		printf("����Ƿ� �����մϴ�. ����� �����!\n");
   			int success = rand() % MAX_DIE + 1;
   			
    		printf("\n%d �̻��� ���� ������ ���� �����Դϴ�.\n", success);
 			printf("�ƹ� Ű�� ���� �ֻ����� �� �� �� ��������.\n");
    		getchar(); // �ƹ� Ű�� ������ ���
    		int attempt = rolldie(player); // �ֻ��� ������
    		printf("�ֻ��� ���: %d\n", attempt);
    		
        	if (attempt < success) { // �������� ��� 
           		printf("���迡 �����Ͽ� ���� �Ͽ� �ٽ� �����ؾ��մϴ�.\n");
            	cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
        		
    		} 
			else if (attempt >= success) { // �������� ��� 
            	printf("���迡 ������ ����ǿ��� Ż���մϴ�!\n");
            	cur_player[player].flag_graduate = 0;
        	}
        break;
		}

        case SMMNODE_TYPE_FOODCHANCE: { //���� ���� ĭ�� ���� 
           if (food_nr>0){
            
           int random_food = rand()%food_nr; // ����ī�� ������ �°� ���� ���� 
           
           void* randomFoodCard = smmdb_getData(LISTNO_FOODCARD, random_food); // ����ī�� ������ �������� 
           printf("���� ī�� ����!\n");
          cur_player[player].energy += smmObj_getNodeEnergy(randomFoodCard);
          printf("����� %s��(��) �԰� �������� ��� �� �������� %d�� �Ǿ����ϴ�.\n", smmObj_getNodeName(randomFoodCard), cur_player[player].energy);
          }
      break;
      }
        
		case SMMNODE_TYPE_FESTIVAL: {// ���� ī�� ĭ�� ����
			if (festival_nr>0){
			
			int random_festival = rand() % festival_nr; // ����ī���� ������ �°� ���� ����
    		void* festivalCard = smmdb_getData(LISTNO_FESTCARD, random_festival); // ���� ������ ��������

    		printf("���� ī�� ����!\n");
    		printf("����� ��� ���� ī��� %s�Դϴ�.\n", smmObj_getNodeName(festivalCard));
    	}
    	break;
		}
		
	}
}


void goForward(int player, int step) {
    int nextNode = cur_player[player].position + step;
    cur_player[player].position = nextNode % board_nr;


    if (nextNode >= board_nr) { // �÷��̾ ���� ����ġ�ų� ���� �������� ��� 
        printf("������ �������ϴ�! ����� �������� ȸ���߽��ϴ�.\n", home_energy);
        cur_player[player].energy += home_energy; 
    }
    
    if (cur_player[player].accumCredit >= GRADUATE_CREDIT) { // �÷��̾ ���� ������ �� ä���� ��� 
        cur_player[player].position = 0; // ������ �̵� 
            }
            
    if((cur_player[player].flag_graduate == 1 && cur_player[player].position!=0)||cur_player[player].flag_graduate==0)
    {
    	int i;
        for (i = 0; i < step; i++) // �÷��̾ ������ ĭ���� ���ʴ�� ��� 
		{
            int pos = (cur_player[player].position - step + i + smmdb_len(LISTNO_NODE)) % smmdb_len(LISTNO_NODE);
            printf("-> %s\n", smmObj_getNodeName(smmdb_getData(LISTNO_NODE, pos)));
        }
        
        // �÷��̾ ���� ������ ��ġ�� ��� ��� 
        int pos = (cur_player[player].position - step + i + smmdb_len(LISTNO_NODE)) % smmdb_len(LISTNO_NODE);
		printf("\n%s �÷��̾�� %s(node %i)�� �̵��մϴ�.\n", 
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
        printf("\n�÷��̾��� ���� �Է��ϼ���.: ");
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
        printf("�ֻ��� ��� : %i\n", die_result);
        
        //4-3. go forward
        goForward(turn, die_result);

    // �÷��̾ ���� ������ �޼��ϰ� ���� �������� ��� 
    if (cur_player[turn].accumCredit >= GRADUATE_CREDIT && cur_player[turn].position == 0) {
        
		printf("%s �÷��̾ ���� ������ �޼��ϰ� ���� �����Ͽ� ������ ����˴ϴ�.\n", cur_player[turn].name);
		printf("\n\n-------------------------------------------------------------------------------------------\n");
        printf("---------------------------- �¸��� �÷��̾ ������ ���ǿ� ����---------------------------");
        printf("\n-------------------------------------------------------------------------------------------\n\n");
        printGrades(turn); // ������ ��ü ����, ���� ��� 
        break; // ���� ���� 
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

