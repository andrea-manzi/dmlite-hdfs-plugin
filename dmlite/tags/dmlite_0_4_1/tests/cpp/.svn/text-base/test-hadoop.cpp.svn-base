#include <dmlite/dmlite++.h>
#include <plugins/hadoop/Hadoop.h>

int main(int argn, char **argv)
{
	dmlite::HadoopIOFactory io;

	dmlite::HadoopIOHandler *handler = io.createIO("/dpm/myfile.txt", std::ios_base::in);

//	handler->deleteFile("/dpm/myfile.txt");
//	handler->deleteFile("/dpm/myfile2.txt");
//	handler->deleteFile("/dpm/myfile3.txt");
//	handler->deleteFile("/dpm/myfile4.txt");
//	handler->deleteFile("/dpm/myfile5.txt");

	//std::string test;
//	const char *test = "This file has been written by DMLITE, this is the first of a long long long serie\0";
	char test[1024] = {'\0'};

	memset(test, '\0', 1024);;
	handler->read(test, 10);
	std::cout << test;


	handler->seek(10, std::ios_base::cur);

	int i = 0;	
	while(handler->eof() != true){
		memset(test, '\0', 1024);;
		handler->read(test, 3);
		std::cout << test;
		i++;
	}

 	std::cout << std::endl;
	std::cout << i << std::endl;
	delete(handler);

  return 0;
}
