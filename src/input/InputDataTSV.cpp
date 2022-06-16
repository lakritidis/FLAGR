/// Process the contents of the eintire TSV file and retrieve the included number of queries
void InputData::get_TSV_queries(char * in, uint32_t len) {
	uint32_t i = 0, occ = 0, y = 0;
	int32_t topic = 0, prev_topic = 0;
	char buf[100];

	for (i = 0; i < len; i++) {
		if (in[i] == 32) {
			while (in[i] != 32) { i++; }
		} else if (in[i] == 10) {
			occ = 0;

			if (topic != prev_topic) {
//				printf("New topic %d\n", topic);
				this->num_queries++;
				prev_topic = topic;
			}

		} else {

			/// Get the first column: topic ID (query)
			if (occ == 0) {
				y = 0;
				while (in[i] != 9 && in[i] != 32) { buf[y++] = in[i++]; }
				buf[y] = 0;
				occ = 1;
				topic = atoi(buf);
				i++;
			}

			/// Ignore the second column
			if (occ == 1) {
				y = 0;
				while (in[i] != 9 && in[i] != 32) { i++; y++; }
				occ = 2;
				i++;
			}

			/// Ignore the third column
			if (occ == 2) {
				y = 0;
				while (in[i] != 9 && in[i] != 32) { i++; y++; }
				occ = 3;
				i++;
			}

			/// Ignore the fourth column
			if (occ == 3) {
				y = 0;
				while (in[i] != 9 && in[i] != 32) { y++; i++; }
				occ = 4;
				i++;
				if (prev_topic == 0) {
					this->num_queries++;
					prev_topic = topic;
				}
			}
		}
	}
}


/// Process a TSV Run: Extract the submitted results and their rankings
void InputData::process_TSV_lists(char * in, uint32_t len, char * voter) {
	uint32_t i = 0, occ = 0, y = 0, line = 0, qctr = 0;
	rank_t rank = 0;
	char topic_id[100], code_c[100], rank_c[100];
	int32_t topic = 0, prev_topic = 0;
	bool add_1 = false;

	class InputList * inlist = NULL;

	for (i = 0; i < len; i++) {
		if(in[i] == 32) {
			while (in[i] != 32) {
				i++;
			}
		} else if (in[i] == 10) {
			occ = 0;

			line++;
			if (topic != prev_topic) {
//				printf("New topic %d - Rank: %d - List Items: %d\n", topic, rank, inlist->get_num_items());
				inlist->sort_by_ranking();
//				inlist->display();

				if (qctr >= this->num_queries) {
					printf("Error! Cannot accommodate more than %d queries\n", qctr);
					break;
				}

				this->queries[qctr]->set_topic_id(topic);

				inlist = this->queries[qctr++]->create_list(voter, 1.00);

				prev_topic = topic;

				if (rank == 0) { add_1 = true; } else { add_1 = false; }
//				rank = 1;

//				getchar();
			}

			if (strcmp(code_c, "") != 0) {
				if (add_1) { rank++; }

//				printf("Topic: %s (%d). Rank=%d. URL=%s, Line: %d\n", topic_id, qctr, rank, code_c, line);

				inlist->insert_item(code_c, rank);
			}

		} else {

			/// Get the first column: topic ID (query)
			if (occ == 0) {
				y = 0;
				while (in[i] != 9 && in[i] != 32) { topic_id[y++] = in[i++]; }
				topic_id[y] = 0;
				occ = 1;
				topic = atoi(topic_id);
				i++;
			}

			/// Ignore the second column: Q0 value (useless)
			if (occ == 1) {
				y = 0;
				while (in[i] != 9 && in[i] != 32) {
					i++;
					y++;
				}
				occ = 2;
				i++;
			}

			/// Get the third column: Document Code (Clueweb Internal Code)
			if (occ == 2) {
				y = 0;
				while (in[i] != 9 && in[i] != 32) {
					if (in[i] >= 65 && in[i] <= 90) {
						code_c[y++] = in[i++] + 32;
					} else {
						code_c[y++] = in[i++];
					}
				}
				code_c[y] = 0;
				occ = 3;
				i++;
			}

			/// Get the fourth column: Item ranking
			if (occ == 3) {
				y = 0;
				while (in[i] != 9 && in[i] != 32) { rank_c[y++] = in[i++]; }
				rank_c[y] = 0;
				rank = atoi(rank_c);
				occ = 4;
				i++;
				if (prev_topic == 0) {
					if (rank == 0) { add_1 = true; }
					this->queries[qctr]->set_topic_id(topic);
					inlist = this->queries[qctr++]->create_list(voter, 1.00);
					prev_topic = topic;
				}
			}
		}
	}
	inlist->sort_by_ranking();
}

/// Read the TSV relevance judgments file (qrels)
void InputData::read_TSV_qrels() {
	uint32_t file_size = 0, i = 0, y = 0, occ = 0, line = 0, qctr = 0;
	int32_t rj = 0;
	uint32_t topic = 0, prev_topic = 0;

	char qrels_filepath[1024], topic_id[100], rj_c[100], code_c[100], * buf, u_c[100];
	size_t nread = 0;

	sprintf(qrels_filepath, "%s%s/%s_%s_prels", this->params->get_base_path(),
		this->params->get_dataset_name(), this->params->get_dataset_name(), this->params->get_dataset_track());

	FILE * fp = fopen(qrels_filepath, "r");
	if (fp) {
		printf("Reading relevance judgments...\n"); fflush(NULL);
		fseek(fp, 0L, SEEK_END);
		file_size = ftell(fp);
		rewind(fp);

		buf = (char *)malloc( (file_size + 2) * sizeof(char));
		nread = fread(buf, sizeof(char), file_size, fp);
		buf[nread] = 0;

		for (i = 0; i < file_size; i++) {
			if(buf[i] == 32) {
				while (buf[i] == 32) {
					i++;
				}
				i--;
			} else if (buf[i] == 10) {
				occ = 0;

				line++;
				if (topic != prev_topic) {
					prev_topic = topic;
					for (qctr = 0; qctr < this->num_queries; qctr++) {
						if (this->queries[qctr]->get_topic_id() == topic) {
							break;
						}
					}
				}

//				printf("Topic: %s. Line=%d. URL=%s, Judge: %s\n", topic_id, line, code_c, rj_c);
				if (strcmp(code_c, "") != 0) {
					if (rj > 0) {
						this->queries[qctr]->insert_relev(topic, code_c, rj);
//						printf("Inserted %d (%s) into query %d (Topic: %d - %d)\n",
//							this->queries[qctr]->get_num_relevs(), code_c, qctr,
//							this->queries[qctr]->get_topic_id(), topic);
					}
				}
			} else {

				/// Get the topic ID (query)
				if (occ == 0) {
					y = 0;
					while (buf[i] != 9 && buf[i] != 32 && buf[i] != 10) { topic_id[y++] = buf[i++]; }
					topic_id[y] = 0;

					/// The input files of TREC-2009 are formatted in a slightly different manner
					if (strcmp(this->params->get_dataset_name(), "TREC2009") == 0) {
						occ = 2;
					} else {
						occ = 1;
					}

					topic = atoi(topic_id);
					if (prev_topic == 0) {
						prev_topic = topic;
					}
				}

				/// Get the Document ID (Clueweb Internal Code)
				else if (occ == 1) {
					y = 0;
					while (buf[i] != 9 && buf[i] != 32 && buf[i] != 10) { u_c[y++] = buf[i++]; }
					u_c[y] = 0;
					occ = 2;
				}

				/// Get the Document ID (Clueweb Internal Code)
				else if (occ == 2) {
					y = 0;
					while (buf[i] != 9 && buf[i] != 32 && buf[i] != 10) {
						if (buf[i] >= 65 && buf[i] <= 90) {
							code_c[y++] = buf[i++] + 32;
						} else {
							code_c[y++] = buf[i++];
						}
					}
					code_c[y] = 0;
					occ = 3;
				}

				/// Get the Relevance Judgement
				else if (occ == 3) {
					y = 0;
					while (buf[i] != 9 && buf[i] != 32 && buf[i] != 10) { rj_c[y++] = buf[i++]; }

					rj_c[y] = 0;
					rj = atoi(rj_c);
					occ = 4;
					i--;
				}
			}
		}
		free(buf);

		fclose(fp);
	} else {
		printf("Error opening qrels file %s\n", qrels_filepath);
		exit(-1);
	}
}

