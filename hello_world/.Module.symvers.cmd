cmd_/home/daniel/Documents/DRVO/hello_world/Module.symvers := sed 's/ko$$/o/' /home/daniel/Documents/DRVO/hello_world/modules.order | scripts/mod/modpost -m -a   -o /home/daniel/Documents/DRVO/hello_world/Module.symvers -e -i Module.symvers   -T -