/* 
	Ayush Khandelwal
	201505512
	CSE
	SHELL
	*/ 


#include<iostream>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<vector>
#include<signal.h>
#include<ctype.h>

using namespace std;

	char buf[1000];																// buffer to store the user input
	char *args[100];
	char temp[100];
	char home[500];																// stores the home directory
	char wbuf[2];																// write buffer to write data to history file
	char rbuf[100];																// read buffer to read data from history file
	int i=0,m,t,n,inp;
	int count=0;																// count of commands in history
	int fd;
	int sin,sout;
	int tempsin=0,tempsout=1;													// to restore stdin and stdout after each command
	int outflag=0,inflag=0;
	int outfd,infd;

void signal_handle(int sig)														// handles ctrl+c.........kills the child only
{
	if(getpid()==0)
		exit (0);
	
}

void get_count(int fd)															// counts number of commands in history
{
	
	while((t=read(fd,rbuf,100))>0) 
	{

		i=0;
		while(i<t)
		{
		
			if(rbuf[i]=='\n')
			{
				count++; 
			}
			i++;
		}
	}			
}

void print_prompt()																// prints the prompts at the screen
{
	char dir[500];
	getcwd(dir,500);
	cout<<"My_Shell:"<<dir<<"$ ";
	return;
}

void run_commands();

void tokenize_outer()														// tokenize the input string on "|" and store in args array
{
	args[0]=strtok(buf,"|");
	n=1;
	while ((args[n]=strtok(NULL,"|"))!=NULL)
		n++;
	args[n]=NULL;
	return;
}

void echo_fn(char *command1[])												// ECHO command
{
	char *temp=command1[1];
/*	if(temp[0]=='"')
	{
		char *temp1;
  		int k=1,j=0;
  		while(temp[k]!='\0')
  		{
  			if(temp[k]!='"')
  				temp1[j++]=temp[k];
  			k++;
  		}
  		temp1[j]='\0';
	}*/
  	if(temp[0]=='$' || (temp[0]=='"' && temp[1]=='$'))							
  	{
  		//cout<< " test ";
  		if(temp[0]=='"')
		{
			char temp1[100];
		//	cout<< " test ";
  			int k=2,j=0;
  			while(temp[k]!='"' || temp[k]!='\0')
  			{
  				//if(temp[k]!='"')
  					temp1[j++]=temp[k];
  					cout<<temp1[j-1];
  				k++;

  			}
  			temp1[j]='\0';
  			//cout<<temp1;
  			/*char *tt=getenv(temp1);
  			cout<<tt<<endl;*/
		}
		else
  		{	//temp1[0]=strtok(command1)
  			char *tt=getenv(temp+1);														// get environment variable
  			cout<<tt<<endl;
  		}
  	}	
  	
  	else
  	{	
  		
  		for(i=1;i<m;i++)
  		{	//	cout<<command1[i]<<" "; 
  			char *tmp[100];
			tmp[0]=strtok(command1[i],"\"");
			int k=1;
			while ((tmp[k]=strtok(NULL,"\""))!=NULL)
				k++;
			tmp[k]=NULL;
			for(int j=0;j<k;j++)
				cout<<tmp[j]<<" ";
		}	


  		cout<<endl;	
  	}		
}

void history_fn(char *command1[])																// HISTORY command
{
	if(command1[1]==NULL)																	// if no arguements present then print the history file
  		{
  			int historyfd=open("./history.txt",O_RDWR|O_CREAT,S_IRWXU);
  				while((t=read(historyfd,rbuf,100))>0) 
			{
					i=0;
				while(i<t)
				{
					cout<<rbuf[i];
					i++;
				}
			}			
 		}
 		else																				// else print last 'lines' number of commands	
 		{
 			int historyfd=open("./history.txt",O_RDWR|O_CREAT,S_IRWXU);
 			int lines=atoi(command1[1]);	
 			int ln=0;
 			while((t=read(historyfd,rbuf,100))>0) 
			{
				i=0;
				while(i<t)
				{
					if(rbuf[i]=='\n')
					ln++;
					if(ln>=(count-lines))
						cout<<rbuf[i];
					i++;
				}
			}			
		}	
}


void bang_fn(char *command1[])																	// BANG command
{
	int historyfd=open("./history.txt",O_RDWR|O_CREAT,S_IRWXU);
		char command[500];
		int j=0;
		if(command1[0][1]=='!')																	// !!
		{
			int ln=0;
			while((t=read(historyfd,rbuf,100))>0) 
  			{	
  				i=0;
				while(i<t)
				{
					if(rbuf[i]=='\n')
					ln++;
					if(ln==count-1 && rbuf[i]!='\n')
						//cout<<rbuf[i];
						command[j++]=rbuf[i];
					i++;
				}
			}
			command[j]='\0';
		}
		/*else if(isalpha(command1[0][1]))
		{
			char ch=command1[0][1];
			char *allline[1000];
			char line[500];
			int t,i=0,lc=0,f=1;
			char tbuf[1];
			while((t=read(historyfd,rbuf,1))>0)
			{
				line[i]=tbuf[0];
				if(tbuf[0]=='\n')
				{
					line[i++]='\0';
					allline[lc]=(char *)malloc((i+2)*sizeof(char));
					strcpy(allline[lc],line);
					f=0;
					
					strcat(allline[lc],"\n");
					lc++;

				}
				i++;
				if(f==0)
				{
					i=0;
					f=1;
				}	
			}



		}*/
		else 
		{
			char str[100];
			int cnt;
			sprintf(str,"%s",command1[0]);
			sscanf(str,"!%d",&cnt);
			//cout<<cnt;

			if(cnt>0)																				// ! positive number
			{
				int ln=0;
				while((t=read(historyfd,rbuf,100))>0) 
  				{	
  					i=0;
					while(i<t)
					{
						if(rbuf[i]=='\n')
						ln++;
						if(ln==cnt-1 && rbuf[i]!='\n')
							//cout<<rbuf[i];
							command[j++]=rbuf[i];
						i++;
					}
				}
				command[j]='\0';

			}
			else																						// ! negative number
			{
				int ln=0;
				while((t=read(historyfd,rbuf,100))>0) 
  				{	
  					i=0;
					while(i<t)
					{
						if(rbuf[i]=='\n')
						ln++;
						if(ln==count+cnt && rbuf[i]!='\n')
							//cout<<rbuf[i];
							command[j++]=rbuf[i];
						i++;
					}
				}
				command[j]='\0';

			}				

		}	
	//cout<<command;
	char command2[100];
	j=0;
	int k=0;
	while(command[j++]!=' ');
	while(command[j]!='\0')
		command2[k++]=command[j++];
	command2[k]='\0';
	//cout<<command2;
	strcpy(buf,command2);
	//---------------------------write in parent
	//count++;
	//write_history(fd);
	tokenize_outer();
	run_commands();	
	//run(command2);
	//------------------------------------------------handle !a
}

void write_history(int fd)																			// write the current command to history file
{
	count++;
	sprintf(temp,"%d ",count);
	write(fd,temp,strlen(temp));
	for(i=0;buf[i]!='\n';i++)
	{
		//strcpy(wbuf,buf[i]);
		if(buf[i]=='\0')
			break;
		wbuf[0]=buf[i];
		wbuf[1]='\0';
		write(fd,wbuf,1);
	}
	write(fd,"\n",1);	
	buf[strlen(buf)-1]='\0';
	return;
}



void run(char *command1[])															// Run the command
{
	if(command1[0]==NULL)
		exit (0);
	//-------------------------------------------------------------CD-------------------------------------
	if(strcmp(command1[0],"cd")==0)									
	{
  		//	cout<<command1[1];
  		if(command1[1]==NULL)
  		{
  			chdir(home);
  		}
  		else if(strcmp(command1[1],"~")==0)
  		{
  			chdir(home);
  			//cout<<home;	
  		}	
  		else
	 		chdir(command1[1]);
  	}
  	//---------------------------------------------------------------------ECHO----------------------------------------
  	else if(strcmp(command1[0],"echo")==0)
  	{
		echo_fn(command1);  		
  	}
  	//-------------------------------------------------------------HISTORY--------------------------------------------
  	else if(strcmp(command1[0],"history")==0)
  	{
  		//cout<<" test ";
  		history_fn(command1);
  		
	}
	//---------------------------------------------------------------BANG----------------------------------------
	else if(command1[0][0]=='!')
	{
		
		bang_fn(command1);
	}
	//---------------------------------------------------------------------------EXPORT-----------------------------------------------------
/*	else if(strstr(command1[0],"=")!=NULL)
	{
		char *tmp[100];
		tmp[0]=strtok(command1[0],"=");
		int k=1;
		while ((tmp[k]=strtok(NULL,"="))!=NULL)
			k++;
		tmp[k]=NULL;
		//for(int j=0;j<k;j++)
		//cout<<tmp[j]<<" ";
		//cout<<" = ";
		setenv(tmp[0],tmp[1],1);
		//putenv(command1[0]);
	}*/
 	else
 	{
 	//	int child=fork();
	//	if(child==0)
	//	{

			execvp(command1[0],&command1[0]);	
	//		exit(0);
	//	}
	//	else
	//	{
	//		wait(0);
	//	}	
	}
	exit (0);
	return;
}

void execute(int input, int output,char *command1[])								// pipelining 
{
	int child=fork();
	if (child== 0)
    {
      //if (input!=0)
        {
          dup2(input,0);
          close(input);
        }
        if(inflag!=0)																// input redirection
        {
        	dup2(infd,0);
        	close(infd);
        }

     // if (output!=1)
        {
          dup2(output,1);
          close(output);
        }
        if(outflag!=0)																// output redirection
        {
        	dup2(outfd,1);
        	close(outfd);
        }	

      run(command1);
    // return;
    }
    else
   		wait(0);

  return;
}

/*void run_last_command()
{
	int child=fork();
	if(child==0)
	{
	char *command1[100];
  	command1[0]=strtok(args[i]," ");
  	m=1;
  	while((command1[m]=strtok(NULL," "))!=NULL)
  		m++;
  	command1[m]=NULL;
  	run(command1);
  	return ;
  }
  else
  	wait(0);
  	return;
}*/

void run_commands()															// pass the commands to the pipe
{
	//int child=fork();
	//if(child==0)
//	{
		int input=0;
		int i=0;
		int pipefd[2];
		outflag=0;
		inflag=0;

		for(i=0;i<n-1;i++)														// first n-1 commands
		{	
			outflag=0;
			inflag=0;
			char *command1[100];
			
  			if(strstr(args[i],">")!=NULL)									// output redirction present
  			{
  				char *outfile[100];
  				outfile[0]=strtok(args[i],">");
  				outfile[1]=strtok(NULL," ");
  				outflag=1;									
				outfd=open(outfile[1],O_RDWR|O_CREAT,S_IRWXU);

				char *tempcmd[100];
				tempcmd[0]=strtok(args[i],"> ");
				int x=1;
				while((tempcmd[x]=strtok(NULL,"> "))!=NULL)
				x++;
				tempcmd[x]=NULL;
				int j=0;
				for(int k=0;k<x;k++)
				{
					if(strcmp(tempcmd[k],outfile[1])!=0)
					{
						strcpy(command1[j++],tempcmd[k]);
						//cout<<command1[j-1];
					}	

				}
				command1[j]=NULL;	
  			}
  			else if(strstr(args[i],"<")!=NULL)											// input redirection present
  			{
  				char *infile[100];
  				infile[0]=strtok(args[i],"<");
  				infile[1]=strtok(NULL," ");
  				inflag=1;
				infd=open(infile[1],O_RDWR,S_IRWXU);

				char *tempcmd[100];
				tempcmd[0]=strtok(args[i],"< ");
				int x=1;
				while((tempcmd[x]=strtok(NULL,"< "))!=NULL)
				x++;
				tempcmd[x]=NULL;
				int j=0;
				for(int k=0;k<x;k++)
				{
					if(strcmp(tempcmd[k],infile[1])!=0)
					{
						strcpy(command1[j++],tempcmd[k]);
						//cout<<command1[j-1];
					}	

				}
				command1[j]=NULL;	
  			}
  			else																			// no redirection
			{
				
  				command1[0]=strtok(args[i]," \"");
  				m=1;
  				while((command1[m]=strtok(NULL," \""))!=NULL)
  					m++;
  				command1[m]=NULL;
  			}
			pipe(pipefd);
			execute(input,pipefd[1],command1);
		//	int child=fork();
		//	if(child==0)
			/*{
				if(inp!=0)
				{	
					dup2(inp,0);
					close(inp);
				}
				if(pipefd[1]!=1)
				{
					dup2(pipefd[1],1);
					close(pipefd[1]);
				}
				run(command1);
		//		exit(0);
			}*/
		//	else
			
			close(pipefd[1]);
			input=pipefd[0];
		}	
			if(input!= 0)
    			dup2(input,0);
    		outflag=0;
			inflag=0;
    		int child=fork();																// run the last command
    		if(child==0)
    		{
    //		run_last_command();
    		/*char *command1[100];
  			command1[0]=strtok(args[i]," \"");
  			m=1;
  			while((command1[m]=strtok(NULL," \""))!=NULL)
  			m++;
  			command1[m]=NULL;*/
  			char *command1[100];
  			
  			if(strstr(args[i],">")!=NULL)														// output redirection present
  			{
  				char *outfile[100];
  				outfile[0]=strtok(args[i],">");
  				outfile[1]=strtok(NULL," ");
  				outflag=1;
				outfd=open(outfile[1],O_RDWR|O_CREAT|O_TRUNC,S_IRWXU);
				dup2(outfd,1);
				close(outfd);
				char *tempcmd[100];
				tempcmd[0]=strtok(args[i],"> ");
				int x=1;
				while((tempcmd[x]=strtok(NULL,"> "))!=NULL)
				x++;
				tempcmd[x]=NULL;
				int j=0;
				for(int k=0;k<x;k++)
				{
					if(strcmp(tempcmd[k],outfile[1])!=0)
					{
						strcpy(command1[j++],tempcmd[k]);
						//cout<<command1[j-1];
					}	
			
				}
				command1[j]=NULL;	
  			}
  			else if(strstr(args[i],"<")!=NULL)														// input redirection present
  			{
  				char *infile[100];
  				infile[0]=strtok(args[i],"<");
  				infile[1]=strtok(NULL," ");
  				inflag=1;
				infd=open(infile[1],O_RDWR,S_IRWXU);

				char *tempcmd[100];
				tempcmd[0]=strtok(args[i],"< ");
				int x=1;
				while((tempcmd[x]=strtok(NULL,"< "))!=NULL)
				x++;
				tempcmd[x]=NULL;
				int j=0;
				for(int k=0;k<x;k++)
				{
					if(strcmp(tempcmd[k],infile[1])!=0)
					{
						strcpy(command1[j++],tempcmd[k]);
						//cout<<command1[j-1];
					}	

				}
				command1[j]=NULL;	
  			}
  			else																					// no redirection
			{
				
  				command1[0]=strtok(args[i]," \"");
  				m=1;
  				while((command1[m]=strtok(NULL," \""))!=NULL)
  					m++;
  				command1[m]=NULL;
  			}

  			run(command1);
			
			exit(0);
	}
	else
		wait(0);	
	return;
}



void shell()																		// Shell - call all the other functions from here
{
	while(1)
	{
	sin=dup(0);																		// reinstating stdin and stdout
	dup2(tempsin,0);	
	sout=dup(1);
	dup2(tempsout,1);

		print_prompt();																// print the prompt
		fgets(buf,sizeof(buf),stdin);												// read user input
		if(buf==NULL)
			exit(0);
		if (buf[0]!='!')
	//	get_count(fd);

		write_history(fd);															// write to  history
//-------------------------------------------------------------EXPORT-----------------------------------------------------------
if(strstr(buf,"=")!=NULL)
	{
		char *tmp[100];
		tmp[0]=strtok(buf,"=");
		int k=1;
		while ((tmp[k]=strtok(NULL,"="))!=NULL)
			k++;
		tmp[k]=NULL;
		//for(int j=0;j<k;j++)
		//cout<<tmp[j]<<" ";
		//cout<<" = ";
		if(buf[0]=='e' && buf[1]=='x' && buf[2]=='p')
		{
			char *tmp1[100];
			tmp1[0]=strtok(tmp[0]," ");
			k=1;
			while ((tmp1[k]=strtok(NULL," "))!=NULL)
			k++;
			tmp1[k]=NULL;
			setenv(tmp1[1],tmp[1],1);
		}
		else
			setenv(tmp[0],tmp[1],1);
	//	shell();
		continue;
		//putenv(command1[0]);
	}



	
		tokenize_outer();																	// tokenize on "|"
		run_commands();																		// run the commands
	
//	run_last_command();
		dup2(sin,0);
		close(sin);
		dup2(sout,1);
		close(sout);
	//shell();
	}
}


int main()
{
	signal(SIGINT,signal_handle);
	getcwd(home,500);																		// set home 
	//cout<<home;
	fd=open("./history.txt",O_RDWR|O_CREAT,S_IRWXU);										

	get_count(fd);																			// count in history
	shell();																				// run the shell
	return 0;
}