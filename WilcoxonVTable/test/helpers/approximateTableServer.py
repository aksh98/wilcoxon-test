def get_table(n):
  if n == 2:
    return [
            [[0, 2.0], [1, 0.0]],
            [[0, 1.6205459295127345], [1, 1.6205459295127345], [2, 0.0]],
            [[0, 1.214972347229377], [2, 0.9953536889798422], [3, 1.8971588149587177]]
           ]
  elif n == 5:
    return [
            [[0, 2.0], [1, 0.0]],
            [[0, 1.6205459295127345], [1, 1.6205459295127345], [2, 0.0]],
            [[0, 1.214972347229377], [2, 0.9953536889798422], [3, 1.8971588149587177], [4, 0.0]],
            [[0, 1.3986890367396447], [1, 0.8392134220437868], [2, 1.0893246187363834], [3, 0.9919926354466744], [4, 1.4303123229988501], [5, 1.0912170125097118], [6, 1.7658010945141505], [7, 0.0]],
            [[0, 1.2132467129102391], [1, 0.9436363322635194], [2, 1.1157981718762753], [3, 0.9643930650108938], [4, 1.195449259395275], [5, 0.9117122198612252], [6, 1.1893359382433348], [7, 1.0623199367707175], [8, 1.4628129542034223], [9, 1.0357044613176811], [10, 1.5089802432234714], [11, 0.0]],
            [[0, 1.0568054040801143], [2, 0.9675913465775252], [3, 1.1038772243975208], [4, 0.9812079061808248], [5, 1.1489012827253042], [6, 0.9545773894981214], [7, 1.1489574032249592], [8, 1.0020136466242562], [9, 1.241369995789273], [10, 0.9365634365634367], [11, 1.1955757956796036], [12, 1.0332013039827017], [13, 1.3602895566761923], [14, 0.9096572120534675], [15, 1.2361110561728421]]
           ]
  else:
    raise Exception("Invalid n")