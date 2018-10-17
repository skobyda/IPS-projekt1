#include <stdio.h>
#include<unistd.h>
#include<thread>
#include<queue>
#include<mutex>
#include<vector>
#include <iostream>
#include<string.h>
#include<regex>


std::vector<std::mutex *> zamky; /* pole zamku promenne velikosti */
std::vector<char *> line; /* pole charov premennej velkosti pre ulozenie riadkov */


/* Struktura ukladajuca regularne vyrazy REi a REPLi */
struct reg_exp_struct {
	std::regex re;
	char *repl;
};


char *to_cstr(std::string a) {
	// prevede retezec v c++ do retezce v "c" (char *)
	char *str;
	str=(char *) malloc(sizeof(char)*(a.length()+1));
	strcpy(str,a.c_str());
	return str;
}

char *read_line(int *res) {
	std::string line;
	char *str;
	if (std::getline(std::cin, line)) {
		str=to_cstr(line);
		*res=1;
		return str;
	} else {
		*res=0;
		return NULL;
	}

}

/*********************************/
/* Funkcia pre jednotlive vlakna */
/*         Paralelni SED         */
/*********************************/

void thr_f(struct reg_exp_struct reg_exp) {
	
	zamky[0]->lock(); 
	for(int i = 0; i < line.size(); i++){
		
		zamky[1]->lock();
		zamky[0]->unlock();
		
		std::string res = std::regex_replace( line[i], reg_exp.re, reg_exp.repl);
		std::cout << res << "\n";
		
		zamky[1]->unlock();
		zamky[0]->lock();
	}
	zamky[0]->unlock();
}

void errorArg()
{
	printf("ERROR => problem with arguments.");
}

int main(int argc, char **argv) {
	
	if (argc < 3){
		errorArg();
		return 0;	
	}

	if ((argc - 1) % 2 == 1){
		errorArg();
		return 0;
	}
	
	/*******************************
	 * Inicializace threadu a zamku
	 * *****************************/
	int num = (argc - 1) / 2;
	int num_zamky = 2;
	
	std::vector <std::thread *> threads; /* pole threadu promenne velikosti */
	
	/* pole struktur na ukladanie regularnych vyrazov*/
        std::vector<reg_exp_struct> reg_exp;    
        
        /* ukadanie regularnych vyrazov do pola struktur*/
        for(int i = 0; i < num; i++){
                reg_exp.push_back(reg_exp_struct()); // constructor noveho elementu struktury
                reg_exp[i].re = argv[i*2+1];
                reg_exp[i].repl = argv[i*2+2];
        } 

	/* vytvorime zamky */
	zamky.resize(num_zamky); /* nastavime si velikost pole zamky */
	for(int i = 0; i < num_zamky; i++){	
		std::mutex *new_zamek = new std::mutex();
		zamky[i] = new_zamek;
	}

	/* vytvorime thready */
	threads.resize(num); /* nastavime si velikost pole threads */
	zamky[0]->lock();
	for(int i = 0; i < num; i++){	
		std::thread *new_thread = new std::thread (thr_f, reg_exp[i]);
		threads[i] = new_thread;
	}

	/**********************************
	 * Vlastni vypocet psed
	 * ********************************/
	int res = 1;
	char *str = read_line(&res);
	while(res) {
		line.push_back(str);
		str = read_line(&res);
	}
	
	zamky[0]->unlock();
	/**********************************
	 * Uvolneni pameti
	 * ********************************/

	/* provedeme join a uvolnime pamet threads */
	for(int i = 0; i < num; i++){
		(*(threads[i])).join();
		delete threads[i];
	}
	/* uvolnime pamet zamku */
	for(int i = 0; i < num_zamky; i++){
		delete zamky[i];
	}
}
