import ctypes
import os
import random
import string
import tempfile
from sys import platform

import pandas as pd


class RAM:
    input_file = ""
    input_df = None
    rels_file = ""
    rels_df = None
    eval_pts = 10
    output_dir = ""
    flagr_lib = ""

    def __init__(self, eval_pts):
        self.eval_pts = eval_pts

        # This is the default directory where the output files are written. It is the preset temp directory of the OS.
        self.output_dir = tempfile.gettempdir()

        self.flagr_lib = None

        # Import the FLAGR shared library in PyFLAGR
        if platform == "linux" or platform == "linux2":
            self.flagr_lib = ctypes.CDLL(os.path.dirname(os.path.realpath(__file__)) + "/flagr.so")

        elif platform == "win32":
            self.flagr_lib = ctypes.CDLL(os.path.dirname(os.path.realpath(__file__)) + "/flagr.dll")

        elif platform == "darwin":
            self.flagr_lib = ctypes.CDLL(os.path.dirname(os.path.realpath(__file__)) + "/flagr.dylib")

    # Check the input file or DataFrame that contains the input lists
    def check_get_input(self, f, df):
        status = 0
        if len(f) > 0:
            self.input_file = f
            if not os.path.isfile(self.input_file):
                print("Error! Input file does not exist")
                status = -1

        elif df is not None:
            self.input_file = tempfile.gettempdir() + "/temp_input.csv"
            df.to_csv(self.input_file, index=False)

        else:
            print("Error! No input data was passed")
            status = -1

        return status

    # Check the input file or DataFrame that contains the Relevant elements for each query
    def check_get_rels_input(self, rf, rdf):
        status = 0
        if len(rf) > 0:
            self.rels_file = rf

        elif rdf is not None:
            self.rels_file = tempfile.gettempdir() + "/temp_input_rels.csv"
            rdf.to_csv(self.rels_file, index=False)

        else:
            self.rels_file = ""

        return status

    # A random bytes generator - to be used in filenames passed to FLAGR
    def get_random_string(self, length):
        letters = string.ascii_lowercase
        result_str = ''.join(random.choice(letters) for _ in range(length))
        return result_str

    # Retrieve the output from a PYFLAGR rank aggregation method. This one reads the output CSV files and returns
    # two DataFrames. The first one contains the aggregate lists for each query; the second one stores the results
    # of the evaluation (provided that a qrels file or DataFrame has been set).
    def get_output(self, od, ran):
        outfile = od + "/out_" + ran + ".csv"
        evalfile = od + "/eval_" + ran + ".csv"

        if os.path.isfile(outfile):
            df_out = pd.read_csv(outfile, engine='c')
            os.remove(outfile)

            if os.path.isfile(evalfile):
                df_eval = pd.read_csv(evalfile)
                os.remove(evalfile)

                return df_out, df_eval
            else:
                df_rel = pd.DataFrame()
                return df_out, df_rel

        else:
            df_out = pd.DataFrame()
            df_rel = pd.DataFrame()
            return df_out, df_rel
