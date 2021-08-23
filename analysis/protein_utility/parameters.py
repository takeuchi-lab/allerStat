import sys


class Parameters():
    """
    Class for management parameters and name of file

    ***** parameter *****
    FOOD_NAIVE
    FOOD_ORDER
    FOOD_ONLY
    NF_NA_ADD
    MTEC
    MTEC_ORDER
    FOOD_WITH_MTEC_NAIVE
    FOOD_WITH_MTEC_ORDER
    ALLERTOP: AllerTopに関するファイルの操作に使用. (aa_train, aa_test, def_allertop_model)
    allertop_data_min_length: allertopの前処理でカットするデータの最小長 最低がデフォの10 11以上ならallertop_tailに追記される
    MEME: MEMEに関するファイルの操作に使用. ()
    SIM: 類似度を使用する場合
    op_tail: FastWYオプションを区別する末尾
    cv_by
    penalty: 正則化("l1" or "l2")
    disp:

    実験条件を追加する場合はhogehoge
    """

    def __init__(self, FOOD_NAIVE=False, FOOD_ORDER=False, FOOD_ONLY=False, NF_NA_ADD=False, MTEC=False, MTEC_ORDER=False, FOOD_WITH_MTEC_NAIVE=False, FOOD_WITH_MTEC_ORDER=False, FOOD_ONLY_WITH_MTEC_ORDER=False,
                 ALLERTOP=False, allertop_lag_max=-1, allertop_data_min_length=10, MEME=False, SIM=False, std=False, std_by_train=False, normalize=False, normalize_by_train=False, add_length=False, op_tail="", cv_by="", penalty="", disp=True,
                 meme_op="", meme_concat_op="", meme_pattern_mode=False, ALLERDICTOR=False, allerdictor_min_sup=-1, sep2020 = False, Jan2021 = False,
                 tame2020 = True):
        # 210622 Hanada
        # tame2020というパラメータが消えたため追加したが、どう使われているのかよく理解できていない
        # もしかすると、このクラス内では使っていなくて外部から参照されるためだけ…？
        
        self.FOOD_NAIVE = FOOD_NAIVE
        self.FOOD_ORDER = FOOD_ORDER
        self.FOOD_ONLY = FOOD_ONLY
        self.NF_NA_ADD = NF_NA_ADD
        self.MTEC = MTEC
        self.MTEC_ORDER = MTEC_ORDER
        self.FOOD_WITH_MTEC_NAIVE = FOOD_WITH_MTEC_NAIVE
        self.FOOD_WITH_MTEC_ORDER = FOOD_WITH_MTEC_ORDER
        self.FOOD_ONLY_WITH_MTEC_ORDER = FOOD_ONLY_WITH_MTEC_ORDER
        self.ALLERTOP = ALLERTOP
        self.allertop_lag_max = allertop_lag_max
        self.allertop_data_min_length = allertop_data_min_length
        self.MEME = MEME
        self.SIM = SIM
        self.std = std
        self.std_by_train = std_by_train
        self.normalize = normalize
        self.normalize_by_train = normalize_by_train
        self.op_tail = op_tail
        self.cv_by = cv_by
        self.penalty = penalty
        self.disp = disp
        self.meme_op = meme_op
        self.meme_concat_op = meme_concat_op
        self.meme_pattern_mode = meme_pattern_mode
        self.ALLERDICTOR = ALLERDICTOR
        self.allerdictor_min_sup = allerdictor_min_sup
        self.sep2020 = sep2020
        self.Jan2021 = Jan2021
        self.tame2020 = tame2020

        self.add_length = add_length

        self.TAB = 15  # 各メンバ関数でprintする時の右寄せの幅

        self.load()
        self.print_all()

    def load(self):
        '''
        init内, 変数更新時に内部変数を更新する
        '''
        # 全Falseか複数TrueはError
        chklist = [self.FOOD_NAIVE, self.FOOD_ORDER, self.FOOD_ONLY, self.NF_NA_ADD, self.MTEC,
                   self.MTEC_ORDER, self.FOOD_WITH_MTEC_NAIVE, self.FOOD_WITH_MTEC_ORDER, self.FOOD_ONLY_WITH_MTEC_ORDER]
        count = sum(x for x in chklist)
        if count == 0:
            print("[Error] All parameters are False.")
            sys.exit()
        elif count >= 2:
            print("[Error] Multi paramaeters are setted True.")
            sys.exit()

        # Feature scalingについても同様
        if self.SIM:
            chklist = [self.std, self.std_by_train,
                       self.normalize, self.normalize_by_train]
            count = sum(x for x in chklist)
            if count >= 2:
                print(
                    "[Error] Multi paramaeters for feature scaling are setted True.")
                sys.exit()

        if self.FOOD_NAIVE:
            self.topdir = "food_naive"
        elif self.FOOD_ORDER:
            self.topdir = "food_order"
        elif self.FOOD_ONLY:
            self.topdir = "food_only"
        elif self.NF_NA_ADD:
            self.topdir = "nf_na_add"
        elif self.MTEC:
            self.topdir = "mtec"
        elif self.MTEC_ORDER:
            self.topdir = "mtec_order"
        elif self.FOOD_WITH_MTEC_NAIVE:
            self.topdir = "food_with_mtec_naive"
        elif self.FOOD_WITH_MTEC_ORDER:
            self.topdir = "food_with_mtec_order"
        elif self.FOOD_ONLY_WITH_MTEC_ORDER:
            self.topdir = "food_only_with_mtec_order"

        if self.Jan2021:
            self.topdir += "_Jan2021"
        if self.sep2020:
            self.topdir += "_sep2020"

        # ファイル名命名規則
        self.fname_tail = "_" + self.topdir
        self.penalty_tail = ""
        self.sim_tail = ""
        self.meme_tail = ""
        self.add_length_tail = ""
        self.allertop_tail = ""
        self.allerdictor_tail = ""

        if self.penalty != "":
            self.penalty_tail = "_" + self.penalty
        if self.SIM:
            self.sim_tail = "_sim"
            if self.std:
                self.sim_tail = "_sim_std"
            if self.std_by_train:
                self.sim_tail = "_sim_std_by_train"
            if self.normalize:
                self.sim_tail = "_sim_norm"
            if self.normalize_by_train:
                self.sim_tail = "_sim_norm_by_train"
        if self.MEME:
            self.meme_tail = "_meme_{}".format(self.meme_op)
            if self.meme_concat_op != "":
                self.meme_tail += self.meme_concat_op
            if self.meme_pattern_mode:
                self.meme_tail += "_pattern_mode"
        if self.add_length:
            self.add_length_tail = "_add_len"
        if self.ALLERDICTOR:
            self.allerdictor_tail = "_allerdictor"
            if self.allerdictor_min_sup >= 0:
                self.allerdictor_tail = "_allerdictor_{}".format(
                    self.allerdictor_min_sup)

        # AllerTop時にファイル名に追加
        # aa_trainとaa_testにのみ適用される
        if self.ALLERTOP:
            self.allertop_tail = "_allertop"
            if self.allertop_data_min_length > 10:
                self.allertop_tail = "_allertop_data_min_length_{}".format(self.allertop_data_min_length)

        # 確認
        # self.print_all()
        print("Parameters loaded.")

    def print_all(self):
        print("{0:<20}: {1}".format("FOOD_NAIVE", self.FOOD_NAIVE))
        print("{0:<20}: {1}".format("FOOD_ORDER", self.FOOD_ORDER))
        print("{0:<20}: {1}".format("FOOD_ONLY", self.FOOD_ONLY))
        print("{0:<20}: {1}".format("NF_NA_ADD", self.NF_NA_ADD))
        print("{0:<20}: {1}".format("MTEC", self.MTEC))
        print("{0:<20}: {1}".format("MTEC_ORDER", self.MTEC_ORDER))
        print("{0:<20}: {1}".format(
            "FOOD_WITH_MTEC_NAIVE", self.FOOD_WITH_MTEC_NAIVE))
        print("{0:<20}: {1}".format(
            "FOOD_WITH_MTEC_ORDER", self.FOOD_WITH_MTEC_ORDER))
        print("{0:<20}: {1}".format(
            "FOOD_ONLY_WITH_MTEC_ORDER", self.FOOD_ONLY_WITH_MTEC_ORDER))
        print()
        print("{0:<20}: {1}".format("topdir", self.topdir))
        print("{0:<20}: {1}".format("fname_tail", self.fname_tail))
        print()

        # あんまよくない
        if self.op_tail == "":
            self.op_tail = "[op_tail]"
        if self.cv_by == "":
            self.cv_by = "[cv_by]"

        # self.def_aa_train(category="${category}", disp=True)
        # self.def_aa_test(category="${category}", disp=True)
        # self.def_numseq_train(category="${category}", disp=True)
        # self.def_order(category="${category}", disp=True)
        # self.def_result(category="${category}", disp=True)
        # self.def_output(category="${category}", disp=True)
        # self.def_Xlabel_train(category="${category}", disp=True)
        # self.def_Xlabel_test(category="${category}", disp=True)
        # self.def_clf(category="${category}", disp=True)
        # self.def_score_dict(disp=True)
        # あんまよくないのおわり

        if self.op_tail == "[op_tail]":
            self.op_tail = ""
        if self.cv_by == "[cv_by]":
            self.cv_by = ""

    def op_check(self):
        if self.op_tail == "":
            print("[Error] op_tail is not defined.")
            sys.exit()

    def cv_check(self):
        if self.cv_by == "":
            print("[Error] cv_by is not defined.")
            sys.exit()

    def def_aa(self, category, train_test, disp=True):
        '''
        アミノ酸配列データのcsvファイル名を返す
        '''
        fname = "{0}/{1}/data/aa_{1}{2}{3}_{4}.csv".format(
            self.topdir, category.lower(), self.fname_tail, self.allertop_tail, train_test)
        if disp and self.disp:
            print("aa_{}".format(train_test).ljust(self.TAB), ":", fname)
        return fname

    def def_numseq_train(self, category, disp=True):
        '''
        FastWYの出力(数字系列状態)のファイル名を返す
        (FastWYの実行はtrainデータに対してのみ)
        '''
        fname = "{0}/{1}/data/numseq_{1}{2}_train.csv".format(
            self.topdir, category.lower(), self.fname_tail)
        if disp and self.disp:
            print("numseq_train".ljust(self.TAB), ":", fname)
        return fname

    def def_order(self, category, disp=True):
        fname = "{0}/{1}/data/order_{1}{2}_train.csv".format(
            self.topdir, category.lower(), self.fname_tail)
        if disp and self.disp:
            print("order".ljust(self.TAB), ":", fname)
        return fname

    def def_result_all(self, category, disp=True):
        self.op_check
        fname = "{0}/{1}/output/resultAll_{1}{2}_{3}.csv".format(
            self.topdir, category.lower(), self.fname_tail, self.op_tail)
        if disp and self.disp:
            print("result".ljust(self.TAB), ":", fname)
        return fname

    def def_result(self, category, disp=True):
        self.op_check()
        fname = "{0}/{1}/output/result_{1}{2}_{3}.csv".format(
            self.topdir, category.lower(), self.fname_tail, self.op_tail)
        if disp and self.disp:
            print("result".ljust(self.TAB), ":", fname)
        return fname

    def def_output(self, category, disp=True):
        self.op_check()
        fname = "{0}/{1}/output/output_{1}{2}_{3}.out".format(
            self.topdir, category.lower(), self.fname_tail, self.op_tail)
        if disp and self.disp:
            print("output".ljust(self.TAB), ":", fname)
        return fname

    def def_aa_result(self, category, disp=True):
        self.op_check()
        fname = "{0}/{1}/output/aa_result_{1}{2}_{3}.csv".format(
            self.topdir, category.lower(), self.fname_tail, self.op_tail)
        if disp and self.disp:
            print("aa_result".ljust(self.TAB), ":", fname)
        return fname

    # std_old: 旧形式のstdファイルを呼び出す用 self.stdに統一して回し直した後削除する.
    def def_X(self, category, train_test, disp=True, std_old=False, std_by_train_for_create=False, normalize_by_train_for_create=False, mix_tail=""):
        self.op_check()
        std_tail = ""
        if std_old:
            std_tail = "_std"
        if mix_tail != "":
            mix_tail = "_" + mix_tail

        # create時のみ使用
        if std_by_train_for_create:
            print("set std_by_train True")
            self.std_by_train = True
            self.load()
        elif normalize_by_train_for_create:
            print("set normalize_by_train True")
            self.normalize_by_train = True
            self.load()

        fname = "{0}/{1}/pickle/X_{1}{2}_{3}{4}{5}{6}{7}{8}{9}_{10}.pickle".format(
            self.topdir, category.lower(), self.fname_tail, self.op_tail, self.sim_tail, self.meme_tail, std_tail, mix_tail, self.allerdictor_tail, self.add_length_tail, train_test)
        if disp and self.disp:
            print("X_{}".format(train_test).ljust(self.TAB), ":", fname)

        if std_by_train_for_create:
            print("set std_by_train False")
            self.std_by_train = False
            self.load()
        elif normalize_by_train_for_create:
            print("set normalize_by_train False")
            self.normalize_by_train = False
            self.load()
        return fname

    def def_label(self, category, train_test, disp=True, mix_tail=""):
        self.op_check()
        if mix_tail != "":
            mix_tail = "_" + mix_tail
        fname = "{0}/{1}/pickle/label_{1}{2}_{3}{4}{5}{6}{7}{8}_{9}.pickle".format(
            self.topdir, category.lower(), self.fname_tail, self.op_tail, self.sim_tail, self.meme_tail, mix_tail, self.allerdictor_tail, self.add_length_tail, train_test)
        if disp and self.disp:
            print("label_{}".format(train_test).ljust(self.TAB), ":", fname)
        return fname

    def def_clf(self, category, disp=True, mix_tail=""):
        self.op_check()
        self.cv_check()
        if mix_tail != "":
            mix_tail = "_" + mix_tail
        fname = "{0}/{1}/pickle/clf_{1}{2}_{3}{4}{5}{6}{7}{8}_CVby{9}{10}.pickle".format(
            self.topdir, category.lower(), self.fname_tail, self.op_tail, self.sim_tail, self.meme_tail, mix_tail, self.allerdictor_tail, self.add_length_tail, self.cv_by, self.penalty_tail)
        if disp and self.disp:
            print("clf".ljust(self.TAB), ":", fname)
        return fname

    def def_allertop_model(self, category, tail="[default]", disp=True):
        if tail == "[default]":
            tail = self.topdir
        fname = "{0}/{1}/pickle/allertop_model_lag_max_{2}_{3}{4}.pickle".format(
            self.topdir, category.lower(), self.allertop_lag_max, tail, self.penalty_tail)
        if disp and self.disp:
            print("allertop_model".ljust(self.TAB), ":", fname)
        return fname

    def def_meme_fasta(self, category, disp=True, aller=False, nonaller=False):
        if aller:
            aller_tail = "_aller"
        elif nonaller:
            aller_tail = "_nonaller"
        else:
            aller_tail = ""
        fname = "../data/fasta/meme/{0}/fasta_meme_{1}_{0}{2}.fasta".format(
            self.topdir, category.lower(), aller_tail)
        if disp and self.disp:
            print("meme_fasta".ljust(self.TAB), ":", fname)
        return fname

    def def_allerdictor_set_six_mer_sup(self, category, train_test, disp=True):
        fname = "{0}/{1}/pickle/allerdictor_set_six_mer_sup_{2}.pickle".format(
            self.topdir, category.lower(), train_test)
        if disp and self.disp:
            print("allerdictor_set_six_mer_sup".ljust(self.TAB), ":", fname)
        return fname


if __name__ == "__main__":
    pass
