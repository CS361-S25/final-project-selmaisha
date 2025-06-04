from math import log
import pandas as pd
import numpy as np
from matplotlib import pyplot as plt

#datafile = pd.read_csv("num100000pop10rew10size80age1000.csv")
datafile = pd.read_csv("worlddata.csv")

#raw_pheno =  [100000,6335,11,0,629,0,0,733,0,0,1042,5461]
#set raw_pheno to the last row of the datafile
raw_pheno = datafile.iloc[-1].tolist()

def shannon_index(phenotypes):
    """
    Calculate the Shannon index for a list of phenotypes.
    
    Parameters:
    phenotypes (list): A list of counts of different phenotypes.
    
    Returns:
    float: The Shannon index.
    """
    total = phenotypes[1]
    if total == 0:
        return 0.0
    
    shannon = 0.0
    for count in phenotypes[2:-1]:
        if count > 0:
            proportion = count / total
            shannon -= proportion * log(proportion)
    
    return shannon

print("Shannon index:", shannon_index(raw_pheno))

#create a plot of shannon index of each row in the datafile
shannon_indices = datafile.apply(shannon_index, axis=1)
plt.figure(figsize=(10, 6))
plt.plot(shannon_indices, marker='o', linestyle='-', color='b')
plt.title('Shannon Index Over Time')
plt.xlabel('Update (x10000)')
plt.ylabel('Shannon Index')
plt.grid()
plt.savefig('shannon_index_plot.png')

#plot org over time, and then the sum of rows 3 through 11 and plot over time
plt.figure(figsize=(10, 6))
plt.plot(datafile['org'], marker='o', linestyle='-', color='g', label='Org')
plt.plot(datafile.iloc[:, 2:11].sum(axis=1), marker='x', linestyle='--', color='r', label='Sum of Phenotypes')
plt.title('Organism and Phenotype Sum Over Time')
plt.xlabel('Update (x10000)')
plt.ylabel('Count')
plt.legend()
plt.grid()
plt.savefig('org_and_phenotype_sum_plot.png')