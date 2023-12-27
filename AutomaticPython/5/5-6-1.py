#!/usr/bin/env python
"""
In this chapter, we used the dictionary value {'1h': 'bking', '6c': 'wqueen', 
'2g': 'bbishop', '5h': 'bqueen', '3e': 'wking'} to represent a chess board. 
Write a function named isValidChessBoard() that takes a dictionary argu
ment and returns True or False depending on if the board is valid.
 A valid board will have exactly one black king and exactly one white 
king. Each player can only have at most 16 pieces, at most 8 pawns, and 
all pieces must be on a valid space from '1a' to '8h'; that is, a piece can’t 
be on space '9z'. The piece names begin with either a 'w' or 'b' to repre
sent white or black, followed by 'pawn', 'knight', 'bishop', 'rook', 'queen', or 
'king'. This function should detect when a bug has resulted in an improper 
chess board.
"""
board= {'1h': 'bking', '6c': 'wqueen', '2g': 'bbishop', '5h': 'bqueen', '3e': 'wking'} 

board2= {'1h': 'bking', '9c': 'wqueen', '2g': 'bbishop', '5h': 'bqueen', '3e': 'wking'} 

board3= {'1h': 'bking', '6c': 'cqueen', '2g': 'bbishop', '5h': 'bqueen', '3e': 'wking'} 

board4= {'1h': 'bking', '6c': 'bqueen', '2g': 'bbishop', '5h': 'bqueen', '3e': 'wking'} 

Board=[board,board2,board3,board4]

colors=['w','b']
pieces=['king','queen','knight','bishop','rook','pawn']
all_pieces=set(color+piece for piece in pieces for color in colors)

rows=['1','2','3','4','5','6','7','8']
columns=['a','b','c','d','e','f','g','h']
all_num=set(row+column for row in rows for column in columns)

vaild_counts={'king': (1, 1),
              'queen': (0, 1),
              'rook': (0, 2),
              'bishop': (0, 2),
              'knight': (0, 2),
              'pawn': (0, 8)}

def isValidChessBoard(board):

    counts={}
    if (len(board))>32:
        return False

    for v in board.values():
        if v not in all_pieces:
            return False
        counts.setdefault(v,0)
        counts[v]+=1

    for v in board.keys():
        if v not in all_num:
            return False

    for v in all_pieces:

        count_piece=counts.get(v,0)
        low,high=vaild_counts[v[1:]] #把v当作字符串处理，去除第一个代表颜色的字符
        
        if not low<=count_piece<=high:
            return False
    return True

for i in range(len(Board)):
    if isValidChessBoard(Board[i])==True:
        print("good")
    else:
        print("not so good")
