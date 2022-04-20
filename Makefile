all:
	clear

info_original:
	gcc -w movies_read.c -o cod
	./cod films.dat info

info_final:
	gcc -w movies_read.c -o cod
	./cod part-00321.dat info

imprimir_original:
	clear
	gcc -w movies_read.c -o cod
	./cod films.dat imprimir

imprimir_final:
	clear
	gcc -w movies_read.c -o cod
	./cod part-00321.dat imprimir

classificação:
	clear
	gcc -w movies_read.c -o cod
	./cod films.dat classificação

intercalação:
	clear
	gcc -w movies_read.c -o cod
	./cod films.dat intercalação

direto_original:
	clear
	gcc -w movies_read.c -o cod
	./cod films.dat direto

direto_final:
	clear
	gcc -w movies_read.c -o cod
	./cod part-00321.dat direto

busca:
	clear
	gcc -w movies_read.c -o cod
	./cod part-00321.dat busca "Salida del público de la iglesia parroquial de Santa María de Sans"

exemplo:
	clear
	gcc -w inteiro.c -o cod
	./cod