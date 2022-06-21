###############################################################################################################
# Required Python modules and libraries
import CombSUM

if __name__ == '__main__':
    lists = '/media/leo/B65266EC5266B1331/phd_Research/08 - Datasets/TREC/Synthetic/MOSO.csv'
    qrels = '/media/leo/B65266EC5266B1331/phd_Research/08 - Datasets/TREC/Synthetic/MOSO_qrels.csv'
 
    csum = CombSUM.CombSUM()
    '''
    df_out, df_eval = csum.aggregate(input_file=lists, rels_file=qrels)
    print(df_eval)
    '''