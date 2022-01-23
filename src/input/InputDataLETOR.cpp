/// PreProcess a Letor Input: Get the number of queries, number of voters and max lists length
void InputData::preprocess_LETOR_lists(char * in, uint32_t len) {
	this->LetorMaxLength = 0;
	this->LetorNumVoters = 0;
	this->num_queries = 0;

	uint32_t i = 0, occ = 0, x = 0, y = 0, z = 0, q = 0, voters_len = 0, v_occ = 0;
	char topic_id[15], voters_buf[512], voters_part[20], voter_id_c[3], rank_c[4];
	rank_t rank = 0;
	int32_t topic = 0, prev_topic = 0, voter_id = 0, LetorVoters[100] = {0};

	for (i = 0; i < len; i++) {
		if(in[i] == 32) {
			while (in[i] != 32) {
				i++;
			}
		} else if (in[i] == 10) {
			occ = 0;

			if (topic != prev_topic) {
				this->num_queries++;
				prev_topic = topic;
			}
		} else {

			/// Get the first column: relevance
			if (occ == 0) {
				y = 0;
				while (in[i] != 9 && in[i] != 32) {
					i++;
				}
				occ = 1;
				i++;
			}

			/// Get the second column: Query ID (Topic ID)
			if (occ == 1) {
				y = 0;
				v_occ = 0;
				while (in[i] != 9 && in[i] != 32) {
					if (v_occ == 1) { topic_id[y++] = in[i]; }
					if (in[i] == 58) { v_occ = 1; y = 0; }
					i++;
				}
				topic_id[y] = 0;
				occ = 2;
				i++;

				topic = atoi(topic_id);
				if (prev_topic == 0) {
					prev_topic = topic;
					this->num_queries++;
				}
			}

			/// Get the voters string
			if (occ == 2) {
				y = 0;
				while (in[i] != 35) {
					voters_buf[y++] = in[i++];
				}
				voters_buf[y] = 0;
				voters_len = y;
				occ = 3;
				i++;

//				printf("%s\n", voters_buf); getchar();
				/// Line End - Start parsing the voters string
				y = 0;
				for (x = 0; x < voters_len; x++) {
					if (voters_buf[x] == 32 || x >= voters_len - 1) {
						voters_part[y] = 0;

//						printf("\t%s\n", voters_part);
						if (voters_part[y - 1] != 'L') {
							v_occ = 1;
							q = 0;
							for (z = 0; z < y; z++) {
								if (voters_part[z] == 58) {
									voter_id_c[q] = 0;
									voter_id = atoi(voter_id_c);
									v_occ = 2;
									q = 0;
								} else {
									if (v_occ == 1) {
										voter_id_c[q++] = voters_part[z];
									} else if (v_occ == 2) {
										rank_c[q++] = voters_part[z];
									}
								}
							}
							rank_c[q] = 0;
							rank = atoi(rank_c);
							LetorVoters[voter_id - 1] = voter_id;
							if (rank > this->LetorMaxLength) {
								this->LetorMaxLength = rank;
							}
//							printf(" == Voter ID: %d, Rank: %d", voter_id, rank); getchar();
						}
						y = 0;
					} else {
						voters_part[y++] = voters_buf[x];
					}
				}
			}
		}
	}

	for (i = 0; i < 100; i++) {
		if (LetorVoters[i] > 0) {
			this->LetorNumVoters++;
		}
	}
}

/// Process a Letor Input: Extract the submitted results, queries, and rankings
void InputData::process_LETOR_lists(char * in, uint32_t len) {
	uint32_t i = 0, occ = 0, x = 0, y = 0, z = 0, q = 0, line = 0, qctr = 0, rel = 0, voters_len = 0, v_occ = 0;
	char topic_id[15], code_c[30], rel_c[10], voters_buf[512], voters_part[20], voter_id_c[3], rank_c[4];
	rank_t rank;
	int32_t topic = 0, prev_topic = 0, voter_id = 0;

	for (i = 0; i < len; i++) {
		if(in[i] == 32) {
			while (in[i] != 32) {
				i++;
			}
		} else if (in[i] == 10) {

			occ = 0;

			line++;

		} else {

			/// Get the first column: relevance
			if (occ == 0) {
				y = 0;
				while (in[i] != 9 && in[i] != 32) {
					rel_c[y++] = in[i++];
				}
				rel_c[y] = 0;
				occ = 1;
				rel = atoi(rel_c);
//				if (rel == 2) { rel = 4; }
				i++;
			}

			/// Get the second column: Query ID (Topic ID)
			if (occ == 1) {
				y = 0;
				v_occ = 0;
				while (in[i] != 9 && in[i] != 32) {
					if (v_occ == 1) { topic_id[y++] = in[i]; }
					if (in[i] == 58) { v_occ = 1; y = 0; }
					i++;
				}
				topic_id[y] = 0;
				occ = 2;
				i++;

				topic = atoi(topic_id);
				if (topic != prev_topic) {
					if (prev_topic != 0) {
						qctr++;
					}
//					printf("New topic %d: %d\n", qctr + 1, topic);
					this->queries[qctr]->set_topic_id(topic);
					prev_topic = topic;
				}
			}

			/// Get the voters string
			if (occ == 2) {
				y = 0;
				while (in[i] != 35) {
					voters_buf[y++] = in[i++];
				}
				voters_buf[y] = 0;
				voters_len = y;
				occ = 3;
				i++;
			}

			/// Get the columns
			if (occ == 3) {
				y = 0;
				while (in[i] != 9 && in[i] != 32) { code_c[y++] = in[i++]; }
				code_c[y] = 0;
				occ = 4;
				i++;
			}

			/// Get the columns
			if (occ == 4) {
				y = 0;
				while (in[i] != 9 && in[i] != 32) { code_c[y++] = in[i++]; }
				code_c[y] = 0;
				occ = 5;
				i++;
			}

			/// Get the columns
			if (occ == 5) {
				y = 0;
				while (in[i] != 9 && in[i] != 32) {
					code_c[y++] = in[i++];
				}
				code_c[y] = 0;
				occ = 6;
				i++;

//				printf("Code: %s\nRelevant: %d\nTopic: %s (%d)\nVoters: %s\n",
//					code_c, rel, topic_id, topic, voters_buf); getchar();

				if (rel > 0) {
					this->queries[qctr]->insert_relev(topic, code_c, rel);
				}

				/// Line End - Start parsing the voters string
				y = 0;
				for (x = 0; x < voters_len; x++) {
					if (voters_buf[x] == 32 || x >= voters_len - 1) {
						voters_part[y] = 0;

//						printf("\t%s", voters_part);
						if (voters_part[y - 1] != 'L') {
							v_occ = 1;
							q = 0;
							for (z = 0; z < y; z++) {
								if (voters_part[z] == 58) {
									voter_id_c[q] = 0;
									voter_id = atoi(voter_id_c);
									v_occ = 2;
									q = 0;
								} else {
									if (v_occ == 1) {
										voter_id_c[q++] = voters_part[z];
									} else if (v_occ == 2) {
										rank_c[q++] = voters_part[z];
									}
								}
							}
							rank_c[q] = 0;
							rank = atoi(rank_c);
//							printf(" == Voter ID: %d, Rank: %d\n", voter_id, rank);

//							this->queries[qctr]->get_input_list(voter_id - 1)->replace_item(code_c, rank);
							this->queries[qctr]->get_input_list(voter_id - 1)->insert_item(code_c, rank);

						} else {
//							printf(" == NULL\n");
						}
						y = 0;
					} else {
						voters_part[y++] = voters_buf[x];
					}
				}
//				getchar();
			}
		}
	}

//	printf("\nSorting input lists...\n\n"); fflush(NULL);
	for (qctr = 0; qctr < this->num_queries; qctr++) {
		for (x = 0; x < this->queries[qctr]->get_num_input_lists(); x++) {
//			printf("Query %d\n", this->queries[qctr]->get_topic_id());
			this->queries[qctr]->get_input_list(x)->reverse_ranking();
//			this->queries[qctr]->get_input_list(x)->display(); getchar();
		}
	}
}

