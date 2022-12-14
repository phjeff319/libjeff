#include<iostream>
#include<fstream>
#include<cstring>
#include<sys/stat.h>

#include"generator.h"
using namespace std;

const int buff_size = 8192;

//only updated by read_taglist()
int num_tag;
char **tag_list;
//[num_tag][buff_size]={"###CPP### ","###CUDA### ","###HOST### ","###DEVICE### ","###GLOBAL### "};
char **tag_update;
//[num_tag][buff_size]={"","","__host__ ","__device__ ","__global__ "};

const int naddonmode=5;
char addonmode[naddonmode][buff_size]={"external","scientific","robotics","database","xgboost"};

bool file_check_exist(char*);
int file_check_length(char*);
void remove_all_tag(char *buff);
void replace_tag(char *buff,const char *tag_to_replace,const char *replace_to);
int check_num_tag_in_string(char *to_check,char *tag);
int check_num_item_in_list(char *filename,char *list_name);
int read_items_in_list(char *filename,char *list_name,int num_item,char **item);
int check_tag(char *buff,char *tag);
int generate(char *item_name,char *item_group,char *generate_mode);
int generate_header(char *filename,char *generate_mode);
int generate_header(char *in_filename,char *generate_mode,char *out_filename);
int generate_source(char *filename,char *generate_mode);
int generate_source(char *in_filename,char *generate_mode,char *out_filename);
int generate_makefile(int num_list,char **listname,int *num_items,char*** items,char *library_name);
int read_taglist();

bool check_mode(char *generation_mode,char *find){
  if(strcmp(generation_mode,find)==0){
    return true;
  }
  else{
    char *ctemp = new char [strlen(find)+10];
    bool result = false;

    strcpy(ctemp,find);
    strcat(ctemp,"+");
    if(strstr(generation_mode,ctemp)!=NULL){
      result = true;
    }

    strcpy(ctemp,"+");
    strcat(ctemp,find);
    if(strstr(generation_mode,ctemp)!=NULL){
      result = true;
    }

    delete [] ctemp;
    return result;
  }
}

void check_line(char **file_buff,int &num_line,int &lines,bool modematch,bool tagmatch){
  if(!modematch && tagmatch){
    int j;
    for(j=lines+1;j<num_line;j++){
      strcpy(file_buff[j-1],file_buff[j]);
    }
    num_line--;
    lines--;
  }
}

int main(int argc,char *argv[]){
  if(argc < 3){
    cout << "Usage: " << argv[0] << " [generation list/file to be generated] [cpu/gpu]" << endl;
    return 0;
  }

  if(read_taglist()){
    cout << "Error in reading taglist.txt" << endl;
    return 0;
  }

  if(strstr(argv[1],".index")!=NULL){
    char *library_name = new char [buff_size];

    int num_slash = check_num_tag_in_string(library_name,"/");
    char *temp = argv[1];
    while(num_slash>0){
      temp = strstr(argv[1],"/")+1;
      num_slash--;
    }
    strcpy(library_name,temp);
    *strstr(library_name,".index")='\0';

    cout << "library to build: " << library_name << ".a" << endl;
    cout << "You can change this library name by changing the name of the .index file." << endl;

    //whole library generation mode
    char *generation_mode = new char [buff_size];
    strcpy(generation_mode,"always+");
    strcat(generation_mode,argv[2]);
    int num_list_to_read = check_num_tag_in_string(generation_mode,"+") + 1;
    
    char **listname;
    int *num_item;
    char ***items = new char **[num_list_to_read];

    listname = new char *[num_list_to_read];
    num_item = new int [num_list_to_read];

    int i,j,k;
    for(i=0;i<num_list_to_read;i++){
      temp = generation_mode;
      for(j=0;j<i;j++){
	temp = strstr(temp,"+")+1;
      }
      listname[i] = new char [buff_size];
      strcpy(listname[i],temp);
      if(strstr(listname[i],"+")!=NULL){
	*strstr(listname[i],"+")='\0';
      }

      num_item[i] = check_num_item_in_list(argv[1],listname[i]);
      if(num_item[i]>0){
	items[i] = new char *[num_item[i]];
	for(j=0;j<num_item[i];j++){
	  items[i][j] = new char [buff_size];
	}
	read_items_in_list(argv[1],listname[i],num_item[i],items[i]);
      }
      else if(num_item[i] < 0){
	cout << "Error in file " << argv[1] <<" for list \"" << listname[i] << "\"" << endl;
	return 0;
      }
    }

    cout << "List of items to be compiled." << endl;
    char *mode = new char [buff_size];

    for(i=0;i<num_list_to_read;i++){
      for(j=0;j<num_item[i];j++){
	if(check_mode(argv[2],"gpu")){
	  strcpy(mode,"gpu");
	}
	else if(check_mode(argv[2],"cpu")){
	  strcpy(mode,"cpu");
	}
	for(k=0;k<naddonmode;k++){
	  if(check_mode(argv[2],addonmode[k])){
	    strcat(mode,"+");
	    strcat(mode,addonmode[k]);
	  }
	}
	cout << items[i][j] << " " << mode << std::endl;
	generate(items[i][j],listname[i],mode);
      }
    }

    delete [] mode;

    generate_makefile(num_list_to_read,listname,num_item,items,library_name);

    for(i=0;i<num_list_to_read;i++){
      for(j=0;j<num_item[i];j++){
	delete [] items[i][j];
      }
      if(num_item[i]>0){
	delete [] items[i];
      }
      delete [] listname[i];
    }
    delete [] items;
    delete [] listname;
    delete [] num_item;
    delete [] generation_mode;
  }
  else if(strstr(argv[1],".JEFFH")!=NULL){
    //header generation mode
    if(argc > 3){
      generate_header(argv[1],argv[2],argv[3]);
    }
    else{
      generate_header(argv[1],argv[2]);
    }
  }
  else if(strstr(argv[1],".JEFFC")!=NULL){
    //source file generation mode
    if(argc > 3){
      generate_source(argv[1],argv[2],argv[3]);
    }
    else{
      generate_source(argv[1],argv[2]);
    }
  }
  else{
    cout << "Input file suffix not recognised." << endl;
    cout << "Nothing was done." << endl;
  }

  return 0;
}

bool file_check_exist(char *filename){
  struct stat results;
  if(stat(filename,&results)==0){
    return 1;
  }
  else{
    return 0;
  }
}

int file_check_length(char *filename){
  if(file_check_exist(filename)){
    ifstream fin;
    int count = 0;

    char temp[buff_size];

    fin.open(filename);
    fin.getline(temp,buff_size,'\n');
    while(!fin.eof()){
      count++;
      fin.getline(temp,buff_size,'\n');
    }
    fin.close();
    return count;
  }
  else{
    return 0;
  }
}

int read_taglist(){
  num_tag=file_check_length("taglist.txt");
  if(num_tag>0){
    tag_list = new char *[num_tag];
    tag_update = new char *[num_tag];
    int i;
    ifstream fin;
    fin.open("taglist.txt");
    for(i=0;i<num_tag;i++){
      tag_list[i] = new char [buff_size];
      tag_update[i] = new char [buff_size];
      
      fin.getline(tag_list[i],buff_size,',');
      fin.getline(tag_update[i],buff_size,'\n');
    }
    fin.close();
  }
  else{
    return 1;
  }
  return 0;
}

int generate(char *item_name,char *item_group,char *generate_mode){
  int error;
  char *temp_buff = new char [buff_size];
  if(strstr(item_name,".JEFFH")!=NULL){
    strcpy(temp_buff,PWD);
    strcat(temp_buff,"/seed/");
    strcat(temp_buff,item_group);
    strcat(temp_buff,"/");
    strcat(temp_buff,item_name);
    error = generate_header(temp_buff,generate_mode);
    if(error){
      return error;
    }
  }
  else if(strstr(item_name,".JEFFC")!=NULL){
    strcpy(temp_buff,PWD);
    strcat(temp_buff,"/seed/");
    strcat(temp_buff,item_group);
    strcat(temp_buff,"/");
    strcat(temp_buff,item_name);
    error = generate_source(temp_buff,generate_mode);
    if(error){
      return error;
    }
  }
  else{
    int error;
    char *temp_buff = new char [buff_size];

    strcpy(temp_buff,PWD);
    strcat(temp_buff,"/seed/");
    strcat(temp_buff,item_group);
    strcat(temp_buff,"/");
    strcat(temp_buff,item_name);
    strcat(temp_buff,".JEFFH");
    error = generate_header(temp_buff,generate_mode);
    if(error){
      return error;
    }

    strcpy(temp_buff,PWD);
    strcat(temp_buff,"/seed/");
    strcat(temp_buff,item_group);
    strcat(temp_buff,"/");
    strcat(temp_buff,item_name);
    strcat(temp_buff,".JEFFC");
    error = generate_source(temp_buff,generate_mode);
    if(error){
      return error;
    }
  }  

  delete [] temp_buff;
  return 0;
}

int generate_header(char *filename,char *generate_mode){
  char *out_filename = new char [buff_size];
  char *temp_filename = new char [buff_size];
  char *temp_buff = temp_filename;

  strcpy(out_filename,PWD);
  strcat(out_filename,"/include/");
  strcpy(temp_filename,filename);
  *strstr(temp_filename,".JEFFH")='\0';
  
  temp_buff=strstr(temp_buff,"/");
  while(temp_buff != NULL && strstr(temp_buff+1,"/") != NULL){
    temp_buff=strstr(temp_buff+1,"/");
  }
  strcat(out_filename,temp_buff+1);
  strcat(out_filename,".h");

  int error = generate_header(filename,generate_mode,out_filename);

  delete [] out_filename;
  delete [] temp_filename;
  return error;
}

int generate_source(char *filename,char *generate_mode){
  char *out_filename = new char [buff_size];
  char *temp_filename = new char [buff_size];
  char *temp_buff = temp_filename;

  strcpy(out_filename,PWD);
  strcat(out_filename,"/src/");
  strcpy(temp_filename,filename);
  *strstr(temp_filename,".JEFFC")='\0';
  
  temp_buff=strstr(temp_buff,"/");
  while(temp_buff != NULL && strstr(temp_buff+1,"/") != NULL){
    temp_buff=strstr(temp_buff+1,"/");
  }
  strcat(out_filename,temp_buff+1);
  if(check_mode(generate_mode,"cpu")){
    strcat(out_filename,".cpp");
  }
  else if(check_mode(generate_mode,"gpu")){
    strcat(out_filename,".c");
  }

  int error = generate_source(filename,generate_mode,out_filename);

  delete [] out_filename;
  delete [] temp_filename;
  return error;
}

int generate_header(char *in_filename,char *generate_mode,char *out_filename){
  int i,j;

  int num_line = file_check_length(in_filename);
  if(num_line > 0){
    char **file_buff, *temp_buff;
    file_buff = new char *[num_line];
    for(i=0;i<num_line;i++){
      file_buff[i] = new char [buff_size];
    }
    temp_buff = new char [buff_size];
    
    ifstream fin;
    fin.open(in_filename);
    
    for(i=0;i<num_line;i++){
      fin.getline(file_buff[i],buff_size,'\n');
    }
    
    fin.close();

    int valid_backet_deg = 0;
    int active_comment = 0;

    bool tagcpp, tagcuda, taghost, tagdevice, tagglobal;
    bool tagscientific, tagrobotics, tagdatabase, tagxgboost, tagexternal;
    bool rm_section_flag = 0;
    i = 0;
    while(i<num_line){
      if(rm_section_flag){
	if(strstr(file_buff[i],"//")==NULL){
	  valid_backet_deg += check_num_tag_in_string(file_buff[i],"{") - check_num_tag_in_string(file_buff[i],"}");
	}
	else{
	  strcpy(temp_buff,file_buff[i]);
	  *strstr(temp_buff,"//") = '\0';
	  valid_backet_deg += check_num_tag_in_string(temp_buff,"{") - check_num_tag_in_string(temp_buff,"}");
	}
	for(j=i+1;j<num_line;j++){
	  strcpy(file_buff[j-1],file_buff[j]);
	}
	num_line--;

	if(valid_backet_deg==0){
	  rm_section_flag=0;
	}
	else if(valid_backet_deg<0){
	  cout << "Error! Unbalance parentheses detected." << endl;
	  return 2001;
	}
      }
      else{
	//check for comment block
	if(strncmp(file_buff[i],"/*",2) == 0 || (strstr(file_buff[i],"/*")!=NULL && *(strstr(file_buff[i],"/*")-1) != '/')){
	  while(strstr(file_buff[i],"*/")==NULL){
	    i++;
	  }
	  i++;
	}
	//check for comment line
	if(i < num_line){
	  //cout << file_buff[i] << endl;
	  while(strstr(file_buff[i],"//")!=NULL){
	    if(strstr(file_buff[i],";")==NULL && strstr(file_buff[i],"{")==NULL && strstr(file_buff[i],"}")==NULL){
	      //pure comment line, just skip
	      i++;
	    }
	    if(
	       (strstr(file_buff[i],";")!=NULL && strstr(file_buff[i],"//")!=NULL && strstr(file_buff[i],";")>strstr(file_buff[i],"//")) || 
	       (strstr(file_buff[i],"{")!=NULL && strstr(file_buff[i],"//")!=NULL && strstr(file_buff[i],"{")>strstr(file_buff[i],"//")) || 
	       (strstr(file_buff[i],"}")!=NULL && strstr(file_buff[i],"//")!=NULL && strstr(file_buff[i],"}")>strstr(file_buff[i],"//"))
	       ){
	      //commented out valid code, just skip
	      i++;
	    }
	    else{
	      //otherwise, there are comments in the code but the comments are after some valid code
	      break;
	    }
	  }
	  tagcpp = check_tag(file_buff[i],"###CPP###");
	  tagcuda = check_tag(file_buff[i],"###CUDA###");
	  tagscientific = check_tag(file_buff[i],"SCIENTIFIC");
	  tagrobotics = check_tag(file_buff[i],"ROBOTICS");
	  tagdatabase = check_tag(file_buff[i],"DATABASE");
	  tagxgboost = check_tag(file_buff[i],"XGBOOST");
	  tagexternal = check_tag(file_buff[i],"EXTERNAL");

	  bool line_delete = false;

	  if((!check_mode(generate_mode,"scientific") && tagscientific) || (!check_mode(generate_mode,"robotics") && tagrobotics) || (!check_mode(generate_mode,"database") && tagdatabase) || (!check_mode(generate_mode,"xgboost") && tagxgboost) || (!check_mode(generate_mode,"external") && tagexternal)){
	    //deactivate code
	    //if(tagscientific){
	      valid_backet_deg += check_num_tag_in_string(file_buff[i],"{") - check_num_tag_in_string(file_buff[i],"}");
	      
	      for(j=i+1;j<num_line;j++){
		strcpy(file_buff[j-1],file_buff[j]);
	      }
	      line_delete = true;
	      num_line--;
	      
	      if(valid_backet_deg>0){
		rm_section_flag=1;
	      }
	      else if(valid_backet_deg<0){
		cout << "Error! Unbalance parentheses detected." << endl;
		return 2001;
	      }
	      //}
	  }
	  /*
	  check_line(file_buff,num_line,i,check_mode(generate_mode,"scientific"),check_tag(file_buff[i],"SCIENTIFIC"));
	  check_line(file_buff,num_line,i,check_mode(generate_mode,"robotics"),check_tag(file_buff[i],"ROBOTICS"));
	  check_line(file_buff,num_line,i,check_mode(generate_mode,"database"),check_tag(file_buff[i],"DATABASE"));
	  check_line(file_buff,num_line,i,check_mode(generate_mode,"xgboost"),check_tag(file_buff[i],"XGBOOST"));
	  check_line(file_buff,num_line,i,check_mode(generate_mode,"external"),check_tag(file_buff[i],"EXTERNAL"));
	  */
	  /*
	  if(!check_mode(generate_mode,"scientific")){
	    //deactivate scientific code
	    if(tagscientific){
	      for(j=i+1;j<num_line;j++){
		strcpy(file_buff[j-1],file_buff[j]);
	      }
	      num_line--;
	      i--;
	    }
	  }

	  if(!check_mode(generate_mode,"robotics")){
	    //deactivate robotics code
	    if(tagrobotics){
	      for(j=i+1;j<num_line;j++){
		strcpy(file_buff[j-1],file_buff[j]);
	      }
	      num_line--;
	      i--;
	    }
	  }

	  if(!check_mode(generate_mode,"robotics")){
	    //deactivate robotics code
	    if(tagrobotics){
	      for(j=i+1;j<num_line;j++){
		strcpy(file_buff[j-1],file_buff[j]);
	      }
	      num_line--;
	      i--;
	    }
	  }
	  */
	  if(check_mode(generate_mode,"cpu")){
	    //NO GPU mode
	    if(tagcuda && !tagcpp){ //remove all those CUDA lines
	      valid_backet_deg += check_num_tag_in_string(file_buff[i],"{") - check_num_tag_in_string(file_buff[i],"}");
	      
	      for(j=i+1;j<num_line;j++){
		strcpy(file_buff[j-1],file_buff[j]);
	      }
	      num_line--;
	      
	      if(valid_backet_deg>0){
		rm_section_flag=1;
	      }
	      else if(valid_backet_deg<0){
		cout << "Error! Unbalance parentheses detected." << endl;
		return 2001;
	      }
	    }
	    else{
	      if(!line_delete){
		remove_all_tag(file_buff[i]);
		i++;
	      }
	    }
	  }
	  else if(check_mode(generate_mode,"gpu")){
	    //CPU+GPU mode
	    if(!tagcuda && tagcpp){ //remove all cpu only lines
	      valid_backet_deg += check_num_tag_in_string(file_buff[i],"{") - check_num_tag_in_string(file_buff[i],"}");
	      
	      for(j=i+1;j<num_line;j++){
		strcpy(file_buff[j-1],file_buff[j]);
	      }
	      num_line--;
	      
	      if(valid_backet_deg>0){
		rm_section_flag=1;
	      }
	      else if(valid_backet_deg<0){
		cout << "Error! Unbalance parentheses detected." << endl;
		return 2001;
	      }
	    }
	    else{
	      for(j=0;j<num_tag;j++){
		replace_tag(file_buff[i],tag_list[j],tag_update[j]);
	      }
	      if(!line_delete){
		i++;
	      }
	    }
	  }
	}
      }
    }

    ofstream fout;
    fout.open(out_filename);
    
    for(i=0;i<num_line;i++){
      fout << file_buff[i] << endl;
    }
    
    fout.close();
    
    for(i=0;i<num_line;i++){
      delete [] file_buff[i];
    }
    delete [] file_buff;
    delete [] temp_buff;
    
    return 0;
  }
  else{
    return 1001;
  }
}

int generate_source(char *in_filename,char *generate_mode,char *out_filename){
  int i,j;

  int num_line = file_check_length(in_filename);
  if(num_line > 0){
    char **file_buff, *temp_buff;
    file_buff = new char *[num_line];
    for(i=0;i<num_line;i++){
      file_buff[i] = new char [buff_size];
    }
    temp_buff = new char [buff_size];
    
    ifstream fin;
    fin.open(in_filename);
    
    for(i=0;i<num_line;i++){
      fin.getline(file_buff[i],buff_size,'\n');
    }
    
    fin.close();

    int valid_backet_deg = 0;
    int active_comment = 0;

    bool tagcpp, tagcuda, taghost, tagdevice, tagglobal;
    bool tagscientific, tagrobotics, tagdatabase, tagxgboost, tagexternal;
    bool rm_section_flag = 0;
    i = 0;
    while(i<num_line){
      if(rm_section_flag){
	if(strstr(file_buff[i],"//")==NULL){
	  valid_backet_deg += check_num_tag_in_string(file_buff[i],"{") - check_num_tag_in_string(file_buff[i],"}");
	}
	else{
	  strcpy(temp_buff,file_buff[i]);
	  *strstr(temp_buff,"//") = '\0';
	  valid_backet_deg += check_num_tag_in_string(temp_buff,"{") - check_num_tag_in_string(temp_buff,"}");
	}
	for(j=i+1;j<num_line;j++){
	  strcpy(file_buff[j-1],file_buff[j]);
	}
	num_line--;

	if(valid_backet_deg==0){
	  rm_section_flag=0;
	}
	else if(valid_backet_deg<0){
	  cout << "Error! Unbalance parentheses detected." << endl;
	  return 2001;
	}
      }
      else{
	//check for comment block
	if(strncmp(file_buff[i],"/*",2) == 0 || (strstr(file_buff[i],"/*")!=NULL && *(strstr(file_buff[i],"/*")-1) != '/')){
	  while(strstr(file_buff[i],"*/")==NULL){
	    i++;
	  }
	  i++;
	}
	//check for comment line
	if(i < num_line){
	  //cout << file_buff[i] << endl;
	  while(strstr(file_buff[i],"//")!=NULL){
	    if(strstr(file_buff[i],";")==NULL && strstr(file_buff[i],"{")==NULL && strstr(file_buff[i],"}")==NULL){
	      //pure comment line, just skip
	      i++;
	    }
	    if(
	       (strstr(file_buff[i],";")!=NULL && strstr(file_buff[i],"//")!=NULL && strstr(file_buff[i],";")>strstr(file_buff[i],"//")) || 
	       (strstr(file_buff[i],"{")!=NULL && strstr(file_buff[i],"//")!=NULL && strstr(file_buff[i],"{")>strstr(file_buff[i],"//")) || 
	       (strstr(file_buff[i],"}")!=NULL && strstr(file_buff[i],"//")!=NULL && strstr(file_buff[i],"}")>strstr(file_buff[i],"//"))
	       ){
	      //commented out valid code, just skip
	      i++;
	    }
	    else{
	      //otherwise, there are comments in the code but the comments are after some valid code
	      break;
	    }
	  }
	
	  tagcpp = check_tag(file_buff[i],"###CPP###");
	  tagcuda = check_tag(file_buff[i],"###CUDA###");
	  tagscientific = check_tag(file_buff[i],"SCIENTIFIC");
	  tagrobotics = check_tag(file_buff[i],"ROBOTICS");
	  tagdatabase = check_tag(file_buff[i],"DATABASE");
	  tagxgboost = check_tag(file_buff[i],"XGBOOST");
	  tagexternal = check_tag(file_buff[i],"EXTERNAL");

	  bool line_delete = false;

	  if((!check_mode(generate_mode,"scientific") && tagscientific) || (!check_mode(generate_mode,"robotics") && tagrobotics) || (!check_mode(generate_mode,"database") && tagdatabase) || (!check_mode(generate_mode,"xgboost") && tagxgboost) || (!check_mode(generate_mode,"external") && tagexternal)){
	    //deactivate code
	    //if(tagscientific){
	      valid_backet_deg += check_num_tag_in_string(file_buff[i],"{") - check_num_tag_in_string(file_buff[i],"}");
	      
	      for(j=i+1;j<num_line;j++){
		strcpy(file_buff[j-1],file_buff[j]);
	      }
	      line_delete = true;
	      num_line--;
	      
	      if(valid_backet_deg>0){
		rm_section_flag=1;
	      }
	      else if(valid_backet_deg<0){
		cout << "Error! Unbalance parentheses detected." << endl;
		return 2001;
	      }
	      //}
	  }
	  /*
	  if(!check_mode(generate_mode,"robotics")){
	    //deactivate robotics code
	    if(tagrobotics){
	      valid_backet_deg += check_num_tag_in_string(file_buff[i],"{") - check_num_tag_in_string(file_buff[i],"}");
	      
	      for(j=i+1;j<num_line;j++){
		strcpy(file_buff[j-1],file_buff[j]);
	      }
	      line_delete = true;
	      num_line--;
	      
	      if(valid_backet_deg>0){
		rm_section_flag=1;
	      }
	      else if(valid_backet_deg<0){
		cout << "Error! Unbalance parentheses detected." << endl;
		return 2001;
	      }
	    }
	  }
	  */
	  if(check_mode(generate_mode,"cpu")){
	    //NO GPU mode
	    if(tagcuda && !tagcpp){ //remove all those CUDA lines
	      valid_backet_deg += check_num_tag_in_string(file_buff[i],"{") - check_num_tag_in_string(file_buff[i],"}");
	      
	      for(j=i+1;j<num_line;j++){
		strcpy(file_buff[j-1],file_buff[j]);
	      }
	      num_line--;
	      
	      if(valid_backet_deg>0){
		rm_section_flag=1;
	      }
	      else if(valid_backet_deg<0){
		cout << "Error! Unbalance parentheses detected." << endl;
		return 2001;
	      }
	    }
	    else{
	      if(!line_delete){
		remove_all_tag(file_buff[i]);
		i++;
	      }
	    }
	  }
	  else if(check_mode(generate_mode,"gpu")){
	    //CPU+GPU mode
	    if(!tagcuda && tagcpp){ //remove all cpu only lines
	      valid_backet_deg += check_num_tag_in_string(file_buff[i],"{") - check_num_tag_in_string(file_buff[i],"}");
	      
	      for(j=i+1;j<num_line;j++){
		strcpy(file_buff[j-1],file_buff[j]);
	      }
	      num_line--;
	      
	      if(valid_backet_deg>0){
		rm_section_flag=1;
	      }
	      else if(valid_backet_deg<0){
		cout << "Error! Unbalance parentheses detected." << endl;
		return 2001;
	      }
	    }
	    else{
	      for(j=0;j<num_tag;j++){
		replace_tag(file_buff[i],tag_list[j],tag_update[j]);
	      }
	      if(!line_delete){
		i++;
	      }
	    }
	  }
	}
      }
    }

    ofstream fout;
    fout.open(out_filename);
    
    for(i=0;i<num_line;i++){
      fout << file_buff[i] << endl;
    }
    
    fout.close();
    
    for(i=0;i<num_line;i++){
      delete [] file_buff[i];
    }
    delete [] file_buff;
    delete [] temp_buff;

    return 0;
  }
  else{
    return 1001;
  }
}

int check_tag(char *buff,char *tag){
  if(strstr(buff,tag)!=NULL){
    return 1;
  }
  else{
    return 0;
  }
}

int check_num_tag_in_string(char *to_check,char *tag){
  int result = 0;
  char *temp = to_check;
  while(strstr(temp,tag)!=NULL){
    temp = strstr(temp,tag)+1;
    result++;
  }
  return result;
}

int check_num_item_in_list(char *filename,char *list_name){
  int result = 0;
  bool found = 0;

  int num_line = file_check_length(filename);
  if(num_line > 0){
    char *temp = new char [buff_size];
    char *lookfor = new char [buff_size];
    ifstream fin;
    fin.open(filename);

    strcpy(lookfor,list_name);
    strcat(lookfor,":");
    int i=0;
    while(i<num_line){
      fin.getline(temp,buff_size);
      i++;
      if(strcmp(temp,lookfor)==0){
	found = 1;
	break;
      }
    }

    if(found){
      found = 0;
      strcpy(lookfor,"/");
      strcat(lookfor,list_name);
      while(i<num_line){
	fin.getline(temp,buff_size);
	i++;
	if(strcmp(temp,lookfor)==0){
	  found = 1;
	  break;
	}
	else{
	  result++;
	}
      }
    }
    
    fin.close();
    delete [] temp;
    delete [] lookfor;
  }

  if(found){
    return result;
  }
  else{
    return -1;
  }
}

int read_items_in_list(char *filename,char *list_name,int num_item,char **item){
  if(num_item>0){
    int num_line = file_check_length(filename);
    char *temp = new char [buff_size];
    char *lookfor = new char [buff_size];
    ifstream fin;
    fin.open(filename);

    strcpy(lookfor,list_name);
    strcat(lookfor,":");
    int i=0;
    while(i<num_line){
      fin.getline(temp,buff_size);
      i++;
      if(strcmp(temp,lookfor)==0){
	break;
      }
    }

    int num_item_read = 0;
    strcpy(lookfor,"/");
    strcat(lookfor,list_name);
    while(i<num_line){
      fin.getline(temp,buff_size);
      if(strcmp(temp,lookfor)==0){
	break;
      }
      else{
	strcpy(item[num_item_read],temp);
	i++;
	num_item_read++;
      }
    }

    fin.close();
    delete [] temp;
    delete [] lookfor;
  }

  return 0;
}

void remove_all_tag(char *buff){
  int i;
  char *temp_buff = new char [buff_size];

  for(i=0;i<num_tag;i++){
    strcpy(temp_buff,buff);

    if(strstr(temp_buff,tag_list[i])!=NULL){
      *strstr(temp_buff,tag_list[i])='\0';
      //cout << strstr(buff,tag_list[i])+strlen(tag_list[i]) << endl;
      strcat(temp_buff,strstr(buff,tag_list[i])+strlen(tag_list[i]));
      strcpy(buff,temp_buff);
    }
  }

  delete [] temp_buff;
}

void replace_tag(char *buff,const char *tag_to_replace,const char *replace_to){
  char *temp_buff = new char [buff_size];
  strcpy(temp_buff,buff);

  if(strstr(temp_buff,tag_to_replace)!=NULL){
    *strstr(temp_buff,tag_to_replace)='\0';
    strcat(temp_buff,replace_to);
    strcat(temp_buff,strstr(buff,tag_to_replace)+strlen(tag_to_replace));
  }

  strcpy(buff,temp_buff);

  delete [] temp_buff;
}

int generate_makefile(int num_list,char **listname,int *num_items,char*** items,char *library_name){
  int i,j,k;
  bool gpumode=0;
  char **inc_file = new char *[num_list];
  for(i=0;i<num_list;i++){
    inc_file[i] = new char [buff_size];
    strcpy(inc_file[i],"makefile.");
    strcat(inc_file[i],listname[i]);
    strcat(inc_file[i],".inc");

    if(strcmp(listname[i],"gpu")==0){
      gpumode=1;
    }
  }

  ofstream fout;

  ifstream fin;
  char *temp_buff = new char [buff_size];
  strcpy(temp_buff,PWD);
  strcat(temp_buff,"/src/Makefile");
  fout.open(temp_buff);
  if(gpumode){
    fout << "compiler=nvcc" << endl;
  }
  else{
    fout << "compiler=g++" << endl;
  }
  delete [] temp_buff;

  char *ctemp=new char [buff_size];
  for(i=0;i<num_list;i++){
    int numline = file_check_length(inc_file[i]);
    
    if(numline>0){
      //cout << "Reading " << inc_file[i] << endl;
      fin.open(inc_file[i]);

      for(j=0;j<numline;j++){
	fin.getline(ctemp,buff_size);
	fout << ctemp << endl;
      }

      fin.close();
      //cout << "Done reading " << inc_file[i] << endl;
    }
    /*
    else{
      cout << inc_file[i] << " not found. Skipping." << endl;
    }
    */
  }
  delete [] ctemp;

  fout << endl;
  for(i=0;i<num_list;i++){
    for(j=0;j<num_items[i];j++){
      if(strstr(items[i][j],".JEFFH")==NULL){
	fout << items[i][j] << "=" << items[i][j] << ".o" << endl;
      }
    }
  }

  fout << endl << "lib=";
  for(i=0;i<num_list;i++){
    for(j=0;j<num_items[i];j++){
      if(strstr(items[i][j],".JEFFH")==NULL){
	fout << "$(" << items[i][j] << ") ";
      }
    }
  }
  fout << endl << endl << "all:\tlibrary" << endl << endl;
  
  fout << "library: $(lib)" << endl;
  fout << "\tar cr " << library_name << ".a $(lib)" << endl << endl;

  if(gpumode){
    fout << ".c.o:" << endl;
    fout << "\tln -sf $*.c $*.cu" << endl;
    fout << "\t$(compiler) $(CFLAGS)" << (gpumode?" $(NCFLAGS)":"") << " -dc $*.cu" << endl;
  }
  fout << ".cpp.o:" << endl;
  fout << "\t$(compiler) $(CFLAGS)" << " -c $*.cpp" << endl;

  fout.close();
  for(i=0;i<num_list;i++){
    delete [] inc_file[i];
  }
  delete [] inc_file;

  return 0;
}
