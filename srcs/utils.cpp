#include "paging.hpp"

void printInfo(void) {
	cout << "Usage : ./paging (time quantum) (logging time tick)\n\n";
	cout << "Example" << endl;
	cout << "./paging 3 1" << endl;
	cout << "=> paging simulator will run with 3 unit time quantum and write log with 1 unit interval" << endl << endl;
	cout << "OR just run './paging reset' to reset backing store" << endl;
}

void checkBackingStorePath(void) {
	struct stat st;
	if (stat("backing_store", &st) == -1) {
		mkdir("backing_store", 0777);
	}
}

void resetBackingStore(void) {
	system("cd backing_store && rm -rf `ls | grep -v .gitignore`");
}

string getStoreFileName(unsigned short page_idx) {
	string store_file_name("backing_store/");
	store_file_name.append(to_string(page_idx));
	return store_file_name;
}

void checkBackingStore(unsigned short page_idx) {
	string store_file_name = getStoreFileName(page_idx);
	
	if (access( store_file_name.c_str(), F_OK ) == -1) {
		ofstream target(store_file_name);
		for (int i = 0; i < PAGE_SIZE; i++)
			target << "0 ";
		target.close();
	}
}

void insertMemory(unsigned short* memory, unsigned short page_idx) {
	ifstream target(getStoreFileName(page_idx));
	for (int i = 0; i < PAGE_SIZE; i++)
		target >> *(memory + i);
	target.close();
}

void writeStoreData(unsigned short* memory, unsigned short page_idx) {
	ofstream target(getStoreFileName(page_idx), ios_base::trunc);
	for (int i = 0; i < PAGE_SIZE; i++)
		target << *(memory + i) << ' ';
	target.close();
}

unsigned short* mmu(unsigned short* const memory, unsigned short page_idx) {
	if (memory == NULL) return NULL;
	return memory + page_idx * PAGE_SIZE;
}