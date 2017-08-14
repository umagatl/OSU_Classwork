#CKY Parser (Chomsky normalized gram)
#Lance Umagat
#CS321
#Extra Credit Assignemtn
#Worked on with Chris Nguyen
#Biblography:
#		http://www.mit.edu/~6.863/fall2012/labs/nltk-ch9.pdf

import re

class CFGToCNF: #convert from CFG to CNF

	def __init__(self, path):
		self.lexical = {}  # unary branching gram prods
		self.phrasal = {}  # binary branching gram prods
		self.start = None  # start symbol
		gram_text = ""
		with open(path) as gram_file:
			gram_text = gram_file.read()
		
		gram_text = gram_text.strip()
		gram_lines = gram_text.split("\n")
		prod = r"""^(\w+)\s*->\s*(\w+\s+\w+|'\S+'|"\S+")$"""
		quoted = r"""^"\S+"|'\S+'$"""
		prod = re.compile(prod)
		QUOTED = re.compile(quoted)
		
		for line in gram_lines:
			prod_match = prod.match(line)
			if prod_match:
				lhs, rhs = prod_match.group(1), prod_match.group(2)
				
				if not self.start:
					self.start = lhs
				quot_match = QUOTED.match(rhs)
				
				if quot_match:
					prod_type = "lexical"
				else:
					prod_type = "phrasal"
					rhs = rhs.split()
				rule_map = getattr(self, prod_type)
				
				if lhs not in rule_map:
					rule_map[lhs] = []
				rule_map[lhs].append(rhs)


        
	def lhs_for_rhs(self, rhs, structure_type):
	
		nonterm = []
		prods = getattr(self, structure_type)
		
		for lhs in prods:
			if rhs in prods[lhs]:
				nonterm.append(lhs)
		return nonterm

class Node: #nodes placed into CKY parse
    
    def __init__(self, sym, left=None, right=None, term=None):
     
        self.sym = symbol
        self.left = left
        self.right = right
        self.term = terminal

    def __str__(self):
        
        return """({0} -> {1} {2})""".format(self.symbol, str(self.left), str(self.right))
		
class CKYParser:  #create a class for the CKY parser
	def __init__(self, path):
		self.gram = CFGToCNF(path)
		
	def parse(self, sentence):
	
		N = len(sentence)
		
		# initializes (N+1)x(N+1) matrix to None. Using only list multiplication yields array of pointers to same array.
		self.chart = [(N+1)*[None] for row_label in xrange(N+1)]
		
		for j in xrange(1, N+1):
			token = sentence[j-1]
			self.chart[j-1][j] = map(lambda preterm: Node(preterm, terminal=token), self.gram.lhs_for_rhs(token, "lexical"))
			
			for i in reversed(xrange(0, j-1)):  # j-2..0
				for k in xrange(i+1, j):        # i+1..j-1
					lcandidates = self.chart[i][k]  # candidate left branches
					rcandidates = self.chart[k][j]  # candidate right branches
					
					# look at all node pairs from two split cells check if node pair is on RHS of any production and add node to appropriate cell
					if lcandidates and rcandidates:
						for lnode in lcandidates:
							lsymbol = lnode.symbol
							for rnode in rcandidates:
								rsymbol = rnode.symbol
								# all mother node symbols that can dominate current pair of node symbols
								msymbols = self.gram.lhs_for_rhs([lsymbol, rsymbol], "phrasal")
								if msymbols and self.chart[i][j] is None:
									self.chart[i][j] = []
								for msymbol in msymbols:
									self.chart[i][j].append(Node(msymbol, lnode, rnode))
									
	def parse_to_string(self, node):
		if node.left and node.right:
			inside = "{0} {1}".format(self.parse_to_string(node.left), self.parse_to_string(node.right))
		else:
			inside = "{0}".format(node.terminal)
		return "({0} {1})".format(node.symbol, inside)
	
	def get_parses(self, sentence):
		self.parse(sentence)
		N = len(sentence)
		parse_roots = self.chart[0][N]
		result = []
		
		if parse_roots: # guard against None
			for root in parse_roots:
				if root.symbol == self.gram.start:
					result.append(self.parse_to_string(root))
		return result