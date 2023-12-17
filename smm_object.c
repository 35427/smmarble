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
       "?????",
       "???????",
       "?????",
       "??",
       "?????????",
       "????????",
       "???????"
};

char* smmObj_getTypeName(int type) // 노드의 타입을 가져옴 
{
      return (char*)smmNodeName[type];
}


typedef enum smmNode{
    lecture,
    restaurant,
    laboratory,
    home,
    gotolab,
    foodChance,
    festival
} smmNode_e;

typedef enum smmObjGrade { // 노드의 성적을 나타냄 
    
    smmObjGrade_Ap = 0,
    smmObjGrade_A0,
    smmObjGrade_Am,
    smmObjGrade_Bp,
    smmObjGrade_B0,
    smmObjGrade_Bm,
    smmObjGrade_Cp,
    smmObjGrade_C0,
    smmObjGrade_Cm
}smmObjGrade_e;

//1. ????u ???? ????  
typedef struct smmObject {
       char name[MAX_CHARNAME];
       smmObjType_e objType; 
       int type;
       int credit;
       int energy;
        grade;
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


