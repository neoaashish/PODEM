#include "input.h"

//D = 3 and D_bar = 4

int and_gate[5][5] = {{0, 0, 0, 0, 0}, {0, 1, 2, 3, 4}, {0, 2, 2, 2, 2}, {0, 3, 2, 3, 0}, {0, 4, 2, 0, 4}};
int or_gate[5][5]  = {{0, 1, 2, 3, 4}, {1, 1, 1, 1, 1}, {2, 1, 2, 2, 2}, {3, 1, 2, 3, 1}, {4, 1, 2, 1, 4}};
int not_gate[5]    = {1, 0, 2, 4, 3};
int xor_gate[5][5] = {{0, 1, 2, 3, 4}, {1, 0, 2, 4, 3}, {2, 2, 2, 2, 2}, {3, 4, 2, 0, 1}, {4, 3, 2, 1, 0}};
int buffer[5]      = {0, 1, 2, 3, 4};
int non_control[11] = {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0};//1s for AND and NAND, 0 for others

int ReadPat(FILE *Pat, PATTERN *Patterns)
{
	char line[Mlin];
	// read line by line
	//char pat_vec[Mpt][1];
	int i = 0;
	int n, k;
	while (!feof(Pat))
	{
		fgets(line, Mlin, Pat); // line - pointer to an initialized string in which characters are copied. Mlin - max number of characters to copy.
		for (n = 0; n < strlen(line); n++)
		{
			if (line[n] == '0')
				Patterns[i].vector[n] = '0';
			else if (line[n] == '1')
				Patterns[i].vector[n] = '1';
			else if (line[n] == 'x' || line[n] == 'X')
				Patterns[i].vector[n] = '2';
		}
		
		i++;
	}
	for (n = 0; n < 3; n++)
	{
		printf("\n");
		/*the number of patterns to display in debug*/
		printf("%s", Patterns[n].vector);
		printf("\n");
	}

	return i;
}

void LogicSimulate(FILE *oplogicsim, GATE *Node, PATTERN *pat_line, int Total_Patterns, int Total_gate)
{
	int i, j;
	int simulated_output[Mpo];
		
	for (i = 0; i < Total_Patterns; i++)
	{
		int n = 0;
		int cnt_i = 0;

		for (j = 0; j <= Total_gate; j++)
		{
			if (Node[j].Type == INPT)
			{
				Node[j].Val = pat_line[i].vector[n] - '0'; // converted char to integer - works for 1 digit
				n++;
			}
			else
			{
				gate_simulate(Node, j); 
			}			
			
			if (Node[j].Po == 1)
			{
				simulated_output[cnt_i] = Node[j].Val;
				cnt_i++;
			}
			
		}
		int k = 0;
		for(k=0; k<cnt_i; k++)
		{
			fprintf(oplogicsim, "%d", simulated_output[k]);
		}
		fprintf(oplogicsim, "\n");
	}
	
}


int get_objective (GATE *Node, OBJ_BACK S_A_F, LIST **D_Frontier, OBJ_BACK *get_obj)
{
	LIST *gate_inputs;
	int d_id,invert;
	
	if(Node[S_A_F.line_n].Val != D && Node[S_A_F.line_n].Val != Db)//if fault is not activated already
	{
		invert = not_gate[S_A_F.line_val];
		get_obj->line_n = S_A_F.line_n;
		get_obj->line_val = invert; //our objective is to activate the fault, i'e. to bring in ~x in place of x for that gate
		return SUCCESS;
	}
	//else // if the saf is already activated
	if (*D_Frontier == NULL)
	{
		return FAILURE;
	}
	d_id = (*D_Frontier) ->Id;//not sure how 
		
	while(!check_D_Frontier(Node,d_id) && *D_Frontier!=NULL)
	{
		d_id = RemoveDfrontier(Node, D_Frontier);
		if (d_id == FAILURE)
		break;
	}

	if(*D_Frontier != NULL)
	{
		gate_inputs = Node[d_id].Fin;
		while(gate_inputs != NULL)
		{
			if (Node[gate_inputs ->Id].Val == 2)
			break;
			gate_inputs = gate_inputs -> Next;
		}
		if (gate_inputs != NULL)
		{
			get_obj->line_n = gate_inputs->Id;
		    get_obj->line_val = non_control[Node[d_id].Type];
		}
		else
		{
			printf("ERROR: INVALID D_FRONTIER - Exiting ...");
			exit (0);
		
		}
	}
	else
	{
		return FAILURE;
	}
	return SUCCESS;
	
}



OBJ_BACK back_trace(GATE *circuit_info, int gate_id, int gate_val)
{
	OBJ_BACK return_backtrace;
	LIST *fan_in_list; //here, the gate_val is first the fault value,the output is the value assigned at PI
	int num_inversion = 0;
	while(circuit_info[gate_id].Type != INPT)
	{	
		int gate_id_temp = gate_id;
		if(circuit_info[gate_id].Type == NAND || circuit_info[gate_id].Type == NOR || circuit_info[gate_id].Type == NOT)
		{
			num_inversion++;
		}
		fan_in_list = circuit_info[gate_id].Fin;
		while(fan_in_list != NULL)
		{
			if(circuit_info[fan_in_list -> Id].Val == 2)
			{
				gate_id = fan_in_list ->Id;
				break; 
			}
			fan_in_list = fan_in_list -> Next;
		}
		if(gate_id_temp == gate_id)
		{
			printf("Error in Backtrace!");
			return_backtrace.line_n = 0;
			return_backtrace.line_val = 9;
			return return_backtrace;
			
		}

	}
		if(num_inversion %2 == 0)
		{
			return_backtrace.line_n = gate_id;
			return_backtrace.line_val = gate_val;
		}
		else
		{
			return_backtrace.line_n = gate_id;
			return_backtrace.line_val = not_gate[gate_val];
		}

	return return_backtrace;

}

void reset_ckt(GATE *ckt_info, int total_gates)
{
	int id;
	for (id=1; id<=total_gates;id++)
	{
		ckt_info[id].Val = 2;
	}
}

int check_D_Frontier(GATE *Node, int gate_id)
{

	LIST *fan_in_list;
	if(Node[gate_id].Val == 2)
	{
		fan_in_list = Node[gate_id].Fin;
		while(fan_in_list != NULL)
		{
			if((Node[fan_in_list -> Id].Val == D)||(Node[fan_in_list -> Id].Val == Db))
			{
				return SUCCESS;
			}
			else
			fan_in_list = fan_in_list -> Next;
		}			
	}
	return FAILURE;
	
}

void update_D_Frontier(GATE *Node, int gate_id, LIST **D_Frontier)
{
	InsertEle(D_Frontier, gate_id);
}

int RemoveDfrontier(GATE *Node, LIST **D_Frontier)
{
	int d_id, new_d_id;
	d_id =(*D_Frontier) ->Id;
	DeleteEle(D_Frontier,d_id);
	if(*D_Frontier == NULL)
	return FAILURE;
	new_d_id = (*D_Frontier) ->Id;
	return new_d_id;
}

int Logic_imply(GATE *Node, int ip_gate_id, int ip_gate_val, int Total_gate, LIST **D_Frontier, OBJ_BACK S_A_F)
{
	int j, current_val;
	//int simulated_output;
	int status = SUCCESS;

	LIST *imply_list = NULL; 

	Node[ip_gate_id].Val = ip_gate_val;

	if(fault_injection(Node, S_A_F, ip_gate_id) == FAILURE)	status=FAILURE; // this checks if fault can be injected in the input when needed
    
	update_imply_list(&imply_list, Node[ip_gate_id].Fot);
	
	while (imply_list!=NULL)
	{
	//i am at the input gate coz backtrace sent me here, so no need to consider case if this is an input gate
		
		j = imply_list ->Id; // first gate is the ip gate

		current_val = Node[j].Val;

		gate_simulate(Node, j);	
			
		if(fault_injection(Node, S_A_F, j) == FAILURE) status=FAILURE; // this checks if fault can be injected in the j gate when needed

		if(check_D_Frontier(Node, j))
		{
			update_D_Frontier(Node, j, D_Frontier);
		}	

		if (current_val != Node[j].Val)//Node[j] value has chaged , or has it?
		{
			update_imply_list (&imply_list, Node[j].Fot);
		}
		
		DeleteEle(&imply_list,j);
	}
	return status;

}

int fault_injection(GATE *Node, OBJ_BACK S_A_F, int gate_id)
{
	if(gate_id == S_A_F.line_n)
	{
		if(S_A_F.line_val == not_gate[Node[S_A_F.line_n].Val]) //fault sensitized
		{
			Node[S_A_F.line_n].Val = 3 + (S_A_F.line_val);
		}
		else if(S_A_F.line_val == Node[S_A_F.line_n].Val)
		{
			return FAILURE;
		}
	}
	return SUCCESS;
}


void update_imply_list(LIST **imply_list, LIST *imply_fanouts)
{
	while(imply_fanouts!= NULL)
	{
		InsertEle(imply_list, imply_fanouts->Id);
		imply_fanouts = imply_fanouts ->Next;
	}
}

void PODEM(GATE *Node, int Total_gate, LIST **D_Frontier, OBJ_BACK S_A_F, FILE *PODEM_PATTERNS)
{
	int result;	
	
	reset_ckt (Node,Total_gate);
	FreeList (D_Frontier);
	result = Podem_Recursion(Node, Total_gate, D_Frontier, S_A_F);
	
	if(result==SUCCESS)
	{
		Print_IP_Vals(Node, Total_gate, PODEM_PATTERNS);
	}
	else
	{
		printf("ERRROR: Line %d S_A_%d IS UNTESTABLE\n", S_A_F.line_n, S_A_F.line_val);
		untestable_faults ++;
		getchar();
	}	
}


int Podem_Recursion(GATE *Node, int Total_gate, LIST **D_Frontier, OBJ_BACK S_A_F)
{
	int i,state;
	OBJ_BACK FROM_OBJECTIVE, BACKTRACED_IP_LINE;
	BACKTRACED_IP_LINE.line_n = -2;
	BACKTRACED_IP_LINE.line_val = -5;
	
	for(i=1;i<=Total_gate;i++)
	{
		if(Node[i].Po==1)
		{
			if(Node[i].Val==3 || Node[i].Val==4)
			return SUCCESS;
		}
	}// success id returned if POut already has D or Dbar
	state = get_objective(Node, S_A_F, D_Frontier, &FROM_OBJECTIVE);

	if(state==FAILURE)	return FAILURE; //basically checks if DFrontier is NULL//
	//coz that's when backtrace gies a FAILURE//
	BACKTRACED_IP_LINE = back_trace(Node,FROM_OBJECTIVE.line_n,FROM_OBJECTIVE.line_val);

	if (BACKTRACED_IP_LINE.line_n == 0) return FAILURE;

	if(Logic_imply(Node, BACKTRACED_IP_LINE.line_n, BACKTRACED_IP_LINE.line_val,Total_gate,D_Frontier,S_A_F)==SUCCESS)
	{
		if(Podem_Recursion(Node,Total_gate,D_Frontier,S_A_F)==SUCCESS)
		return SUCCESS;
	}
	
	BACKTRACED_IP_LINE.line_val=not_gate[BACKTRACED_IP_LINE.line_val];//Backtrace if necessary
	//Backtra9cing to the previous decision and changing 1 to 0 or 0 to 1


	if(Logic_imply(Node, BACKTRACED_IP_LINE.line_n, BACKTRACED_IP_LINE.line_val,Total_gate,D_Frontier,S_A_F)==SUCCESS)
	{
		if(Podem_Recursion(Node,Total_gate,D_Frontier,S_A_F)==SUCCESS)
		return SUCCESS;
	}
	//If this also does not work, then basically set the backtraced 
	//value to X and move ahead declaring failure in implication
	BACKTRACED_IP_LINE.line_val=2;

	Logic_imply(Node,Total_gate,BACKTRACED_IP_LINE.line_n,BACKTRACED_IP_LINE.line_val, D_Frontier, S_A_F);
	
	return FAILURE;
}

void Print_IP_Vals(GATE *Node, int Total_gate, FILE *PODEM_PATTERNS)
{
	int i;
	for(i=1;i<=Total_gate;i++)
	{
		if(Node[i].Type==INPT)
		{
			if(Node[i].Val==3)
			{
				Node[i].Val=1;
				fprintf(PODEM_PATTERNS,"%d",Node[i].Val);
			}
			
			else if(Node[i].Val==4)
			{
				Node[i].Val=0;
				fprintf(PODEM_PATTERNS,"%d",Node[i].Val);
			}
			
			else if(Node[i].Val==2)
			{
				fprintf(PODEM_PATTERNS,"x");
			}
				
			else
			{
				fprintf(PODEM_PATTERNS,"%d",Node[i].Val);
			}
		}
	}

	fprintf(PODEM_PATTERNS,"\n");

}



void gate_simulate(GATE *Node, int j)//j is the gate_id
{
	int i, x = 0, y = 0;
	int l = 0;
	LIST *temp;
	int sim_and, sim_nand, sim_or, sim_nor, sim_xor, sim_inverter, sim_xnor, sim_buffer;
	int two_inpt_output,input_count;
	int simulated_output;

	if (Node[j].Type == NAND)
	{
		temp = Node[j].Fin;
		input_count = 2;

		while (temp->Next != NULL)
		{
			x = Node[temp->Id].Val;
			y = Node[temp->Next->Id].Val;

			if (input_count == 2)
			{
				sim_and = and_gate[x][y];
			}
			else
			{
				sim_and = and_gate[two_inpt_output][y];
			}
			input_count++;
			Node[j].Val = sim_and;
			two_inpt_output = Node[j].Val;
			temp = temp->Next;
		}
		sim_nand = not_gate[sim_and];//careful here !
		Node[j].Val = sim_nand;
	}
	else if (Node[j].Type == FROM)
	{
		temp = Node[j].Fin;
		Node[j].Val = Node[temp->Id].Val;
	}
	else if (Node[j].Type == NOR)
	{
		temp = Node[j].Fin;
		input_count = 2;

		while (temp->Next != NULL)
		{
			x = Node[temp->Id].Val;
			y = Node[temp->Next->Id].Val;
			if (input_count == 2)
			{
				sim_or = or_gate[x][y];
			}
			else
			{
				sim_or = or_gate[two_inpt_output][y];
			}
			input_count++;
			Node[j].Val = sim_or;
			two_inpt_output = Node[j].Val;
			temp = temp->Next;
		}
		sim_nor = not_gate[sim_or];
		Node[j].Val = sim_nor;
	}
	else if (Node[j].Type == AND)
	{
		temp = Node[j].Fin;
		input_count = 2;

		while (temp->Next != NULL)
		{
			x = Node[temp->Id].Val;
			y = Node[temp->Next->Id].Val;
			if (input_count == 2)
			{
				sim_and = and_gate[x][y];
			}
			else
			{
				sim_and = and_gate[two_inpt_output][y];
			}
			input_count++;
			Node[j].Val = sim_and;
			two_inpt_output = Node[j].Val;
			temp = temp->Next;
		}
		Node[j].Val = sim_and;
	}

	else if (Node[j].Type == OR)
	{
		temp = Node[j].Fin;
		input_count = 2;

		while (temp->Next != NULL)
		{
			x = Node[temp->Id].Val;
			y = Node[temp->Next->Id].Val;
			if (input_count == 2)
			{
				sim_or = or_gate[x][y];
			}
			else
			{
				sim_or = or_gate[two_inpt_output][y];
			}
			input_count++;
			Node[j].Val = sim_or;
			two_inpt_output = Node[j].Val;
			temp = temp->Next;
		}
		Node[j].Val = sim_or;
	}
	else if (Node[j].Type == XOR)
	{
		temp = Node[j].Fin;
		input_count = 2;

		while (temp->Next != NULL)
		{
			x = Node[temp->Id].Val;
			y = Node[temp->Next->Id].Val;
			if (input_count == 2)
			{
				sim_xor = xor_gate[x][y];
			}
			else
			{
				sim_xor = xor_gate[two_inpt_output][y];
			}
			input_count++;
			Node[j].Val = sim_xor;
			two_inpt_output = Node[j].Val;
			temp = temp->Next;
		}
		Node[j].Val = sim_xor;
	}
	else if (Node[j].Type == NOT)
	{
		temp = Node[j].Fin;
		x = Node[temp->Id].Val;
		sim_inverter = not_gate[x];
		Node[j].Val = sim_inverter;
	}
	else if (Node[j].Type == XNOR)
	{
		temp = Node[j].Fin;
		input_count = 2;

		while (temp->Next != NULL)
		{
			x = Node[temp->Id].Val;
			y = Node[temp->Next->Id].Val;
			if (input_count == 2)
			{
				sim_xor = xor_gate[x][y];
			}
			else
			{
				sim_xor = xor_gate[two_inpt_output][y];				
			}
			input_count++;
			Node[j].Val = sim_xor;
			two_inpt_output = Node[j].Val;
			temp = temp->Next;
		}
		sim_xnor = not_gate[sim_xor];
		Node[j].Val = sim_xnor;
	}

	else if(Node[j].Type == BUFF)
	{
		temp = Node[j].Fin;
		x = Node[temp->Id].Val;
		sim_buffer = buffer[x];
		Node[j].Val = sim_buffer;
	}	
	else{};		
}


/***************************************************************************************************/