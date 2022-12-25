/// Header file to be included in C/C++ apps
/// Compile with either -l flagr.a (Windows) or -l flagr.so (Linux)

extern "C" {
	void Linear(const char inf[], const char relf[], const int evpts, const int ram, const char ranstr[], const char out[]);

	/// Wrapper for the Condorcet Winners Method
	void Condorcet(const char inf[], const char relf[], const int evpts, const char ranstr[], const char out[]);

	/// Wrapper for the Copeland Winners Method
	void Copeland(const char inf[], const char relf[], const int evpts, const char ranstr[], const char out[]);

	/// Wrapper for the Outranking Approach of Farah and Vanderpooten [2]
	void OutrankingApproach (const char inf[], const char relf[], const int evpts, const char ranstr[], const char out[], const float pref_t, const float veto_t, const float conc_t, const float disc_t);

	/// Wrapper for Kemeny Optimal Aggregation (brute force implementation)
	void Kemeny(const char inf[], const char relf[], const int evpts, const char ranstr[], const char out[]);

	/// Wrapper for Robust Rank Aggregation of Kolde et. al, 2012 [7]
	void RobustRA(const char inf[], const char relf[], const int evpts, const char ranstr[], const char out[], const bool exact);

	/// Wrapper the distance-based iterative rank aggregation method of Akritidis et. al [5] - DIBRA
	void DIBRA
			(const char inf[], const char relf[], const int evpts, const int agg, const char ranstr[],
			const char out[], const int wnorm, const int dist, const bool prune, const float gamma,
			const float d1, const float d2, const float tol, const int iter,
			const float pref_t, const float veto_t, const float conc_t, const float disc_t);

	/// Wrapper the preference relation rank aggregation method of Desarkar et. al, 2016 [3]
	void PrefRel(const char inf[], const char relf[], const int evpts, const char ranstr[], const char out[], const float alpha, const float beta);

	/// Wrapper for Agglomerative rank aggregation method of Chatterjee et. al, 2018 [4]
	void Agglomerative(const char inf[], const char relf[], const int evpts, const char ranstr[], const char out[], const float c1, const float c2);

	/// Wrapper for the Markov Chains methods of Dwork et. al, 2001 [6]
	void MC(const char inf[], const char relf[], const int evpts, const int ram, const char ranstr[], const char out[], const float ergodic_number, const float delta, const int iter);

	/// Wrapper for the first custom method
	void Custom1(const char inf[], const char relf[], const int evpts, const char ranstr[], const char out[]);

	/// Wrapper for the second custom method
	void Custom2(const char inf[], const char relf[], const int evpts, const char ranstr[], const char out[]);
}
