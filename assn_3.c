#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <limits.h>
int choice;	//0 for --basic; 1 for multistep; 2 for replacement
int input[1000];
int output[1000];
char *fi;
char *fo;
int num_of_keys;
FILE *inputFile;
FILE *outputFile;
int size_inputFile;
int count_inputFile;
int global=0;


struct timeval exec_tm;
struct timeval startTime, endTime;

void putIntoBuffer(int start_pos,int num_to_put, FILE *fromFile);


int compare(const int *x,const int *y)
{
	return ((*x>=*y)?1:-1);
}

void swap(int a, int b)
{
	int swapper=input[a];
	input[a]=input[b];
	input[b]=swapper;
}

void sift(int i, int end)
{
	int m;
	m=(end-1)/2;
	int smaller;
	int j,k;
	while(i<=m)
	{
		//printf("In while\n");

		j=(2*i)+1;
		k=j+1;
		if((k<=end)&&(input[k]<input[j]))
		{
			smaller=k;
		}
		else
		{
			smaller=j;
		}
		if(input[smaller]<input[i])
		{
			swap(i,smaller);
			i=smaller;
		}
		else return;
	}
}

void heapify(int end)
{
	/*
	int min_pos1=0;
	int l;
	for(l=1;l<=end;l++)
	{
		if(input[l]<input[min_pos1])
		{
			min_pos1=l;
		}
	}

	printf("Input[Min_pos] is %d\t",input[min_pos1]);

	swap(0,min_pos1);
	*/
	if(end==0)	return;
	int i = (end-1)/2;
	while(i>=0)
	{
		sift(i,end);
		i--;
	}

	/*
	int x,isit=0;;
	for(x=1;x<=end;x++)
	{
		if(input[x]<input[0])
			isit=1;
	}
	if(isit==1)
		printf("Minimum not found!\n");
		*/
}

void replacement()
{
	//printf("Replacement Function called\n");

	//int h1=0,h750=749,b1=750,b250=999;	//point to positions in input buffer;
	inputFile = fopen(fi,"r+b");
	if(inputFile!=NULL)
	{
		//printf("Not NULL");
	}
	else return;
	fseek(inputFile,0,SEEK_END);	//moving pointer to end to get size
	size_inputFile = ftell(inputFile);
	count_inputFile = size_inputFile/sizeof(int);
	if(count_inputFile==0)	return;
	clearerr(inputFile);
	fseek(inputFile,0,SEEK_SET);
	//printf("Size = %d; Count = %d\n",size_inputFile,count_inputFile);

/*	//printing input file for debugging
	int cr=0;
	int crz[250000];
	fread(crz,sizeof(int),count_inputFile,inputFile);
	for(cr=0;cr<count_inputFile;cr++)
	{
		printf("%d\n",crz[cr]);
	}
	printf("Done printing--------------------------\n");
	clearerr(inputFile);
	fseek(inputFile,0,SEEK_SET);
	*/

	int counth;
	counth = fread(input,sizeof(int),750,inputFile);	//for heap
	int countb;
	countb = fread(input+750,sizeof(int),250,inputFile);	//for buffer

	FILE *runs[1000];
	int start=0,end=749;	//For heap
	end=counth-1;
	int sec_start=0;	//start of secndary heap, end will be 749 if sec heap is there...making this -1 before start of a run
	int output_pos=0;	//for output[1000];
	int pos=750,pos_end=999;	//for last 250 elements, ie the buffer
	pos_end=750+countb-1;
	int i=0;	//for keeping track of run number
			int tot=0,med=0;

	while(sec_start!=-1)	//if there is no secondary buffer, it means that there is no need for another run!
	{
		//printf("Starting new run\n");
		//if(i==1000)	printf("Warnning! i is greater than 999\n");
		sec_start=-1;	//initializing for each run
		char runName[50]="";
		strcat(runName,fi);	//put common part of name ie input files name... .xxx is left
		char runNum[5];	//to store .xxx\0
		snprintf(runNum,5,".%03d",i);	//puts .xxx\0 in runNum!
		strcat(runName,runNum);
		//Now that naming is done, we will open file in write binary mode
		//printf("RunName is %s\n",runName);
		runs[i] = fopen(runName,"w+b");
		med=0;
		//printf("RUNS[%d] data:\n",i);
		//Now current run's while loop comes!
		while(1)
		{
			//printf("In inner while, end=%d\n",end);
			//printf("About to call heapify\n");
			heapify(end);
			//printf("Heap[0]=%d\n",input[0]);
			//printf("Heapify done\n");
			//printf("%d end is %d, pos is %d->%d,input[pos]=%d\n",input[start],end,pos,pos_end,input[pos]);
			output[output_pos++]=input[start];
			tot++;
			med++;
			if(pos<=pos_end)	//signifies whether input buffer [750-999] empty or no
			{
				if(input[start]<=input[pos])
				{
					swap(start,pos);
					pos++;
				}
				else
				{
					swap(start,end);
					swap(end,pos);
					sec_start=end;
					end--;
					pos++;
				}
			}//end if(pos<=pos_end);
			else	//if no mre inputs, then in current run, keep removing first/smallest element of heap and heapifying again! 
			{
				swap(start,end);
				end--;
			}

			if(output_pos==1000)	//output buffer full, flush and reset pointer
			{
				fwrite(output,sizeof(int),1000,runs[i]);
				output_pos=0;
			}

			if(pos>pos_end)	//if ran through input buffer, need to get more and reset position of pos;
			{
				pos=750;
				int num=fread(input+750,sizeof(int),250,inputFile);
				//printf("Input buffer filled with %d integers\n",num);
				pos_end=750+num-1;
			}
			if(start>end)	//means heap is empty, time to break out of while and end current run
			{
				//printf("First run about to finish,start\n");

				if(output_pos>0)
					{
						//printf("Output_pos=%d\n",output_pos );
						fwrite(output,sizeof(int),output_pos,runs[i]);
						output_pos=0;
					}
				if(sec_start>0)	//happens only if input buffer is empty; now since run has ended, need to get secondary heap from wherever t starts to now start from 0.
				{
					//printf("sec_start=%d\n",sec_start);

					int n;
					for(n=0;n+sec_start<750;n++)
					{
						input[n]=input[n+sec_start];
					}
					end=749-sec_start;	//setting up for next run!
					//printf("end is now %d, n is %d\n",end, n);
					//printf("start is %d\n",start);
				}
				else
				{
					//printf("sec_start =%d\n",sec_start);
					end=749;
				}
				break;
			}
		}//end of while for current run, signifies current run is over
		//printf("Broken out of inner while\n");
		clearerr(runs[i]);
		fseek(runs[i],0,SEEK_SET);
		//printf("Count of runs[%d]=%d\n",i,med);

		/*
		int b[20000];
		fread(b,sizeof(int),med,runs[i]);
		int xyz=0;
		for(xyz=0;xyz<med;xyz++)
			printf("%d\n",b[xyz]);

		clearerr(runs[i]);	
		fseek(runs[i],0,SEEK_SET);
		*/

		//printf("seek_set done\n");
		i++;
		//printf("sec_start=%d\n",sec_start );
	}//end of while, used for runs
	//printf("Total written to runs=%d",tot);
	//printf("Number of runs");


	if(1)
	{
		int j;
		for(j=0;j<1000;j++)
		{
			input[j]=INT_MAX;
			output[j]=0;
		}
	}		

	//read 1000/num_of_runs
	int num_of_runs=i;
	int count_in_buffer_for_each_run = 1000/num_of_runs;
	//printf("Count = %d, Number of runs= %d",count_in_buffer_for_each_run,num_of_runs);
	int k;
	for(k=0;k<num_of_runs;k++)
	{
		//put count_in_buffer_for_each_run integers from runs[k] into input from pos 
		putIntoBuffer(k*count_in_buffer_for_each_run,count_in_buffer_for_each_run,runs[k]);
		//(start pos in dest, number of integers to put,from file);
	}

	outputFile = fopen(fo,"w+b");
	int overall_writes=0,minimum_pos;
	output_pos=0;


	while(overall_writes<count_inputFile)	//runs for count_inputFile times
		{
			minimum_pos=0;
			int min=INT_MAX;
			int tempMin;
			for(tempMin=0;tempMin<1000;tempMin++)
			{
				if(input[tempMin]<input[minimum_pos])
				{
					minimum_pos = tempMin;
				}
			}
			if(input[minimum_pos]==INT_MAX)
			{
			//!!	//CHECK IF BUFFER EMPTY, if not break; if yes, then flush to outputfile and then break;
				if(output_pos>0)
				{
					fwrite(output,sizeof(int),output_pos,outputFile);
				}
				break;
			}
			else	//not finished all
			{
				//Put value in buffer
				output[output_pos++]=input[minimum_pos];

				//If output buffer full, flush to outputFile & output_pos=0;
				if(output_pos==1000)
				{
					output_pos=0;
					fwrite(output,sizeof(int),1000,outputFile);
				}
				input[minimum_pos] = INT_MAX;
				//if the run is full, then buffer run from corresponding runs[] file
				if(((count_in_buffer_for_each_run)-(minimum_pos%count_in_buffer_for_each_run))==1)
				{
								//(start pos in dest, number of integers to put,from file);
					int whichRun = minimum_pos/count_in_buffer_for_each_run;//-1;
					putIntoBuffer(whichRun*count_in_buffer_for_each_run,count_in_buffer_for_each_run,runs[whichRun]);
				}
			}

		}
		/*
		clearerr(outputFile);
		fseek(outputFile,0,SEEK_SET);
		int a[250000];
		int readss = fread(a,sizeof(int),count_inputFile,outputFile);
		int xyz=0;
		for(xyz=0;xyz<count_inputFile;xyz++)
			printf("%d\n",a[xyz]);
		*/

		fclose(outputFile);

		/*
		printf("Data from original sorted file:\n");
		outputFile = fopen("sort.basic.bin","r+b");
		fseek(outputFile,0,SEEK_SET);
		//int a[250000];
		readss = fread(a,sizeof(int),1000,outputFile);
		//int xyz=0;
		for(xyz=0;xyz<1000;xyz++)
			printf("%d\n",a[xyz]);
		*/


	int j;
	for(j=0;j<i;j++)
		fclose(runs[j]);
	fclose(inputFile);
	return;

}

void multistep()
{
	//printf("Multistep Function called\n");

	int num_of_runs,rem_of_runs=0;
	FILE *runs[1000];

	inputFile = fopen(fi,"r+b");
	if(inputFile!=NULL)
	{
		//printf("Opened inputFile\n");

		fseek(inputFile,0,SEEK_END);	//moving pointer to end to get size
		size_inputFile = ftell(inputFile);
		count_inputFile = size_inputFile/sizeof(int);
		if(count_inputFile==0)	return;
		clearerr(inputFile);
		fseek(inputFile,0,SEEK_SET);
		num_of_runs = count_inputFile/1000;
		rem_of_runs = count_inputFile%1000;
		if(rem_of_runs>0)
		{
			num_of_runs++;	//in case greater than multiple of 1000 is present, last run will have only rem_of_runs integers and count/1000 will have to be incremented by 1;eg 1001 needs 2 runfiles not 1;
		}
		//printf("Size = %d; Count = %d\n",size_inputFile,count_inputFile);

		//Now to create file names to store runs, get data from inputFile 1000 at a time, sort and store them in thre run files.
		
		//runName = (char *)malloc(sizeof(char)*15);
		//strncpy(runName,"input-bin.");

		int j1;
				for(j1=0;j1<1000;j1++)
					input[j1]=INT_MAX;


		int i;
		for(i=0;i<num_of_runs;i++)
		{
			char runName[50]="";
			strcat(runName,fi);	//put common part of name ie input files name... .xxx is left
			char runNum[5];	//to store .xxx\0
			snprintf(runNum,5,".%03d",i);	//puts .xxx\0 in runNum!
			strcat(runName,runNum);
			//Now that naming is done, we will open file in write binary mode
			runs[i] = fopen(runName,"w+b");

			//taking 1000 integers from inputFile
			fread(input,sizeof(int),1000,inputFile);
			//sorting using qsort
			qsort(input,1000,sizeof(int),compare);
			/*
			//CHECKING IF QSORT WORKED FINE
			int j,temp=0;
			for(j=0;j<999;j++)
			{
				if(input[j]>input[j+1])
					temp=1;
			}
			if(temp==0) printf("no qsort error\n");*/
			//writing run[i] in...
			fwrite(input,sizeof(int),1000,runs[i]);
			clearerr(runs[i]);
			fseek(runs[i],0,SEEK_SET);
			if((i==num_of_runs-2)&&(rem_of_runs>0))	//in case last run des not have 1000 integers, filling with INT_MAX to overwrite previous integers
			{
				//printf("last run does not have enough integers\n");
				int j;
				for(j=0;j<1000;j++)
					input[j]=INT_MAX;
			}
			//printf("i = %d done\n",i);
		}
		fclose(inputFile);		

		if(1)
		{
			int j;
			for(j=0;j<1000;j++)
				input[j]=INT_MAX;
		}

		int num_of_super_runs = num_of_runs/15;
		int rem_of_super_runs = num_of_runs%15;
		int count_in_buffer_for_each_super_run = 1000/15; //66

		if(rem_of_super_runs>0)
		{
			num_of_super_runs++;	//in case greater than multiple of 1000 is present, last run will have only rem_of_runs integers and count/1000 will have to be incremented by 1;eg 1001 needs 2 runfiles not 1;
		}
		FILE * superRuns[num_of_super_runs];

		int i_s;
		for(i_s=0;i_s<num_of_super_runs;i_s++)
		{
			char superRunName[50] = "";
			strcat(superRunName,fi);
			char superRunNum[11];
			snprintf(superRunNum,11,".super.%03d",i_s);
			strcat(superRunName,superRunNum);

			superRuns[i_s] = fopen(superRunName,"w+b");

			int k_s,k_s_max;

			//now 2 cases...if tsthe last super run then runs[k_s] could give buffer overflow! so if its last and remainder is not 0, then only pull from runs[remainder]
			if((i_s==num_of_super_runs-1) &&(rem_of_super_runs!=0))	//if its last super_run
			{
				k_s_max=rem_of_super_runs;
			}
			else	//if its ot last super run, runs are all within buffer overflow!
			{
				k_s_max=15;
			}
			//printf("%d\t",k_s_max);
			for(k_s=0;k_s<k_s_max;k_s++)	//15 runs need be merged hence till <15 if not last super run;
			{
				int r_s = i_s*15;
				putIntoBuffer(k_s*count_in_buffer_for_each_super_run,count_in_buffer_for_each_super_run,runs[r_s+k_s]);	//startpos,number of integers,from file
			}

			//int num_of_int_in_super_run_file;
			int minimum_pos,output_pos=0;
			int number=0;
			while(1)
			{
				minimum_pos=0;
				int min=INT_MAX;
				int tempMin;
				for(tempMin=0;tempMin<1000;tempMin++)
				{
					if(input[tempMin]<input[minimum_pos])
					{
						minimum_pos = tempMin;
					}
				}
				if(input[minimum_pos]==INT_MAX)
				{
				//!!	//CHECK IF BUFFER EMPTY, if not break; if yes, then flush to outputfile and then break;
					if(output_pos>0)
					{
						fwrite(output,sizeof(int),output_pos,superRuns[i_s]);
					}
					break;
				}
				else	//not finished all
				{
					//Put value in buffer
					output[output_pos++]=input[minimum_pos];
					number++;
					//If output buffer full, flush to outputFile & output_pos=0;
					if(output_pos==1000)
					{
						output_pos=0;
						fwrite(output,sizeof(int),1000,superRuns[i_s]);
					}
					input[minimum_pos] = INT_MAX;
					//if the run is full, then buffer run from corresponding runs[] file
					if(((count_in_buffer_for_each_super_run)-(minimum_pos%count_in_buffer_for_each_super_run))==1)
					{
									//(start pos in dest, number of integers to put,from file);
						int whichRun = minimum_pos/count_in_buffer_for_each_super_run;//-1;
						putIntoBuffer(whichRun*count_in_buffer_for_each_super_run,count_in_buffer_for_each_super_run,runs[i_s*15+whichRun]);
					}
				}
			}
			//printf("Integers added in superrun[%d] = %d\n",i_s,number);

			//now that superRuns[i_s] has been filled, seek to start!
			clearerr(superRuns[i_s]);
			fseek(superRuns[i_s],0,SEEK_SET);
			
		}

		for(i=0;i<num_of_runs;i++)
		{
			fclose(runs[i]);
		}


		//Now 2nd phase of merging


		if(1)
		{
			int j;
			for(j=0;j<1000;j++)
				input[j]=INT_MAX;
		}

		int count_in_buffer_for_each_second_run = 1000/num_of_super_runs;

		//Filling input array with integers from all super runs
		int k_s_s;
		for(k_s_s=0;k_s_s<num_of_super_runs;k_s_s++)
		{
			putIntoBuffer(k_s_s*count_in_buffer_for_each_second_run,count_in_buffer_for_each_second_run,superRuns[k_s_s]);
		}
		//printf("Here\n");
		//setting up file and variables for merge
		outputFile = fopen(fo,"w+b");
		int overall_writes=0,minimum_pos,output_pos=0;
		//int number=0;
		//Merge:
		//printf("Here\n");
		while(overall_writes<count_inputFile)
		{
			//printf("Start of while\n");
			minimum_pos=0;
			int min=INT_MAX;
			int tempMin;
			for(tempMin=0;tempMin<1000;tempMin++)
			{
				if(input[tempMin]<input[minimum_pos])
				{
					minimum_pos = tempMin;
				}
			}
			//printf("%d\n",minimum_pos);
			if(input[minimum_pos]==INT_MAX)
			{
			//!!	//CHECK IF BUFFER EMPTY, if not break; if yes, then flush to outputfile and then break;
				if(output_pos>0)
				{
					//printf("Flushing buffer for last time\n");
					fwrite(output,sizeof(int),output_pos,outputFile);
				}
				break;
			}
			else	//not finished all
			{
				//Put value in buffer
				output[output_pos++]=input[minimum_pos];
				//number++;
				//If output buffer full, flush to outputFile & output_pos=0;
				if(output_pos==1000)
				{
					output_pos=0;
					//printf("Flushing buffer\n");
					fwrite(output,sizeof(int),1000,outputFile);
				}
				input[minimum_pos] = INT_MAX;
				//if the run is full, then buffer run from corresponding runs[] file
				if(((count_in_buffer_for_each_second_run)-(minimum_pos%count_in_buffer_for_each_second_run))==1)
				{

								//(start pos in dest, number of integers to put,from file);
					int whichRun = minimum_pos/count_in_buffer_for_each_second_run;//-1;
					//printf("Filling buffer for superRun[%d]",whichRun);
					putIntoBuffer(whichRun*count_in_buffer_for_each_second_run,count_in_buffer_for_each_second_run,superRuns[whichRun]);
				}
			}

		}//end while
		//printf("Ran %d times",number);

		//printf("Here\n");

		fclose(outputFile);
		for(i_s=0;i_s<num_of_super_runs;i_s++)
		{
			fclose(superRuns[i_s]);
		}

	}
	//else	printf("No input file\n");




	return;
}

//(start pos in dest, number of integers to put,from file);
void putIntoBuffer(int start_pos,int num_to_put, FILE *fromFile)
{
	//Changing a bit as of issue if end of file reached!!
	int tempPut=0;
	for(tempPut=0;tempPut<num_to_put;tempPut++)
	{
			input[start_pos+tempPut]= INT_MAX;
	}
	fread(input+start_pos,sizeof(int),num_to_put,fromFile);
	
	/*//USED to check values of input buffer!
	if(start_pos==0&&global==0)
	{
		for(tempPut=0;tempPut<num_to_put;tempPut++)
		{
			printf("%d\n",input[start_pos+tempPut]);
		}
		global=1;
	}*/
	
}
void basic()
{

	int num_of_runs,rem_of_runs=0;
	FILE *runs[1000];
	//printf("Basic Function called\n");
	
	inputFile = fopen(fi,"r+b");
	if(inputFile!=NULL)
	{
		//printf("Opened inputFile\n");

		fseek(inputFile,0,SEEK_END);	//moving pointer to end to get size
		size_inputFile = ftell(inputFile);
		count_inputFile = size_inputFile/sizeof(int);
		if(count_inputFile==0)	return;
		clearerr(inputFile);
		fseek(inputFile,0,SEEK_SET);
		num_of_runs = count_inputFile/1000;
		rem_of_runs = count_inputFile%1000;
		if(rem_of_runs>0)
		{
			num_of_runs++;	//in case greater than multiple of 1000 is present, last run will have only rem_of_runs integers and count/1000 will have to be incremented by 1;eg 1001 needs 2 runfiles not 1;
		}
		//printf("Size = %d; Count = %d\n",size_inputFile,count_inputFile);

		//Now to create file names to store runs, get data from inputFile 1000 at a time, sort and store them in thre run files.
		
		//runName = (char *)malloc(sizeof(char)*15);
		//strncpy(runName,"input-bin.");

		int j1;
				for(j1=0;j1<1000;j1++)
					input[j1]=INT_MAX;

		int i;
		for(i=0;i<num_of_runs;i++)
		{
			char runName[50]="";
			strcat(runName,fi);	//put common part of name ie input files name... .xxx is left
			char runNum[5];	//to store .xxx\0
			snprintf(runNum,5,".%03d",i);	//puts .xxx\0 in runNum!
			strcat(runName,runNum);
			//Now that naming is done, we will open file in write binary mode
			runs[i] = fopen(runName,"w+b");

			//taking 1000 integers from inputFile
			fread(input,sizeof(int),1000,inputFile);
			//sorting using qsort
			qsort(input,1000,sizeof(int),compare);
			/*
			//CHECKING IF QSORT WORKED FINE
			int j,temp=0;
			for(j=0;j<999;j++)
			{
				if(input[j]>input[j+1])
					temp=1;
			}
			if(temp==0) printf("no qsort error\n");*/
			//writing run[i] in...
			fwrite(input,sizeof(int),1000,runs[i]);
			clearerr(runs[i]);
			fseek(runs[i],0,SEEK_SET);
			if((i==num_of_runs-2)&&(rem_of_runs>0))	//in case last run des not have 1000 integers, filling with INT_MAX to overwrite previous integers
			{
				//printf("last run does not have enough integers\n");
				int j;
				for(j=0;j<1000;j++)
					input[j]=INT_MAX;
			}
			//printf("i = %d done\n",i);
		}
		//fclose(inputFile);		

		if(1)
		{
			int j;
			for(j=0;j<1000;j++)
				input[j]=INT_MAX;
		}

		

		//read 1000/num_of_runs
		int count_in_buffer_for_each_run = 1000/num_of_runs;



		int k;
		for(k=0;k<num_of_runs;k++)
		{
			//put count_in_buffer_for_each_run integers from runs[k] into input from pos 
			putIntoBuffer(k*count_in_buffer_for_each_run,count_in_buffer_for_each_run,runs[k]);
			//(start pos in dest, number of integers to put,from file);
		}

		outputFile = fopen(fo,"w+b");
		int overall_writes=0,minimum_pos,output_pos=0;

		while(overall_writes<count_inputFile)	//runs for count_inputFile times
		{
			minimum_pos=0;
			int min=INT_MAX;
			int tempMin;
			for(tempMin=0;tempMin<1000;tempMin++)
			{
				if(input[tempMin]<input[minimum_pos])
				{
					minimum_pos = tempMin;
				}
			}
			if(input[minimum_pos]==INT_MAX)
			{
			//!!	//CHECK IF BUFFER EMPTY, if not break; if yes, then flush to outputfile and then break;
				if(output_pos>0)
				{
					fwrite(output,sizeof(int),output_pos,outputFile);
				}
				break;
			}
			else	//not finished all
			{
				//Put value in buffer
				output[output_pos++]=input[minimum_pos];

				//If output buffer full, flush to outputFile & output_pos=0;
				if(output_pos==1000)
				{
					output_pos=0;
					fwrite(output,sizeof(int),1000,outputFile);
				}
				input[minimum_pos] = INT_MAX;
				//if the run is full, then buffer run from corresponding runs[] file
				if(((count_in_buffer_for_each_run)-(minimum_pos%count_in_buffer_for_each_run))==1)
				{
								//(start pos in dest, number of integers to put,from file);
					int whichRun = minimum_pos/count_in_buffer_for_each_run;//-1;
					putIntoBuffer(whichRun*count_in_buffer_for_each_run,count_in_buffer_for_each_run,runs[whichRun]);
				}
			}

		}
		fclose(outputFile);

/*
		fseek(inputFile,0,SEEK_SET);
		outputFile = fopen("sort.750.bin","w");
		fwrite(inputFile,sizeof(int),750,outputFile);
		fclose(outputFile);

		fseek(inputFile,0,SEEK_SET);
		outputFile = fopen("sort.740.bin","w");
		fwrite(inputFile,sizeof(int),740,outputFile);
		fclose(outputFile);

		fseek(inputFile,0,SEEK_SET);
		outputFile = fopen("sort.850.bin","w");
		fwrite(inputFile,sizeof(int),850,outputFile);
		fclose(outputFile);
		*/

				fclose(inputFile);


		for(i=0;i<num_of_runs;i++)
		{
			fclose(runs[i]);
		}
	}
	//else	printf("No input file\n");

	return;
}

int main(int parameterCount, char* argument[])
{
	//printf("Hello\n");
	
	
	if(parameterCount!=4)	//if command line input is wrong
	{
		//printf("Invalid input\n");
		return 0;
	}
	
	choice = 0;
	if(!strcmp(argument[1],"--basic"))	
		choice = 0;

	else if(!strcmp(argument[1],"--multistep"))	
		choice = 1;

	else if(!strcmp(argument[1],"--replacement"))	
		choice = 2;

	fi = argument[2];
	fo = argument[3];
	
	//char
	//printf("Choice= %d; inputFile = %s; outputFile = %s\n",choice,fi,fo);

	gettimeofday(&startTime, NULL);	//start time
	switch(choice)
	{
		case 0:
			basic();
			break;
		case 1:
			multistep();
			break;
		case 2:
			replacement();
			break;
		default:	//will technically never reach here!
			//printf("Choice is none; exiting...\n");
			return 0;
	}
	gettimeofday(&endTime, NULL);	//end time

	exec_tm.tv_sec=endTime.tv_sec - startTime.tv_sec;	//calculating difference now
	if(endTime.tv_usec < startTime.tv_usec)
	{
		exec_tm.tv_sec=exec_tm.tv_sec-1;
		exec_tm.tv_usec=startTime.tv_usec - endTime.tv_usec;
	}
	else
		exec_tm.tv_usec=endTime.tv_usec - startTime.tv_usec;

	printf( "Time: %ld.%06ld", exec_tm.tv_sec, exec_tm.tv_usec );
	
}