# FPGA-based-Battery-SOC-estimator-and-Controller

Designed an FPGA (XILINX SPARTAN-3E) based constant current charging and discharging controller using Buck and Boost converters. This controller is used to pre-estimate the OCV vs SOC curve and other parameters using HPPC pulse test. Using terminal voltage, current flow to/from the battery and the pre-estimated curves, State of charge is estimated using Extended Kalman Filtering algorithm. Finally, the battery bank should be integrated with the microgrid.
