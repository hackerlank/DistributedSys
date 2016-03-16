#!/usr/bin/awk -f

{
	if ($14 && $15) {
		ctime = $7;
		if ($13 == "REDUCE") {
#			if ($14 < $7)
				stime = $14;
#			else
#				stime = $7;
			printf("%s\t%s:%s\t%s\tREDUCE\t%.3f\t%.3f\t%.3f\n",
			       $2, $1, $3, $12, ctime/1000.0, stime/1000.0, $15/1000.0)
		} else
			printf("%s\t%s:%s\t%s\tMAP\t%.3f\t%.3f\t%.3f\n",
			       $2, $1, $3, $12, ctime/1000.0, $14/1000.0, $15/1000.0)
	}
}
