#!/usr/bin/awk -f

function red(s) {
    printf "\033[1;31m" s "\033[0m\n"
}

function green(s) {
    printf "\033[1;32m" s "\033[0m\n"
}

function blue(s) {
    printf "\033[1;34m" s "\033[0m\n"
}

{
	yidx = NF/6*5 + 1;
	if (FNR % (BS+1) == 1) {
		if (FNR == 1) {
			print $0;
			for (i = yidx; i <= NF; ++i)
				_[i-yidx] = $i;
		} else {
			cnt = 0;
			for (i = yidx; i <= NF; ++i)
				cnt += $i <= _[i-yidx];
			if (cnt == NF - yidx + 1)
				green($0);
			else
				red($0);
		}
	} else
		blue($0);
}
