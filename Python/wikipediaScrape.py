import wikipedia

file = open("high-budget.txt", "w", encoding="utf-8")
seperator = ", "
count = 1#
linksArray = []

#print(wikipedia.page("List of philosophers (A–C)").links)
#phils = wikipedia.page("List of philosophers (A–C)").links
#phils = phils + wikipedia.page("List of philosophers (D–H)").links
#phils = phils + wikipedia.page("List of philosophers (I–Q)").links
#phils = phils + wikipedia.page("List of philosophers (R–Z)").links
phils = wikipedia.page("List of most expensive films").links
print(len(phils))

for phil in phils:
    #print(phil)
    file.write(phil)
    file.write("\n")

    try:
        linksArray = wikipedia.page(phil).links
    except wikipedia.exceptions.PageError as e:
        print("failed " + phil)
        print(e)
    except wikipedia.exceptions.DisambiguationError as e:
        print("failed " + phil)
        print(e)

    #print(linksArray)

    try:
        file.write(seperator.join(linksArray))
    except UnicodeEncodeError as e:
        print("skipped write " + phil)
        print(e)
    file.write("\n")
    print(count)
    count+=1

