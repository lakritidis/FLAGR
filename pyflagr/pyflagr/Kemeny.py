import os.path
import ctypes

from pyflagr.RAM import RAM


# KEMENY OPTIMAL AGGREGATION METHOD ===================================================================================
class KemenyOptimal(RAM):

    def __init__(self, eval_pts=10):
        RAM.__init__(self, eval_pts)

        self.flagr_lib.Kemeny.argtypes = [
            ctypes.c_char_p,  # Input data file with the lists to be aggregated
            ctypes.c_char_p,  # Input data file with the relevant elements per query - used for evaluation
            ctypes.c_int,     # Number of evaluation points
            ctypes.c_char_p,  # Random string to be embedded into the output file names
            ctypes.c_char_p   # The directory where the output files will be written
        ]

        self.flagr_lib.Kemeny.restype = None

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

        # Call the exposed Kemeny C function
        self.flagr_lib.Kemeny(
            bytes(self.input_file, 'ASCII'),
            bytes(self.rels_file, 'ASCII'),
            self.eval_pts,
            bytes(ran_str, 'ASCII'),
            bytes(self.output_dir, 'ASCII')
        )

        df_out, df_eval = self.get_output(self.output_dir, ran_str)
        return df_out, df_eval
