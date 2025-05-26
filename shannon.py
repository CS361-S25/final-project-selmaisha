from math import log

raw_pheno =  [40000, 6348, 15, 0, 615, 0, 0, 764, 0, 0, 1052]
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
    for count in phenotypes[2:]:
        if count > 0:
            proportion = count / total
            shannon -= proportion * log(proportion)
    
    return shannon

print("Shannon index:", shannon_index(raw_pheno))