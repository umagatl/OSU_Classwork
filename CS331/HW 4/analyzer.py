from math import log
import numpy as np
import string

class Vocab:
        def __init__(self):
                self.query = set([])
                self.training_data = open('trainingSet.txt', 'r')
                self.testing_data = open('testSet.txt', 'r')
        def build(self):
                self.bagStuffer(self.training_data)
                self.bagStuffer(self.testing_data)
                self.training_data.close()
                self.testing_data.close()
                return sorted(self.query)

        def bagStuffer(self, file):
                for line in file:
                        for word in line.translate(None, string.punctuation).lower().split()[:-1]:
                                if word not in self.query:
                                        self.query.add(word)

class classifier:
    def __init__(self, query, filename):
        file = open(filename, 'r')
        self.query = query
        (self.features, self.classlabels) = self.initArray(file)

    def __getitem__(self, pos):
        return (self.features[pos], self.classlabels[pos])

    def fileWrite(self, filename):
        file = open(filename, 'w')
        file.write(','.join(self.query) + ',classlabel\n')
        for pos, entry in enumerate(self.features):
            entry_list = entry.tolist()
            entry_list.append(self.classlabels[pos])
            file.write(','.join(map(str, entry_list)))
            file.write('\n')
        file.close()

    def initArray(self, file):
        (features, classlabels) = self.initList(file)
        return (np.array(features, dtype=int), np.array(classlabels,
            dtype=int))

    def initList(self, file):
        (features, classlabels) = ([], [])
        for line in file:
            feature_list = [0] * len(self.query)
            split_line = line.translate(None, string.punctuation).lower().split()
            for word in split_line[:-1]:
                feature_list[self.query.index(word)] = 1
            features.append(feature_list)
            classlabels.append(split_line[-1])
        return (features, classlabels)

class conProbTable:
    def __init__(self, training_data):
        self.training = training_data
        self.query = training_data.query
        self.classlabelProbs = {}
        self.params_classlabeled = {}
        self.params_unknown = {}

    def get_prob(self, classlabel, idx=None):
        if idx is None:
            return self.classlabelProbs[classlabel]
        else:
            word = self.query[idx]
            if classlabel:
                return self.params_classlabeled[word]
            else:
                return self.params_unknown[word]

    def train(self):
        self.calcClassLabel()
        self.calcConFeat()

    def calcClassLabel(self):
        prob = (np.sum(self.training.classlabels) + 1) / float(
                (len(self.training.classlabels) + 2))
        self.classlabelProbs[1] = prob

        prob = (len(self.training.classlabels) -
                np.sum(self.training.classlabels) + 1) / float(
                        (len(self.training.classlabels) + 2))
        self.classlabelProbs[0] = prob

    def calcConFeat(self):
        trueCount = len(self.training.classlabels[self.training.classlabels == 1])
        falseCount = len(self.training.classlabels[self.training.classlabels == 0])
        classLabeledFeats = self.training.features[self.training.classlabels == 1]
        unlabeledFeats = self.training.features[self.training.classlabels == 0]
        queryLen = len(self.query)

        for pos, word in enumerate(self.query):
            trueProbClasslabel = (len(classLabeledFeats[
                classLabeledFeats[:,pos] == 1]) + 1) / float((trueCount + queryLen))
            trueProbNoLabel = (len(unlabeledFeats[
                unlabeledFeats[:,pos] == 1]) + 1) / float(
                        (falseCount + queryLen))

            self.params_classlabeled[word] = trueProbClasslabel
            self.params_unknown[word] = trueProbNoLabel

class NaiveBayes:
    def __init__(self, training_data):
        self.para = conProbTable(training_data)

    def train(self):
        self.para.train()

    def test(self, testing_data):
        predict = []
        for vec_feat in testing_data.features:
            predict.append(self.predictMove(vec_feat))

        accuracy = (len(predict) - sum(abs(predict -
            testing_data.classlabels))) / float(len(predict))

        return ''.join([str(round(accuracy, 4) * 100), '%'])

    def predictMove(self, vec_feat):
        probs = []
        probs.append(self.probCalc(vec_feat, 0))
        probs.append(self.probCalc(vec_feat, 1))
        return probs.index(max(probs))

    def probCalc(self, vec_feat, classlabel):
        sum = log(self.para.get_prob(classlabel))
        for idx, val in enumerate(vec_feat):
            if val:
                sum += log(self.para.get_prob(classlabel, idx))
        return sum
		
def display(trainingAcc, testingAcc):
    results = open('results.txt', 'w')
    results.write('RESULTS\n')
    results.write('#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n\n')
    results.write('Training done on: trainingSet.txt\n')
    results.write('Testing done on: trainingSet.txt\n')
    results.write(''.join(['Accuracy percentage: ', trainingAcc, '\n']))
    results.write('\n#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#\n\n')
    results.write('Training done on: trainingSet.txt\n')
    results.write('Testing done on: testSet.txt\n')
    results.write(''.join(['Accuracy percentage: ', testingAcc, '\n']))
    results.write('\n#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#')
    results.close()

if __name__ == '__main__':
        v = Vocab()
        query = v.build()

        training_data = classifier(query, 'trainingSet.txt')
        testing_data = classifier(query, 'testSet.txt')
        training_data.fileWrite('TrainingData.txt')
        testing_data.fileWrite('TestingData.txt')

        naiveBayes = NaiveBayes(training_data)
        naiveBayes.train()
        trainingAcc = naiveBayes.test(training_data)
        testingAcc = naiveBayes.test(testing_data)

        print ' '.join(['Training Complete with Accuracy:', trainingAcc])
        print ' '.join(['Testing Complete with Accuracy:', testingAcc])

        display(trainingAcc, testingAcc)
