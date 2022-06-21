import ctypes

from pyflagr.RAM import RAM


# MARKOV CHAINS 4 METHOD ==============================================================================================
class MC4(RAM):
    erg_num = 0.15
    delt = 0.00000001
    niter = 200

    def __init__(self, eval_pts=10, ergodic_number=erg_num, delta=delt, max_iterations=niter):
        RAM.__init__(self, eval_pts)

        self.erg_num = ergodic_number
        self.delt = delta
        self.niter = max_iterations

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

    def aggregate(self, input_file="", input_df=None, rels_file="", rels_df=None):
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
            804,
            bytes(ran_str, 'ASCII'),
            bytes(self.output_dir, 'ASCII'),
            self.erg_num,
            self.delt,
            self.niter
        )

        df_out, df_eval = self.get_output(self.output_dir, ran_str)
        return df_out, df_eval
