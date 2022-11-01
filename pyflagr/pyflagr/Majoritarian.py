import os.path
import ctypes

from pyflagr.RAM import RAM


# CONDORCET WINNERS METHOD ============================================================================================
class CondorcetWinners(RAM):

    def __init__(self, eval_pts=10):
        RAM.__init__(self, eval_pts)

        self.flagr_lib.Condorcet.argtypes = [
            ctypes.c_char_p,  # Input data file with the lists to be aggregated
            ctypes.c_char_p,  # Input data file with the relevant elements per query - used for evaluation
            ctypes.c_int,     # Number of evaluation points
            ctypes.c_char_p,  # Random string to be embedded into the output file names
            ctypes.c_char_p  # The directory where the output files will be written
        ]

        self.flagr_lib.Condorcet.restype = None

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
        self.flagr_lib.Condorcet(
            bytes(self.input_file, 'ASCII'),
            bytes(self.rels_file, 'ASCII'),
            self.eval_pts,
            bytes(ran_str, 'ASCII'),
            bytes(self.output_dir, 'ASCII')
        )

        df_out, df_eval = self.get_output(self.output_dir, ran_str)
        return df_out, df_eval


# COPELAND WINNERS METHOD ============================================================================================
class CopelandWinners(RAM):

    def __init__(self, eval_pts=10):
        RAM.__init__(self, eval_pts)

        self.flagr_lib.Copeland.argtypes = [
            ctypes.c_char_p,  # Input data file with the lists to be aggregated
            ctypes.c_char_p,  # Input data file with the relevant elements per query - used for evaluation
            ctypes.c_int,     # Number of evaluation points
            ctypes.c_char_p,  # Random string to be embedded into the output file names
            ctypes.c_char_p   # The directory where the output files will be written
        ]

        self.flagr_lib.Copeland.restype = None

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

        # Call the exposed Copeland C function
        self.flagr_lib.Copeland(
            bytes(self.input_file, 'ASCII'),
            bytes(self.rels_file, 'ASCII'),
            self.eval_pts,
            bytes(ran_str, 'ASCII'),
            bytes(self.output_dir, 'ASCII')
        )

        df_out, df_eval = self.get_output(self.output_dir, ran_str)
        return df_out, df_eval


# OUTRANKING APPROACH =================================================================================================
class OutrankingApproach(RAM):
    preference_t = 0.0
    veto_t = 0.0
    concordance_t = 0.0
    discordance_t = 0.0

    def __init__(self, eval_pts=10, preference=0.0, veto=0.75, concordance=0.0, discordance=0.25):
        RAM.__init__(self, eval_pts)

        self.preference_t = preference
        self.veto_t = veto
        self.concordance_t = concordance
        self.discordance_t = discordance

        self.flagr_lib.OutrankingApproach.argtypes = [
            ctypes.c_char_p,  # Input data file with the lists to be aggregated
            ctypes.c_char_p,  # Input data file with the relevant elements per query - used for evaluation
            ctypes.c_int,     # Number of evaluation points
            ctypes.c_char_p,  # Random string to be embedded into the output file names
            ctypes.c_char_p,  # The directory where the output files will be written
            ctypes.c_float,   # Preference Threshold
            ctypes.c_float,   # Veto Threshold
            ctypes.c_float,   # Concordance Threshold
            ctypes.c_float    # Discordance Threshold
        ]

        self.flagr_lib.OutrankingApproach.restype = None

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

        # Call the exposed OutrankingApproach C function
        self.flagr_lib.OutrankingApproach(
            bytes(self.input_file, 'ASCII'),
            bytes(self.rels_file, 'ASCII'),
            self.eval_pts,
            bytes(ran_str, 'ASCII'),
            bytes(self.output_dir, 'ASCII'),
            float(self.preference_t),
            float(self.veto_t),
            float(self.concordance_t),
            float(self.discordance_t)
        )

        df_out, df_eval = self.get_output(self.output_dir, ran_str)
        return df_out, df_eval
