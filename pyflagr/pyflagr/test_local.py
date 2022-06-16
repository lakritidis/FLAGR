###############################################################################################################
# Required Python modules and libraries
import Comb
import Majoritarian
import MarkovChains
import Weighted

if __name__ == '__main__':
    lists = '/media/leo/B65266EC5266B1331/phd_Research/08 - Datasets/TREC/Synthetic/MOSO.csv'
    qrels = '/media/leo/B65266EC5266B1331/phd_Research/08 - Datasets/TREC/Synthetic/MOSO_qrels.csv'

    # method = Comb.BordaCount(eval_pts=20)
    # method = Comb.CombSUM(eval_pts=20, norm="rank")
    # method = Comb.CombMNZ(eval_pts=20, norm="borda")
    # method = Majoritarian.CondorcetWinners(eval_pts=20)
    # method = Majoritarian.OutrankingApproach(eval_pts=20)
    # method = MarkovChains.MC4(eval_pts=20)
    # method = Weighted.PreferenceRelationsGraph(eval_pts=20)
    # method = Weighted.Agglomerative(eval_pts=20)
    # method = Weighted.DIBRA(eval_pts=20)
    # method = Weighted.DIBRA(eval_pts=20, gamma=1.5, prune=True, d1=0.3, d2=0.05)
    method = Weighted.DIBRA(eval_pts=20, aggregator="outrank")

    df_out, df_eval = method.aggregate(input_file=lists, rels_file=qrels)
    print(df_eval)
