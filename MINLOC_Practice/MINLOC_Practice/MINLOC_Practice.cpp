#include <iostream>
#include <mpi.h>
using namespace std;

int main(int args, char** argv) //pozvati za 4 procesa => mpiexec -n 4 MINLOC_Practice.exe
{
	int size, rank, root = 0;
	int matA[3][4] = {	{11,12,13,14}, 
						{5,6,7,8}, 
						{9, 10,11,12} };
	int matB[4][3] = {	{121,5,9}, 
						{32, 622, 10}, 
						{3, 7, 11}, 
						{4,8, 12} };

	int minB, lok_sum = 0, uk_sum = 0;
	struct { double value; int rank; } min[4], minEL[4], m;
	int niz[4], koloneB[4], vrsteB[3];
	MPI_Init(&args, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	//pristupa se svim procesima odjednom i gledaju se 4 elementa
	MPI_Reduce(&matB[rank], &niz, 4, MPI_INT, MPI_MIN, root, MPI_COMM_WORLD);
	MPI_Reduce(&matB[rank], &koloneB, 4, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);
	niz[3] = niz[2]; //zadnji se cisto preklapa

	MPI_Bcast(&niz, 4, MPI_INT, root, MPI_COMM_WORLD); //zato sto svi procesi moraju da imaju podatke da bi odredili minimalni
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Reduce(&niz[rank], &minB, 1, MPI_INT, MPI_MIN, root, MPI_COMM_WORLD);

	if (rank == root) {
		cout << "Minimalni element matrice B je: " << minB << endl;
		cout << "Suma svake kolone matrice B je: ";
		for (int i = 0; i < 3; i++) //3 kolone ima, zadnji element je isti kao pretposlednji
			cout << koloneB[i] << " ";
		cout << endl;
	}

	MPI_Barrier(MPI_COMM_WORLD);

	//svaki proces dobija po jednu vrstu matrice B
	MPI_Scatter(matB, 3, MPI_INT, &vrsteB, 3, MPI_INT, root, MPI_COMM_WORLD);

	for (int i = 0; i < 4; i++)
	{
		if (i != 3) {
			min[i].value = (double)vrsteB[i];
		}
		else {
			min[i].value = (double)vrsteB[2]; //samo se ponovi zadnja vrednost vektora 2 puta, ne utice ni na sta
		}
		min[i].rank = rank;
	}


	MPI_Reduce(&min, &minEL, 4, MPI_DOUBLE_INT, MPI_MINLOC, root, MPI_COMM_WORLD);
	MPI_Bcast(&minEL, 4, MPI_DOUBLE_INT, root, MPI_COMM_WORLD);
	MPI_Reduce(&minEL[rank], &m, 1, MPI_DOUBLE_INT, MPI_MINLOC, root, MPI_COMM_WORLD);

	if (rank == root) {
		cout << "Minimalni element matrice B nakon raspodele ima proces sa rankom " << m.rank << ", vrednost je: " << m.value << endl;
	}

	MPI_Finalize();
}
