#include<cstring>
#include<iostream>

#include"file_check.h"
#include"file_handler.h"
#include"string_man.h"

int main(int argc,char *argv[]){
  if(argc < 3){
    std::cout << "Usage: " << argv[0] << " {first file} {second file}" << std::endl;
    return 0;
  }

  libjeff::file_handler *file1 = NULL, *file2 = NULL;

  openfile(file1,argv[1]);
  openfile(file2,argv[2]);

  if(file1 != NULL && file2 != NULL){
    char *buff1 = NULL, *buff2 = NULL;
    size_t size[2];

    file1->read_file(buff1,size[0]);
    file2->read_file(buff2,size[1]);

    if(size[0] > 0 && size[1] > 0){
      int nline1, nline2;
      nline1 = string_count_pattern(buff1,"\n");
      nline2 = string_count_pattern(buff2,"\n");

      if(nline1 > 0 && nline2 > 0){
	bool *match1 = new bool [nline1];
	bool *match2 = new bool [nline2];

	int i,j;
	char **lines1 = new char *[nline1];
	char **lines2 = new char *[nline2];
	for(i=0;i<nline1;i++){
	  match1[i] = false;
	  lines1[i] = new char [size[0]+10];
	  string_extract_field(buff1,"\n",i,lines1[i]);
	}
	for(i=0;i<nline2;i++){
	  match2[i] = false;
	  lines2[i] = new char [size[1]+10];
	  string_extract_field(buff2,"\n",i,lines2[i]);
	}

	for(i=0;i<nline1;i++){
	  for(j=0;j<nline2;j++){
	    if(!match2[j]){
	      if(strcmp(lines1[i],lines2[j])==0){
		match1[i] = true;
		match2[j] = true;
		break;
	      }
	    }
	  }
	}

	std::cout << "Lines in " << argv[1] << " that cannot be matched to " << argv[2] << std::endl;
	for(i=0;i<nline1;i++){
	  if(!match1[i]){
	    std::cout << lines1[i] << std::endl;
	  }
	}
	
	std::cout << "Lines in " << argv[2] << " that cannot be matched to " << argv[1] << std::endl;
	for(i=0;i<nline2;i++){
	  if(!match2[i]){
	    std::cout << lines2[i] << std::endl;
	  }
	}

	for(i=0;i<nline1;i++){
	  delete [] lines1[i];
	}
	delete [] lines1;
	for(i=0;i<nline2;i++){
	  delete [] lines2[i];
	}
	delete [] lines2;
	
	delete [] match1;
	delete [] match2;
      }
    }

    if(buff1 != NULL){
      delete [] buff1;
    }
    if(buff2 != NULL){
      delete [] buff2;
    }
  }

  if(file1 != NULL){
    file1->close();
    delete file1;
  }
  if(file2 != NULL){
    file2->close();
    delete file2;
  }

  return 0;
}
