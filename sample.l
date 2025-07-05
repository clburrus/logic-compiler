D = 8
A = 16

#                              1 1   1 1 1 1
#    0 1 2 3    4 5 6 7    8 9 0 1   2 3 4 5
d0 = 1 x x x    x x x x    x x x x   x x x x    # buffer d0 = a0
d1 = x 0 x x    x x x x    x x x x   x x x x	# inverter d1 = !a1
d2 = x x 1 x    x x x x    x x x x   x x x x    #
d2 = x x x 1    x x x x    x x x x   x x x x    # d2 = a2 | a3
d3 = x x x x    1 1 x x    x x x x   x x x x    # d3 = a4 & a5
d4 = x x x x    x x 1 0    x x x x   x x x x    #
d4 = x x x x    x x 0 1    x x x x   x x x x    # d4 = a6 ^ a7(XOR)
d5 = x x x x    x x x x    0 0 1 x   x x x x    #
d6 = x x x x    x x x x    0 1 0 x   x x x x    # LUT
d7 = x x x x    x x x x    0 1 1 x   x x x x    #

# Other examples
d0 ~1 1 1 1    1 1 1 1    1 1 1 1   1 1 1 0    # d0 = ~(0x8000)
d1 = 1 0 x x    x x x x    x x x x   x x x x    # d1 = a0 & ~a1(a1 gates a0)

#  ...in this case...
#  d1 | a0  a1  a2  a3
#  ------------------ -
#  1 | 1   0   0   0
#  0 | 0   1   0   0
#  0 | 1   1   0   0
#  0 | 0   0   1   0
#  1 | 1   0   1   0
#  0 | 0   1   1   0
#  0 | 1   1   1   0
#  0 | 0   0   0   1
#  1 | 1   0   0   1

#  address[18..0]
#  data[7..0]

assign /rd 	a0
assign /wr 	a1
assign A18 	a18
assign A17 	a17
assign A16 	a16
assign A15			a15
assign /wait 	d5
assign RomSel	d1


d1 = A18
