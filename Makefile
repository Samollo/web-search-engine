NAME1= crawler
NAME2 = launcher

SRCS1 =  crawler.c
SRCS2 = launcher.c

OBJS1 = $(SRCS1:.c=.o)
OBJS2 = $(SRCS2:.c=.o)

INC =

DEPEND1 = -lcurl -lpython2.7

.PHONY:clean

all : $(NAME1) $(NAME2)


$(NAME1) : $(OBJS1)
	gcc $(INC) $(OBJS1) $(DEPEND1) -o $(NAME1)
	./crawler

$(NAME2) : $(OBJS2)
	gcc $(INC) $(OBJS2) -o $(NAME2)

%.o: %.c
	gcc $(INC) -c $^

clean :
	rm -f $(OBJS1)
	rm -f $(OBJS2)

fclean : clean
	rm -f $(NAME1)
	rm -f $(NAME2)

re : fclean all
