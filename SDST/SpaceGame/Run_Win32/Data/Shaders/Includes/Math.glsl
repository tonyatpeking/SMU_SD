#define RangeMap(val, inStart, inEnd, outStart, outEnd) \
(((val) - (inStart)) / ((inEnd) - (inStart)) * ((outEnd) - (outStart)) + (outStart))


#define RangeMapTo01(val, inStart, inEnd) \
(((val) - (inStart)) / ((inEnd) - (inStart)))


#define RangeMapFrom01(val, outStart, outEnd) \
(((val) * ((outEnd) - (outStart)) + (outStart))

