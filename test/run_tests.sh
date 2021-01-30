#!/bin/sh

# このシェルスクリプトは引数に指定されたテストコマンドを全て実行する
# 引数付きのテストコマンドに対応している

ECHO='echo -e'
case `$ECHO` in
    -e)
        ECHO='echo'
esac

res=0
for i in "$@"
do
    head=`echo $i | cut -d' ' -f1`
    if [ ! -d $head ]
    then
		./$i > ./$head.log
		if [ $? -eq 0 ]
        then
			$ECHO "\033[32mPASS\033[m $head"
        else
            res=1
			$ECHO "\033[31mFAIL\033[m $head For more information, see $head.log"
		fi
    fi
done

exit $res
