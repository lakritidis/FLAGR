import os.path
import ctypes

from pyflagr.RAM import RAM

def_ergodic_number = 0.15
def_max_iterations = 100


# MARKOV CHAINS BASE CLASS ===========================================================================================
class MC(RAM):
    erg_num = def_ergodic_number
    niter = def_max_iterations
    chain_type = 804

    def __init__(self, eval_pts, ergodic_number=def_ergodic_number, max_iterations=def_max_iterations, chain=804):
        RAM.__init__(self, eval_pts)

        self.erg_num = ergodic_number
        self.niter = max_iterations
        self.chain_type = chain

        self.flagr_lib.MC.argtypes = [
            ctypes.c_char_p,  # Input data file with the lists to be aggregated
            ctypes.c_char_p,  # Input data file with the relevant elements per query - used for evaluation
            ctypes.c_int,     # Number of evaluation points
            ctypes.c_int,     # Type of Markov Chain
            ctypes.c_char_p,  # Random string to be embedded into the output file names
            ctypes.c_char_p,  # The directory where the output files will be written
            ctypes.c_float,   # Ergodic number
            ctypes.c_float,   # delta parameter
            ctypes.c_int      # Maximum number of iterations
        ]

        self.flagr_lib.MC.restype = None

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

        # Call the exposed Condorcet C function
        self.flagr_lib.MC(
            bytes(self.input_file, 'ASCII'),
            bytes(self.rels_file, 'ASCII'),
            self.eval_pts,
            self.chain_type,
            bytes(ran_str, 'ASCII'),
            bytes(self.output_dir, 'ASCII'),
            self.erg_num,
            0.0,
            self.niter
        )

        df_out, df_eval = self.get_output(self.output_dir, ran_str)
        return df_out, df_eval


# MARKOV CHAINS 1 METHOD ===========================================================================================
class MC1(MC):
    def __init__(self, eval_pts, ergodic_number=def_ergodic_number, max_iterations=def_max_iterations):
        MC.__init__(self, eval_pts, ergodic_number, max_iterations, 801)


# MARKOV CHAINS 2 METHOD ===========================================================================================
class MC2(MC):
    def __init__(self, eval_pts, ergodic_number=def_ergodic_number, max_iterations=def_max_iterations):
        MC.__init__(self, eval_pts, ergodic_number, max_iterations, 802)


# MARKOV CHAINS 3 METHOD ===========================================================================================
class MC3(MC):
    def __init__(self, eval_pts, ergodic_number=def_ergodic_number, max_iterations=def_max_iterations):
        MC.__init__(self, eval_pts, ergodic_number, max_iterations, 803)


# MARKOV CHAINS 4 METHOD ===========================================================================================
class MC4(MC):
    def __init__(self, eval_pts, ergodic_number=def_ergodic_number, max_iterations=def_max_iterations):
        MC.__init__(self, eval_pts, ergodic_number, max_iterations, 804)


# MARKOV CHAINS THURSTONE METHOD ===================================================================================
class MCT(MC):
    def __init__(self, eval_pts, ergodic_number=def_ergodic_number, max_iterations=def_max_iterations):
        MC.__init__(self, eval_pts, ergodic_number, max_iterations, 805)
