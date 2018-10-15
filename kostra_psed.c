#include <stdio.h>
#include<unistd.h>
#include<thread>
#include<queue>
#include<mutex>
#include<vector>
#include <iostream>
#include<string.h>



std::vector<std::mutex *> zamky; /* pole zamku promenne velikosti */

struct reg_exp_struct {
	char *re;
	char *repl;
};

char *line;

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


void thr_f(struct reg_exp_struct reg_exp) {
	/* funkce implementujici thread */
	printf("Thread %s started\n",reg_exp.re);
}

int main(int argc, char **argv) {
	
	if (argc < 3){
		//error
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
                reg_exp[i].repl = argv[i*2+1];
        } 

	/* vytvorime zamky */
	zamky.resize(num_zamky); /* nastavime si velikost pole zamky */
	for(int i = 0; i < num_zamky; i++){	
		std::mutex *new_zamek = new std::mutex();
		zamky[i] = new_zamek;
	}

	/* vytvorime thready */
	threads.resize(num); /* nastavime si velikost pole threads */
	for(int i = 0; i < num; i++){	
		std::thread *new_thread = new std::thread (thr_f, reg_exp[i]);
		threads[i] = new_thread;
	}

	/**********************************
	 * Vlastni vypocet psed
	 * ********************************/
	int res;
	line = read_line(&res);
	while (res) {
		printf("%s\n",line);
		free(line); /* uvolnim pamet */
		line = read_line(&res);
	}


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
