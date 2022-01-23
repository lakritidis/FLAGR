void MergedList::Agglomerative(class InputList ** inlists, class InputParams * params) {
	uint32_t m = 0, n = 0;
	score_t d = 0;

	for (m = 0; m < this->num_input_lists; m++) {
        for (n = 0; n < this->num_input_lists; n++) {
			if ( m != n ){

				inlists[m]->display();
				inlists[n]->display();
				d = inlists[m]->SpearmanRho(inlists[n]);

				printf("Rho = %5.3f\n", d);
				getchar();
			}
        }
	}
}
