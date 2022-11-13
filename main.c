#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "structs.h"
// functia apelata cand se introduce comanda "insert"
int add_last(void **arr, int *len, data_structure *data){
	// cazul in care se adauga primul element
	if(*len == 0){
		// lungimea se initializeaza cu lungimea headerului (plus padding) + lungimea blocului de date ce va fi introdus
		*len = sizeof(head) + data->header->len;
		// se aloca in arr, len octeti pentru introducerea datelor
		*arr = calloc(1, *len);
		// intai se introduce headerul (cu tot cu padding)
		memmove(*arr, data->header, sizeof(head));
		// imediat dupa header se introduc datele efective
		memmove(*arr + sizeof(head), data->data, data->header->len);
	}
	// cazul in care se adauga dupa primul element
	else{
		// daca deja exista macar un element in arr, se realoca memorie astfel: se adauga la fosta lungime (in octeti) spatiu
		// pentru inca un header si lungimea urmatorului bloc de date
		*arr = realloc(*arr, *len + sizeof(head) + data->header->len);
		// la octetul de pe pozitia len se adauga urmatorul header
		memmove(*arr + *len, data->header, sizeof(head));
		// la octetul imediat de dupa headerul adaugat, se adauga blocul de date
		memmove(*arr + *len + sizeof(head), data->data, data->header->len);
		// creste len cu numarul de octeti ce a fost adaugat
		*len += sizeof(head) + data->header->len;
	}
	return 0;
}
// functia apelata cand se introduce comanda "insert_at"
int add_at(void **arr, int *len, data_structure *data, int index){
	// folosesc aux pentru a calcula offset-ul i in functie de indexul elementului din vector
	data_structure *aux;
	// in dataBlock tin minte toate datele de dupa indexul la care trebuie sa inserez un element
	// pentru a le copia ulterior in vectorul marit
	void *dataBlock;
	int i = 0;
	// daca indexul e negativ, se intoarce un cod de eroare
	if(index < 0)
		return -1;
	// calculez offset-ul de octeti in i folosindu-ma de valoarea data de index, deoarece
	// len-ul meu tine minte numarul de octeti, nu cel de elemente
	while(index > 0){
		// aloc si eliberez memorie constant pentru a afla valorile din aux->header->len si a le adauga la offset
		aux = calloc(1, sizeof(data_structure));
		aux->header = calloc(1, sizeof(head));
		memmove(aux->header, *arr + i, sizeof(head));
		aux->data = calloc(1, aux->header->len);
		memmove(aux->data, *arr + i + sizeof(head), aux->header->len);
		index--;
		i += aux->header->len + sizeof(head);
		free(aux->data);
		free(aux->header);
		free(aux);
		// daca offset-ul de octeti ajunge la  lungimea totala de octeti sau o depaseste, se apeleaza add_last si se
		// opreste executia functiei
		if(i >= *len){
			add_last(arr, len, data);
			return 0;
		}
	}
	// aloc pentru dataBlock lungimea necesara pentru a retine toate elementele de dupa offset-ul i, adica lungimea
	// intregului vector - acel offset
	dataBlock = calloc(1, *len - i);
	// dupa aceea mut in data block continutul adresei vectorului cu offset-ul i
	memmove(dataBlock, *arr + i, *len - i);
	// realoc vectorul astfel incat sa aiba loc si elementul dat ca parametru
	*arr = realloc(*arr, *len + sizeof(head) + data->header->len);
	// mut, pe rand, campurile din data la adresa vectorului + offset
	memmove(*arr + i, data->header, sizeof(head));
	memmove(*arr + i + sizeof(head), data->data, data->header->len);
	// dupa ce am introdus elementul nou, introduc datele retinute anterior in dataBlock
	memmove(*arr + i + sizeof(head) + data->header->len, dataBlock, *len - i);
	// lungimea finala a vectorului se mareste cu lungimea elementului introdus
	*len += sizeof(head) + data->header->len;
	free(dataBlock);
	return 0;
}
// functia apelata cand se introduce comanda "find"
void find(void *data_block, int len, int index){
	// valorile auxiliare sunt aceleasi cu cele din functia print
	data_structure *aux;
	char *data;
	int name1Len, i = 0;
	int8_t bill1T1, bill2T1;
	int16_t bill1T2;
	int32_t bill2T2, bill1T3, bill2T3;
	// verific daca indexul dat ca parametru e pozitiv pentru a continua operatiile
	if(index < 0)
		return;
	// calculez offset-ul de octeti in i folosindu-ma de valoarea data de index, deoarece
	// len-ul meu tine minte numarul de octeti, nu cel de elemente
	while(index > 0){
		// aloc si eliberez memorie constant pentru a afla valorile din aux->header->len si a le adauga la offset
		aux = calloc(1, sizeof(data_structure));
		aux->header = calloc(1, sizeof(head));
		memmove(aux->header, data_block + i, sizeof(head));
		aux->data = calloc(1, aux->header->len);
		memmove(aux->data, data_block + i + sizeof(head), aux->header->len);
		index--;
		i += aux->header->len + sizeof(head);
		free(aux->data);
		free(aux->header);
		free(aux);
		// daca offset-ul de octeti ajunge la lungimea totala de octeti sau o depaseste, se opreste executia functiei
		if(i >= len)
			return;
	}
	// acum ca am aflat care e offset-ul de la care incepe elementul cautat, execut instructiunile
	// de printare, copiate din print, pentru un singur element
	aux = calloc(1, sizeof(data_structure));
	aux->header = calloc(1, sizeof(head));
	memmove(aux->header, data_block + i, sizeof(head));
	aux->data = calloc(1, aux->header->len);
	memmove(aux->data, data_block + i + sizeof(head), aux->header->len);
	// afisez tipul de dedicatie
	printf("Tipul %c\n", aux->header->type);
	// aloc memorie si copiez in data blocul de date din structura
	data = calloc(1, aux->header->len);
	memmove(data, aux->data, aux->header->len);
	// deoarece in data se afla caractere '\0', folosesc strlen pentru a afla lungimea
	// primului nume inserat
	name1Len = strlen(data) + 1;
	// afisez acel nume
	printf("%s pentru ", data);
	// afisarea celorlalte date se intampla conditionat, in functie de tipul dedicatiei
	switch(aux->header->type){
		// pentru tipul 1, copiez cate un octet la adresa variabilelor auxiliare de tip int8_t,
		// apoi mut aux->header->len - name1Len - 2 (lungimea celui de-al doilea nume) octeti
		// la inceputul adresei lui data, pentru a afisa cel de-al doilea nume; apoi sunt afisate
		// bancnotele
		case '1':
			memmove(&bill1T1, data + name1Len, 1);
			memmove(&bill2T1, data + name1Len + 1, 1);
			memmove(data, data + name1Len + 2, aux->header->len - name1Len - 2);
			printf("%s\n", data);
			printf("%"PRId8"\n%"PRId8"\n", bill1T1, bill2T1);
			break;
		// pentru tipul 2, copiez cate 2, respectiv 4 octeti la adresa variabilelor auxiliare de tip int16_t,
		// respectiv int32_t, apoi mut aux->header->len - name1Len - 6 (lungimea celui de-al doilea nume) octeti
		// la inceputul adresei lui data, pentru a afisa cel de-al doilea nume; apoi sunt afisate
		// bancnotele
		case '2':
			memmove(&bill1T2, data + name1Len, 2);
			memmove(&bill2T2, data + name1Len + 2, 4);
			memmove(data, data + name1Len + 6, aux->header->len - name1Len - 6);
			printf("%s\n", data);
			printf("%"PRId16"\n%"PRId32"\n", bill1T2, bill2T2);
			break;
		// pentru tipul 3, copiez cate 4 octeti la adresa variabilelor auxiliare de tip int32_t,
		// apoi mut aux->header->len - name1Len - 8 (lungimea celui de-al doilea nume) octeti
		// la inceputul adresei lui data, pentru a afisa cel de-al doilea nume; apoi sunt afisate
		// bancnotele
		case '3':
			memmove(&bill1T3, data + name1Len, 4);
			memmove(&bill2T3, data + name1Len + 4, 4);
			memmove(data, data + name1Len + 8, aux->header->len - name1Len - 8);
			printf("%s\n", data);
			printf("%"PRId32"\n%"PRId32"\n", bill1T3, bill2T3);
			break;
	}
	// se afiseaza newline-ul dintre dedicatii
	printf("\n");
	free(data);
	free(aux->data);
	free(aux->header);
	free(aux);
}
// functia apelata cand se introduce comanda "delete_at"
int delete_at(void **arr, int *len, int index){
	data_structure *aux;
	void *dataBlock;
	int i = 0;
	if(index < 0)
		return -1;
	// calculez offset-ul de octeti in i folosindu-ma de valoarea data de index, deoarece
	// len-ul meu tine minte numarul de octeti, nu cel de elemente
	while(index > 0){
		// aloc si eliberez memorie constant pentru a afla valorile din aux->header->len si a le adauga la offset
		aux = calloc(1, sizeof(data_structure));
		aux->header = calloc(1, sizeof(head));
		memmove(aux->header, *arr + i, sizeof(head));
		aux->data = calloc(1, aux->header->len);
		memmove(aux->data, *arr + i + sizeof(head), aux->header->len);
		index--;
		i += aux->header->len + sizeof(head);
		free(aux->data);
		free(aux->header);
		free(aux);
		// daca offset-ul de octeti ajunge la  lungimea totala de octeti sau o depaseste, se opreste executia functiei
		if(i >= *len)
			return -1;
	}
	// acum ca stiu care e offset-ul de pornire a elementului pe care vreau sa il sterg, tin minte in aux datele
	// acelui element, iar in dataBlock tin minte toate datele incepand cu elementul de dupa, pentru a putea
	// muta acele date pe pozitia elementului ce trebuie sters
	aux = calloc(1, sizeof(data_structure));
	aux->header = calloc(1, sizeof(head));
	memmove(aux->header, *arr + i, sizeof(head));
	aux->data = calloc(1, aux->header->len);
	memmove(aux->data, *arr + i + sizeof(head), aux->header->len);
	// lungimea cu care aloc un bloc de date in dataBlock este gandita in felul urmator: din lungimea totala
	// a vectorului se scade lungimea elementului ce trebuie sters(sizeof(head) + aux->header->len), precum si
	// offsetul de pornire al elementului ce trebuie sters
	dataBlock = calloc(1, *len - i - aux->header->len - sizeof(head));
	// datele care sunt mutate sunt mutate cu aceeasi lungime de la sursa vectorului + offset de pornire +
	// lungimea elementului ce trebuie sters
	memmove(dataBlock, *arr + i + sizeof(head) + aux->header->len, *len - i - aux->header->len - sizeof(head));
	// se suprascriu datele de la offset cu cele din dataBlock
	memmove(*arr + i, dataBlock, *len - i - aux->header->len - sizeof(head));
	// lungimea finala a vectorului se modifica, si memoria alocata la acea adresa de memorie scade
	*len = *len - aux->header->len - sizeof(head);
	*arr = realloc(*arr, *len);
	free(dataBlock);
	free(aux->header);
	free(aux->data);
	free(aux);
	return 0;
}
// functia apelata cand se introduce comanda "print"
void print(void *arr, int len){
	// folosesc structura aux pentru a tine minte toate elementele din arr + un offset i
	// rearanjate intr-o structura de tip data_structure
	data_structure *aux;
	// in variabil auxiliara de tip char * tin minte toate datele din aux->data pentru usurinta
	// la afisarea numelor
	char *data;
	// name1Len este lungimea primului nume inserat
	int i = 0, name1Len;
	// variabilele billxTy sunt folosite pentru a retine datele din array care se afla intre cele
	// doua siruri de caractere
	int8_t bill1T1, bill2T1;
	int16_t bill1T2;
	int32_t bill2T2, bill1T3, bill2T3;
	// parcurg intreg vectorul pana cand acesta depaseste lungimea (numar de octeti)
	while(i < len){
		// aloc memorie pentru structura in sine
		aux = calloc(1, sizeof(data_structure));
		// aloc memorie si adaug headerul elementul de pe pozitia i in headerul structurii
		aux->header = calloc(1, sizeof(head));
		memmove(aux->header, arr + i, sizeof(head));
		// aloc memorie (numarul de octeti e preluat din header) pentru blocul de date si apoi initializez
		// cu datele efective
		aux->data = calloc(1, aux->header->len);
		memmove(aux->data, arr + i + sizeof(head), aux->header->len);
		// afisez tipul de dedicatie
		printf("Tipul %c\n", aux->header->type);
		// aloc memorie si copiez in data blocul de date din structura
		data = calloc(1, aux->header->len);
		memmove(data, aux->data, aux->header->len);
		// deoarece in data se afla caractere '\0', folosesc strlen pentru a afla lungimea
		// primului nume inserat
		name1Len = strlen(data) + 1;
		// afisez acel nume
		printf("%s pentru ", data);
		// afisarea celorlalte date se intampla conditionat, in functie de tipul dedicatiei
		switch(aux->header->type){
			// pentru tipul 1, copiez cate un octet la adresa variabilelor auxiliare de tip int8_t,
			// apoi mut aux->header->len - name1Len - 2 (lungimea celui de-al doilea nume) octeti
			// la inceputul adresei lui data, pentru a afisa cel de-al doilea nume; apoi sunt afisate
			// bancnotele
			case '1':
				memmove(&bill1T1, data + name1Len, 1);
				memmove(&bill2T1, data + name1Len + 1, 1);
				memmove(data, data + name1Len + 2, aux->header->len - name1Len - 2);
				printf("%s\n", data);
				printf("%"PRId8"\n%"PRId8"\n", bill1T1, bill2T1);
				break;
			// pentru tipul 2, copiez cate 2, respectiv 4 octeti la adresa variabilelor auxiliare de tip int16_t,
			// respectiv int32_t, apoi mut aux->header->len - name1Len - 6 (lungimea celui de-al doilea nume) octeti
			// la inceputul adresei lui data, pentru a afisa cel de-al doilea nume; apoi sunt afisate
			// bancnotele
			case '2':
				memmove(&bill1T2, data + name1Len, 2);
				memmove(&bill2T2, data + name1Len + 2, 4);
				memmove(data, data + name1Len + 6, aux->header->len - name1Len - 6);
				printf("%s\n", data);
				printf("%"PRId16"\n%"PRId32"\n", bill1T2, bill2T2);
				break;
			// pentru tipul 3, copiez cate 4 octeti la adresa variabilelor auxiliare de tip int32_t,
			// apoi mut aux->header->len - name1Len - 8 (lungimea celui de-al doilea nume) octeti
			// la inceputul adresei lui data, pentru a afisa cel de-al doilea nume; apoi sunt afisate
			// bancnotele
			case '3':
				memmove(&bill1T3, data + name1Len, 4);
				memmove(&bill2T3, data + name1Len + 4, 4);
				memmove(data, data + name1Len + 8, aux->header->len - name1Len - 8);
				printf("%s\n", data);
				printf("%"PRId32"\n%"PRId32"\n", bill1T3, bill2T3);
				break;
		}
		// se afiseaza newline-ul dintre dedicatii
		printf("\n");
		// offset-ul de parcurgere se incrementeaza cu lungimea unui element din arr (lungimea unui header
		// + lungimea blocului de date)
		i += aux->header->len + sizeof(head);
		// se elibereaza toate variabilele alocate dinamic
		free(data);
		free(aux->header);
		free(aux->data);
		free(aux);
	}
}
int main(){
	void *arr = NULL;
	char *command = calloc(256, sizeof(char)), *commandCpy = calloc(256, sizeof(char)), *name1, *name2;
	unsigned char type;
	data_structure *d;
	int len = 0, i, sizeName1, sizeName2, dataLen;
	int8_t bill1T1, bill2T1;
	int16_t bill1T2;
	int32_t bill2T2, bill1T3, bill2T3;
	// se citeste un buffer de 256 de caractere pana la introducerea comenzii exit
	do{
		fgets(command, 256, stdin);
		// fgets adauga un '\n' la finalul buffer-ului, asa ca il elimin manual, adaugand '\0' pe pozitia sa
		command[strlen(command) - 1] = '\0';
		// pentru functiile add_last, add_at, find, delete_at verific daca se apeleaza functia cu strstr, deoarece in buffer-ul
		// command mai sunt si alte caractere
		if(strstr(command, "insert ") != NULL){
			// aloc un spatiu pentru structura d in sine
			d = calloc(1, sizeof(data_structure));
			// numarul final de octeti in d->data
			dataLen = 0;
			// copie pentru o a doua parcurgere a comenzii cu strtok
			strcpy(commandCpy, command);
			// apelez prima oara strtok pentru a plasa pointerul la caracterul tipului de dedicatie
			strtok(command, " ");
			// variabila type retine continutul primului octet al valorii urmatorului apel al functiei strtok, castat la unsigned char
			type = (unsigned char)(*strtok(NULL, " "));
			// aflu doar cati octeti trebuie alocati pentru primul nume, dar strtok tokenizeaza deja stringul, asa ca trebuie sa folosesc
			// o copie pentru a introduce sirul efectiv in structura d
			sizeName1 = strlen(strtok(NULL, " ")) + 1;
			// lungimea lui d->data creste cu numarul de octeti din primul nume
			dataLen += sizeName1;
			switch(type){
				// pentru tipul 1, se mai adauga 2 octeti (2 * sizeof(int8_t)) la lungimea lui d->data si se initializeaza bancnotele
				// de tip 1
				case '1':
					dataLen += 2;
					bill1T1 = (int8_t)(atoi(strtok(NULL, " ")));
					bill2T1 = (int8_t)(atoi(strtok(NULL, " ")));
					break;
				// pentru tipul 2, se mai adauga 6 octeti (sizeof(int16_t) + sizeof(int32_t)) la lungimea lui d->data si se initializeaza
				// bancnotele de tip 2
				case '2':
					dataLen += 6;
					bill1T2 = (int16_t)(atoi(strtok(NULL, " ")));
					bill2T2 = (int32_t)(atoi(strtok(NULL, " ")));
					break;
				// pentru tipul 3, se mai adauga 8 octeti (2 * sizeof(int32_t)) la lungimea lui d->data si se initializeaza bancnotele
				// de tip 3
				case '3':
					dataLen += 8;
					bill1T3 = (int32_t)(atoi(strtok(NULL, " ")));
					bill2T3 = (int32_t)(atoi(strtok(NULL, " ")));
					break;
			}
			// analog, aflu cati octeti trebuie adaugati pentru marimea celui de-al doilea nume
			sizeName2 = strlen(strtok(NULL, " ")) + 1;
			// mai adaug lungimea celui de-al doilea nume la lungimea totala a lui d->data
			dataLen += sizeName2;
			// aloc memoria necesara si pentru campurile structurii
			d->header = calloc(1, sizeof(head));
			d->data = calloc(1, dataLen);
			// acum parcurg copia buffer-ului de comanda cu strtok pentru a prelua datele efective si a le pune in structura
			strtok(commandCpy, " ");
			strtok(NULL, " ");
			// initializez campurile din header
			d->header->type = type;
			d->header->len = dataLen;
			// aloc exact spatiul necesar pentru cele doua nume
			name1 = calloc(sizeName1, sizeof(char));
			name2 = calloc(sizeName2, sizeof(char));
			// la a doua parcurgere, initializez numele pentru a le introduce in campul de date
			memmove(name1, strtok(NULL, " "), sizeName1);
			strtok(NULL, " ");
			strtok(NULL, " ");
			memmove(name2, strtok(NULL, " "), sizeName2);
			// introduc primul nume in structura
			memmove(d->data, name1, sizeName1);
			// pe cazuri, introduc restul datelor in structura
			switch(type){
				// pentru tipul 1, se adauga cate un octet pentru fiecare bancnota, iar dupa se adauga numele 2
				case '1':
					memmove(d->data + sizeName1, &bill1T1, 1);
					memmove(d->data + sizeName1 + 1, &bill2T1, 1);
					memmove(d->data + sizeName1 + 2, name2, sizeName2);
					break;
				// pentru tipul 2, se adauga 2 si 4 octeti, pentru prima, respectiv a doua bancnota, iar dupa se adauga numele 2
				case '2':
					memmove(d->data + sizeName1, &bill1T2, 2);
					memmove(d->data + sizeName1 + 2, &bill2T2, 4);
					memmove(d->data + sizeName1 + 6, name2, sizeName2);
					break;
				// pentru tipul 3, se adauga cate 4 octeti pentru fiecare bancnota, iar dupa se adauga numele 2
				case '3':
					memmove(d->data + sizeName1, &bill1T3, 4);
					memmove(d->data + sizeName1 + 4, &bill2T3, 4);
					memmove(d->data + sizeName1 + 8, name2, sizeName2);
					break;
			}
			add_last(&arr, &len, d);
			free(name1);
			free(name2);
			free(d->header);
			free(d->data);
			free(d);
		}
		else if(strstr(command, "insert_at ") != NULL){
			// aloc un spatiu pentru structura d in sine
			d = calloc(1, sizeof(data_structure));
			// numarul final de octeti in d->data
			dataLen = 0;
			// copie pentru o a doua parcurgere a comenzii cu strtok
			strcpy(commandCpy, command);
			// apelez prima oara strtok pentru a plasa pointerul la caracterul tipului de dedicatie
			strtok(command, " ");
			// primul caracter de dupa numele comenzii este indexul pe care il retin in i
			i = atoi(strtok(NULL, " "));
			// variabila type retine continutul primului octet al valorii urmatorului apel al functiei strtok, castat la unsigned char
			type = (unsigned char)(*strtok(NULL, " "));
			// aflu doar cati octeti trebuie alocati pentru primul nume, dar strtok tokenizeaza deja stringul, asa ca trebuie sa folosesc
			// o copie pentru a introduce sirul efectiv in structura d
			sizeName1 = strlen(strtok(NULL, " ")) + 1;
			// lungimea lui d->data creste cu numarul de octeti din primul nume
			dataLen += sizeName1;
			switch(type){
				// pentru tipul 1, se mai adauga 2 octeti (2 * sizeof(int8_t)) la lungimea lui d->data si se initializeaza bancnotele
				// de tip 1
				case '1':
					dataLen += 2;
					bill1T1 = (int8_t)(atoi(strtok(NULL, " ")));
					bill2T1 = (int8_t)(atoi(strtok(NULL, " ")));
					break;
				// pentru tipul 2, se mai adauga 6 octeti (sizeof(int16_t) + sizeof(int32_t)) la lungimea lui d->data si se initializeaza
				// bancnotele de tip 2
				case '2':
					dataLen += 6;
					bill1T2 = (int16_t)(atoi(strtok(NULL, " ")));
					bill2T2 = (int32_t)(atoi(strtok(NULL, " ")));
					break;
				// pentru tipul 3, se mai adauga 8 octeti (2 * sizeof(int32_t)) la lungimea lui d->data si se initializeaza bancnotele
				// de tip 3
				case '3':
					dataLen += 8;
					bill1T3 = (int32_t)(atoi(strtok(NULL, " ")));
					bill2T3 = (int32_t)(atoi(strtok(NULL, " ")));
					break;
			}
			// analog, aflu cati octeti trebuie adaugati pentru marimea celui de-al doilea nume
			sizeName2 = strlen(strtok(NULL, " ")) + 1;
			// mai adaug lungimea celui de-al doilea nume la lungimea totala a lui d->data
			dataLen += sizeName2;
			// aloc memoria necesara si pentru campurile structurii
			d->header = calloc(1, sizeof(head));
			d->data = calloc(1, dataLen);
			// acum parcurg copia buffer-ului de comanda cu strtok pentru a prelua datele efective si a le pune in structura
			strtok(commandCpy, " ");
			strtok(NULL, " ");
			strtok(NULL, " ");
			// initializez campurile din header
			d->header->type = type;
			d->header->len = dataLen;
			// aloc exact spatiul necesar pentru cele doua nume
			name1 = calloc(sizeName1, sizeof(char));
			name2 = calloc(sizeName2, sizeof(char));
			// la a doua parcurgere, initializez numele pentru a le introduce in campul de date
			memmove(name1, strtok(NULL, " "), sizeName1);
			strtok(NULL, " ");
			strtok(NULL, " ");
			memmove(name2, strtok(NULL, " "), sizeName2);
			// introduc primul nume in structura
			memmove(d->data, name1, sizeName1);
			// pe cazuri, introduc restul datelor in structura
			switch(type){
				// pentru tipul 1, se adauga cate un octet pentru fiecare bancnota, iar dupa se adauga numele 2
				case '1':
					memmove(d->data + sizeName1, &bill1T1, 1);
					memmove(d->data + sizeName1 + 1, &bill2T1, 1);
					memmove(d->data + sizeName1 + 2, name2, sizeName2);
					break;
				// pentru tipul 2, se adauga 2 si 4 octeti, pentru prima, respectiv a doua bancnota, iar dupa se adauga numele 2
				case '2':
					memmove(d->data + sizeName1, &bill1T2, 2);
					memmove(d->data + sizeName1 + 2, &bill2T2, 4);
					memmove(d->data + sizeName1 + 6, name2, sizeName2);
					break;
				// pentru tipul 3, se adauga cate 4 octeti pentru fiecare bancnota, iar dupa se adauga numele 2
				case '3':
					memmove(d->data + sizeName1, &bill1T3, 4);
					memmove(d->data + sizeName1 + 4, &bill2T3, 4);
					memmove(d->data + sizeName1 + 8, name2, sizeName2);
					break;
			}
			add_at(&arr, &len, d, i);
			free(name1);
			free(name2);
			free(d->header);
			free(d->data);
			free(d);
		}
		else if(strstr(command, "find ") != NULL){
			// apelez o data strtok pentru a trece peste numele comenzii
			strtok(command, " ");
			// initializez indexul de cautare cu indexul introdus de la tastatura
			i = atoi(strtok(NULL, " "));
			find(arr, len, i);
		}
		else if(strstr(command, "delete_at ") != NULL){
			// apelez o data strtok pentru a trece peste numele comenzii
			strtok(command, " ");
			// initializez indexul de cautare cu indexul introdus de la tastatura
			i = atoi(strtok(NULL, " "));
			delete_at(&arr, &len, i);
		}
		// pentru print si exit verific cu strcmp deoarece acestea sunt singurele caractere din buffer
		else if(strcmp(command, "print") == 0){
			print(arr, len);
		}
	}while(strcmp(command, "exit") != 0);
	free(command);
	free(commandCpy);
	free(arr);
	return 0;
}
