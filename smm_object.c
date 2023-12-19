//
//  smm_node.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include "smm_common.h"
#include "smm_object.h"
#include <string.h>

#define MAX_NODETYPE    7
#define MAX_GRADE       9
#define MAX_NODE        100


static char smmNodeName[SMMNODE_TYPE_MAX][MAX_CHARNAME] = { 
       "강의실",
       "실험실",
       "실험실로이동",
       "집",
       "음식찬스",
       "축제시간",
       "식당"
};

char* smmObj_getTypeName(int type) // 노드의 타입 이름을 가져옴 
{
      return (char*)smmNodeName[type];
}



static char smmGradeName[9][MAX_CHARNAME] = {
	"Ap",
	"A0",
	"Am",
	"Bp",
	"B0",
	"B-",
	"Cp",
	"C0",
	"Cm"
};

char* smmObj_getGradeName(smmObjGrade_e grade) // 노드의 성적을 가져옴 
{
      return (char*)smmGradeName[grade];
}

//1. ????u ???? ????  
typedef struct smmObject {
       char name[MAX_CHARNAME];
       smmObjType_e objType; 
       smmObjGrade_e grade; 
       int type;
       int credit;
       int energy;
       
} smmObject_t;

//static smmObject_t smm_node[MAX_NODE];
//static int smmObj_noNode = 0;

//3. ???? ??? ????  
//object generation
void* smmObj_genObject(char* name, smmObjType_e objType, int type, int credit, int energy, smmObjGrade_e grade)
{    
    smmObject_t* ptr;
    
    ptr = (smmObject_t*)malloc(sizeof(smmObject_t));
    
    strcpy(ptr->name, name);
    ptr->objType = objType;
    ptr->type = type;
    ptr->credit = credit;
    ptr->energy = energy;
    ptr->grade = grade;
    
    return ptr;
}

//3. 관련 함수 변경 
char* smmObj_getNodeName(void* obj) // 노드의 이름을 가져옴 
{
    smmObject_t* ptr = (smmObject_t*)obj;
    
    return ptr->name;
}

int smmObj_getNodeType(void* obj) // 노드의 타입을 가져옴 
{
    smmObject_t* ptr = (smmObject_t*)obj;
    return ptr->type;
}

int smmObj_getNodeCredit(void* obj) // 노드의 학점을 가져옴
{
    smmObject_t* ptr = (smmObject_t*)obj;
    return ptr->credit;
}

int smmObj_getNodeEnergy(void* obj) // 노드의 에너지를 가져옴 
{
    smmObject_t* ptr = (smmObject_t*)obj;
    return ptr->energy;
}
int smmObj_getNodeGrade(void* obj) // 노드의 성적을 가져옴 
{
	smmObject_t* ptr= (smmObject_t*)obj;
    return ptr->grade;
}

