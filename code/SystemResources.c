#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdio.h>
#include<ctype.h>
#include<time.h>
#define max 100

void mem(int memfree,int memavail);
void virtualmem(int pagetable,int vmused);
void pagefaults(int pagefault_before,int pagefault_after);
void pagepersec(int pagein_1,int pageout_1,int pgswpin_1,int pgswpout_1,int pagein_2,int pageout_2,int pgswpin_2,int pgswpout_2);
void pagesize(int page_before);
void cpu_utilization(int cpu_threshold);
void mem_utilization(int mem_threshold);

static FILE *mfp=NULL;


int main(int argc, char **argv)
{
	if(argc!=5)
	{
		printf("Usage : %s <number of times /proc/stat should be polled>,<delay in seconds>,<Thresholds>",argv[0]);
		return -1;
	}
	
	int times=atoi(argv[1]);
	int lag=atoi(argv[2]);
	int cpu_threshold=atoi(argv[3]);
	int mem_threshold=atoi(argv[4]);
	mfp=fopen("overallinfo.csv","a");
	FILE *vmstat_fp,*meminfo_fp;
	
	char command[500];
	char *token;
	int i=0,x=0; 
	int page_before[6],page_after[5]; // stores pgpgin,pgpgout,pgfault,free_pages before
	int meminfo[4]; //stores the memory information
  	

	while(times>0)
	{
		
		cpu_utilization(cpu_threshold);
		mem_utilization(mem_threshold);
		meminfo_fp=popen("egrep \"MemFree|MemAvailable|PageTables|VmallocUsed\" ./proc/meminfo.txt","r");
		if(meminfo_fp==NULL)
		{
			printf("\nerror in opening file /proc/meminfo\n");
		}
		else
		{
			fread(&command,sizeof(char),500,meminfo_fp);
		 	//printf("Memory\n%s\n",command);
		 	token = strtok(command," ");
		 	
		 	while(token != NULL)
		 	{
		 		//printf("inside %d :%s\n",i,token);
		 		if(isdigit(token[0]))
		 		{
		 			//printf("%d: %s\n",i,token);
		 			meminfo[i]=atoi(token);
		 			//printf("before %d:%d\n",i,meminfo[i]);
		 			i++;
		 		}
		 		token = strtok(NULL," ");
		 		
			}
	 	}
	 	mem(meminfo[0],meminfo[1]);
	 	virtualmem(meminfo[2],meminfo[3]);
	 	
	 	i=0;
	 	command[0]='\0';
	 	vmstat_fp=popen("egrep \"pgfault|pgpgin|pgpgout|nr_free_pages|pswpin|pswpout\" ./proc/vmstat.txt","r");
		if(vmstat_fp==NULL)
		{
			printf("\nerror in opening file /proc/vmstat\n");
		}
		else
		{
			fread(&command,sizeof(char),500,vmstat_fp);
		 	//printf("before\n%s\n",command);
		 	token = strtok(command," ");
		 	
		 	while(token != NULL)
		 	{
		 		//printf("inside %d :%s\n",i,token);
		 		if(isdigit(token[0]))
		 		{
		 			//printf("%d: %s\n",i,token);
		 			page_before[i]=atoi(token);
		 			//printf("before %d:%d\n",i,page_before[i]);
		 			i++;
		 		}
		 		token = strtok(NULL," ");
		 		
			}
	 	}
	 	
	 	pagesize(page_before[0]);
	 	//readfile(vmstat_fp,page_before);
	 	//printf("%d\n",page_before[0]);
	 	times--;
	 	sleep(lag);
	 	i=0;
	 	command[0]='\0';
	 	vmstat_fp=popen("egrep \"pgfault|pgpgin|pgpgout|pswpin|pswpout\" ./proc/vmstat.txt","r");
		if(vmstat_fp==NULL)
		{
			printf("\nerror in opening file /proc/vmstat.txt\n");
		}
		else
		{
			fread(&command,sizeof(char),500,vmstat_fp);
		 	//printf("after\n%s\n",command);
		 	token = strtok(command," ");
		 	
		 	while(token != NULL)
		 	{
		 		//printf("inside %d :%s\n",i,token);
		 		if(isdigit(token[0]))
		 		{
		 			//printf("%d: %s\n",i,token);
		 			page_after[i]=atoi(token);
		 			//printf("after %d:%d\n",i,page_after[i]);
		 			i++;
		 		}
		 		token = strtok(NULL," ");
		 		
			}
	 	}
	 	
	 	pagefaults(page_before[5],page_after[4]);
	pagepersec(page_before[1],page_before[2],page_before[3],page_before[4],page_after[0],page_after[1],page_after[2],page_after[3]);
	 	//graph(page_before[5],page_after[4],x);
	 	x++;    
	
	}
	int choice;
	printf("\nDo you wish to analyse the data\nPRESS 1 to analyse\nAny other Number to Quit");
	printf("\nyour choice : ");
	scanf("%d",&choice);
	if((choice-1)==0)
	{
		system("python3 analysis.py ");
	}
	else
	{
		exit(0);
	}
	
	
}

void cpu_utilization(int cpu_threshold)
{
	char buf[200];
	long int idle,lastIdle,lastSum,nice,user,psum=0;
	long int sum=0;
	long double res=0.0;
	char str[max]; char* token;const char d[2]=" ";int i;
	FILE* fp = fopen("./proc/stat.txt","r");
	i = 0;
	fgets(str,max,fp);
	fclose(fp);
	token = strtok(str,d);
	
	time_t rtime;
  	struct tm * timeinfo;
  	time(&rtime);
  	timeinfo = localtime(&rtime);
  	char* dt= asctime(timeinfo);
	dt[strlen(dt)-1]=',';
	
	
	while(token!=NULL)
	{
		token = strtok(NULL,d);
		if(token!=NULL)
		{
			sum+=atoi(token);
			if(i!=3 && i!=4)
			{
				psum+=atoi(token);
			}
			switch(i)
			{
				case 0: user= atoi(token);
					break;
				case 1:  nice = atoi(token);
					break;
				case 3:  idle=atoi(token);
					break;
			}
			i++;
		}
	}
	
	res=100-(idle*100/sum);
	snprintf(buf,200,"notify-send \'DANGER:CPU THRESHOLD CROSSED BY %Lf\' && aplay ./beep-01a.wav",(res-cpu_threshold));
	if(res>=cpu_threshold)
	{
		system(buf);
		exit(0);
	}
	else if(cpu_threshold-10<res)
	{
		system("notify-send \'WARNING:CPU THRESHOLD ALMOST REACHED\'");
	}
	fprintf(mfp,"%s",dt);
	fprintf(mfp,"%Lf",res);
	fprintf(mfp,"%c",',');
	fprintf(mfp,"%ld",psum);
	fprintf(mfp,"%c",',');
	fprintf(mfp,"%ld",user);
	fprintf(mfp,"%c",',');
	fprintf(mfp,"%ld",nice);
	fprintf(mfp,"%c",',');
	fprintf(mfp,"%ld",idle);
	fprintf(mfp,"%c",',');
	
}
void mem_utilization(int mem_threshold)
{
	FILE* mem_fp=popen("egrep \"MemTotal|MemFree|Buffers|Cached|Slab\" ./proc/meminfo.txt","r");
	char command[500];char* token;
	long int mem[5]; int i=0;
	char buf[200];
		if(mem_fp==NULL)
		{
			printf("\nerror in opening file /proc/meminfo\n");
		}
		else
		{
			fread(&command,sizeof(char),500,mem_fp);
		 	//printf("Memory\n%s\n",command);
		 	token = strtok(command," ");
		 	
		 	while(token != NULL)
		 	{
		 		//printf("inside %d :%s\n",i,token);
		 		if(isdigit(token[0]))
		 		{
		 			//printf("%d: %s\n",i,token);
		 			mem[i]=atoi(token);
		 			//printf("before %d:%d\n",i,meminfo[i]);
		 			i++;
		 		}
		 		token = strtok(NULL," ");
		 		
			}
	 	}
	long int used=mem[0]-mem[1]-mem[2]-mem[3]-mem[4];
	long double perused=0.0;
	perused=100.0-(used*100/mem[0]);
	snprintf(buf,200,"notify-send \'DANGER:MEMORY THRESHOLD CROSSED BY %Lf\' && aplay ./beep-01a.wav",(perused-mem_threshold));
	if(perused>=mem_threshold)
	{
		fprintf(mfp,"%s\n",",,,,,,,,,,,,,,");
		system(buf);
		exit(0);
	}
	else if(mem_threshold-10<perused)
	{
		system("notify-send \'WARNING:MEMORY THRESHOLD ALMOST REACHED\'");
	}
	fprintf(mfp,"%Lf",perused);
	fprintf(mfp,"%c",',');
	fprintf(mfp,"%ld",used);
	fprintf(mfp,"%c",',');
	fprintf(mfp,"%ld",mem[0]);
	fprintf(mfp,"%c",',');
}
void mem(int memfree,int memavail)
{
	fprintf(mfp,"%d",memfree);
	fprintf(mfp,"%c",',');
	fprintf(mfp,"%d",memavail);
	fprintf(mfp,"%c",',');
	
}
void virtualmem(int pagetable,int vmused)
{
	fprintf(mfp,"%d",pagetable);
	fprintf(mfp,"%c",',');
	fprintf(mfp,"%d",vmused);
	fprintf(mfp,"%c",',');
	//printf("Virtual memory used : %d KB\n",vmused);
	//printf("Page Table size : %d KB\n",pagetable);	
}
void pagesize(int num_pages)
{
	fprintf(mfp,"%d",num_pages);
	fprintf(mfp,"%c",',');
	//printf("Available pages : %d\n",num_pages);
}

void pagefaults(int pagefault_before,int pagefault_after)
{
		fprintf(mfp,"%d",pagefault_before);
		fprintf(mfp,"%c",',');
		fprintf(mfp,"%d",pagefault_after-pagefault_before);
		fprintf(mfp,"%c",',');
	//printf("pagefault : %d \n",pagefault_before);
	//printf("Page fault /sec : %d \n",pagefault_after-pagefault_before);
}

void pagepersec(int pagein_1,int pageout_1,int pgswpin_1,int pgswpout_1,int pagein_2,int pageout_2,int pgswpin_2,int pgswpout_2)
{	
	int ppgin_s=pagein_2-pagein_1;
	int ppgout_s=pageout_2-pageout_1;
	int pageread_s=ppgin_s+ppgout_s;
	int pgswpin=pgswpin_2-pgswpin_1;
	int pgswpout=pgswpout_2-pgswpout_1;
	fprintf(mfp,"%d",pgswpin);
	fprintf(mfp,"%c",',');
	fprintf(mfp,"%d",pgswpout);
	fprintf(mfp,"%c",',');
	fprintf(mfp,"%d",ppgin_s);
	fprintf(mfp,"%c",',');
	fprintf(mfp,"%d",ppgout_s);
	fprintf(mfp,"%c",',');
	fprintf(mfp,"%d\n",pageread_s);
	

	//printf("pages swapped in per sec : %d\n",pgswpin);
	//printf("pages swapped out per sec : %d\n",pgswpout);
	//printf("pages read per sec : %d\n",ppgin_s);
	//printf("pages write per sec : %d\n",ppgout_s);
	//printf("pages per second : %d\n",pageread_s);
}


