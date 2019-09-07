// CartesianStructure.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <mpi.h>
using namespace std;

#define TRUE 1
#define FALSE 0


int main(int argc, char** argv)
{
	int rank, size, root = 0;
	MPI_Comm commCart;
	MPI_Comm group;
	MPI_Comm groupTmp;
	MPI_Comm commTmp;
	int dim[2], period[2], preorder;
	int right, left;
	int kol = 0, red = 0;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_group(MPI_COMM_WORLD, &group);

	if (rank == root) {

		while (TRUE) {
			cout << "Unesite broj procesa po kolonama: ";
			cin >> kol;
			cout << "Unesite broj procesa po vrstama: ";
			cin >> red;

			if (kol * red <= size) {
				break;
			}
			else {
				cout << "Broj M*N mora biti manji ili jednak od ukupnog broja procesa!" << endl;
			}
		}
	}

	MPI_Bcast(&kol, 1, MPI_INT, root, MPI_COMM_WORLD);
	MPI_Bcast(&red, 1, MPI_INT, root, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);

	int* members = new int[kol * red];
	for (int j = 0; j < kol * red; j++) {
		members[j] = j;
	}

	MPI_Group_incl(group, kol * red, members, &groupTmp);
	MPI_Comm_create(MPI_COMM_WORLD, groupTmp, &commTmp);

	if (rank < kol * red) {
		dim[0] = red;
		dim[1] = kol;
		period[0] = TRUE;
		period[1] = FALSE;
		preorder = TRUE;

		MPI_Cart_create(commTmp, 2, dim, period, preorder, &commCart);

		MPI_Cart_shift(commCart, 0, 2, &left, &right);
		cout << "Proces =>" << rank << " levi sused je " << left << ", a desni je " << right << endl;
	}

	MPI_Finalize();
}
