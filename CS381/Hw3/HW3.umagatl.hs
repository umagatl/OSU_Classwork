-- Chris Nguyen, nguyenc5
-- Lance Umagat, umagatl

module HW3 where

import MiniMiniLogo
import Render


--
-- * Semantics of MiniMiniLogo
--

-- NOTE:
--   * MiniMiniLogo.hs defines the abstract syntax of MiniMiniLogo and some
--     functions for generating MiniMiniLogo programs. It contains the type
--     definitions for Mode, Cmd, and Prog.
--   * Render.hs contains code for rendering the output of a MiniMiniLogo
--     program in HTML5. It contains the types definitions for Point and Line.

-- | A type to represent the current state of the pen.
type State = (Mode,Point)

-- | The initial state of the pen.
start :: State
start = (Up,(0,0))

-- | A function that renders the image to HTML. Only works after you have
--   implemented `prog`. Applying `draw` to a MiniMiniLogo program will
--   produce an HTML file named MiniMiniLogo.html, which you can load in
--   your browswer to view the rendered image.
draw :: Prog -> IO ()
draw p = let (_,ls) = prog p start in toHTML ls


-- Semantic domains:
--   * Cmd:  State -> (State, Maybe Line)
--   * Prog: State -> (State, [Line])


-- | Semantic function for Cmd.
--
--   >>> cmd (Pen Down) (Up,(2,3))
--   ((Down,(2,3)),Nothing)
--
--   >>> cmd (Pen Up) (Down,(2,3))
--   ((Up,(2,3)),Nothing)
--
--   >>> cmd (Move 4 5) (Up,(2,3))
--   ((Up,(4,5)),Nothing)
--
--   >>> cmd (Move 4 5) (Down,(2,3))
--   ((Down,(4,5)),Just ((2,3),(4,5)))
--
cmd :: Cmd -> State -> (State, Maybe Line)
cmd (Pen m)    (_,xy) = ((m,xy), Nothing)
cmd (Move x y) (m,xy) = (
                         (m,(x, y)),
                         case m of Down -> Just (xy,(x,y))
                                   Up   -> Nothing
                        )


-- | Semantic function for Prog.
--
--   >>> prog (nix 10 10 5 7) start
--   ((Down,(15,10)),[((10,10),(15,17)),((10,17),(15,10))])
--
--   >>> prog (steps 2 0 0) start
--   ((Down,(2,2)),[((0,0),(0,1)),((0,1),(1,1)),((1,1),(1,2)),((1,2),(2,2))])
prog :: Prog -> State -> (State, [Line])
prog p s = foldl progHandler (s,[]) p

progHandler :: (State, [Line]) -> Cmd -> (State, [Line])
progHandler (s, ls) c = case (cmd c s) of (t, Nothing) -> (t, ls)
                                          (t, Just l)  -> (t, ls ++ [l])


--
-- * Extra credit
--

-- | This should be a MiniMiniLogo program that draws an amazing picture.
--   Add as many helper functions as you want.
amazing :: Prog
amazing = changePenPos (3, 23) ++
          [Move 0 23, Move 0 20, Move 3 20] ++
          changePenPos (7, 23) ++
          [Move 4 23, Move 4 22, Move 7 22, Move 7 20, Move 4 20] ++
          changePenPos (8, 23) ++
          [Move 11 23, Move 11 22, Move 8 22, Move 11 22, Move 11 20, Move 8 20] ++
          changePenPos (15, 22) ++
          [Move 15 23, Move 12 23, Move 12 22, Move 15 22, Move 15 20, Move 12 20, Move 12 22] ++
          changePenPos (16, 23) ++
          [Move 17 23, Move 17 20] ++
          changePenPos (16, 20) ++ 
          [Move 18 20]

changePenPos :: (Int, Int) -> Prog
changePenPos (x, y) = [Pen Up, Move x y, Pen Down]