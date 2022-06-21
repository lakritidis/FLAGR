import ctypes
import os
import random
import string
import tempfile
from sys import platform

import pandas as pd


# Check the input file or DataFrame that contains the input lists
def check_get_input(input_file, input_df):
    status = 0
    infile = ""
    if len(input_file) > 0:
        infile = input_file
        if not os.path.isfile(infile):
            print("Error! Input file does not exist")
            status = -1

    elif input_df is not None:
        infile = "A dataframe is required"

    else:
        print("Error! No input data was passed")
        status = -1

    return status, infile


# Check the input file or DataFrame that contains the Relevant elements for each query
def check_get_rels_input(rels_file, rels_df):
    if len(rels_file) > 0:
        relfile = rels_file
    elif rels_df is not None:
        relfile = "A rels dataframe is required"
    else:
        relfile = ""

    return relfile


# A random bytes generator - to be used in filenames passed to FLAGR
def get_random_string(length):
    letters = string.ascii_lowercase
    result_str = ''.join(random.choice(letters) for _ in range(length))
    return result_str


# Retrieve the output from a PYFLAGR rank aggregation method. This one reads the output CSV files and returns
# two DataFrames. The first one contains the aggregate lists for each query; the second one stores the results
# of the evaluation (provided that a qrels file or DataFrame has been set).
def get_output(od, ran):
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


# This is the default directory where the output files are written. It is the preset temp directory of the OS.
out_dir = tempfile.gettempdir()

PyFLAGR_Shared_Lib = None

#path = os.path.dirname(os.path.realpath(__file__))

# Import the FLAGR shared library in PyFLAGR
if platform == "linux" or platform == "linux2":
    PyFLAGR_Shared_Lib = ctypes.CDLL(os.path.dirname(os.path.realpath(__file__)) + "/flagr.so")

elif platform == "win32":
    PyFLAGR_Shared_Lib = ctypes.CDLL(os.getcwd() + "/flagr.dll")

elif platform == "darwin":
    PyFLAGR_Shared_Lib = ctypes.CDLL(os.getcwd() + "/flagr.dylib")

PyFLAGR_Shared_Lib.Comb.argtypes = [
    ctypes.c_char_p,  # Input data file with the lists to be aggregated
    ctypes.c_char_p,  # Input data file with the relevant elements per query - used for evaluation
    ctypes.c_int,  # Number of evaluation points
    ctypes.c_int,  # Rank/Score normalization method (Rank Aggregation Method)
    ctypes.c_char_p,  # Random string to be embedded into the output file names
    ctypes.c_char_p]  # The directory where the output files will be written
