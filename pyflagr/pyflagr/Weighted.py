import os.path
import ctypes

from pyflagr.RAM import RAM


# PREFERENCE RELATIONS METHOD =========================================================================================
class PreferenceRelationsGraph(RAM):
    Alpha = 0.1
    Beta = 0.5

    def __init__(self, eval_pts=10, alpha=Alpha, beta=Beta):
        RAM.__init__(self, eval_pts)

        self.Alpha = alpha
        self.Beta = beta

        self.flagr_lib.PrefRel.argtypes = [
            ctypes.c_char_p,  # Input data file with the lists to be aggregated
            ctypes.c_char_p,  # Input data file with the relevant elements per query - used for evaluation
            ctypes.c_int,     # Number of evaluation points
            ctypes.c_char_p,  # Random string to be embedded into the output file names
            ctypes.c_char_p,  # The directory where the output files will be written
            ctypes.c_float,   # alpha parameter
            ctypes.c_float    # beta parameter
        ]

        self.flagr_lib.PrefRel.restype = None

    def aggregate(self, input_file="", input_df=None, rels_file="", rels_df=None, out_dir=None):
        # This is the directory where the output files are written. If nothing is provided, then the preset temp
        # directory of the OS is used. If an invalid path is provided, the aforementioned temp dir is used silently.
        if out_dir is not None and os.path.isdir(out_dir):
            self.output_dir = out_dir

        status = self.check_get_input(input_file, input_df)
        if status != 0:
            return

        status = self.check_get_rels_input(rels_file, rels_df)
        if status != 0:
            return

        ran_str = self.get_random_string(16)

        # Call the exposed PrefRel C function
        self.flagr_lib.PrefRel(
            bytes(self.input_file, 'ASCII'),
            bytes(self.rels_file, 'ASCII'),
            self.eval_pts,
            bytes(ran_str, 'ASCII'),
            bytes(self.output_dir, 'ASCII'),
            self.Alpha,
            self.Beta
        )

        df_out, df_eval = self.get_output(self.output_dir, ran_str)
        return df_out, df_eval


# AGGLOMERATIVE METHOD ===============================================================================================
class Agglomerative(RAM):
    C1 = 0.1
    C2 = 0.5

    def __init__(self, eval_pts=10, c1=C1, c2=C2):
        RAM.__init__(self, eval_pts)

        self.C1 = c1
        self.C2 = c2

        self.flagr_lib.Agglomerative.argtypes = [
            ctypes.c_char_p,  # Input data file with the lists to be aggregated
            ctypes.c_char_p,  # Input data file with the relevant elements per query - used for evaluation
            ctypes.c_int,     # Number of evaluation points
            ctypes.c_char_p,  # Random string to be embedded into the output file names
            ctypes.c_char_p,  # The directory where the output files will be written
            ctypes.c_float,   # c1 parameter
            ctypes.c_float    # c2 parameter
        ]

        self.flagr_lib.Agglomerative.restype = None

    def aggregate(self, input_file="", input_df=None, rels_file="", rels_df=None, out_dir=None):
        # This is the directory where the output files are written. If nothing is provided, then the preset temp
        # directory of the OS is used. If an invalid path is provided, the aforementioned temp dir is used silently.
        if out_dir is not None and os.path.isdir(out_dir):
            self.output_dir = out_dir

        status = self.check_get_input(input_file, input_df)
        if status != 0:
            return

        status = self.check_get_rels_input(rels_file, rels_df)
        if status != 0:
            return

        ran_str = self.get_random_string(16)

        # Call the exposed Agglomerative C function
        self.flagr_lib.Agglomerative(
            bytes(self.input_file, 'ASCII'),
            bytes(self.rels_file, 'ASCII'),
            self.eval_pts,
            bytes(ran_str, 'ASCII'),
            bytes(self.output_dir, 'ASCII'),
            self.C1,
            self.C2
        )

        df_out, df_eval = self.get_output(self.output_dir, ran_str)
        return df_out, df_eval


# DIBRA METHOD : DISTANCE-BASED ITERATIVE =============================================================================
class DIBRA(RAM):
    agg = 5100
    weight_norm = 2
    distance_metric = 3
    list_pruning = False,
    g = 1.2,
    d_1 = 0.4
    d_2 = 0.1
    tolerance = 0.01
    miter = 50
    preference_t = 0.0
    veto_t = 0.75
    concordance_t = 0.0
    discordance_t = 0.25

    def __init__(self, eval_pts=10, aggregator='combsum:borda', w_norm='minmax', dist='cosine', prune=False,
                 gamma=1.5, d1=0.4, d2=0.1, tol=0.01, max_iter=50, pref=0.0, veto=0.75, conc=0.0, disc=0.25):

        RAM.__init__(self, eval_pts)

        # Set the aggregator code according to the selected aggregation method
        self.agg = 5100
        if aggregator == "combsum:borda":
            self.agg = 5100
        elif aggregator == "combsum:rank":
            self.agg = 5101
        elif aggregator == "combsum:score":
            self.agg = 5102
        elif aggregator == "combsum:z-score":
            self.agg = 5103
        elif aggregator == "combsum:simple-borda":
            self.agg = 5104
        elif aggregator == "combmnz:borda":
            self.agg = 5110
        elif aggregator == "combmnz:rank":
            self.agg = 5111
        elif aggregator == "combmnz:score":
            self.agg = 5112
        elif aggregator == "combmnz:z-score":
            self.agg = 5113
        elif aggregator == "combmnz:simple-borda":
            self.agg = 5114
        elif aggregator == "condorcet":
            self.agg = 5200
        elif aggregator == "copeland":
            self.agg = 5201
        elif aggregator == "outrank":
            self.agg = 5300

        # Set the voter weights normalization method
        self.weight_norm = 2
        if w_norm == "none":
            self.weight_norm = 1
        elif w_norm == "minmax":
            self.weight_norm = 2
        elif w_norm == "z":
            self.weight_norm = 3

        # Set the list distance metric
        self.distance_metric = 3
        if dist == "rho":
            self.distance_metric = 1
        elif dist == "cosine":
            self.distance_metric = 3
        elif dist == "footrule":
            self.distance_metric = 4
        elif dist == "tau":
            self.distance_metric = 5

        self.list_pruning = prune
        self.g = gamma
        self.d_1 = d1
        self.d_2 = d2
        self.tolerance = tol
        self.miter = max_iter
        self.preference_t = pref
        self.veto_t = veto
        self.concordance_t = conc
        self.discordance_t = disc

        self.flagr_lib.DIBRA.argtypes = [
            ctypes.c_char_p,  # Input data file with the lists to be aggregated
            ctypes.c_char_p,  # Input data file with the relevant elements per query - used for evaluation
            ctypes.c_int,     # Number of evaluation points
            ctypes.c_int,     # basic un-weighted rank aggregation method
            ctypes.c_char_p,  # Random string to be embedded into the output file names
            ctypes.c_char_p,  # The directory where the output files will be written
            ctypes.c_int,     # Voter weights normalization
            ctypes.c_int,     # List distance function
            ctypes.c_bool,    # List pruning algorithm
            ctypes.c_float,   # gamma parameter
            ctypes.c_float,   # d1 parameter
            ctypes.c_float,   # d2 parameter
            ctypes.c_float,   # convergence precision tolerance
            ctypes.c_int,     # Maximum number of iterations
            ctypes.c_float,   # Preference Threshold
            ctypes.c_float,   # Veto Threshold
            ctypes.c_float,   # Concordance Threshold
            ctypes.c_float    # Discordance Threshold
        ]

        self.flagr_lib.DIBRA.restype = None

    def aggregate(self, input_file="", input_df=None, rels_file="", rels_df=None, out_dir=None):
        # This is the directory where the output files are written. If nothing is provided, then the preset temp
        # directory of the OS is used. If an invalid path is provided, the aforementioned temp dir is used silently.
        if out_dir is not None and os.path.isdir(out_dir):
            self.output_dir = out_dir

        status = self.check_get_input(input_file, input_df)
        if status != 0:
            return

        status = self.check_get_rels_input(rels_file, rels_df)
        if status != 0:
            return

        ran_str = self.get_random_string(16)

        # Call the exposed Agglomerative C function
        self.flagr_lib.DIBRA(
            bytes(self.input_file, 'ASCII'),
            bytes(self.rels_file, 'ASCII'),
            self.eval_pts,
            self.agg,
            bytes(ran_str, 'ASCII'),
            bytes(self.output_dir, 'ASCII'),
            self.weight_norm,
            self.distance_metric,
            self.list_pruning,
            self.g,
            self.d_1,
            self.d_2,
            self.tolerance,
            self.miter,
            self.preference_t,
            self.veto_t,
            self.concordance_t,
            self.discordance_t
        )

        df_out, df_eval = self.get_output(self.output_dir, ran_str)
        return df_out, df_eval
