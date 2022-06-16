#include "SimpleScoreStats.h"

/// Default (empty) constructor
SimpleScoreStats::SimpleScoreStats() : min_val(1000.0), max_val(0.0), mean_val(0.0), std_val(0.0) { }

/// Destructor
SimpleScoreStats::~SimpleScoreStats() {

}

/// Display the values
void SimpleScoreStats::display() {
	printf("Min Val: %5.3f, Max Val: %5.3f, Mean Val: %5.3f, Std: %5.3f\n",
		this->min_val, this->max_val, this->mean_val, this->std_val);
}

/// Accessors
score_t SimpleScoreStats::get_min_val() { return this->min_val; }
score_t SimpleScoreStats::get_max_val() { return this->max_val; }
score_t SimpleScoreStats::get_mean_val() { return this->mean_val; }
score_t SimpleScoreStats::get_std_val() { return this->std_val; }

/// Mutators
void SimpleScoreStats::set_min_val(score_t v) { this->min_val = v; }
void SimpleScoreStats::set_max_val(score_t v) { this->max_val = v; }
void SimpleScoreStats::set_mean_val(score_t v) { this->mean_val = v; }
void SimpleScoreStats::set_std_val(score_t v) { this->std_val = v; }
