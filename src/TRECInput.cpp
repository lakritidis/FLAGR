#include "TRECInput.h"

/// Default Constructor
TRECInput::TRECInput() {
	this->dir = NULL;
	this->num_queries = 0;
	this->queries = NULL;
	this->mean_F1 = NULL;
	this->mean_precision = NULL;
	this->mean_recall = NULL;
	this->mean_dcg = NULL;
	this->MAP = 0.0;
	this->MNDCG = 0.0;
	this->LetorMaxLength = 0;
	this->LetorNumVoters = 0;
	this->avg_sprho = 0.0;
}

/// Constructor 2
/// t: Constructor type
/// t==1: TREC2009-TREC2013 Adhoc Task of Web Track
/// t==2: MQ2007-agg
/// t==3: MQ2008-agg
TRECInput::TRECInput(uint32_t t, char * input_path, char * e_file) {
	uint32_t no_file = 0;
	this->type = t;

	this->MAP = 0.0;
	this->MNDCG = 0.0;
	this->mean_precision = new double[MAX_LIST_ITEMS];
	this->mean_recall = new double[MAX_LIST_ITEMS];
	this->mean_F1 = new double[MAX_LIST_ITEMS];
	this->mean_dcg = new double[MAX_LIST_ITEMS];
	this->mean_ndcg = new double[MAX_LIST_ITEMS];
	this->eval_file = fopen(e_file, "w+");

	this->initialize_stats();

	if (t == 1) {
		char filename[1024];

		this->dir = new char [strlen(input_path) + 1];
		strcpy(this->dir, input_path);

		DIR *dir;
		struct dirent *ent;

		if ((dir = opendir(this->dir)) != NULL) {

			this->num_queries = DATASET_NUM_QUERIES;
			this->queries = new Query * [this->num_queries];
			for (uint32_t i = 0; i < this->num_queries; i++) {
				this->queries[i] = new Query(1);
			}

			while ((ent = readdir(dir)) != NULL) {
				if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
					strcpy(filename, this->dir);
					strcat(filename, ent->d_name);
					filename[strlen(this->dir) + strlen(ent->d_name)] = 0;

					FILE * f = fopen(filename, "rb");
					if (f) {
						printf("Reading Run %d: %s (Items: ", ++no_file, filename); fflush(NULL);
						this->decompress_file(f, ent->d_name);
						printf("%d)\n", this->compute_avg_list_length()); fflush(NULL);
						fclose(f);
					} else {
						printf("Error Opening Input File %s\n", filename);
					}
				}
			}
			closedir(dir);
		} else {
			printf("Error Opening Input Directory %s\n", this->dir);
			exit(-1);
		}

		for (uint32_t i = 0; i < this->num_queries; i++) {
			this->queries[i]->init_weights();
		}

		this->read_qrels();

	} else if (t == 2) {
		this->num_queries = 0;

		uint32_t init_buf_len = 16777216, nread = 0, cur_buf_len = 0;
		class InputList * inlist = NULL;
		char voter[100], res[100];
		char * buf = (char *)malloc(init_buf_len * sizeof(char));

		printf("Preprocessing LETOR Input..."); fflush(NULL);

		FILE * f = fopen(input_path, "rb");
		if (f) {
			while(!feof(f)) {
				nread = fread(buf + cur_buf_len, sizeof(char), 4096, f);
				if (nread == 0) {
					break;
				}

				cur_buf_len += nread;
				if (cur_buf_len >= init_buf_len) {
					init_buf_len *= 2;
					buf = (char *)realloc(buf, init_buf_len * sizeof(char));
				}
			}
			this->preprocessLetor(buf, cur_buf_len);

//			printf("%s\n", buf);
			fclose(f);
		} else {
			printf("Error Opening Input File %s\n", input_path);
			exit(-1);
		}

		printf("\t[ OK, %d queries, %d voters, max list length: %d ]\n", this->num_queries,
				this->LetorNumVoters, this->LetorMaxLength);

		/// Initialize 1692 queries and 21 Input lists with 500 random elements each
		printf("Initializing Data..."); fflush(NULL);

		this->queries = new Query * [this->num_queries];

		for (uint32_t q = 0; q < this->num_queries; q++) {
//			printf("QUERY %d\n", q);
			this->queries[q] = new Query(1);
			this->queries[q]->set_topic_id(q+1);

			for (uint32_t v = 0; v < this->LetorNumVoters; v++) {
				sprintf(voter, "voter-%d", v + 1);
				inlist = this->queries[q]->create_list(voter, 1.00);

				for (uint32_t i = 0; i < this->LetorMaxLength; i++) {
					sprintf(res, "ELEMENT-%d-%d-%d", q, v + 1, i + 1);
//					printf("ELEMENT-%d-%d-%d\n", q, v, i);
                    inlist->insert_item(res, i + 1);
				}
			}
		}

		printf("\t [ OK ]\nReading Input..."); fflush(NULL);

		this->processLetorInput(buf, cur_buf_len);

		free(buf);
	}
}

/// Destructor
TRECInput::~TRECInput() {
	if (this->dir) {
		delete [] this->dir;
	}

	if (this->num_queries > 0) {
		if (this->queries) {
			for (uint32_t i = 0; i < this->num_queries; i++) {
				if (this->queries[i]) {
					delete this->queries[i];
				}
			}
			delete [] this->queries;
			if (this->mean_precision) { delete [] this->mean_precision; }
			if (this->mean_recall) { delete [] this->mean_recall; }
			if (this->mean_F1) { delete [] this->mean_F1; }
			if (this->mean_dcg) { delete [] this->mean_dcg; }
			if (this->mean_ndcg) { delete [] this->mean_ndcg; }
		}
	}

	if (this->eval_file) {
		fclose(eval_file);
	}
}

/// Decompress a TREC Run (Input file submitted by a research group)
char * TRECInput::decompress_file(FILE * source, char * voter) {
	int32_t CHUNK = 1048576, os = CHUNK, os2 = 0, ret = 0;
	unsigned have = 0;
	z_stream strm;
	Bytef in[CHUNK];

	char * out = (char *)malloc(CHUNK * sizeof(char));

	/// Allocate inflate state
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;

	ret = inflateInit2(&strm, 16 + MAX_WBITS);
	if (ret != Z_OK) {
		printf ("Error 1");
	}

	/// Decompress until deflate stream ends or end of file
	do {
		strm.avail_in = fread(in, 1, CHUNK, source);
		if (ferror(source)) {
			(void)inflateEnd(&strm);
			return NULL;
		}
		if (strm.avail_in == 0) {
			break;
		}
		strm.next_in = in;

		/// run inflate() on input until output buffer not full
		do {
			strm.avail_out = CHUNK;
			strm.next_out = (Bytef*)out + os2;

			ret = inflate(&strm, Z_NO_FLUSH);
			assert(ret != Z_STREAM_ERROR);  /// state not clobbered
			switch (ret) {
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR;     /// and fall through
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					(void)inflateEnd(&strm);
			}
			have = CHUNK - strm.avail_out;
			os2 += have;
			os += have;

			out = (char*)realloc(out, os * sizeof(char));
			if (out == NULL) {
				printf("Out of Memory during decompression."); fflush(NULL);
			}

//			printf("os=%d, have: %d\n", os, have); fflush(NULL);
		} while (strm.avail_out == 0);

		#ifdef DEBUG_WARC
			if (os > 150000000) {
				break;
			}
		#endif

		/// Done when inflate() says it's done
	} while (ret != Z_STREAM_END);

	/// Clean up and return
	(void)inflateEnd(&strm);

	out[os2] = 0;

	this->processInput(out, os2, voter);

	free(out);
	return out;
}

/// Process a TREC Run: Extract the submitted results and their rankings
void TRECInput::processInput(char * in, uint32_t len, char * voter) {
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

				this->queries[qctr]->set_topic_id(topic);

				inlist = this->queries[qctr++]->create_list(voter, 1.00);

				prev_topic = topic;

				if (rank == 0) { add_1 = true; } else { add_1 = false; }
//				rank = 1;

				if (qctr > DATASET_NUM_QUERIES) {
					printf("Cannot accommodate an additional query %d\n", qctr);
				}
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

/// PreProcess a Letor Input: Get the number of queries, number of voters and max lists length
void TRECInput::preprocessLetor(char * in, uint32_t len) {
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
				while (in[i] != 9 && in[i] != 32) { i++; }
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
							LetorVoters[voter_id - 1] = voter_id;
							if (rank > this->LetorMaxLength) { this->LetorMaxLength = rank; }
//							printf(" == Voter ID: %d, Rank: %d\n", voter_id, rank);
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
		if (LetorVoters[i] > 0) { this->LetorNumVoters++; }
	}
}

/// Process a Letor Input: Extract the submitted results, queries, and rankings
void TRECInput::processLetorInput(char * in, uint32_t len) {
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

			if (topic != prev_topic) {
				printf("New topic %d: %d\n", qctr + 1, topic);
//				this->queries[qctr]->display();
				this->queries[++qctr]->set_topic_id(topic);
				prev_topic = topic;
			}

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
					this->queries[0]->set_topic_id(topic);
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
				while (in[i] != 9 && in[i] != 32) { code_c[y++] = in[i++]; }
				code_c[y] = 0;
				occ = 6;
				i++;

//				printf("Code: %s\nRelevant: %d\nTopic: %s (%d)\nVoters: %s\n",
//					code_c, rel, topic_id, topic, voters_buf);

				/// Line End - Start parsing the voters string
				y = 0;
				for (x = 0; x < voters_len; x++) {
					if (voters_buf[x] == 32 || x >= voters_len - 1) {
						voters_part[y] = 0;

//						printf("\t%s", voters_part);
						if (voters_part[y-1] != 'L') {
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

							this->queries[qctr]->get_input_list(voter_id - 1)->replace_item(code_c, rank);
							if (rel == 1 || rel == 2) {
								this->queries[qctr]->insert_relev(topic, code_c, rel);
							}
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
}

/// Read the TREC relevance judgments file (qrels)
void TRECInput::read_qrels() {
	uint32_t file_size = 0, i = 0, y = 0, occ = 0, line = 0, qctr = 0;
	int32_t rj = 0;
	uint32_t topic = 0, prev_topic = 0;

	char qrels_filepath[1024], topic_id[100], rj_c[100], code_c[100], * buf, u_c[100];
	size_t nread = 0;

	sprintf(qrels_filepath, "%s%s/%s_%s%s_prels", BASE_PATH, TREC_NAME, TREC_NAME, TREC_TRACK, TREC_TASK);

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

					if (strcmp(TREC_NAME, "TREC2009") == 0) { occ = 2; } else { occ = 1; }

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

/// Initialize statistics
void TRECInput::initialize_stats() {
	this->avg_sprho = 0.0;
	this->MAP = 0.0;
	this->MNDCG = 0.0;
	for (rank_t i = 0; i < MAX_LIST_ITEMS; i++) {
		this->mean_precision[i] = 0.0;
		this->mean_recall[i] = 0.0;
		this->mean_F1[i] = 0.0;
		this->mean_dcg[i] = 0.0;
		this->mean_ndcg[i] = 0.0;
	}
}

/// Dstroy the output lists for all quesries
void TRECInput::destroy_output_lists() {
	if (this->num_queries > 0) {
		if (this->queries) {
			for (uint32_t i = 0; i < this->num_queries; i++) {
				if (this->queries[i]) {
					this->queries[i]->destroy_output_list();
				}
			}
		}
	}
}

void TRECInput::print_header() {
	char m1[1024], m2[1024], m3[1024], m4[1024], m5[1024];
	if (AGGREGATION_METHOD == 1) { strcpy(m1, "BORDA COUNT"); } else
	if (AGGREGATION_METHOD == 2) { strcpy(m1, "CONDORCET METHOD"); } else
	if (AGGREGATION_METHOD == 3) { strcpy(m1, "OUTRANKING"); } else
	if (AGGREGATION_METHOD == 4) { strcpy(m1, "RANK POSITION"); }

	if (ITERATIONS == 0) { strcpy(m2, "(BASELINE"); } else
	if (ITERATIONS == -1) { strcpy(m2, "(ITERATIVE"); } else
	{ sprintf(m2, "(ITERATIVE %d", ITERATIONS); }

	printf("| %s / %s / %s -- %s %s", TREC_NAME, TREC_TRACK, TREC_TASK, m1, m2);

	if (ITERATIONS != 0) {
		if (CORRELATION_METHOD == 1) { strcpy(m3, "Spearman's Rho correlation"); } else
		if (CORRELATION_METHOD == 2) { strcpy(m3, "Scaled Footrule Distance"); } else
		if (CORRELATION_METHOD == 3) { strcpy(m3, "Weighted Cosine Distance"); } else
		if (CORRELATION_METHOD == 4) { strcpy(m3, "Local Scaled Footrule Distance"); } else
		if (CORRELATION_METHOD == 5) { strcpy(m3, "Kendall's Tau correlation"); }

		sprintf(m4, " - %s  - g = %3.1f)", m3, GAMMA);
	} else {
		strcpy(m4, ")");
	}

	if (POST_PROCESSING == 1) {
		strcpy(m5, "List Pruning Enabled.");
	} else {
		strcpy(m5, "List Pruning Disabled.");
	}
	printf("%s - %s\n", m4, m5); fflush(NULL);

	if (this->eval_file) {
		fprintf(eval_file, "| %s / %s / %s -- %s %s", TREC_NAME, TREC_TRACK, TREC_TASK, m1, m2);
		fprintf(eval_file, "%s - %s\n", m4, m5); fflush(NULL);
	}
}

/// Evaluate the output lists of each query
void TRECInput::evaluate() {
	double precision_acc[MAX_LIST_ITEMS], recall_acc[MAX_LIST_ITEMS], F1_acc[MAX_LIST_ITEMS];
	double dcg_acc[MAX_LIST_ITEMS], ndcg_acc[MAX_LIST_ITEMS];
	double sum_avep = 0.0, sum_aven = 0.0;
	uint32_t m = 0;
	rank_t i = 0, cutoff = 0;
	char dash[151], lit[151]; dash[0] = 0; lit[0] = 0;

	/// Initialize the retrieval effectiveness metrics
	this->initialize_stats();

	for (i = 0; i < MAX_LIST_ITEMS; i++) {
		precision_acc[i] = 0.0;
		recall_acc[i] = 0.0;
		F1_acc[i] = 0.0;
		dcg_acc[i] = 0.0;
		ndcg_acc[i] = 0.0;
	}

	/// Print the column headers of the results (according to RESULTS_TYPE)
	if (RESULTS_TYPE == 1) {
		printf("%s\n", PadStr(dash, 137, '-'));
		if (eval_file) { fprintf(eval_file, "%s\n", PadStr(dash, 137, '-')); }

		this->print_header();
		strcpy(lit, "Topic");
		printf("%s\n", PadStr(dash, 137, '-'));
		printf("| %s\t(Rel)\t|  AveP  |   P@5  |  P@10  |  P@15  |  P@20  |  P@30  |  P@100 |  P@200 |  P@500 | P@1000 | nDCG@10 | nDCG@20 |\n",
			PadStr(lit, 10, ' '));
		printf("%s\n", PadStr(dash, 137, '-')); fflush(NULL);

		if (this->eval_file) {
			fprintf(eval_file, "%s\n", PadStr(dash, 137, '-'));
			fprintf(eval_file, "| %s\t(Rel)\t|  AveP  |   P@5  |  P@10  |  P@15  |  P@20  |  P@30  |  P@100 |  P@200 |  P@500 | P@1000 | nDCG@10 | nDCG@20 |\n",
				PadStr(lit, 10, ' '));
			fprintf(eval_file, "%s\n", PadStr(dash, 137, '-')); fflush(NULL);
		}

	} else if (RESULTS_TYPE == 2) {
		printf("%s\n", PadStr(dash, 137, '-'));
		if (eval_file) { fprintf(eval_file, "%s\n", PadStr(dash, 137, '-')); }

		this->print_header();

		strcpy(lit, "Topic");
		printf("%s\n", PadStr(dash, 137, '-'));
		printf("%s\n", PadStr(dash, 137, '-'));
		printf("| %s\t(Rel)\t| AvePr |  P@5  |  P@10 |  P@20 |  P@30 |  P@50 | P@100 | ANGCG |  N@5  |  N@10 |  N@20 |  N@30 |  N@50 | N@100 |\n",
			PadStr(lit, 10, ' '));
		printf("%s\n", PadStr(dash, 137, '-')); fflush(NULL);

		if (this->eval_file) {
			fprintf(eval_file, "%s\n", PadStr(dash, 137, '-'));
			fprintf(eval_file, "%s\n", PadStr(dash, 137, '-'));
			fprintf(eval_file, "| %s\t(Rel)\t| AvePr |  P@5  |  P@10 |  P@20 |  P@30 |  P@50 | P@100 | ANGCG |  N@5  |  N@10 |  N@20 |  N@30 |  N@50 | N@100 |\n",
				PadStr(lit, 10, ' '));
			fprintf(eval_file, "%s\n", PadStr(dash, 137, '-')); fflush(NULL);
		}
	}

	/// Start the evaluation
	for (m = 0; m < this->num_queries; m++) {
//		printf("Evaluating Query %d (%d items)\n", m + 1, this->queries[m]->get_num_items());
		this->queries[m]->evaluate(this->eval_file);

		if (this->queries[m]->get_num_items() > MAX_LIST_ITEMS) {
			cutoff = MAX_LIST_ITEMS;
		} else {
			cutoff = this->queries[m]->get_num_items();
		}

		for (i = 0; i < cutoff; i++) {
			precision_acc[i] += this->queries[m]->get_precision(i + 1);
			recall_acc[i] += this->queries[m]->get_recall(i + 1);
			F1_acc[i] += this->queries[m]->get_F1(i + 1);
			dcg_acc[i] += this->queries[m]->get_dcg(i + 1);
			ndcg_acc[i] += this->queries[m]->get_ndcg(i + 1);
		}

		sum_avep += this->queries[m]->get_average_precision();
		sum_aven += this->queries[m]->get_average_ndcg();

		this->avg_sprho += this->queries[m]->evaluate_experts_list();
	}

	if (RESULTS_TYPE == 1) { printf("%s\n", PadStr(dash, 137, '-')); } else
	if (RESULTS_TYPE == 2) { printf("%s\n", PadStr(dash, 105, '-')); }

	for (i = 0; i < MAX_LIST_ITEMS; i++) {
		this->mean_precision[i] = precision_acc[i] / (double) this->num_queries;
		this->mean_recall[i] = recall_acc[i] / (double) this->num_queries;
		this->mean_F1[i] = F1_acc[i] / (double) this->num_queries;
		this->mean_dcg[i] = dcg_acc[i] / (double) this->num_queries;
		this->mean_ndcg[i] = ndcg_acc[i] / (double) this->num_queries;
	}
	this->MAP = sum_avep / (double) this->num_queries;
	this->MNDCG = sum_aven / (double) this->num_queries;
	this->avg_sprho = this->avg_sprho / (double) this->num_queries;
}

/// Evaluate the input lists of each query
void TRECInput::evaluate_input() {
	uint32_t m = 0;

	/// Initialize the retrieval effectiveness metrics
	this->initialize_stats();

//	char dash[151], lit[151]; dash[0] = 0; lit[0] = 0;
//	strcpy(lit, "Run - Topic");
//	printf("%s\n", PadStr(dash, 145, '-'));
//	printf("| %s |  AveP  |   P@5  |  P@10  |  P@15  |  P@20  |  P@30  |  P@100 |  P@200 |  P@500 | nDCG@10 | nDCG@20 |\n",
//		PadStr(lit, 40, ' '));
//	printf("%s\n", PadStr(dash, 145, '-'));

	for (m = 0; m < this->num_queries; m++) {
		this->queries[m]->evaluate_input();
	}
}

/// Apply the rank aggregation method and construct the output lists of each Aggregator
void TRECInput::aggregate(uint32_t ram, uint32_t pp, score_t d1, score_t d2) {
	printf("\nPerforming Aggregation...\nParameters ");
	this->print_header();
	printf("\n");

	for (uint32_t m = 0; m < this->num_queries; m++) {
		printf("Processing Query %d / %d... [ ", m + 1, this->num_queries); fflush(NULL);
		this->queries[m]->aggregate(ram, pp, d1, d2);
		printf(" OK ]\n"); fflush(NULL);
	}
}

/// Compute the average list length per query
uint32_t TRECInput::compute_avg_list_length() {
	uint32_t sum = 0, sum2 = 0;
	for (uint32_t q = 0; q < this->num_queries; q++) {
		for (uint32_t l = 0; l < this->queries[q]->get_num_input_lists(); l++) {
			sum2++;
			sum += this->queries[q]->get_input_list(l)->get_num_items();
		}
	}
	return sum / sum2;
}

/// Accessors
uint32_t TRECInput::get_num_queries() { return this->num_queries; }
class Query * TRECInput::get_query(uint32_t i) { return this->queries[i]; }
double TRECInput::get_mean_precision(uint32_t i) { return this->mean_precision[i - 1]; }
double TRECInput::get_mean_recall(uint32_t i) { return this->mean_recall[i - 1]; }
double TRECInput::get_mean_F1(uint32_t i) { return this->mean_F1[i - 1]; }
double TRECInput::get_mean_dcg(uint32_t i) { return this->mean_dcg[i - 1]; }
double TRECInput::get_mean_ndcg(uint32_t i) { return this->mean_ndcg[i - 1]; }
double TRECInput::get_MAP() { return this->MAP; }
double TRECInput::get_MNDCG() { return this->MNDCG; }
double TRECInput::get_avg_sprho() { return this->avg_sprho; }
FILE * TRECInput::get_eval_file() { return this->eval_file; }

