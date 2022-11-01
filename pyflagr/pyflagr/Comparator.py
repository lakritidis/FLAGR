import pandas as pd
import matplotlib.pyplot as plt


# Comparator class to plot/export the values of multiple performance evaluation metrics
class Comparator:
    aggregators = None
    results = None
    ev_pts = 0

    def __init__(self, evaluation_points):
        self.aggregators = []
        self.results = None
        self.ev_pts = evaluation_points

    # Add an aggregator to the local list of aggregators
    def add_aggregator(self, name, obj):
        self.aggregators.append((name, obj))

    # Sequentially invoke the aggregate method of each aggregator.
    # Collect the evaluation results in the local Dataframe named self.results.
    def aggregate(self, input_file="", input_df=None, rels_file="", rels_df=None):
        if len(input_file) == 0 and input_df is None:
            print("Error! You must provide an input file with the preference lists to be aggregated.")
            return

        if len(rels_file) == 0 and rels_df is None:
            print("Error! You must provide a file with the relevance judgements of the list elements.")
            return

        self.results = pd.DataFrame()
        for ram in self.aggregators:
            print("Running", ram[0], "...")

            df_out, df_eval = ram[1].aggregate(input_file, input_df, rels_file, rels_df)

            df_eval['Method'] = ram[0]

            self.results = pd.concat([self.results, df_eval])

        self.results = self.results.rename(columns={'map': 'Mean Average Precision (MAP)'})

    # Create a bar plot for MAP
    def plot_average_precision(self, dimensions, show_grid, query='all'):
        df_new = self.results.loc[lambda df: df['q'] == query]

        df_new.plot.bar(figsize=dimensions, grid=show_grid, x='Method',
                        y='Mean Average Precision (MAP)', fontsize=14, position=1)

        plt.xlabel('', fontsize=14)
        plt.ylabel('Mean Average Precision', fontsize=14)
        plt.legend(prop={'size': 14})
        plt.show()

    # Create a plot for a specific metric at a given cutoff point. The cutoff point must be lower than self.ev_pts
    def plot_metric(self, cutoff, metric, plot_type='bar', dimensions=(10.24, 7.68), show_grid=True, query='all'):
        assert cutoff <= self.ev_pts, 'Cannot plot ' + metric + ' at cutoff point ' + str(cutoff) + \
                                      '. The length of the list is ' + self.ev_pts + '.'

        df_new = self.results.loc[lambda df: df['q'] == query]
        df_new = self.get_df_slice(cutoff, metric, df_new)

        if plot_type == 'bar':
            df_new.plot(kind=plot_type, fontsize=14, width=0.8, figsize=dimensions, grid=show_grid)
        else:
            df_new.plot(kind=plot_type, fontsize=14, figsize=dimensions, grid=show_grid)

        plt.xlabel('List cutoff point', fontsize=14)
        plt.ylabel(metric, fontsize=14)
        plt.legend(bbox_to_anchor=(1.0, 1.05), prop={'size': 14})
        plt.show()

    # Slice the evaluation dataframe by specifying rows (query) and columns (metric)
    def get_df_slice(self, cutoff, metric, df):
        left_columns = 5
        if metric == 'map':
            start_col = 4
            end_col = 5
        elif metric == 'precision':
            start_col = left_columns
            end_col = start_col + cutoff
        elif metric == 'recall':
            start_col = left_columns + self.ev_pts
            end_col = start_col + cutoff
        elif metric == 'dcg':
            start_col = left_columns + 2 * self.ev_pts
            end_col = start_col + cutoff
        elif metric == 'ndcg':
            start_col = left_columns + 3 * self.ev_pts
            end_col = start_col + cutoff
        else:
            print(metric, ": Not supported metric. Please use one of the following:\n")
            print("\tprecision\n\trecall\n\tdcg\n\tndcg\n")
            return

        df_ret = df.iloc[:, start_col:end_col].T
        df_ret.columns = df.iloc[:, -1]
        return df_ret

    # Slice the evaluation dataframe by specifying rows (query) and columns (metric)
    def get_results(self, cutoff, metric='all', query='all'):
        df_ret = self.results.loc[lambda df: df['q'] == query]

        if metric != 'all':
            df_x = pd.DataFrame()

            for m in metric:
                df_slice = self.get_df_slice(cutoff, m, df_ret)
                df_x = pd.concat([df_x, df_slice], axis=0)

            df_ret = df_x.T

        return df_ret

    # Convert the evaluation results to a LaTeX tabular
    def convert_to_latex(self, cutoff, metric='all', query='all', dec_pts=6):
        # return self.get_results(cutoff, metric, query).style.format(precision=dec_pts).to_latex()
        return self.get_results(cutoff, metric, query).round(dec_pts).to_latex()

