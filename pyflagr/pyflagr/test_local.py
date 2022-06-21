###############################################################################################################
# Required Python modules and libraries
import Comb
import Majoritarian
import MarkovChains
import Weighted

import pandas as pd

if __name__ == '__main__':
    lists = '/media/leo/B65266EC5266B1331/phd_Research/08 - Datasets/TREC/Synthetic/MOSO.csv'
    qrels = '/media/leo/B65266EC5266B1331/phd_Research/08 - Datasets/TREC/Synthetic/MOSO_qrels.csv'

    lists2 = '/media/leo/B65266EC5266B1331/phd_Research/08 - Datasets/TREC/Synthetic/FESO.csv'
    qrels2 = '/media/leo/B65266EC5266B1331/phd_Research/08 - Datasets/TREC/Synthetic/FESO_qrels.csv'

    input_dataframe = pd.read_csv(lists)
    rels_dataframe = pd.read_csv(qrels)

    # method = Comb.BordaCount(eval_pts=20)
    # method = Comb.CombSUM(eval_pts=20, norm="rank")
    # method = Comb.CombMNZ(eval_pts=20, norm="borda")
    # method = Majoritarian.CondorcetWinners(eval_pts=20)
    # method = Majoritarian.CopelandWinners(eval_pts=20)
    # method = Majoritarian.OutrankingApproach(eval_pts=20)
    # method = MarkovChains.MC4(eval_pts=20)
    # method = Weighted.PreferenceRelationsGraph(eval_pts=20)
    # method = Weighted.Agglomerative(eval_pts=20)
    # method = Weighted.DIBRA(eval_pts=20)
    method = Weighted.DIBRA(eval_pts=20, gamma=1.5, prune=True, d1=0.3, d2=0.05)
    # method = Weighted.DIBRA(eval_pts=20, aggregator="outrank")

    # df_out, df_eval = method.aggregate(input_file=lists, rels_file=qrels)
    df_out, df_eval = method.aggregate(input_df=input_dataframe, rels_df=rels_dataframe)
    print(df_eval)
