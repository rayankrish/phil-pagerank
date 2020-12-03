import wikipedia

file = open("sp500.txt", "w", encoding="utf-8")
seperator = ", "
count = 1
linksArray = []

#print(wikipedia.page("List of philosophers (A–C)").links)
phils = wikipedia.page("List_of_S%26P_500_companies").links
print(len(phils))

for phil in phils:
    print(phil)
    print(wikipedia.page(phil).links)

