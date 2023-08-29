client:
    gcc -o cl client.c -g -Wall -Wextra -Werror && ./cl

server:
    gcc -o sv server.c -g -Wall -Wextra && ./sv

clean:
    rm -f cl sv