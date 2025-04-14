#ifndef SIMPLESCORESTATS_H
#define SIMPLESCORESTATS_H


class SimpleScoreStats {
	private:
		score_t min_val;
		score_t max_val;
		score_t mean_val;
		score_t std_val;

	public:
		SimpleScoreStats();
		~SimpleScoreStats();

		void display();
		void reset();

		/// Accessors
		score_t get_min_val();
		score_t get_max_val();
		score_t get_mean_val();
		score_t get_std_val();

		/// Mutators
		void set_min_val(score_t);
		void set_max_val(score_t);
		void set_mean_val(score_t);
		void set_std_val(score_t);
};

#endif // SIMPLESCORESTATS_H
