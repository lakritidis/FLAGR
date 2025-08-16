import os.path
import ctypes
import time

from pyflagr.RAM import RAM


# ROBUST RANK AGGREGATION METHOD ===================================================================================
class RRA(RAM):
    exact = False

    def __init__(self, eval_pts=10, exact=False):
        RAM.__init__(self, eval_pts)

        self.exact = exact

        self.flagr_lib.RobustRA.argtypes = [
            ctypes.c_char_p,  # Input data file with the lists to be aggregated
            ctypes.c_char_p,  # Input data file with the relevant elements per query - used for evaluation
            ctypes.c_int,     # Number of evaluation points
            ctypes.c_char_p,  # Random string to be embedded into the output file names
            ctypes.c_char_p,  # The directory where the output files will be written
            ctypes.c_bool     # Correct p-values with Stuart-Ares algorithm
        ]

        self.flagr_lib.RobustRA.restype = None

    def aggregate(self, input_file="", input_df=None, rels_file="", rels_df=None, out_dir=None):
        # This is the directory where the output files are written. If nothing is provided, then the preset temp
        # directory of the OS is used. If an invalid path is provided, the aforementioned temp dir is used silently.
        if out_dir is not None and os.path.isdir(out_dir):
            self.output_dir = out_dir

        status = self.check_get_input(input_file, input_df)
        if status != 0:
            return None

        status = self.check_get_rels_input(rels_file, rels_df)
        if status != 0:
            return None

        ran_str = self.get_random_string(16)

        # Call the exposed RobustRA C function
        st = time.time()
        self.flagr_lib.RobustRA(
            bytes(self.input_file, 'ASCII'),
            bytes(self.rels_file, 'ASCII'),
            self.eval_pts,
            bytes(ran_str, 'ASCII'),
            bytes(self.output_dir, 'ASCII'),
            self.exact
        )
        ed = time.time()

        df_out, df_eval = self.get_output(self.output_dir, ran_str)
        df_eval['time'] = ed - st

        return df_out, df_eval
