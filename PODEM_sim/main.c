#include "input.h"
/***************************************************************************************************
Command Instructions
***************************************************************************************************/
//To Compile: make
//To Run: ./project c17.isc 
/***************************************************************************************************
 Main Function
***************************************************************************************************/
int main(int argc,char **argv)
{
//FILE *Isc,*Pat,*Res, *oplogicsim;   //File pointers used for .isc, .pattern, logicsim and .res files
FILE *Isc, *PODEM_PATTERNS;           //File pointers used for reading isc and writing to podem_patterns files
int Npi,Npo,Tgat;                     //Tot no of PIs,Pos,Maxid,Tot no of patterns in.vec,.fau
GATE *Node;                           //Structure to store the ckt given in .isc file 

//clock_t Start,End;                    //Clock variables to calculate the Cputime
//double Cpu;                           //Total cpu time

int Tpat;
//char pat_array[Mpt];
//int Patterns[Mpt][Mpi];
//int Patterns[Mpt];

static PATTERN Patterns[Mpt];
OBJ_BACK return_back_trace;
OBJ_BACK S_A_F, get_obj;
LIST *D_Frontier = NULL;
//LIST *imply_list = NULL;


/****************PART 1.-Read the .isc file and store the information in Node structure***********/
Npi=Npo=Tgat=0;                                //Intialize values of all variables
Isc=fopen(argv[1],"r");                        //File pointer to open .isc file 

Node=(GATE *) malloc(Mnod * sizeof(GATE));     //Dynamic memory allocation for Node structure

Tgat=ReadIsc(Isc,Node);                        //Read .isc file and return index of last node. Need to store the info in structure
fclose(Isc);                                   //Close file pointer for .isc file
PrintGats(Node,Tgat);                          //Print the information of each active gate in Node structure after reading .isc file
CountPri(Node,Tgat,&Npi,&Npo);                 //Count the No of Pis and Pos
printf("\n\nNpi: %d Npo: %d\n",Npi,Npo);       //Print the no of primary inputs and outputs
/***************************************************************************************************/

/*

**** WE DO NOT NEED TO READ THE PATTERNS AND LOGIC SIMULATE HERE****

Pat=fopen(argv[2],"r");                        //File pointer to open .pattern file 
Tpat=ReadPat(Pat,Patterns);                    // Returns total number of I/p vectors - vecs need to be stored in an array in the function
printf("No. of i/p patterns = %d \n", Tpat);                    //Read .pattern file and return index of last node
fclose(Pat); 

oplogicsim=fopen(argv[3],"w+");  // we write the simulated value in pat_2
LogicSimulate(oplogicsim, Node, Patterns, Tpat, Tgat);
fclose(oplogicsim); 

*/

PODEM_PATTERNS = fopen(argv[2], "w");
int i,j, line_count = 0;
untestable_faults = 0;

for(i = 1; i<=Tgat; i++)
{
    if(Node[i].Type==0)
    continue;

	for(j = 0; j<2; j++)
    {
		S_A_F.line_n=i;
		S_A_F.line_val=j;
		//fprintf(PODEM_PATTERNS, "SAL: %d  SAF: %d\n", i,j);
		PODEM(Node, Tgat, &D_Frontier, S_A_F, PODEM_PATTERNS);
	} 
    line_count++;
}

float Total_SA_Lines;
float coverage, temp, ss;
Total_SA_Lines = line_count*2;

fprintf(PODEM_PATTERNS, "TOTAL NUMBER OF SA LINES = %.3f \n", Total_SA_Lines);
fprintf(PODEM_PATTERNS, "TOTAL NUMBER OF UNTESTABLE FAULTS = %.3f \n",untestable_faults);
ss = Total_SA_Lines - untestable_faults;
temp = (Total_SA_Lines - untestable_faults)/(Total_SA_Lines);
//printf("Total tested faults = %.3f and coverage is %.3f", ss, temp); //getchar();
coverage = temp*100;
fprintf(PODEM_PATTERNS, "FAULT COVERAGE = %.3f Percent", coverage);

fclose(PODEM_PATTERNS);
//reset_ckt(Node, Tgat);
// S_A_F.line_n = 9;
// S_A_F.line_val = 0;

// get_objective(Node,S_A_F.line_n,S_A_F.line_val,&D_Frontier,&get_obj);
// printf("objective_value=%d\n objective_gate=%d\n",get_obj.line_val, get_obj.line_n);

// //checking Back_Trace
// back_trace(Node, get_obj.line_n, get_obj.line_val, &return_back_trace);

// printf("IP node line = %d",return_back_trace.line_n);
// printf("\n");
// printf("IP node val = %d",return_back_trace.line_val);
// printf("\n");

// Logic_imply(Node, return_back_trace.line_n, return_back_trace.line_val, Tgat, &D_Frontier, S_A_F);
// printf("\n D_Frontier in this step is : ");
// PrintList(D_Frontier);

PrintGats(Node, Tgat);

/***************************************************************************************************/

ClearGat(Node,Tgat);
free(Node);                                      //Clear memeory for all members of Node
return 0;
}//end of main
/****************************************************************************************************************************/

