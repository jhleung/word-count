awk '{filename = sprintf("text%d.txt", NR); print >filename; close(filename)}' test.txt

