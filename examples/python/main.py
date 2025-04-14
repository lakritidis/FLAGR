# This example project demonstrates the PyFLAGR's tools for testing and comparing rank aggregation methods.  We present
# the Comparator class and its member methods. This class allows the researchers to perform head-to-head comparisons of
# the performance of various rank aggregation algorithms.

# Import the PyFLAGR modules for rank aggregation
import pyflagr.Linear as Linear
import pyflagr.Majoritarian as Majoritarian
import pyflagr.MarkovChains as MarkovChains
import pyflagr.RRA as RRA
import pyflagr.Weighted as Weighted
import pyflagr.Comparator as Comparator

# Import the Pandas Dataframes package
import pandas as pd
pd.set_option('display.max_columns', None)


# This example project utilizes  a sample dataset  that includes two files:  testdata.csv and  testdata_qrels.csv.  The
# former contains preference lists that were submitted by  50 voters for  20 queries. Each preference list contains  30
# elements. Therefore, the number of rows in this file is equal to  50 X 20 X 30 = 30000.  The columns of this CSV file
# must be organized in the following manner:
#                         Query, Voter Name, Item Code, Item Score, Algorithm/Dataset
# On the other hand,  testdata_qrels.csv contains  relevance judgments for the preference list  elements of the primary
# input file for each query. It is organized in the following fashion:
#                                      Query, 0, Item Code, Relevance
# Please refer to this article for more details: http://flagr.site/docs/38/input-and-output-files

# The input data file with the input lists to be aggregated.
lists = '../testdata/testdata.csv'

# The input data file with the relevance judgements.
qrels = '../testdata/testdata_qrels.csv'


# The constructor receives a single argument that represents the cutoff  point  (termed evaluation points) at which the
# evaluation metrics will be computed. In the following example,  we determine that the cutoff point is 10,  therefore,
# the following metrics will be computed:
#   *  Mean Average Precision (MAP),
#   *  Precision at points 1,2,...10 (namely, P@1, P@2, ..., P@N),
#   *  Recall at points 1,2,...10 (namely, R@1, R@2, ..., R@N),
#   *  Discounted Cumulative Gain, DCG  (namely, D@1, D@2, ..., D@N),, and
#   *  normalized Discounted Cumulative Gain, nDCG (namely, N@1, N@2, ..., N@N),
EV_PTS = 10
cmp = Comparator.Comparator(EV_PTS)


# The Comparator class contains an aggregators array that stores the rank aggregation methods to be compared. Each rank
# aggregation method  is handled by a particular object that is passed to Comparator, through the add_aggregator member
# function.  Please notice  that the eval_pts parameter that is  passed to each class constructor  must be equal to the
# aforementioned value of the cutoff parameter. Here we pass 20 rank aggregation methods to Comparator. This means that
# all these methods will be included in the forthcoming evaluation tests.
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
cmp.add_aggregator("Agglomerative", Weighted.Agglomerative(c1=0.1, c2=0.2, eval_pts=EV_PTS))
cmp.add_aggregator("DIBRA", Weighted.DIBRA(aggregator='combsum:borda', gamma=1.2, prune=None, w_norm='minmax',
                                           eval_pts=EV_PTS))
cmp.add_aggregator("DIBRA-Prune", Weighted.DIBRA(aggregator='combsum:borda', gamma=1.2, prune='low', w_norm='minmax',
                                                 d1=0.3, d2=0.05, eval_pts=EV_PTS))
cmp.add_aggregator("DIBRA-Prune-WIRE", Weighted.DIBRA(aggregator='combsum:borda', gamma=1.2, prune='wire', w_norm='minmax',
                                                 num_buckets=5, d1=0.5, d2=1.0, eval_pts=EV_PTS))


# After the  Comparator  class  has  been  initialized  and  the  algorithms  to be evaluated have been determined, the
# aggregate()  member function must be called  to start the evaluation process.  The aggregate() method accepts the two
# following arguments:
#   *   The input data file that contains the preference lists to be aggregated (in our case testdata.csv), and
#   *   The Rels file that contains relevance judgments for the elements of the preference lists (in our case
#       testdata_qrels.csv). This is the ground truth file that will be used to evaluate the generated aggregate lists.
cmp.aggregate(input_file=lists, rels_file=qrels)


# The  evaluation results are stored in the results Dataframe of the Comparator.  Each row in this Dataframe represents
# the performance of an algorithm for a particular query. There is also one additional row per method, that records its
# average performance for all queries.  In this particular example, we have 20 rank aggregation methods and 20 queries,
# consequently, the results Dataframe contains 20 x (20 + 1) = 420 rows. The number of columns in the results Dataframe
# depends on the value of  the cutoff  point that we set during the  construction of  Comparator.  In this example this
# value is 10 so the Dataframe has 46 columns:
#  *  1 column that represents the query string,
#  *  1 column that represents the length (i.e. the number of elements in the) aggregate list,
#  *  1 column for the total number of relevant elements for this query,
#  *  1 column for the number of relevant elements included in the aggregate list,
#  *  1 column for the Average Precision of a method for this query,
#  *  10 columns for Precision (at 10 points in the aggregate list),
#  *  10 columns for Recall (at 10 points in the aggregate list),
#  *  10 columns for DCG (at 10 points in the aggregate list),
#  *  10 columns for nDCG (at 10 points in the aggregate list),
#  *  1 column for the method's name
# The get_results()  method can be used to easily retrieve slices  of the results Dataframe.  In the following example,
# we get the mean values of MAP, Precision and nDCG for all queries, and for each method.
df = cmp.get_results(cutoff=5, metric=["map", "precision", "ndcg"], query='all')
df.style.format(precision=4)
print(df)

# Create a bar plot of the Mean Average Precision of all methods.
cmp.plot_average_precision((16, 7), True, query='all')

# The plot_metric() method produces plots of the values of a particular evaluation metric at specific cutoff points of
# the aggregate list. The argument `metric` determines the metric to be plotted; it may receive one of these values:
# `precision`, `recall`, `dcg`, and `ndcg`.
cmp.plot_metric(EV_PTS, metric='precision', plot_type='bar', dimensions=(16, 8), show_grid=True, query='all')
