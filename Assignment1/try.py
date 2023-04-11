# read a file and count the number of words in it
wordCount = 0
for i in [1, 2, 4, 7, 9]:
    asgn = "Assgn1_" + str(i) + "_15.sh"
    asgnmntfile = open(asgn, "r")
    content = asgnmntfile.read().replace('=', ' = ').replace('|', ' | ').replace(';', ' ; ').replace(',', ' , ').replace('<', ' < ').replace('>', ' > ').replace('(', ' ( ').replace(')', ' ) ').replace('{', ' { ').replace('}', ' } ').replace('[', ' [ ').replace(']', ' ] ')
    print(content)
    wordCount += len(content.split())

print(wordCount)