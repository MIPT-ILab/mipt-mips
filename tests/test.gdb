set pagination off
handle SIGTRAP nostop noprint noignore
handle SIGBUS nostop noprint noignore
target sim --mars -f
load
run
quit
