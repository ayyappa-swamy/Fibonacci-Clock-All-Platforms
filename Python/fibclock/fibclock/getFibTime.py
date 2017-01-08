import time

firstFib = [1,1,2,3,5]

currentHours = time.localtime().tm_hour % 12
currentMinutes = time.localtime().tm_min

#print "\n\nHours: ", currentHours," Minutes: ", currentMinutes

def getColor(number, hours, mins):

	if (number in hours) and (number in mins):
		color = "Blue"
	elif number in hours:
		color = "Red"
	elif number in mins:
		color = "Green"
	else :
		color = "White"

	if number == 1 :
		if (hours.count(1) > 1) and (mins.count(1) > 1):
			return [color, "Blue"]
		elif hours.count(1) > 1:
			return [color, "Red"]
		elif mins.count(1) > 1:
			return [color, "Green"]
		elif hours.count(1) == 1 and mins.count(1) == 1:
			return ["Red", "Green"]
		else :
			return [color, "White"]
	else:
		return color

def getFibonacciList(number, auxillaryList): #Auxillary list is to search for numbers which are of higher priority

	finalList = []
	index = len(auxillaryList) - 1
	sum = 0

	if number == 0:
		return []

	while index > -1 :
		sum += auxillaryList[index]
		if sum < number:
			finalList += [auxillaryList[index]]
		elif sum == number:
			finalList += [auxillaryList[index]]
			break
		else :
			sum -= auxillaryList[index]
		index -= 1

	if sum < number:
		return finalList+ getFibonacciList(number-sum, [x for x in firstFib if x not in finalList])

	return finalList

def currentFibonacciTime():

	hourList = getFibonacciList(currentHours,firstFib)
	minList = getFibonacciList(currentMinutes/5, firstFib)

	#print "\nHours List: ",hourList
	#print "Min List: ", minList

	colors = {x: getColor(x,hourList, minList) for x in firstFib}

	return colors

print currentFibonacciTime()


def getFibonacciTime(hrs,mins):
	hourList = getFibonacciList(hrs,firstFib)
	minList = getFibonacciList(mins/5,firstFib)

	#print "\nHours List: ",hourList
	#print "Min List: ", minList

	colors = {x: getColor(x,hourList, minList) for x in firstFib}

	return colors
