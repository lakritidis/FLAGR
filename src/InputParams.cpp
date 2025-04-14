#include "InputParams.h"

/// Default Constructor
InputParams::InputParams() :
		input_file(NULL),
		rels_file(NULL),
		output_file(NULL),
		eval_file(NULL),
		random_string(NULL),
		aggregation_method(0),
		correlation_method(0),
		weights_normalization(0),
		max_iterations(0),
		max_list_items(0),
		eval_points(0),
		item_selection(0),
		exact(false),
		convergence_precision(0.0),
		alpha(0.0),
		beta(0.0),
		gamma(0.0),
		delta1(0.0),
		delta2(0.0),
		num_buckets(0),
		c1(0.0),
		c2(0.0),
		pref_thr(0.0),
		veto_thr(0.0),
		conc_thr(0.0),
		disc_thr(0.0) { }

/// Default Constructor
InputParams::InputParams(struct UserParams uParams) :
		input_file(NULL),
		rels_file(NULL),
		output_file(NULL),
		eval_file(NULL),
		random_string(NULL),
		aggregation_method(uParams.rank_aggregation_method),
		correlation_method(uParams.distance),
		weights_normalization(uParams.weight_normalization),
		max_iterations(uParams.max_iter),
		max_list_items(0),
		eval_points(uParams.eval_points),
		item_selection(uParams.item_selection),
		exact(uParams.exact),
		convergence_precision(uParams.tol),
		alpha(uParams.alpha),
		beta(uParams.beta),
		gamma(uParams.gamma),
		delta1(uParams.delta1),
		delta2(uParams.delta2),
		num_buckets(uParams.num_buckets),
		c1(uParams.c1),
		c2(uParams.c2),
		pref_thr(uParams.pref_thr),
		veto_thr(uParams.veto_thr),
		conc_thr(uParams.conc_thr),
		disc_thr(uParams.disc_thr) {

			this->set_input_file(uParams.input_file);
			if (uParams.rels_file) {
				this->set_rels_file(uParams.rels_file);
			}

			this->set_random_string(uParams.random_string);
			this->set_output_files(uParams.output_dir);
		}

/// Destructor
InputParams::~InputParams() {
	if (this->input_file) { delete [] this->input_file; }
	if (this->rels_file) { delete [] this->rels_file; }
	if (this->output_file) { delete [] this->output_file; }
	if (this->eval_file) { delete [] this->eval_file; }
	if (this->random_string) { delete [] this->random_string; }
}

/// Display members
void InputParams::display() {
	printf("FLAGR execution parameters:\n");
	if (input_file)    { printf("\tInput file:         %s\n", this->input_file); }
				else   { printf("\tInput file:         [not set]\n"); }

	if (rels_file)     { printf("\tQ-Rels file:        %s\n", this->rels_file); }
				else   { printf("\tQ-Rels file:        [not set]\n"); }

	if (output_file)   { printf("\tOutput file:        %s\n", this->output_file); }
				else   { printf("\tOutput file:        [not set]\n"); }

	if (eval_file)     { printf("\tEvaluation file:    %s\n", this->eval_file); }
				else   { printf("\tEvaluation file:    [not set]\n"); }

	if (random_string) { printf("\tRandom string:      %s\n", this->random_string); }
				else   { printf("\tRandom string:      [not set]\n"); }

	printf("\tAggregation method: %d\n",	this->aggregation_method);
	printf("\tDistance measure:   %d\n",	this->correlation_method);
	printf("\tVoter weights norm: %d\n",	this->weights_normalization);
	printf("\tMax iterations:     %d\n",	this->max_iterations);
	printf("\tMax list items:     %d\n",	this->max_list_items);
	printf("\tEvaluation points:  %d\n",	this->eval_points);

	printf("\nAlgorithm hyper-parameters:\n");
	printf("\tItem selection:     %d\n", 	this->item_selection);
	printf("\tExact computation:  %d\n", 	this->exact);
	printf("\tConvergence precis: %7.6f\n", this->convergence_precision);
	printf("\talpha:              %4.3f\n", this->alpha);
	printf("\tbeta:               %4.3f\n", this->beta);
	printf("\tc_1:                %4.3f\n", this->c1);
	printf("\tc_2:                %4.3f\n", this->c2);
	printf("\tgamma:              %4.3f\n", this->gamma);
	printf("\tNum voter buckets:  %d\n",	this->num_buckets);
	printf("\tdelta_1:            %4.3f\n", this->delta1);
	printf("\tdelta_2:            %4.3f\n", this->delta2);
	printf("\tPreference thresh:  %4.3f\n", this->pref_thr);
	printf("\tVeto threshold:     %4.3f\n", this->veto_thr);
	printf("\tConcordance thresh: %4.3f\n", this->conc_thr);
	printf("\tDiscordance thresh: %4.3f\n", this->disc_thr);
	printf("\n"); fflush(NULL);
}

/// Random String generator: it will be used for the output filename and the evaluation filename
void InputParams::generate_random_string(size_t size) {

    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK0123456789";
	this->random_string = new char[size + 1];

    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            this->random_string[n] = charset[key];
        }
        this->random_string[size] = '\0';
    }
}

/// Prepare the output files (aggregation output and evaluation report)
void InputParams::set_output_files(char * out_dir) {
	if (!this->random_string) {
		this->generate_random_string(16);
	}

	this->output_file = new char[1024];
	sprintf(this->output_file, "%s/out_%s.csv", out_dir, this->random_string);

	if (this->rels_file) {
		this->eval_file = new char[1024];
		sprintf(this->eval_file, "%s/eval_%s.csv", out_dir, this->random_string);
	}

	FILE * fp = NULL;
	fp = fopen(this->output_file, "w+");
	if (fp) {
		// fprintf(fp, "Query,Voter,ItemID,Rank,Score,Aggregator\n");
		fclose(fp);
	}

	fp = fopen(this->eval_file, "w+");
	if (fp) { fclose(fp); }
}


/// Accessors
char * InputParams::get_input_file() { return this->input_file; }
char * InputParams::get_rels_file() { return this->rels_file; }
char * InputParams::get_output_file() { return this->output_file; }
char * InputParams::get_eval_file() { return this->eval_file; }
char * InputParams::get_random_string() { return this->random_string; }

uint32_t InputParams::get_aggregation_method() { return this->aggregation_method; }
uint32_t InputParams::get_correlation_method() { return this->correlation_method; }
uint32_t InputParams::get_weights_normalization() { return this->weights_normalization; }
int32_t InputParams::get_max_iterations() { return this->max_iterations; }
uint32_t InputParams::get_max_list_items() { return this->max_list_items; }
rank_t InputParams::get_eval_points() { return this->eval_points; }
uint32_t InputParams::get_item_selection() { return this->item_selection; }
bool InputParams::get_exact() { return this->exact; }

score_t InputParams::get_convergence_precision() { return this->convergence_precision; }
score_t InputParams::get_alpha() { return this->alpha; }
score_t InputParams::get_beta() { return this->beta; }
score_t InputParams::get_gamma() { return this->gamma; }
score_t InputParams::get_delta1() { return this->delta1; }
score_t InputParams::get_delta2() { return this->delta2; }
uint32_t InputParams::get_num_buckets() { return this->num_buckets; }
score_t InputParams::get_c1() { return this->c1; }
score_t InputParams::get_c2() { return this->c2; }
score_t InputParams::get_pref_thr() { return this->pref_thr; }
score_t InputParams::get_veto_thr() { return this->veto_thr; }
score_t InputParams::get_conc_thr() { return this->conc_thr; }
score_t InputParams::get_disc_thr() { return this->disc_thr; }


/// ///////////////////////////////////////////////////////////////////////////////////////////////
/// Mutators
void InputParams::set_input_file(char * v) {
	this->input_file = new char[strlen(v) + 1];
	strcpy(this->input_file, v);
}

void InputParams::set_rels_file(char * v) {
	this->rels_file = new char[strlen(v) + 1];
	strcpy(this->rels_file, v);
}

void InputParams::set_output_file(const char * v) {
	this->output_file = new char[strlen(v) + 1];
	strcpy(this->output_file, v);
}

void InputParams::set_eval_file(const char * v) {
	this->eval_file = new char[strlen(v) + 1];
	strcpy(this->eval_file, v);
}

void InputParams::set_random_string(const char * v) {
	this->random_string = new char[strlen(v) + 1];
	strcpy(this->random_string, v);
}

void InputParams::set_aggregation_method(uint32_t v) { this->aggregation_method = v; }
void InputParams::set_correlation_method(uint32_t v) { this->correlation_method = v; }
void InputParams::set_weights_normalization(uint32_t v) { this->weights_normalization = v; }
void InputParams::set_max_iterations(int32_t v) { this->max_iterations = v; }
void InputParams::set_max_list_items(uint32_t v) { this->max_list_items = v; }
void InputParams::set_eval_points(rank_t v) { this->eval_points = v; }
void InputParams::set_item_selection(uint32_t v) { this->item_selection = v; }

void InputParams::set_convergence_precision(score_t v) { this->convergence_precision = v; }
void InputParams::set_alpha(score_t v) { this->alpha = v; }
void InputParams::set_beta(score_t v) { this->beta = v; }
void InputParams::set_gamma(score_t v) { this->gamma = v; }
void InputParams::set_delta1(score_t v) { this->delta1 = v; }
void InputParams::set_delta2(score_t v) { this->delta2 = v; }
void InputParams::set_num_buckets(uint32_t v) { this->num_buckets = v; }
void InputParams::set_c1(score_t v) { this->c1 = v; }
void InputParams::set_c2(score_t v) { this->c2 = v; }
void InputParams::set_pref_thr(score_t v) { this->pref_thr = v; }
void InputParams::set_veto_thr(score_t v) { this->veto_thr = v; }
void InputParams::set_conc_thr(score_t v) { this->conc_thr = v; }
void InputParams::set_disc_thr(score_t v) { this->disc_thr = v; }
