# ADC Notes for K715 Pro

In the stock firmware, the following operations are performed:

1. ADC1 reads the ADC V<sub>REFINT</sub> on Channel 17.
2. 

Possible ADC pins are:

| Pin name  | Function      | Observed ADC Behavior (10-bit resolution) |
|-----------|---------------|-------------------|
| PC0       | ADC123_IN10   | ADC reads between 913-938 and 547-590 |
| PC1       | ADC123_IN11   | ADC initially reads zero, after a while reads ~850 or ~990 (10-bit resolution) |
| PC2       | ADC123_IN12   | ADC always reads zero |
| PC3       | ADC123_IN13   | ADC reads ~300 or ~138 |
