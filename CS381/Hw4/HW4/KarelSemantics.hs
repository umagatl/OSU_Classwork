--Lance Umagat (Umagatl)
--Chris Nguyen (Nguyenc5)
--Benny Zhao (Zhaobe)

module KarelSemantics where

import Prelude hiding (Either(..))
import Data.Function (fix)

import KarelSyntax
import KarelState

-- | Valuation function for Test.
test :: Test -> World -> Robot -> Bool
test (Not t) w r     = not(test t w  r)
test (Facing ca) w r = ca == getFacing r 
test (Clear d) w r   = isClear (relativePos d r) w 
test (Beeper) w r    = hasBeeper (getPos r) w  
test (Empty) w r     = isEmpty r


-- | Valuation function for Stmt.
stmt :: Stmt -> Defs -> World -> Robot -> Result
stmt Shutdown   _ _ r          = Done r
stmt Move       _ w r          = let p = relativePos Front r 
                                    in if isClear p w
                                      then OK w (setPos p r)
                                      else Error ("Blocked at: " ++ show p)
stmt PickBeeper  _ w r         = let p = getPos r
                                    in if hasBeeper p w
                                      then OK (decBeeper p w) (incBag r)
                                      else Error ("No beeper to pick at: " ++ show p)
stmt PutBeeper _ w r           = let p = getPos r
                                    in if isEmpty r
                                      then Error ("No beeper to put.")
                                      else OK (incBeeper p w )(decBag r)
stmt (Turn d) _ w r            = let ca = getFacing r in
                                      OK w (setFacing (cardTurn d ca)  r)
stmt (Call m) de w r           = case lookup m de of 
                                      (Just b) -> stmt b de w r 
                                      _ -> Error ("Undefined macro: " ++ m)
stmt (Iterate 0 eval) de w r   = OK w r
stmt (Iterate num eval) de w r = if num /= 0 
                                    then case stmt eval de w r of
                                            (OK w' r') -> stmt (Iterate (num - 1) eval) de w' r'
                                            (Done r') -> Done r'
                                            (Error m) -> Error m
                                    else Error ("Fail in the loop, but why? =P")
stmt (If tr pa fa) de w r      = if test tr w r
                                    then stmt pa de w r
                                    else stmt fa de w r
stmt (While c pa) de w r       = if test c w r 
                                    then case stmt pa de w r of
                                        (OK w' r') -> stmt (While c pa) de w' r'
                                        (Done r') -> Done r'
                                        (Error m) -> Error m
                                    else OK w r
stmt (Block []) de w r         = OK w r
stmt (Block (x:xs)) de w r     = case stmt x de w r of
                                    (OK w' r') -> stmt (Block xs) de w' r'
                                    otherwise -> otherwise

-- | Run a Karel program.
prog :: Prog -> World -> Robot -> Result
prog (m,s) w r                 = stmt s m w r
