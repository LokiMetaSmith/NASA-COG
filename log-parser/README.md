json_log_parser.py is a simple scraper that reads malformed JSON data and performs a series of statistical operations on the data to display the graphs of different properties of the data. 

Usage:

Server access to raw files is nessisary for viewing, in the script there is an array of file locations that can be used to load one or more logs for analysis. They are read in order, so time series data should be preserved if read in the correct order. 

Currently the standard deviation for Temperature, Voltage, Current, and Resistance(as a function of Voltage and Current) are calculated
Other statistical parameters can be added easily

Currently six graphs are produced, Temperature vs Time (unitless), Voltage and current vs Time (unitless), Resistance vs Time (Unix Standard ), Change in Resistance Vs Change in Temperature (this doesn't appear useful at this time), Delta Resistance vs average temperature 