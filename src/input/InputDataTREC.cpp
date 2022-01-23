/// Decompress a TREC Run (Input file submitted by a research group)
char * InputData::read_TREC_file(FILE * source, char * voter) {
	long file_size = 0;

	fseek(source , 0L, SEEK_END);
	file_size = ftell(source);
	rewind(source);

	char * out = (char *)malloc((file_size + 1) * sizeof(char));

	if (fread(out, file_size, 1 , source) != 1) {
		fclose(source);
		free(out);
		fputs("entire read fails",stderr);
		exit(1);
	}
	out[file_size - 1] = 0;

//	printf("contents: %s", out); getchar();

	this->process_TREC_lists(out, file_size, voter);

	free(out);
	return out;
}

/// Decompress a TREC Run (Input file submitted by a research group)
char * InputData::decompress_TREC_file(FILE * source, char * voter) {
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

		/// Done when inflate() says it's done
	} while (ret != Z_STREAM_END);

	/// Clean up and return
	(void)inflateEnd(&strm);

	out[os2] = 0;

	this->process_TREC_lists(out, os2, voter);

	free(out);
	return out;
}

/// Process a TREC Run: Extract the submitted results and their rankings
void InputData::process_TREC_lists(char * in, uint32_t len, char * voter) {
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

/// Read the TREC relevance judgments file (qrels)
void InputData::read_TREC_qrels() {
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

