###############################################################################################################
# Required Python modules and libraries
import Linear
import Majoritarian
import MarkovChains
import Kemeny
import RRA
import Weighted

from sys import platform

import pandas as pd

if __name__ == '__main__':
    base_path = ''
    if platform == "linux" or platform == "linux2":
        base_path = '/media/leo/B65266EC5266B133/phd_Research/08 - Datasets/TREC/Synthetic/'
    elif platform == "win32":
        base_path = 'D:/phd_Research/08 - Datasets/TREC/Synthetic/'
    else:
        exit(1)

    lists = base_path + 'MOSO.csv'
    qrels = base_path + 'MOSO_qrels.csv'

    input_dataframe = pd.read_csv(lists)
    rels_dataframe = pd.read_csv(qrels)

    # method = Linear.BordaCount(eval_pts=20)
    # method = Linear.SimpleBordaCount(eval_pts=20)
    # method = Linear.CombSUM(eval_pts=20, norm="rank")
    # method = Linear.CombSUM(eval_pts=20, norm="score")
    # method = Linear.CombMNZ(eval_pts=20, norm="borda")
    # method = Linear.CombMNZ(eval_pts=20, norm="simple-borda")
    # method = Majoritarian.CondorcetWinners(eval_pts=20)
    # method = Majoritarian.CopelandWinners(eval_pts=20)
    # method = Majoritarian.OutrankingApproach(eval_pts=20)
    # method = MarkovChains.MC4(eval_pts=20)
    # method = RRA.RRA(eval_pts=20)
    # method = RRA.RRA(eval_pts=20, exact=True)
    # method = Weighted.PreferenceRelationsGraph(eval_pts=20)
    # method = Weighted.Agglomerative(eval_pts=20)
    # method = Weighted.DIBRA(eval_pts=20)
    # method = Weighted.DIBRA(eval_pts=20, gamma=1.5, prune=True, d1=0.3, d2=0.05)
    # method = Weighted.DIBRA(eval_pts=20, aggregator="outrank")

    df_out, df_eval = method.aggregate(input_file=lists, rels_file=qrels, out_dir='/home/leo/Documents')

    print(df_eval)
