-- Lance Umagat (umagatl)
-- Chris Nguyen (nguyenc5)
-- Justin Bibler(biblerj)

module MiniLogo.Umagatl where

-- Imports and hiding num from prelude
import Prelude hiding (Num)
import Data.List

-- Type declarations
type Num = Int
type Var = String
type Macro = String
type Prog = [Cmd]

-- Grammar
data Mode = Down
		  | Up
	 deriving(Eq,Show)
	 
data Expr = VarExpr Var
		  | NumExpr Num
		  | Add Expr Expr
	 deriving(Eq,Show)
	 
data Cmd = Pen Mode
		 | Move Expr Expr
		 | Define Macro [Var] Prog
		 | Call Macro [Expr]
	 deriving(Eq,Show)
	 
-- Main Task stuff
line :: Cmd
line = Define "line"
			  ["x1", "y1", "x2", "y2"]
			  [Pen Up,
			   Move (VarExpr "x1") (VarExpr "y1"),
			   Pen Down,
			   Move (VarExpr "x2") (VarExpr "y2"),
			   Pen Up]
			   
nix :: Cmd
nix = Define "nix"
			 ["x", "y", "w", "h"]
			 [Call "line" [VarExpr "x",
						   VarExpr "y",
						   Add (VarExpr "x") (VarExpr "w"),
						   Add (VarExpr "y") (VarExpr "h")],
			  Call "line" [Add (VarExpr "x") (VarExpr "w"),
						   VarExpr "y",
						   VarExpr "x",
						   Add (VarExpr "y") (VarExpr "h")]]
						   
steps :: Int -> Prog
steps n = Pen Down : stepsHelper n

stepsHelper :: Int -> Prog
stepsHelper 0 = []
stepsHelper n = stepsHelper (n - 1) ++ [Move (NumExpr (n - 1)) (NumExpr (n - 1)),
                                        Move (NumExpr (n - 1)) (NumExpr n),
                                        Move (NumExpr n) (NumExpr n)]
										
macros :: Prog -> [Macro]
macros p = [getDefineName c | c <- p, isDefine c]

isDefine :: Cmd -> Bool
isDefine (Define _ _ _) = True
isDefine _ 				= False

getDefineName :: Cmd -> Macro
getDefineName (Define name _ _) = name
getDefineName _ 				= "Error"

-- "Pretty" stuff
pretty :: Prog -> String
pretty p = concat (map prettyCmd p)

prettyCmd :: Cmd -> String
prettyCmd (Pen m)                  = "pen " ++ (prettyMode m) ++ ";\n"
prettyCmd (Move x y)               = "move (" ++ (prettyExpr x) ++ ", " ++ (prettyExpr y) ++ ");\n"
prettyCmd (Define macro args cmds) = "define " ++ macro ++ " " ++ (prettyArgs args) ++ " {\n" ++ (pretty cmds) ++ "};\n"
prettyCmd (Call macro args)        = "call " ++ macro ++ " " ++ (prettyExprs args) ++ ";\n"

prettyMode :: Mode -> String
prettyMode Up   = "up"
prettyMode Down = "down"

prettyExprs :: [Expr] -> String
prettyExprs exprs = "(" ++ concat (intersperse ", " (map prettyExpr exprs) ) ++ ")"

prettyExpr :: Expr -> String
prettyExpr (VarExpr v) = v
prettyExpr (NumExpr n) = show n
prettyExpr (Add a b)   = (prettyExpr a) ++ " + " ++ (prettyExpr b)

prettyArgs :: [Var] -> String
prettyArgs vs = "(" ++ (concat (intersperse ", " vs)) ++ ")"

-- Bonus stuff
optE :: Expr -> Expr
optE (Add (NumExpr a) (NumExpr b)) = NumExpr (a + b)
optE (Add a b)                 = case (optE a, optE b) of
                                   ((NumExpr aa), (NumExpr bb)) -> NumExpr (aa + bb)
                                   (aa, bb)                 -> Add aa bb
optE x                         = x

optP :: Prog -> Prog
optP p = map pHelper p

pHelper :: Cmd -> Cmd
pHelper (Move x y)     = Move (optE x) (optE y)
pHelper (Call m exprs) = Call m [optE expr | expr <- exprs]
pHelper c              = c