import os
import sys
import subprocess
import openpyxl
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path
from IPython.display import Image
from IPython.display import display
from sklearn.metrics import classification_report
from openpyxl.workbook import Workbook
import datasets as ds
import random_forests as rf
import persistence as tree_io
import visualization as tree_view

source_path = os.path.abspath('/home/matheus/Desktop/Approximated Born Again Tree Ensembles/src')
output_path = os.path.abspath('/home/matheus/Desktop/Approximated Born Again Tree Ensembles/src/output')
sys.path.append(source_path)

Datasets = ["COMPAS-ProPublica","FICO","HTRU2"]
df = {' ': [], 'Train-Acc': [], 'Train-F1': [], 'Test-Acc': [], 'Test-F1': []}

def add_report(df, method, train_stat, test_stat):
    df[' '].append(method)
    df['Train-Acc'].append(train_stat['accuracy'])
    df['Train-F1'].append(train_stat['weighted avg']['f1-score'])
    df['Test-Acc'].append(test_stat['accuracy'])
    df['Test-F1'].append(test_stat['weighted avg']['f1-score'])

# Loading data
def run_test():
    for current_dataset in Datasets:
        for current_fold in range (1,11):

            df_train, df_test, ds_infos = ds.load(source_path, current_dataset, current_fold)
            X_train, y_train = df_train.iloc[:, :-1].values, df_train.iloc[:, -1].values
            X_test, y_test = df_test.iloc[:, :-1].values, df_test.iloc[:, -1].values

            for v in range(0, 3, 2):

                fileName = current_dataset + '.BA' + str(current_fold) + '.O5.T10.V' + str(v)
                results_dir = os.path.join(output_path, 'trees10', current_dataset,'Results_' + current_dataset)
                curr_dir = os.path.join(output_path, 'trees10', current_dataset, fileName)
                source_dir = Path(curr_dir)
                files = source_dir.iterdir()
                files = source_dir.glob('*.tree')
                resultFile = os.path.join(results_dir,fileName + '.results.txt')
                f = open(resultFile, 'w')

                for file in files:
                    str_file = str(file)
                    pos = str_file.find('.D')
                    pos2 = str_file.find('.tree')
                    depth = str_file[pos+2:pos2]
                    born_againO5 = tree_io.classifier_from_file(str_file, X_train, y_train, pruning=False)

                    # BornAgainNew
                    banew_test_pred = born_againO5.predict(X_test)
                    banew_train_pred = born_againO5.predict(X_train)
                    report_banew = classification_report(y_test, banew_test_pred, output_dict=True)

                    test_acc = report_banew['accuracy']
                    test_F1 = report_banew['weighted avg']['f1-score']

                    f.write(depth)
                    f.write(" " + str(test_acc))
                    f.write(" " + str(test_F1))
                    f.write("\n")

                f.close()
                print(fileName)
                tree_view.plotStatistics(resultFile)


def run_average():

    for v in range(0, 3, 2):
        for current_dataset in Datasets:

            avgAccRF = avgAccBA = avgAccBANew = avgF1RF = avgF1BA = avgF1BANew = 0.0;

            for current_fold in range(1, 11):
                df_train, df_test, ds_infos = ds.load(source_path, current_dataset, current_fold)
                X_train, y_train = df_train.iloc[:, :-1].values, df_train.iloc[:, -1].values
                X_test, y_test = df_test.iloc[:, :-1].values, df_test.iloc[:, -1].values
                fileName = current_dataset + '.RF' + str(current_fold) + ".txt"
                random_forest_file = os.path.join(source_path, 'resources', 'forests', current_dataset, fileName)
                random_forest = tree_io.classifier_from_file(random_forest_file, X_train, y_train, pruning=False)
                fileName = current_dataset + '.BA' + str(current_fold) + '.O0.T10.tree'
                born_again_O0_file = os.path.join(output_path, 'trees10', current_dataset, fileName)
                born_againO0 = tree_io.classifier_from_file(born_again_O0_file, X_train, y_train, pruning=False)
                # print(born_again_O0_file)
                fileName = current_dataset + '.BA' + str(current_fold) + '.O5.T10.V' + str(v) + '.tree'
                born_again_O5_file = os.path.join(output_path, 'trees10', current_dataset, 'ExactDepth', fileName)
                print(born_again_O5_file)
                born_againO5 = tree_io.classifier_from_file(born_again_O5_file, X_train, y_train, pruning=False)

                # RandomForest
                rf_test_pred = random_forest.predict(X_test)
                rf_train_pred = random_forest.predict(X_train)
                report_rf = classification_report(y_test, rf_test_pred, output_dict=True)
                report_rf_train = classification_report(y_train, rf_train_pred, output_dict=True)

                # BornAgain
                ba_test_pred = born_againO0.predict(X_test)
                ba_train_pred = born_againO0.predict(X_train)
                report_ba = classification_report(y_test, ba_test_pred, output_dict=True)
                report_ba_train = classification_report(y_train, ba_train_pred, output_dict=True)

                # BornAgainNew
                banew_test_pred = born_againO5.predict(X_test)
                banew_train_pred = born_againO5.predict(X_train)
                report_banew = classification_report(y_test, banew_test_pred, output_dict=True)
                report_banew_train = classification_report(y_train, banew_train_pred, output_dict=True)

                add_report(df, 'RandomForest', report_rf_train, report_rf)
                add_report(df, 'BornAgain', report_ba_train, report_ba)
                add_report(df, 'BornAgainNew', report_banew_train, report_banew)

                avgAccRF = avgAccRF + report_rf_train['accuracy']
                avgF1RF = avgF1RF + report_rf['weighted avg']['f1-score']
                avgAccBA = avgAccBA + report_ba_train['accuracy']
                avgF1BA = avgF1BA + report_ba['weighted avg']['f1-score']
                avgAccBANew = avgAccBANew + report_banew_train['accuracy']
                avgF1BANew = avgF1BANew + report_banew['weighted avg']['f1-score']

            print("Average RF Accuracy and F1 in " + current_dataset + " with value " + str(v) + " : " + str(
                avgAccRF / 10) + " " + str(avgF1RF / 10))
            print("Average BA Accuracy and F1 in " + current_dataset + " with value " + str(v) + " : " + str(
                avgAccBA / 10) + " " + str(avgF1BA / 10))
            print("Average BANew Accuracy and F1 in " + current_dataset + " with value " + str(v) + " : " + str(
                avgAccBANew / 10) + " " + str(avgF1BANew / 10))

        a = pd.DataFrame(data=df, index=None)
        path = output_path + '/ResultsV' + str(v) + '.xlsx'
        a.to_excel(path)

run_test()
run_average()