# .c / .h files (clang-format)
find . -iname *.h -o -iname *.c | xargs clang-format -i

# .py files (black)
black .
