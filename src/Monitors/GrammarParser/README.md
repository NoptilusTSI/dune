Grammar Parser
--------------------------

All required configuration files are located in etc/parser/ :
For each parser instance 3 files are required:
<name>.ini Contains the required dune config values
<name>.grammar Contains the grammar definition
<name>.quantization Contains the input quantization definition

An example  .ini is lauv-noptilus-1-parse.ini

to start the example Dune instance, first build it and then run it using
./dune -c lauv-noptilus-1-parse -p Simulation

It should create two .csv files in log/lauv-noptilus-1/
