import os.path
import ctypes

from pyflagr.RAM import RAM


# COMB SUM ===========================================================================================================
class CombSUM(RAM):
    normalization = "borda"

    def __init__(self, norm="borda", eval_pts=10):
        RAM.__init__(self, eval_pts)

        self.normalization = norm
        self.flagr_lib.Linear.argtypes = [
            ctypes.c_char_p,  # Input data file with the lists to be aggregated
            ctypes.c_char_p,  # Input data file with the relevant elements per query - used for evaluation
            ctypes.c_int,     # Number of evaluation points
            ctypes.c_int,     # Rank/Score normalization method (Rank Aggregation Method)
            ctypes.c_char_p,  # Random string to be embedded into the output file names
            ctypes.c_char_p]  # The directory where the output files will be written

        self.flagr_lib.Linear.restype = None

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

        # Rank/Score normalization
        if self.normalization == "borda":
            ram = 100
        elif self.normalization == "rank":
            ram = 101
        elif self.normalization == "score":
            ram = 102
        elif self.normalization == "z-score":
            ram = 103
        elif self.normalization == "simple-borda":
            ram = 104
        else:
            ram = 100

        ran_str = self.get_random_string(16)

        # Call the exposed Linear C function
        self.flagr_lib.Linear(
            bytes(self.input_file, 'ASCII'),
            bytes(self.rels_file, 'ASCII'),
            self.eval_pts,
            ram,
            bytes(ran_str, 'ASCII'),
            bytes(self.output_dir, 'ASCII')
        )

        df_out, df_eval = self.get_output(self.output_dir, ran_str)
        return df_out, df_eval


# BORDA COUNT IS EQUIVALENT TO COMBSUM WITH BORDA NORMALIZATION ======================================================
class BordaCount(CombSUM):
    def __init__(self, eval_pts=10):
        CombSUM.__init__(self, "borda", eval_pts)


# SIMPLE BORDA COUNT IS EQUIVALENT TO COMBSUM WITH BORDA NORMALIZATION ===============================================
class SimpleBordaCount(CombSUM):
    def __init__(self, eval_pts=10):
        CombSUM.__init__(self, "simple-borda", eval_pts)


# COMB MNZ ===========================================================================================================
class CombMNZ(RAM):
    normalization = "borda"

    def __init__(self, norm="borda", eval_pts=10):
        RAM.__init__(self, eval_pts)

        self.normalization = norm
        self.flagr_lib.Linear.argtypes = [
            ctypes.c_char_p,  # Input data file with the lists to be aggregated
            ctypes.c_char_p,  # Input data file with the relevant elements per query - used for evaluation
            ctypes.c_int,     # Number of evaluation points
            ctypes.c_int,     # Rank/Score normalization method (Rank Aggregation Method)
            ctypes.c_char_p,  # Random string to be embedded into the output file names
            ctypes.c_char_p]  # The directory where the output files will be written

        self.flagr_lib.Linear.restype = None

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

        # Rank/Score normalization
        if self.normalization == "borda":
            ram = 110
        elif self.normalization == "rank":
            ram = 111
        elif self.normalization == "score":
            ram = 112
        elif self.normalization == "z-score":
            ram = 113
        elif self.normalization == "simple-borda":
            ram = 114
        else:
            ram = 110

        ran_str = self.get_random_string(16)

        # Call the exposed Linear C function
        self.flagr_lib.Linear(
            ctypes.c_char_p(self.input_file.encode('ascii')),
            ctypes.c_char_p(self.rels_file.encode('ascii')),
            self.eval_pts,
            ram,
            ctypes.c_char_p(ran_str.encode('ascii')),
            ctypes.c_char_p(self.output_dir.encode('ascii'))
            )

        df_out, df_eval = self.get_output(self.output_dir, ran_str)

        return df_out, df_eval
