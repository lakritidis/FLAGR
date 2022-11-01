###############################################################################################################
# Required Python modules and libraries
import os.path
import sys

# Comment the following line to execute the code from the installed library. Otherwise, Python executes the local files.
sys.path.insert(1, os.path.dirname(sys.path[0]))

import Linear
import Majoritarian
import MarkovChains
# import Kemeny
import RRA
import Weighted
import Comparator

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
    # method = RRA.RRA(eval_pts=20, exact=False)
    # method = RRA.RRA(eval_pts=20, exact=True)
    # method = Weighted.PreferenceRelationsGraph(eval_pts=20)
    # method = Weighted.Agglomerative(eval_pts=20)
    # method = Weighted.DIBRA(eval_pts=20, aggregator="combmnz:simple-borda")
    # method = Weighted.DIBRA(eval_pts=20, gamma=1.5, prune=True, d1=0.4, d2=0.1)
    # method = Weighted.DIBRA(eval_pts=10, aggregator="condorcet", w_norm="minmax",
    # dist="cosine", prune=False, gamma=1.5, d1=0.4, d2=0.1, max_iter=50)
    # method = MarkovChains.MC1(eval_pts=20, ergodic_number=0.15)
    # method = MarkovChains.MC2(eval_pts=20, ergodic_number=0.15)
    # method = MarkovChains.MC3(eval_pts=20, ergodic_number=0.15)
    # method = MarkovChains.MC4(eval_pts=20, ergodic_number=0.15)
    # method = MarkovChains.MCT(eval_pts=20, ergodic_number=0.15)

    # df_out, df_eval = method.aggregate(input_file=lists, rels_file=qrels, out_dir='/home/leo/Documents')
    # print(df_eval)

    EV_PTS = 10

    cmp = Comparator.Comparator(EV_PTS)
    cmp.add_aggregator("CombSUM-Rank", Linear.CombSUM(norm='rank', eval_pts=EV_PTS))
    cmp.add_aggregator("CombSUM-Borda", Linear.CombSUM(norm='borda', eval_pts=EV_PTS))
    cmp.add_aggregator("CombSUM-Score", Linear.CombSUM(norm='score', eval_pts=EV_PTS))
    cmp.add_aggregator("CombMNZ-Rank", Linear.CombMNZ(norm='rank', eval_pts=EV_PTS))
    cmp.add_aggregator("CombMNZ-Borda", Linear.CombMNZ(norm='borda', eval_pts=EV_PTS))
    cmp.add_aggregator("CombMNZ-Score", Linear.CombMNZ(norm='score', eval_pts=EV_PTS))
    cmp.add_aggregator("Condorcet", Majoritarian.CondorcetWinners(eval_pts=EV_PTS))
    cmp.add_aggregator("Copeland", Majoritarian.CopelandWinners(eval_pts=EV_PTS))
    cmp.add_aggregator("Outranking Approach", Majoritarian.OutrankingApproach(preference=0, veto=0.75, concordance=0,
                                                                              discordance=0.25, eval_pts=EV_PTS))
    cmp.add_aggregator("MC1", MarkovChains.MC1(max_iterations=50, ergodic_number=0.15, eval_pts=EV_PTS))
    cmp.add_aggregator("MC2", MarkovChains.MC2(max_iterations=50, ergodic_number=0.15, eval_pts=EV_PTS))
    cmp.add_aggregator("MC3", MarkovChains.MC3(max_iterations=50, ergodic_number=0.15, eval_pts=EV_PTS))
    cmp.add_aggregator("MC4", MarkovChains.MC4(max_iterations=50, ergodic_number=0.15, eval_pts=EV_PTS))
    cmp.add_aggregator("MCT", MarkovChains.MCT(max_iterations=50, ergodic_number=0.15, eval_pts=EV_PTS))
    cmp.add_aggregator("RRA-Exact", RRA.RRA(exact=True, eval_pts=EV_PTS))
    cmp.add_aggregator("RRA", RRA.RRA(exact=False, eval_pts=EV_PTS))
    cmp.add_aggregator("PrefRel", Weighted.PreferenceRelationsGraph(alpha=0.1, beta=0.5, eval_pts=EV_PTS))
    # cmp.add_aggregator("Agglomerative", Weighted.Agglomerative(c1=0.1, c2=0.2, eval_pts=EV_PTS))
    cmp.add_aggregator("DIBRA", Weighted.DIBRA(aggregator='combsum:borda', gamma=1.2, prune=False, w_norm='minmax',
                                               eval_pts=EV_PTS))
    cmp.add_aggregator("DIBRA-Prune", Weighted.DIBRA(aggregator='combsum:borda', gamma=1.2, prune=True, w_norm='minmax',
                                                     d1=0.3, d2=0.05, eval_pts=EV_PTS))

    cmp.aggregate(input_file=lists, rels_file=qrels)
    # cmp.plot_average_precision((10.24, 7.68), True, query='all')
    # cmp.plot_metric(EV_PTS, metric='ndcg', plot_type='bar', dimensions=(10.24, 7.68), show_grid=True, query='all')
    # print(cmp.get_results(cutoff=3, metric=["dcg", "ndcg"], query='Topic 1'))
    print(cmp.convert_to_latex(dec_pts=4, cutoff=EV_PTS, metric=["map", "precision", "ndcg"], query='all'))