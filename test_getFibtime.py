import getFibTime

firstFib = getFibTime.firstFib

print getFibTime.getFibonacciTime(17,57)


for x in range(1,13):
	for y in range(0,61,5):

		#hourList = getFibTime.getFibonacciList(x,firstFib)
		#minList = getFibTime.getFibonacciList(y/5, [n for n in firstFib if n not in hourList])

		#colors = {c: getFibTime.getColor(c,hourList, minList) for c in firstFib}
		colors = getFibTime.getFibonacciTime(x,y)

		testHours = 0
		testMins = 0

		for k in colors:
			if k == 1:
				for i in colors[1]:
					if i == "Red":
						testHours += 1
					elif i == "Blue":
						testHours +=1
						testMins += 1
					elif i == "Green":
						testMins += 1
			else:
				if colors[k] == "Red":
					testHours += k
				elif colors[k] == "Green":
					testMins += k
				elif colors[k] == "Blue":
					testHours += k
					testMins += k

		if not testMins*5 == y:
			print "Hours: ",x, "\tMin: ",y
			#print "\tHours List: ",hourList
			#print "\tMin List: ", minList

			print testHours == x, testMins*5 == y

			print colors,'\n'
