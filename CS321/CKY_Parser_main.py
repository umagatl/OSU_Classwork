#CKY Parser (Chomsky normalized gram)
#Lance Umagat
#CS321
#Extra Credit Assignemtn
#Worked on with Chris Nguyen
#Biblography:
#		http://www.mit.edu/~6.863/fall2012/labs/nltk-ch9.pdf


if __name__ == "__main__":

	import nltk, sys
	from CKY_Parser import CKYParser
	from Output import print_parses

	try:
		sent_path = sys.argv[1]
	except IndexError:
		exit("Please give a path to a file of sents.")

	try:
		gram_path = sys.argv[2]
	except IndexError:
		exit("Please give a path to a file with a grammar.")

	with open(sent_path) as sentence_file:
		sent_data = sentence_file.read()

	sents = sent_data.strip().split("\n")
	sents = [nltk.wordpunct_tokenize(sent) for sent in sents]

	parser = CKYParser(gram_path)

	# iterate over tokenized sents, parse each and output parses
	for sent in sents:
		sent = ["'{0}'".format(token) for token in sent]	# make sure tokens are single-quoted
		parses = parser.get_parses(sent)
		#sys.stdout.write(print_parses(parses))
		print len(parses)