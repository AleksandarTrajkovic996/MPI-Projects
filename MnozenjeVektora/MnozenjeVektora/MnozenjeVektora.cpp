#include <iostream>
#include <mpi.h>
using namespace std;
#define MAX 100

int main(int* argv, char** args)
{
	int size, rank;
	int nizA[MAX];
	int nizB[MAX];
	int pomNizA[MAX];
	int pomNizB[MAX];
	int pomNizC[MAX];
	int vel = 0;
	int lok_suma = 0, ukupna_suma = 0, ls = 0;
	MPI_Request req;
	MPI_Status stat;
	int s1;
	MPI_Init(argv, &args);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == 0) {
		while (1) {
			cout << "Unesite velicinu vektora(mora da bude deljiva i veca od ukupnog broja procesa):" << endl;
			cin >> vel;

			if (vel % size == 0 && vel > size)
				break;
		}
		cout << "Unesite elemente vektora A:" << endl;
		for (int i = 0; i < vel; i++) {
			cin >> nizA[i];
		}
		cout << "Unesite elemente vektora B:" << endl;
		for (int i = 0; i < vel; i++) {
			cin >> nizB[i];
		}

		int br;
		s1 = vel / size; //velicina bloka

		for (int i = 1; i < size; i++) {
			br = 0;
			for (int j = i; j <= vel; j = j + size) {
				pomNizA[br] = nizA[j];
				pomNizB[br] = nizB[j];
				++br;
			}

			MPI_Isend(&pomNizA, s1, MPI_INT, i, 15, MPI_COMM_WORLD, &req);
			MPI_Isend(&pomNizB, s1, MPI_INT, i, 20, MPI_COMM_WORLD, &req);

			MPI_Wait(&req, &stat);
		}

		for (int i = 0; i < vel; i = i + s1) {
			lok_suma += nizA[i] * nizB[i]; //master racuna odmah, ima sve potrebne podatke
		}
	}

	MPI_Bcast(&s1, 1, MPI_INT, 0, MPI_COMM_WORLD); //salje se svima koliko je veliki blok podataka

	if (rank != 0) {

		MPI_Irecv(&pomNizA, s1, MPI_INT, 0, 15, MPI_COMM_WORLD, &req);
		MPI_Wait(&req, &stat);
		MPI_Irecv(&pomNizB, s1, MPI_INT, 0, 20, MPI_COMM_WORLD, &req);
		MPI_Wait(&req, &stat);

		for (int i = 0; i < s1; i++)
			lok_suma += pomNizA[i] * pomNizB[i];

		MPI_Isend(&lok_suma, 1, MPI_INT, 0, 25, MPI_COMM_WORLD, &req); //svaki proces salje svoju sumu
		MPI_Wait(&req, &stat);
	}

	if (rank == 0) {
		ukupna_suma = lok_suma; //zato sto master proces ima izracunat svoj deo
		for (int i = 1; i < size; i++) {
			MPI_Irecv(&ls, 1, MPI_INT, i, 25, MPI_COMM_WORLD, &req); //mora u ls da smesti, jer on svoju sumu cuva u lok_suma
			MPI_Wait(&req, &stat); //root prima od svakog procesa njihovu sumu

			ukupna_suma += ls;
		}

		cout << "Proizvod dva vektora je: " << ukupna_suma << endl;
	}

	MPI_Finalize();
}

