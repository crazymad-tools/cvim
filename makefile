PROJECT=cvim
OBJECT=main.o Draw.o
SRC_CC=main.cc Draw.cc

$(PROJECT) : $(OBJECT)
	g++ -o $(PROJECT) $(OBJECT) -L/usr/lib64/crazy -lcrazy_net
$(OBJECT) : $(SRC_CC)
	g++ -c $(XX) $(SRC_CC) -I./

.PHONY : clean
clean :
	-rm $(PROJECT) $(OBJECT)

