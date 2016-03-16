hadoop job -list | cut -f1 -d\	  | tail -n +3 | xargs --verbose --replace=jid hadoop job -kill jid
