/// Preprocess the CSV file to retrieve the number of queries in the file and the number of voters.
void InputData::preprocess_CSV(char * in, uint32_t len) {
	uint32_t i = 0, occ = 0, y = 0, q = 0, v = 0;
	uint32_t num_alloc_topics = 10, num_alloc_voters = 4, num_voters = 0;

	char buf[100], prev_topic[100], prev_voter[100], topic[100], voter[100];
	bool found;

	buf[0] = 0;
	topic[0] = 0;
	voter[0] = 0;
	prev_topic[0] = 0;
	prev_voter[0] = 0;

	/// Temporary memory buffers
	char ** query_strings = (char**)malloc(num_alloc_topics * sizeof(char *));
	char ** voter_strings = (char**)malloc(num_alloc_voters * sizeof(char *));

	/// For each character in the file's text:
	for (i = 0; i < len; i++) {

		/// A comma character was found
		if (in[i] == 44) {

			/// Get the first column: topic ID (query)
			if (occ == 0) {
				buf[y] = 0;
				strcpy(topic, buf);

				if (strcmp(topic, prev_topic) != 0) {
					strcpy(prev_topic, topic);
//					printf("Topic: %s\n", buf); fflush(NULL); getchar();
					found = false;
					for (q = 0; q < this->num_queries; q++) {
						if (strcmp(buf, query_strings[q]) == 0) {
							found = true;
							break;
						}
					}

					if (!found) {
						query_strings[this->num_queries] = (char *) malloc((y + 1) * sizeof(char));
						strcpy(query_strings[this->num_queries], buf);
						this->num_queries++;

						if (this->num_queries >= num_alloc_topics) {
							num_alloc_topics *= 2;
							query_strings = (char **)realloc
								(query_strings, num_alloc_topics * sizeof(char *));
						}
					}
				}

				occ = 1;
				y = 0;
			}

			/// Get the second column: Voter ID
			else if (occ == 1) {
				buf[y] = 0;
				strcpy(voter, buf);

				if (strcmp(voter, prev_voter) != 0) {
					strcpy(prev_voter, voter);
					found = false;
					for (v = 0; v < num_voters; v++) {
						if (strcmp(buf, voter_strings[v]) == 0) {
							found = true;
							break;
						}
					}

					if (!found) {
						voter_strings[num_voters] = (char *) malloc((y + 1) * sizeof(char));
						strcpy(voter_strings[num_voters], buf);
						num_voters++;

						if (num_voters >= num_alloc_voters) {
							num_alloc_voters *= 2;
							voter_strings = (char **)realloc
								(voter_strings, num_alloc_voters * sizeof(char *));
						}
					}
				}

				occ = 2;
				y = 0;

			/// All the other columns are ignored
			} else {
				y = 0;
			}

		/// A line-break character was found
		} else if (in[i] == 10) {
			occ = 0;
			y = 0;

		/// Otherwise (neither comma, nor line-break characters), we copy the data to the buffer.
		} else {
			buf[y++] = in[i];
		}
	}

	/// After num_queries and num_voters have been determined, we initialize the queries array and
	/// we construct num_voters Voter objects for each query.
	this->queries = new Query * [this->num_queries];
	for (q = 0; q < this->num_queries; q++) {
		this->queries[q] = new Query(1);
//		this->queries[q]->set_topic_id(atoi(query_strings[q]));
		this->queries[q]->set_topic(query_strings[q]);

		for (v = 0; v < num_voters; v++) {
			this->queries[q]->create_list(voter_strings[v], 1.00);
		}
	}

	/// Deallocate the temporary memory buffers
	for (q = 0; q < this->num_queries; q++) { free(query_strings[q]); }
	for (v = 0; v < num_voters; v++) { free(voter_strings[v]); }
	free(query_strings);
	free(voter_strings);
}

/// After the initialization of the array of Queries and the number of voters, transfer the file
/// contents to the pre-constructed objects
void InputData::construct_CSV_lists(char * in, uint32_t len) {
	uint32_t i = 0, occ = 0, y = 0, q = 0, v = 0, active_query = 0, active_voter = 0;
	score_t scr;
	char buf[100], prev_topic[100], prev_voter[100], topic[100], voter[100], code_c[100];
	class InputList * inlist = NULL;

	buf[0] = 0;
	topic[0] = 0;
	voter[0] = 0;
	prev_topic[0] = 0;
	prev_voter[0] = 0;

	/// For each character in the file's text:
	for (i = 0; i < len; i++) {

		/// A comma character was found
		if (in[i] == 44) {

			/// Get the first column: topic ID (query)
			if (occ == 0) {
				buf[y] = 0;
				strcpy(topic, buf);

				if (strcmp(topic, prev_topic) != 0) {
					strcpy(prev_topic, topic);
//					printf("Topic: %s\n", buf); fflush(NULL); getchar();
					for (q = 0; q < this->num_queries; q++) {
						if (strcmp(topic, this->queries[q]->get_topic()) == 0) {
							active_query = q;
							break;
						}
					}
				}

				occ = 1;
				y = 0;
			}

			/// Get the second column: Voter ID
			else if (occ == 1) {
				buf[y] = 0;
				strcpy(voter, buf);

				/// We search for the voter only if the present voter is different than the previous
				/// one. Otherwise, the current voter (active_voter) does not change. In this way we
				/// avoid searching for each encountered voter.
				if (strcmp(voter, prev_voter) != 0) {
					strcpy(prev_voter, voter);
					for (v = 0; v < this->queries[active_query]->get_num_input_lists(); v++) {
						if (strcmp(buf, this->queries[active_query]->get_input_list(v)->get_voter()->get_name()) == 0) {
							active_voter = v;
							break;
						}
					}
				}

				inlist = this->queries[active_query]->get_input_list(active_voter);
				occ = 2;
				y = 0;

			/// Get the third column: Item Code
			} else if (occ == 2) {
				buf[y] = 0;
				strcpy(code_c, buf);
				occ = 3;
				y = 0;

			/// Get the fourth column: Item Score
			} else if (occ == 3) {
				buf[y] = 0;
				scr = strtod(buf, NULL);

//				printf("Inserting %s with score: %5.3f\n", code_c, scr); getchar();
				inlist->insert_item(code_c, 0, scr);

				occ = 4;
				y = 0;

			} else {
				y = 0;
			}

		/// A line-break character was found
		} else if (in[i] == 10) {
			occ = 0;
			y = 0;

		/// Otherwise (neither comma, nor line-break characters), we copy the data to the buffer.
		} else {
			if (in[i] != 10) {
				buf[y++] = in[i];
			}
		}
	}

	/// Sort the lists in increasing rank order
	for (q = 0; q < this->num_queries; q++) {
		for (v = 0; v < this->queries[q]->get_num_input_lists(); v++) {
			this->queries[q]->get_input_list(v)->sort_by_score();
//			this->queries[q]->get_input_list(v)->display();
		}
	}
}

/// Read the CSV relevance judgments file (qrels)
void InputData::read_CSV_qrels() {
	uint32_t i = 0, occ = 0, y = 0, q = 0, active_query = 0, rel_judg = 0;
	size_t nread = 0, file_size = 0;

	char buf[100], prev_topic[100], topic[100], code_c[100], rj_c[100];
	char * in = NULL;

	buf[0] = 0;
	topic[0] = 0;
	prev_topic[0] = 0;
	code_c[0] = 0;
	rj_c[0] = 0;

	FILE * fp = fopen(this->params->get_rels_file(), "r");
	if (fp) {
		fseek(fp, 0L, SEEK_END);
		file_size = ftell(fp);
		rewind(fp);

		in = (char *)malloc( (file_size + 2) * sizeof(char));
		nread = fread(in, sizeof(char), file_size, fp);
		in[nread] = 0;


		for (i = 0; i < file_size; i++) {

			/// A comma character was found
			if (in[i] == 44) {

				/// Get the first column: topic ID (query)
				if (occ == 0) {
					buf[y] = 0;
					strcpy(topic, buf);
					occ = 1;
					y = 0;
				}


				/// Unused column
				else if (occ == 1) {
					buf[y] = 0;
					occ = 2;
					y = 0;
				}

				/// Get the Item code
				else if (occ == 2) {
					buf[y] = 0;
					strcpy(code_c, buf);
					occ = 3;
					y = 0;
				}

			/// A line-break character was found
			} else if (in[i] == 10) {
				buf[y] = 0;
				strcpy(rj_c, buf);
				rel_judg = atoi(rj_c);

				occ = 0;
				y = 0;

				if (strcmp(topic, prev_topic) != 0) {
					strcpy(prev_topic, topic);

					for (q = 0; q < this->num_queries; q++) {
						if (strcmp(topic, this->queries[q]->get_topic()) == 0) {
							active_query = q;
							break;
						}
					}
				}

				if (rel_judg > 0) {
//					printf("\tQuery %d (%s) => Inserting: %s - relevance: d\n",
//								active_query, topic, code_c, rel_judg);
					this->queries[active_query]->insert_relev(code_c, rel_judg);
				}

			/// Otherwise (neither comma, nor line-break characters), we copy the data to the buffer.
			} else {
				buf[y++] = in[i];
			}
		}

		free(in);
		fclose(fp);
	} else {
		printf("Error opening rels file %s. Continuing without evaluation\n",
				this->params->get_rels_file());
		fflush(NULL);
	}
}
