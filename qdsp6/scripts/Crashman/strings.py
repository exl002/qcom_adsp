import sys
import re

if (len(sys.argv) < 3):
    exit()

with open(sys.argv[1],"rb") as readfile:
    with open(sys.argv[2],"a") as writefile:
        writefile.write("B::Analysis\n")
        for line in readfile:
            #print line
            if "Q6_BUILD" in line:
                match = re.search(r'(?<=\x00)(QCOM time:Q6_BUILD[^\x00]+)\x00',line)
                if match:
                    print match.group(1)
                    writefile.write(match.group(1)+"\n")
                match = re.search(r'(?<=\x00)(ENGG time:Q6_BUILD[^\x00]+)\x00',line)
                if match:
                    print match.group(1)
                    writefile.write(match.group(1)+"\n")