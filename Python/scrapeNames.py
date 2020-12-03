# take scraped data and create new file with names alone


input = open("high-budget.txt", "r", encoding="utf-8") #philosopher-links-v3.txt
output = open("high-budget-names.txt", "w", encoding="utf-8") #philosopher-names-v3.txt
count = 1

while True:
        # Read next line
        line = input.readline()
        # If line is blank, then you struck the EOF
        if not line :
            break
        output.write(line)
        line = input.readline()  #throw away links
        print(count)
        count+=1

input.close()



