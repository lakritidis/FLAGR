#include "InputParams.h"

/// Default Constructor
InputParams::InputParams() {
	this->base_path = NULL;
	this->data_dir = NULL;
	this->dataset_type = 0;
	this->dataset_name = NULL;
	this->dataset_track = NULL;
	this->dataset_compressed = NULL;

	this->aggregation_method = 0;
	this->correlation_method = 0;
	this->weights_normalization = 0;
	this->max_iterations = 0;
	this->iterations = 0;
	this->max_list_items = 0;
	this->list_pruning = false;

	this->convergence_precision = 0.0;
	this->alpha = 0.0;
	this->beta = 0.0;
	this->gamma = 0.0;
	this->delta1 = 0.0;
	this->delta2 = 0.0;

	this->eval_file = NULL;
	this->dvar_file = NULL;
}

/// Constructor
InputParams::InputParams(char * bp, uint32_t dtype, char * dname, char * dtrack, bool dcomp,
	uint32_t amet, uint32_t cmet, uint32_t wnrm, int32_t mit, int32_t it, uint32_t mli, bool lp,
	score_t conv_p, score_t a, score_t b, score_t g, score_t d1, score_t d2)
	{
		this->set_base_path(bp);
		this->dataset_type = dtype;
		this->set_dataset_name(dname);
		this->set_dataset_track(dtrack);
		this->dataset_compressed = dcomp;

		this->aggregation_method = amet;
		this->correlation_method = cmet;
		this->weights_normalization = wnrm;
		this->max_iterations = mit;
		this->iterations = it;
		this->max_list_items = mli;
		this->list_pruning = lp;

		this->convergence_precision = conv_p;
		this->alpha = a;
		this->beta = b;
		this->gamma = g;
		this->delta1 = d1;
		this->delta2 = d2;

		this->build_filenames();
}

/// Destructor
InputParams::~InputParams() {
	if (this->base_path) { delete [] this->base_path; }
	if (this->data_dir) { delete [] this->data_dir; }
	if (this->dataset_name) { delete [] this->dataset_name; }
	if (this->dataset_track) { delete [] this->dataset_track; }
	if (this->eval_file) { delete [] this->eval_file; }
	if (this->dvar_file) { delete [] this->dvar_file; }
}

void InputParams::build_filenames() {
	this->data_dir = new char[1024];
	if (this->dataset_type == 1) {
		sprintf(this->data_dir, "%s%s/%s_%s_Input/",
			this->base_path, this->dataset_name, this->dataset_name, this->dataset_track);

	} else if (this->dataset_type == 2) {
		sprintf(this->data_dir, "%s%s-agg/%s", this->base_path, this->dataset_name, this->dataset_track);
	}

	this->eval_file = new char[1024];
	sprintf(this->eval_file, "/home/leo/Desktop/code/rankagg/results/eval_%d_%d_%s_%s.txt",
		this->aggregation_method, this->correlation_method,
		this->dataset_name, this->dataset_track);

	this->dvar_file = new char[1024];
	sprintf(this->dvar_file, "/home/leo/Desktop/code/rankagg/results/dvar_%d_%d_%s_%s.txt",
		this->aggregation_method, this->correlation_method,
		this->dataset_name, this->dataset_track);
}

/// Acessors
char * InputParams::get_base_path() { return this->base_path; }
char * InputParams::get_data_dir() { return this->data_dir; }
uint32_t InputParams::get_dataset_type() { return this->dataset_type; }
char * InputParams::get_dataset_name() { return this->dataset_name; }
char * InputParams::get_dataset_track() { return this->dataset_track; }
bool InputParams::get_dataset_compressed() { return this->dataset_compressed; }

uint32_t InputParams::get_aggregation_method() { return this->aggregation_method; }
uint32_t InputParams::get_correlation_method() { return this->correlation_method; }
uint32_t InputParams::get_weights_normalization() { return this->weights_normalization; }
int32_t InputParams::get_max_iterations() { return this->max_iterations; }
int32_t InputParams::get_iterations() { return this->iterations; }
uint32_t InputParams::get_max_list_items() { return this->max_list_items; }
bool InputParams::get_list_pruning() { return this->list_pruning; }

char * InputParams::get_eval_file() { return this->eval_file; }
char * InputParams::get_dvar_file() { return this->dvar_file; }

score_t InputParams::get_convergence_precision() { return this->convergence_precision; }
score_t InputParams::get_alpha() { return this->alpha; }
score_t InputParams::get_beta() { return this->beta; }
score_t InputParams::get_gamma() { return this->gamma; }
score_t InputParams::get_delta1() { return this->delta1; }
score_t InputParams::get_delta2() { return this->delta2; }

/// ///////////////////////////////////////////////////////////////////////////////////////////////
/// Mutators
void InputParams::set_base_path(char * v) {
	this->base_path = new char[strlen(v) + 1];
	strcpy(this->base_path, v);
}

void InputParams::set_data_dir(char * v) {
	this->data_dir = new char[strlen(v) + 1];
	strcpy(this->data_dir, v);
}

void InputParams::set_dataset_type(uint32_t v) { this->dataset_type = v; }

void InputParams::set_dataset_name(char * v) {
	this->dataset_name = new char[strlen(v) + 1];
	strcpy(this->dataset_name, v);
}

void InputParams::set_dataset_track(char * v) {
	this->dataset_track = new char[strlen(v) + 1];
	strcpy(this->dataset_track, v);
}

void InputParams::set_dataset_compressed(bool v) { this->dataset_compressed = v; }

void InputParams::set_aggregation_method(uint32_t v) { this->aggregation_method = v; }
void InputParams::set_correlation_method(uint32_t v) { this->correlation_method = v; }
void InputParams::set_weights_normalization(uint32_t v) { this->weights_normalization = v; }
void InputParams::set_max_iterations(int32_t v) { this->max_iterations = v; }
void InputParams::set_iterations(int32_t v) { this->iterations = v; }
void InputParams::set_max_list_items(uint32_t v) { this->max_list_items = v; }
void InputParams::set_list_pruning(bool v) { this->list_pruning = v; }

void InputParams::set_eval_file(char * v) {
	this->eval_file = new char[strlen(v) + 1];
	strcpy(this->eval_file, v);
}

void InputParams::set_dvar_file(char * v) {
	this->dvar_file = new char[strlen(v) + 1];
	strcpy(this->dvar_file, v);
}

void InputParams::set_convergence_precision(score_t v) { this->convergence_precision = v; }
void InputParams::set_alpha(score_t v) { this->alpha = v; }
void InputParams::set_beta(score_t v) { this->beta = v; }
void InputParams::set_gamma(score_t v) { this->gamma = v; }
void InputParams::set_delta1(score_t v) { this->delta1 = v; }
void InputParams::set_delta2(score_t v) { this->delta2 = v; }
