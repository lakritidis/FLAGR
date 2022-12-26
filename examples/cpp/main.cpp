#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "flagr.h"


/**
Main Function
Input arguments:
	0: program name
	1: Evaluation cut off point. How many elements of the aggregate list will be included in the
		evaluation process. If nothing is passed, the default value is 10.
	2: Input file: This where the aggregation algorithms read data from. If nothing is passed, the
		default value is ../../examples/testdata/testdata.csv.
	3: Output directory: This is where the program writes the generated aggregate lists and the
		results of the evaluation process. If nothing is passed, the default value is output.
	4: Qrels file: This is where the program reads the relevance judgments of the list elements.
		This file is required for the evaluation to take place. If nothing is passed, the default
		value is ../../examples/testdata/testdata_qrels.csv.
**/
int main(int argc, char *argv[]) {
	char * input_file = NULL, *qrels_file = NULL, *output_dir = NULL;
	int cut_off = 0;

	if (argc == 1) {
		input_file = new char[strlen("../../examples/testdata/testdata.csv") + 1];
		strcpy(input_file, "../../examples/testdata/testdata.csv");

		qrels_file = new char[strlen("../../examples/testdata/testdata_qrels.csv") + 1];
		strcpy(qrels_file, "../../examples/testdata/testdata_qrels.csv");

		output_dir = new char[strlen("output") + 1];
		strcpy(output_dir, "output");

		cut_off = 10;

	} else if (argc == 2) {
		input_file = new char[strlen("../../examples/testdata/testdata.csv") + 1];
		strcpy(input_file, "../../examples/testdata/testdata.csv");

		qrels_file = new char[strlen("../../examples/testdata/testdata_qrels.csv") + 1];
		strcpy(qrels_file, "../../examples/testdata/testdata_qrels.csv");

		output_dir = new char[strlen("output") + 1];
		strcpy(output_dir, "output");

		cut_off = atoi(argv[1]);

	} else if (argc == 3) {
		input_file = new char[strlen(argv[2]) + 1];
		strcpy(input_file, argv[2]);

		qrels_file = new char[1];
		strcpy(qrels_file, "");

		output_dir = new char[strlen("output") + 1];
		strcpy(output_dir, "output");

		cut_off = atoi(argv[1]);

	} else if (argc == 4) {
		input_file = new char[strlen(argv[2]) + 1];
		strcpy(input_file, argv[2]);

		qrels_file = new char[1];
		strcpy(qrels_file, "");

		output_dir = new char[strlen(argv[3]) + 1];
		strcpy(output_dir, argv[3]);

		cut_off = atoi(argv[1]);

	} else if (argc == 5) {
		input_file = new char[strlen(argv[2]) + 1];
		strcpy(input_file, argv[2]);

		qrels_file = new char[strlen(argv[4]) + 1];
		strcpy(qrels_file, argv[4]);

		output_dir = new char[strlen(argv[3]) + 1];
		strcpy(output_dir, argv[3]);

		cut_off = atoi(argv[1]);
	}

	printf("Aggregating with linear combination (rank normalization)...\n"); fflush(NULL);
	Linear(input_file, qrels_file, cut_off, 101, "Linear_out", output_dir);

	printf("Aggregating with Borda Count...\n"); fflush(NULL);
	Linear(input_file, qrels_file, cut_off, 100, "Borda_out", output_dir);

	printf("Aggregating with Condorcet Winners...\n"); fflush(NULL);
	Condorcet(input_file, qrels_file, cut_off, "Condorcet_out", output_dir);

	printf("Aggregating with Copeland Winners...\n"); fflush(NULL);
	Copeland(input_file, qrels_file, cut_off, "Copeland_out", output_dir);

	printf("Aggregating with the Outranking Approach...\n"); fflush(NULL);
	OutrankingApproach(input_file, qrels_file, cut_off, "Outrank_out", output_dir, 0.0, 0.75, 0.0, 0.25);

//	Kemeny(input_file, qrels_file, 20, "Kemeny_out", output_dir);

	printf("Aggregating with Markov Chains method 4...\n"); fflush(NULL);
	MC(input_file, qrels_file, cut_off, 804, "MC_out", output_dir, 0.15, 0.00000001, 200);

	printf("Aggregating with Robust Rank Aggregation...\n"); fflush(NULL);
	RobustRA(input_file, qrels_file, cut_off, "RRA", output_dir, false);

	printf("Aggregating with the Preference Relations Method...\n"); fflush(NULL);
	PrefRel(input_file, qrels_file, cut_off, "PrefRel_out", output_dir, 0.3, 0.2);

	printf("Aggregating with the Agglomerative Method...\n"); fflush(NULL);
	Agglomerative(input_file, qrels_file, cut_off, "Agglomerative_out", output_dir, 2.5, 1.5);

	printf("Aggregating with DIBRA + Outranking Approach...\n"); fflush(NULL);
	DIBRA(input_file, qrels_file, cut_off, 5300, "DIBRA_out", output_dir, 2, 3, false, 1.5, 0.4, 0.1, 0.01, 50, 0.0, 0.75, 0.0, 0.25);

	delete [] input_file;
	delete [] qrels_file;
	delete [] output_dir;

	printf("\n\nDone. Press any key to exit...\n"); fflush(NULL);
	getchar();
	return 0;
}

