for((i=1;i<50;i++));
do
    echo -n "testcases/test$i.tig: "
    ./a.out "../testcases/test$i.tig"
done
echo -n "testcases/merge.tig: "
./a.out "../testcases/merge.tig"
echo -n "testcases/queens.tig: "
./a.out "../testcases/queens.tig"
