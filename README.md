# AllerStat: Finding Statistically Significant Allergen-Specific Patterns in Protein Sequences by Machine Learning

Given a dataset of amino acid sequences (i.e., proteins) accompanied by their allergenicity information (allergenic or not) and biological categories (e.g., species or genus), this program computes *allergen-specific patterns* (ASPs), that is, sub-sequences that are specific to allergenic proteins.

See the file `data/supData1_full.csv` for the dataset, and the file `data/supData4.xlsx` for extracted ASPs.

# Technical overview

The identification of ASPs are based on *FastWY* [1]. Note that, since the original FastWY is developed for finding patterns in sets, we extended it for patterns in sequences.

FastWY extracts patterns having smaller p-values of *Fisher exact test* (FET) with the multiple testing correction, than a predefined threshold $\alpha$. (In this code we use $\alpha=0.05$ and $\alpha=0.01$.) Given a pattern, suppose that we write a contingency table between the allergenicity (allergenic or not) and the existence of the pattern for all sequences in the dataset. Then FET computes the p-value of the independence of them. Since we need to compute FET for a huge number of all possible patterns in this setup, both the computational cost and the multiple testing correction become problematic. FastWY provides a smart algorithm for it.

After patterns are extracted by FastWY, first we keep only patterns specific to allergenic proteins, since we employed the two-tailed FET and therefore it also extracts patterns specific to non-allergenic proteins. Then patterns meeting both of the following conditions are identified as ASPs.
-   The pattern must not be observed in any of the non-allergenic proteins.
    -   We consider that an ASP should be a cause of allergic reaction.
-   The pattern must be observed either (1) in proteins from two or more biological categories, or (2) in proteins from one biological category having both allergenic and non-allergenic proteins in the dataset.
    -   We need to extract patterns that are not category-specific but allergen-specific.

[1]: Aika Terada *et al.*, "[Fast Westfall-Young permutation procedure for combinatorial regulation discovery](https://ieeexplore.ieee.org/abstract/document/6732479)". IEEE BIBM 2013.

# Requirements

-   g++ (tested with GCC 9.3.0 on Ubuntu)
-   make
-   Anaconda (tested with 2021.05 version)

# Procedure

1.  Run `make` in the top directory of the project. This will produce the file named `train` (FastWY executable).
2.  Launch Jupyter notebook with the installed Anaconda.
    ```
    $ jupyter-notebook

    ### In another shell
    ```
3.  Open `analysis/01_makeTrainTest.ipynb` in Jupyter notebook and run "Kernel" -> "Restart & Run All". This will produce the script named `analysis/02_run_fastwy.sh`.
4.  Let the script be executable.
    ```
    $ cd analysis
    $ chmod u+x 02_run_fastwy.sh
    ```
5.  Run the script **in "analysis" directory**.
    ```
    $ ./02_run_fastwy.sh

    ### run in "analysis" directory!
    ### This takes fairly long time
    ```
    **This takes fairly long time: about a half day is needed in author's environment**.  
    This will produce files in the directory `analysis/food_with_mtec_order_Jan2021/all/output/` named `result_all_food_with_mtec_order_Jan2021_C1GT1L1800R10k.csv` and `result_all_food_with_mtec_order_Jan2021_C1Ga001T1L1800R10k.csv`. ("a001" denotes the FWER significance level is 0.01; the other is for 0.05.)
6.  Open `analysis/03_numseq_to_aa.ipynb` in Jupyter notebook and run "Kernel" -> "Restart & Run All". This will produce files in the directory `analysis/food_with_mtec_order_Jan2021/all/output/` named `aa_result_all_food_with_mtec_order_Jan2021_C1GT1L1800R10k.csv` and `aa_result_all_food_with_mtec_order_Jan2021_C1Ga001T1L1800R10k.csv`.
7.  Open `analysis/04_output_fig4e_supData4.ipynb` in Jupyter notebook and run "Kernel" -> "Restart & Run All". This will produce files in the directory `analysis/output_ipynb/04_output_fig4e_supData4` named
    -   `supData4_allergen_ps4_a001_Jan2021.xlsx`,
    -   `supData4_allergen_ps4_a005_Jan2021.xlsx`,
    -   `figure4e_hist_pattern_all_ps4_001_category_num_Jan2021.pdf`, and
    -   `figure4e_hist_pattern_all_ps4_005_category_num_Jan2021.pdf`.
    The first two .xlsx files will contain the same content as the file `data/supData4.xlsx`

# Copyright notice

Authors: Kento Goto (1), Norimasa Tamehiro (2), Takumi Yoshida (1), Hiroyuki Hanada (3), Takuto Sakuma (1), Reiko Adachi (2), Kazunari Kondo (2), Ichiro Takeuchi (1,3)

1.  Nagoya Institute of Technology
2.  National Institute of Health Sciences
3.  RIKEN

These files are released under MIT LICENSE. The document file of the license is `LICENSE.txt`.
